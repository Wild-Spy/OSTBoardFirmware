//
// Created by mcochrane on 1/04/17.
//

#ifndef WS_OST_MIN_TRANSMIT_CMDS_H
#define WS_OST_MIN_TRANSMIT_CMDS_H

// Functions in this file can be called by c or c++!  But the underlying code is in c++
// so that we can call c++ functions from within these functions.
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <time_avr.h>
#ifdef __cplusplus
}
#endif

/* Report messages to the host */
#define MIN_ID_DEADBEEF					    (0x0eU)			/* Layer 1 frame; Payload always 4 bytes = 0xdeadbeef */
#define MIN_ID_RESPONSE_GENERAL             (0xF0U)
#define MIN_ID_ENVIRONMENT				    (0x23U)			/* Layer 1 frame; Temperature and humidity sensor values */
#define MIN_ID_MOTOR_STATUS				    (0x24U)			/* Layer 1 frame; Report the status of the motor */
#define MIN_ID_RESPONSE_GET_RULE_COUNT      (0x26u)
#define MIN_ID_RESPONSE_GET_RULE_WITH_ID    (0x27u)
#define MIN_ID_GET_RESPONSE_ALL_RULES       (0x28u)
#define MIN_ID_RESPONSE_GET_RTC_TIME        (0x29u)

#define MIN_ID_PRINT_START                  (0x30u)
#define MIN_ID_PRINT_DATA                   (0x31u)
#define MIN_ID_PRINT_END                    (0x32u)

/* Functions to take application data and send to host */
EXTERNC void report_environment(uint16_t temperature, uint16_t humidity);
EXTERNC void report_motor(uint8_t status, uint32_t position);
EXTERNC void report_deadbeef(uint32_t deadbeef);
EXTERNC void report_receive_rule_response();
EXTERNC void report_rule_count();
EXTERNC void report_rtc_time(time_t_avr time);
EXTERNC void report_response_ack();
EXTERNC void report_response_nak();

EXTERNC void report_start_print();
EXTERNC void report_data_print();
EXTERNC void report_end_print();

EXTERNC void report_printl(const char* data, uint16_t len);
EXTERNC void report_prints(const char* data); //null terminated string
EXTERNC void report_printf(const char* data, ...);
//EXTERNC void report_printf_P(const char *data, ...);

#undef EXTERNC
#endif //WS_OST_MIN_TRANSMIT_CMDS_H
