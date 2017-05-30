//
// Created by mcochrane on 1/04/17.
//

#include <datetime/DateTime.h>
#include <string.h>
#include <interrupt.h>
#include "min_transmit_cmds.h"
#include <nvm/NvmRuleManager.h>
#include <cstdarg>
#include <libs/TimerDescriptionLanguage/TdlRules.h>
#include "layer2_helper.h"
#include "min.h"

/* Report the current state of the environment */
void report_environment(uint16_t temperature, uint16_t humidity)
{
    DECLARE_BUF(4);

    PACK16(temperature);
    PACK16(humidity);

    SEND_FRAME(MIN_ID_ENVIRONMENT);
}

/* Report the current motor position, including a status */
void report_motor(uint8_t status, uint32_t position)
{
    DECLARE_BUF(5);

    PACK8(status);
    PACK32(position);

    SEND_FRAME(MIN_ID_MOTOR_STATUS);
}

/* Report 0xdeadbeef as a heartbeat signal */
void report_deadbeef(uint32_t deadbeef)
{
    DECLARE_BUF(4);

    PACK32(deadbeef);

    SEND_FRAME(MIN_ID_DEADBEEF);
}

void report_rule_count()
{
    DECLARE_BUF(1);
    PACK8(TdlRules_GetInstance().getCount());
    SEND_FRAME(MIN_ID_RESPONSE_GET_RULE_COUNT);
}

void report_response_ack() {
    DECLARE_BUF(1);
    PACK8(0x00);
    SEND_FRAME(MIN_ID_RESPONSE_GENERAL);
}

void report_response_nak() {
    DECLARE_BUF(1);
    PACK8(0x01);
    SEND_FRAME(MIN_ID_RESPONSE_GENERAL);
}

void report_receive_rule_response() {

}

void report_rtc_time(time_t_avr time) {
    DECLARE_BUF(4);
    PACK32((uint32_t)time);
    SEND_FRAME(MIN_ID_RESPONSE_GET_RTC_TIME);
}

void report_start_print(uint16_t length) {
    DECLARE_BUF(2);
    PACK16(length);
    SEND_FRAME(MIN_ID_PRINT_START);
}

void report_data_print(uint8_t* buf, uint8_t len) {
//    DECLARE_BUF(len);
    min_tx_frame(MIN_ID_PRINT_DATA, buf, len);
}

void report_end_print() {
    DECLARE_BUF(0);
    SEND_FRAME(MIN_ID_PRINT_END);
}

void report_printl(const char *data, uint16_t len) {
    report_start_print(len);
    while (len > 0) {
        if (len >= MAX_FRAME_PAYLOAD_SIZE) {
            report_data_print((uint8_t*)data, MAX_FRAME_PAYLOAD_SIZE);
            len -= MAX_FRAME_PAYLOAD_SIZE;
            data += MAX_FRAME_PAYLOAD_SIZE;
        } else {
            report_data_print((uint8_t*)data, len);
            len = 0;
        }
    }
    report_end_print();
}

void report_prints(const char *data) {
    size_t len = strlen(data);
    report_printl(data, len);
}

void report_printf(const char *data, ...) {
    cpu_irq_enter_critical();
    char buf[128];
    va_list argptr;
    va_start(argptr, data);
    vsprintf(buf, data, argptr);
    va_end(argptr);

    report_prints(buf);
    cpu_irq_leave_critical();
}