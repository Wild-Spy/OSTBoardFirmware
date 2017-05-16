//
// Created by mcochrane on 1/04/17.
//

#ifndef WS_OST_MIN_RECEIVED_CMDS_H
#define WS_OST_MIN_RECEIVED_CMDS_H

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

#ifdef __cplusplus
}
#endif

/* Command frames from the host */
#define MIN_ID_PING						(0x02U)			/* Layer 1 frame; Ping test: returns the same frame back */
#define MIN_ID_RULE_START_RECEIVE       (0x03u)
#define MIN_ID_RULE_DATA_RECEIVE        (0x04u)
#define MIN_ID_RULE_END_RECEIVE         (0x05u)
#define MIN_ID_GET_RULE_COUNT           (0x06u)
#define MIN_ID_GET_RULE_WITH_ID         (0x07u)
#define MIN_ID_GET_ALL_RULES            (0x08u)
#define MIN_ID_ERASE_ALL_RULES          (0x09u)
#define MIN_ID_GET_RTC_TIME             (0x0au)
#define MIN_ID_SET_RTC_TIME             (0x0bu)
#define MIN_ID_SAVE_RULES               (0x0cu)
#define MIN_ID_PRINT_EEPROM 			(0x0du)

/* Motor control request from the host */
extern uint8_t motor_requested;
extern uint32_t motor_position_request;
extern uint16_t motor_speed_request;

EXTERNC void handle_print_eeprom(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);
EXTERNC void handle_ping(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);
EXTERNC void handle_get_rule_count(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);
EXTERNC void handle_rx_rule_start(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);
EXTERNC void handle_rx_rule_data(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);
EXTERNC void handle_rx_rule_end(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);
EXTERNC void handle_erase_all_rules(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);
EXTERNC void handle_get_rtc_time(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);
EXTERNC void handle_set_rtc_time(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);
EXTERNC void handle_save_rules(uint8_t m_id, uint8_t *m_buf, uint8_t m_control);

#undef EXTERNC
#endif //WS_OST_MIN_RECEIVED_CMDS_H
