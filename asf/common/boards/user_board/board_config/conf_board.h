/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#define BOARD USER_BOARD

//! Definition of USART connection for this example
//! TXC0 on PC3 and RXC0 on PC2
#define  USART               USARTC0
#define  USART_RX_Vect       USARTC0_RXC_vect
#define  USART_DRE_Vect      USARTC0_DRE_vect
#define  USART_SYSCLK        SYSCLK_USART0
#define  USART_PORT          PORTC
#define  USART_PORT_PIN_TX   (1<<3)  // PC3 (TXC0)
#define  USART_PORT_PIN_RX   (1<<2)  // PC2 (RXC0)
#define  USART_PORT_SYSCLK   SYSCLK_PORT_C

#endif // CONF_BOARD_H
