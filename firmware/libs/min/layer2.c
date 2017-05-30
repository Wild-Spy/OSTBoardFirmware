/*
 * Application layer (MIN layer 2)
 *
 * Application-specific handling of data to and from the host.
 *
 * Author: Ken Tindell
 * Copyright (c) 2014-2015 JK Energy Ltd.
 * Licensed under MIT License.
 */

#include "layer2.h"
#include <samd21.h>
//#include <nvm/rule_writer.h>
#include "serial/usb_serial.h"
#include "min/min.h"

#include "usb_handlers.h"
#include <hw/hal_gpio.h>

/* Main function to process incoming bytes and pass them into MIN layer */
void poll_rx_bytes(void)
{
    /* Handle all the outstanding characters in the input buffer */
    while(serial_receive_ready()) {
        uint8_t byte;
        serial_receive(&byte, 1U);
        min_rx_byte(byte);
    }
}

/* Callback from MIN layer 1 to indicate the frame has been received */
void min_frame_received(uint8_t buf[], uint8_t control, uint8_t id)
{
//    report_printf("rx - c:%u, id:%u", control, id);
//    PORT->Group[HAL_GPIO_PORTA].OUTSET.reg = (1 << 27);

    switch(id) {
        case MIN_ID_PING:
            handle_ping(id, buf, control);
            break;
        case MIN_ID_PRINT_EEPROM:
//            handle_print_nvm(id, buf, control);
            break;
        case MIN_ID_RULE_START_RECEIVE:
            handle_rx_rule_start(id, buf, control);
            break;
        case MIN_ID_RULE_DATA_RECEIVE:
            handle_rx_rule_data(id, buf, control);
            break;
        case MIN_ID_RULE_END_RECEIVE:
            handle_rx_rule_end(id, buf, control);
            break;
        case MIN_ID_GET_RULE_COUNT:
            handle_get_rule_count(id, buf, control);
            break;
        case MIN_ID_ERASE_ALL_RULES:
            handle_erase_all_rules(id, buf, control);
            break;
        case MIN_ID_GET_RTC_TIME:
            handle_get_rtc_time(id, buf, control);
            break;
        case MIN_ID_SET_RTC_TIME:
            handle_set_rtc_time(id, buf, control);
            break;
        case MIN_ID_SAVE_RULES:
            handle_save_rules(id, buf, control);
            break;
    }
}

/* Callback from MIN to send a byte over the UART (in this example, queued in a FIFO) */
void min_tx_byte(uint8_t byte)
{
    if (!get_cdc_enumerated()) return;
    /* Ignore FIFO overrun issue - don't send frames faster than the FIFO can handle them
     * (and make sure the FIFO is big enough to take a maximum-sized MIN frame).
     */
    serial_send(&byte, 1U);

//    PORTB.OUTSET = (1<<3);
    while (serial_send_space() == 0);
//    PORTB.OUTCLR = (1<<3);
}

/* Callback from MIN to see how much transmit buffer space there is */
uint8_t min_tx_space(void)
{
    return serial_send_space();
}

void init_min(void)
{
    init_serial();
    min_init_layer1();
}