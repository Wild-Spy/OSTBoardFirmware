/* Serial driver for ATmega640 family.
 *
 * The driver is interrupt driven where a pair of ISRs feed a USART from
 * a transmit FIFO and feed a receive FIFO from the USART. The application
 * then polls the receive FIFO and loads data into the transmit FIFO.
 *
 * Author: Ken Tindell
 * Copyright (c) 2014-2015 JK Energy Ltd.
 * Licensed under MIT License.
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <usb_protocol_cdc.h>

/* Bind to specific USART hardware */
#define F_OSC_HZ					(16000000UL)		            /* 16MHz main clock */

/* Set up USART as a UART
 *
 * baud			Baud rate, defined by macros above
 * stop_bits	1 or 2
 * data_bits	5, 6, 7, 8
 * parity		None, odd or even, defined by macros above
 *
 * The UART is set up for interrupt handling on transmit and receive.
 */
//void init_uart(uint16_t baud);
void init_uart(uint8_t port, usb_cdc_line_coding_t * cfg);

/* Select FIFO buffer sizes */
#define TX_FIFO_MAXSIZE             (70U)
#define RX_FIFO_MAXSIZE             (40U)

/* Functions for application layer to use */
uint8_t uart_receive(uint8_t *dest, uint8_t n);     /* Take n bytes from receive FIFO */
uint8_t uart_send(uint8_t *src, uint8_t n);         /* Push n bytes into transmit FIFO */
uint8_t uart_send_space(void);                      /* Return how much space left in transmit FIFO */
uint8_t uart_receive_ready(void);                   /* Returns true if receive FIFO not empty */

int8_t uart_ireceive(uint8_t port);
int8_t uart_isend(uint8_t port);

#endif /* SERIAL_H_ */