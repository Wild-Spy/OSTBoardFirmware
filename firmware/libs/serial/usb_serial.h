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

#include <io.h>
//#include <interrupt.h>
#include <usb_protocol_cdc.h>

void init_serial();

/* Select FIFO buffer sizes */
#define TX_FIFO_MAXSIZE             (70U)
#define RX_FIFO_MAXSIZE             (40U)

/* Functions for application layer to use */
uint8_t serial_receive(uint8_t *dest, uint8_t n);     /* Take n bytes from receive FIFO */
uint8_t serial_send(uint8_t *src, uint8_t n);         /* Push n bytes into transmit FIFO */
uint8_t serial_send_space(void);                      /* Return how much space left in transmit FIFO */
uint8_t serial_receive_ready(void);                   /* Returns true if receive FIFO not empty */

int8_t serial_ireceive(uint8_t port);
int8_t serial_isend(uint8_t port);

#endif /* SERIAL_H_ */