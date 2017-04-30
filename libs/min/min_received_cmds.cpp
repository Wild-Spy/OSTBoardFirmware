//
// Created by mcochrane on 1/04/17.
//

#include <stdbool.h>
#include <DS3232SN/DS3232SN.h>
#include <TimerDescriptionLanguage/TdlRules.h>
#include <TimerDescriptionLanguage/TdlRuleRunner.h>
#include <TimerDescriptionLanguage/TdlChannels.h>
#include "min_received_cmds.h"
#include "min_transmit_cmds.h"
#include "config_comms/ruler_writer_cpp.h"
#include "exception/ExceptionValues.h"

extern "C" {
#include <string.h>
#include "layer2_helper.h"
#include "min.h"
#include <eeprom_driver/eeprom_driver.h>
}

/* Functions to unpack incoming MIN frames into application-specific data.
 */

uint8_t rule_buffer[200];
uint8_t rule_buffer_index = 0;
uint16_t rule_size;
bool receiving_rule = false;

void handle_print_eeprom(uint8_t m_id, uint8_t *m_buf, uint8_t m_control)
{
    uint8_t buflen;
    uint16_t buf_start;


    DECLARE_UNPACK();
    UNPACK16(buf_start);
    UNPACK8(buflen);

    uint8_t buffer[buflen];
    report_printf_P(PSTR("Printing buffer!"));
    //clear buffer
    for (uint8_t i = 0; i < buflen; i++) {
        buffer[i] = 0x00;
    }

    ReadEEPROM(buffer, buflen, buf_start);
    for (uint8_t i = 0; i < buflen; i++) {
        report_printf_P(PSTR("buf[%u] = %u"), i+buf_start, buffer[i]);
    }
}

/* Other application-specific frame-decode functions would be added here.
 */

/* Provides a 'ping' service by echoing back the ping frame with
 * the same ID and payload.
 */
void handle_ping(uint8_t m_id, uint8_t *m_buf, uint8_t m_control)
{
    min_tx_frame(m_id, m_buf, m_control);
}

void handle_get_rule_count(uint8_t m_id, uint8_t *m_buf, uint8_t m_control) {
//    DECLARE_UNPACK();
//    uint8_t id;
//    UNPACK8(id);

    report_rule_count();
}

void handle_rx_rule_start(uint8_t m_id, uint8_t *m_buf, uint8_t m_control)
{
    DECLARE_UNPACK();
    UNPACK16(rule_size);
    rule_buffer_index = 0;
    receiving_rule = true;
}

void handle_rx_rule_data(uint8_t m_id, uint8_t *m_buf, uint8_t m_control)
{
    if (!receiving_rule) return;
    uint8_t frame_length = min_frame_length(m_control);
    memcpy(rule_buffer + rule_buffer_index, m_buf, frame_length);
    rule_buffer_index += frame_length;
}

void handle_rx_rule_end(uint8_t m_id, uint8_t *m_buf, uint8_t m_control)
{
    CEXCEPTION_T e;
    CEXCEPTION_FILELINEDEFS;

    if (!receiving_rule || rule_buffer_index!=rule_size) {
        report_response_nak();
        receiving_rule = false;
        return;
    }

    receiving_rule = false;

    Try {
        RuleWriter_AddNewRule(rule_buffer, rule_size);
    } Catch(e) {
        report_response_nak();
        return;
    }

    report_response_ack();

    //TODO: send back the target ID that was just created?
}
void handle_save_rules(uint8_t m_id, uint8_t *m_buf, uint8_t m_control)
{
    if (receiving_rule) {
        report_response_nak();
        return;
    }

    TdlChannels_GetInstance().resetStates();
    RuleWriter_SaveRuleCount();

    TdlRuleRunner_GetInstance().stop();

    TdlRuleRunner_GetInstance().reloadRules();


    TdlRuleRunner_GetInstance().start();

    report_response_ack();
}

void handle_erase_all_rules(uint8_t m_id, uint8_t *m_buf, uint8_t m_control) {
    DECLARE_UNPACK();
    uint8_t safety_byte1 = 0;
    uint8_t safety_byte2 = 0;

    UNPACK8(safety_byte1);
    UNPACK8(safety_byte2);

    if (safety_byte1 == 0x5D && safety_byte2 == 0x92) {
        RuleWriter_ClearRules();
        report_response_ack();
    } else {
        report_response_nak();
    }
}

//DateTime ttttt;

void handle_get_rtc_time(uint8_t m_id, uint8_t *m_buf, uint8_t m_control) {
//    report_rtc_time(ttttt.toTimet());
    report_rtc_time(getNow().toTimet());
    //report_rtc_time((time_t) ((uint32_t)ONE_HOUR*10));

//    report_rtc_time(DateTime((time_t)0).toTimet());
//    struct tm t;
//    t.tm_mday = 1;
//    t.tm_hour = 0;
//    t.tm_sec = 0;
//    t.tm_min = 0;
//    t.tm_year = 2010-1900;
//    t.tm_mon = 0;
//
//    report_rtc_time(DateTime(&t).toTimet());
    //report_rtc_time(DateTime(0, 0, 0, 1, 1, 2020).toTimet());
    //report_rtc_time(DateTime((time_t)0).toTimet());
}

void handle_set_rtc_time(uint8_t m_id, uint8_t *m_buf, uint8_t m_control) {
    DECLARE_UNPACK();
    time_t time;
    UNPACK32(time);
    DateTime dt = DateTime(time);
//    ttttt = dt;

    TdlChannels_GetInstance().resetStates();

    TdlRuleRunner_GetInstance().reloadRules();
    TdlRuleRunner_GetInstance().start(dt);
//    getRtc().set(dt);
//    TdlRules_Init();
//    report_response_ack();
}
