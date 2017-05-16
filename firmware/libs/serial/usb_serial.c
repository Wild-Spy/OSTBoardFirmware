/*
 * Serial drivers for ATmega640 family, with software FIFOs for
 * receive and transmit.
 *
 *  Author: Ken Tindell
 * Copyright (c) 2014-2015 JK Energy Ltd.
 * Licensed under MIT License.
 */

#include <udi_cdc.h>
#include "usb_serial.h"

/* Sending process:
 * - A FIFO for sending
 * - "Character sent" interrupt reads a character from the FIFO (if there
 *   is one) and puts it in the UART data buffer
 * - Application layer writes to the FIFO; if it was empty it triggers the
 *   "sent" interrupt (or writes directly to the UART)
 *
 * Receiving process:
 * - A FIFO for receiving
 * - A "character received" interrupt reads the UART buffer and writes to
 *   the FIFO (writes to full are ignored and character discarded)
 * - Application layer polls the FIFO to see if there are enough characters
 *   to process
 */

struct fifo {
    uint8_t *buf;		/* Space allocated to FIFO */
    uint8_t size;		/* Size of FIFO */
    uint8_t head;		/* Indexes first free byte (unless full) */
    uint8_t tail;		/* Indexes last filled byte (unless empty) */
    uint8_t used;		/* Between 0..size */
    uint8_t max_used;	/* Maximum space used in the FIFO */
};

#define FIFO_EMPTY(f)	((f)->used == 0)
#define FIFO_FULL(f)	((f)->used == (f)->size)
#define FIFO_USED(f)	((f)->used)

static void fifo_init(struct fifo *f, uint8_t buf[], uint8_t size)
{
    f->used = 0;
    f->head = f->tail = 0;
    f->buf = buf;
    f->size = size;
    f->max_used = 0;
}

static void fifo_write(struct fifo *f, uint8_t b)
{
    if(!FIFO_FULL(f)) {						/* Only proceed if there's space */
        f->used++;							/* Keep track of the number of used bytes in the FIFO */
        f->buf[f->head++] = b;				/* Add to where head indexes */
        if(f->head == f->size) {			/* If head goes off the end of the FIFO buffer then wrap it */
            f->head = 0;
        }
    }
    if(f->used > f->max_used) {				/* For diagnostics keep a high-water mark of used space */
        f->max_used = f->used;
    }
}

static uint8_t fifo_read(struct fifo *f)
{
    uint8_t ret = 0;

    if(!FIFO_EMPTY(f)) {					/* Only proceed if there's something there */
        f->used--;							/* Keep track of the used space */
        ret = f->buf[f->tail++];
        if(f->tail == f->size) {			/* Wrap tail around if it goes off the end of the buffer */
            f->tail = 0;
        }
    }

    return ret;
}

static struct fifo tx_fifo;
static struct fifo rx_fifo;

static uint8_t tx_buf[TX_FIFO_MAXSIZE];
static uint8_t rx_buf[RX_FIFO_MAXSIZE];

#define LOCK_INTERRUPTS(i)		    		{(i) = SREG; cli();}			/* NB: SIDE-EFFECT MACRO! */
#define UNLOCK_INTERRUPTS(i)				{SREG = i;}

/* Main setup of ATmega640 USART
 *
 * Hardwired to asynchronous and not using clock doubler so will do 16 samples on receive (better for noise tolerance).
 * The baud parameter will be written straight into the baud rate register.
 */
//void init_uart(uint16_t baud)
void init_uart(uint8_t port, usb_cdc_line_coding_t * cfg)
{
    /* Initialize FIFOs for UART */
    fifo_init(&tx_fifo, tx_buf, TX_FIFO_MAXSIZE);
    fifo_init(&rx_fifo, rx_buf, RX_FIFO_MAXSIZE);
}

/* Handle interrupt generated when the transmit buffer is free to write to.
 */
int8_t uart_isend(uint8_t port)
{
    uint8_t used = tx_fifo.used;
    /* Called when the transmit buffer can be filled */
    if (used > 0) {
        uint8_t byte;

        byte = fifo_read(&tx_fifo);

        // Transfer UART RX fifo to CDC TX
        if (!udi_cdc_is_tx_ready()) {
            // Fifo full
            udi_cdc_signal_overrun();
            return -2;
        } else {
            udi_cdc_putc(byte);
        }

        used = tx_fifo.used;

        if(used == 1U) {
            /* If it was just one byte in the FIFO then it will now be empty; stop interrupts until FIFO becomes not empty */

        }
    } else if (used <= 1U) {
        /* There was either nothing to send (spurious interrupt) or else there is nothing to send now so must disable the UDREn interrupt
         * to stop interrupts - must be re-enabled when FIFO becomes not empty
         */
        //UCSRnB(UART) &= ~(1U << UDRIEn);
        return -1;
    }
    return 0;
}

/* Handle receive interrupt.
 *
 * The CPU must handle the received bytes as fast as they come in (on average): there is no flow control.
 */
int8_t uart_ireceive(uint8_t port)
{
    int val;
    /* TODO handle the error flags (DORn should be logged - it indicates the interrupt handling isn't fast enough; parity error frames should be discarded) */

    if (!udi_cdc_is_rx_ready()) return -1;
    /* This read also clears down the interrupt */
    val = udi_cdc_getc();

    /* Write byte to FIFO; if there is no room in the FIFO the byte is discarded */
    fifo_write(&rx_fifo, (uint8_t)val);

    return 0;
}

/* Send n bytes to the given USART from the given source. Returns the number of bytes actually buffered. */
uint8_t uart_send(uint8_t *src, uint8_t n)
{
    uint8_t written = 0;
    uint8_t tmp;

    /* Interrupts are locked out only through the body of the loop so that transmission can
     * start as soon as there is data to send, which happens concurrently with filling the
     * FIFO
     */
    while (n--) {
        LOCK_INTERRUPTS(tmp);
        if (FIFO_FULL(&tx_fifo)) {
            n = 0;	/* No space left, terminate the sending early */
        }
        else {
            fifo_write(&tx_fifo, *(src++));
            /* The FIFO is not empty so enable 'transmit buffer ready' interrupt
             * (may already be enabled but safe to re-enable it anyway).
             */
            uart_isend(0);
            written++;
        }
        UNLOCK_INTERRUPTS(tmp);
    }

    return written;
}

/* Read up to n bytes from the given USART into the given destination. Returns the number of bytes actually read. */
uint8_t uart_receive(uint8_t *dest, uint8_t n)
{
    uint8_t tmp;
    uint8_t read = 0;

    while (n--) {
        LOCK_INTERRUPTS(tmp);
        if (FIFO_EMPTY(&rx_fifo)) {
            n = 0;	/* Nothing left, terminate early */
        }
        else {
            *(dest++) = fifo_read(&rx_fifo);
            read++;
        }
        UNLOCK_INTERRUPTS(tmp);
    }

    return read;
}

uint8_t uart_send_space(void)
{
    uint8_t tmp;
    uint8_t ret;

    LOCK_INTERRUPTS(tmp);
    ret = tx_fifo.size - tx_fifo.used;
    UNLOCK_INTERRUPTS(tmp);

    return ret;
}

uint8_t uart_receive_ready(void)
{
    uint8_t tmp;
    uint8_t ret;

    LOCK_INTERRUPTS(tmp);
    ret = rx_fifo.used;
    UNLOCK_INTERRUPTS(tmp);

    return ret;
}