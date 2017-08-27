/* Layer 2 definitions (application layer).
 *
 * Defines all the MIN setup for the application, selecting IDs to be used
 * and how to pack and unpack signal data and commands.
 *
 * Author: Ken Tindell
 * Copyright (c) 2014-2015 JK Energy Ltd.
 * Licensed under MIT License.
 */

#ifndef LAYER2_H_
#define LAYER2_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "min_received_cmds.h"
#include "min_transmit_cmds.h"

/* Configure the UART speed (see serial.h for binding to UART and FIFO buffer sizes) */
#define MIN_BAUD                        (UART_BAUD_9600)

/* Set up communications */
void init_min(void);

/* Poll the incoming uart to send bytes into MIN layer 1 */
void poll_rx_bytes(void);

#ifdef __cplusplus
}
#endif


#endif /* LAYER2_H_ */