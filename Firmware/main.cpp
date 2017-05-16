/**
 * \file
 *
 * \brief CDC Application Main functions
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

static volatile bool main_b_cdc_enable = false;
#include "exception/ExceptionValues.h"

extern "C" {
#include "libs/asf.h"
#include <sleepmgr.h>
#include <udc/udc.h>
#include <sysclk.h>
#include <udi_cdc.h>
#include "conf_usb.h"
#include "uart.h"
#include <util/delay.h>
#include <min/layer2.h>
#include <memory/MemoryWrapper.h>
#include <time.h>
#include "eeprom_driver/eeprom_driver.h"
}

#include <TimerDescriptionLanguage/TdlChannel.h>
#include <TimerDescriptionLanguage/TdlRule.h>
#include <TimerDescriptionLanguage/TdlRules.h>
#include <TimerDescriptionLanguage/TdlRuleRunner.h>
#include <TimerDescriptionLanguage/TdlChannels.h>
#include "CppNewDeleteOps.h"
#include "DS3232SN/DS3232SN.h"
#include "helpers/compile_time_sizeof.h"

TdlChannel led_test;
//long cntr = 0;

//COMPILE_TIME_SIZEOF(TdlRule);

ISR(PORTE_INT0_vect) {
    TdlRuleRunner_GetInstance().interruptCallback();
}

//void eepromtest() {
//    uint16_t buf_start = 100;
//    uint8_t buflen = 51;
//    uint8_t buffer[buflen*2];
//    for (uint8_t i = 0; i < buflen; i++) {
//        buffer[i] = 1;
//    }
//    WriteEEPROM(buffer, buflen, buf_start);
//    EEPROM_FlushBuffer();
//
//    for (uint8_t i = 0; i < buflen; i++) {
//        buffer[i] = 123;
//    }
//    WriteEEPROM(buffer, buflen, buf_start);
//    EEPROM_FlushBuffer();
//
//    while(1) {
//        report_prints("Printing buffer!");
//        //clear buffer
//        for (uint8_t i = 0; i < buflen*2; i++) {
//            buffer[i] = 0x00;
//        }
//
//        ReadEEPROM(buffer, buflen*2, buf_start);
//        for (uint8_t i = 0; i < buflen*2; i++) {
//            report_printf("buf[%u] = %u", i, buffer[i]);
//        }
//    }
//}

//void DS3232SNTest() {
//    DateTime now;
//    DateTime nextAlarmTime;
//
//    _delay_ms(5000);
//
//    initRtc();
//    // clear oscillator stopped flag if it's set
//    getRtc().oscStopped(true);
//    getRtc().set(DateTime(2000, 1, 1, 0, 0, 0));
//    now = getNow(); //function to get current time
//
//    nextAlarmTime = now;
//    nextAlarmTime.addSeconds(5);
//    report_printf_P(PSTR("Set First Alarm %s"), nextAlarmTime.isotime());
//    getRtc().setAlarm(ALM1_MATCH_DATE, nextAlarmTime);
//    getRtc().alarmInterrupt(1, true);
//    getRtc().alarm(1);
//    getRtc().enablePinInterrupt();
//
//    while (1) {
//        if (rtc_interrupt_triggered) {
//            if (getRtc().alarm(1)) {
//
//                now = getNow();
//                report_printf_P(PSTR("Handle Alarm   %s"), now.isotime());
//                nextAlarmTime = now;
//                nextAlarmTime.addSeconds(5);
//
//                getRtc().setAlarm(ALM1_MATCH_DATE, nextAlarmTime);
//                getRtc().alarmInterrupt(1, true);
//
//                report_printf_P(PSTR("Set Next Alarm %s"), nextAlarmTime.isotime());
//            } else {
//                report_printf_P(PSTR("Alarm Off???   %s"), getNow().isotime());
//            }
//            rtc_interrupt_triggered = false;
//            getRtc().enablePinInterrupt();
//        }
//    }
//
////    while (1) {
////        if (rtc_interrupt_triggered) {
////            report_printf_P(PSTR("TRIG!"));
////            if (getNow() >= nextAlarmTime) {
////                now = getNow();
////                report_printf_P(PSTR("Handle Alarm   %s"), now.isotime());
////                nextAlarmTime = now;
////                nextAlarmTime.addSeconds(5);
////
////                getRtc().dumpRTC(0x0F, 1);
////
////                while(getRtc().busy())
////                    report_printf_P(PSTR("BUSY!"));
////
////                report_printf_P(PSTR("Busy Done  %s"), now.isotime());
////
////
////                cli();
////                getRtc().alarm(1);
////                getRtc().setAlarm(ALM1_MATCH_DATE, nextAlarmTime);
////                getRtc().alarmInterrupt(1, true);
////
////                report_printf_P(PSTR("Set Next Alarm %s"), nextAlarmTime.isotime());
//////                report_printf_P(PSTR("Next Alarm Read %s"), getRtc().readAlarm().isotime());
////                getRtc().dumpRTC(0, 0x10);
////                sei();
////            } else {
////                //report_printf_P(PSTR("Alarm Off???   %s"), getNow().isotime());
////            }
////            rtc_interrupt_triggered = false;
////            getRtc().enablePinInterrupt();
////        }
////    }
//}

//void printClockInfo() {
//
//    report_printf_P(PSTR("sysclk_get_main_hz() = %lu Hz"), sysclk_get_main_hz());
//    report_printf_P(PSTR("sysclk_get_per4_hz() = %lu Hz"), sysclk_get_per4_hz());
//    report_printf_P(PSTR("sysclk_get_per2_hz() = %lu Hz"), sysclk_get_per2_hz());
//    report_printf_P(PSTR("sysclk_get_per_hz() = %lu Hz"), sysclk_get_per_hz());
//    report_printf_P(PSTR("sysclk_get_cpu_hz() = %lu Hz"), sysclk_get_cpu_hz());
//    report_printf_P(PSTR("sysclk_get_cpu_hz(&TWIE) = %lu Hz"), sysclk_get_peripheral_bus_hz(&TWIE));
//}

void init() {
    CEXCEPTION_T e = CEXCEPTION_NONE;

    led_test = TdlChannel(0, TDLCHANNELSTATE_ENABLED, &PORTC, 7);

    // Initialize the sleep manager
    sleepmgr_init();

    sysclk_init();
    board_init();

    //set_zone(0);

    init_min();
    sysclk_enable_peripheral_clock(&TWIE);

    // Start USB stack to authorize VBus monitoring
    udc_start();
    udc_attach();


//    Throw(EX_DATA_NOT_AVAILABLE);
//    led_test.disable();
//    _delay_ms(1000);

//    EEPROM_EraseAll();
//    _delay_ms(1000);
//    eepromtest();

//    DS3232SNTest();

    //_delay_ms(5000);

//    while(1) {printClockInfo();}

    TdlRuleRunner_Init();
    if (TdlRules_GetInstance().getCount() > 0) TdlRuleRunner_GetInstance().start();
//    led_test.enable();
}

void loop() {
    CEXCEPTION_T e = CEXCEPTION_NONE;

    TdlRuleRunner_GetInstance().mainLoopCallback();

//    if (rtc_interrupt_triggered) {
//        TdlRuleRunner_GetInstance().handleRtcAfterInterrupt();
//        rtc_interrupt_triggered = false;
//    }
    if (main_b_cdc_enable) {
        //uart_ireceive(0);
        poll_rx_bytes();
        //led_test.enable();

        //_delay_ms(1000);

//        if (cntr > 100000) {
            //report_prints("print test!");
//                report_printf("i = %d", i);
//                if (!TdlRuleRunner_GetInstance().isInitialised()) {
//                    Try {
//                        TdlRuleRunner_Init();
//                    } Catch(e) {
//                        report_printf("rule runner init exception = %u", e);
//                    }
//                }

//                report_deadbeef(time.getSecondOfMinute());
//                led_test.enable();
//                _delay_ms(100);
//                led_test.disable();


//            char nwu[20];
//            getRtc().getNextAlarm().isotime(nwu);
//            report_printf("RTC Next Wakeup: %s", nwu);
//
//            TdlRuleRunner_GetInstance().getNextWakeupTime().isotime(nwu);
//            report_printf("RR Next Wakeup: %s", nwu);
//
//            report_printf("Running: %d", TdlRuleRunner_GetInstance().isRunning());
//
//            TdlRuleRunner_GetInstance().getStartTime().isotime(nwu);
//            report_printf("Start Time: %s", nwu);
//
//            TdlRuleRunner_GetInstance().getNow().isotime(nwu);
//            report_printf("Now: %s", nwu);
//
//            TdlRuleRunner_GetInstance().getCurrentTime().isotime(nwu);
//            report_printf("RR Current Time: %s", nwu);

//            led_test.toggle();
//            cntr = 0;
//        }
    }

//    cntr++;

    sleepmgr_enter_sleep();

//    while (true) {
//        sleepmgr_enter_sleep();
//    }
}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
    CEXCEPTION_T e = CEXCEPTION_NONE;
    CEXCEPTION_FILELINEDEFS;

    irq_initialize_vectors();
    cpu_irq_enable();

    Try {
        init();
    } CatchFL(e) {
        while(1) {
            if (main_b_cdc_enable)
                report_printf_P(PSTR("Error in setup! Exception: %u (%s:%u)"), e, ex_file, ex_line);
            for (uint8_t i = 0; i < 10; i++) {
                if (main_b_cdc_enable) {
                    poll_rx_bytes();
                }
            }
        }
    }

//    _delay_ms(5000);

    Try {
        while (1) loop();
    } CatchFL(e) {
        while(1) {
            if (main_b_cdc_enable)
                report_printf_P(PSTR("Error in loop! Exception: %u (%s:%u)"), e, ex_file, ex_line);
        }
        //while(1); // TODO: what to do????? - restart???
    }
}

void main_suspend_action(void) {
    main_b_cdc_enable = false;
//    led_test.enable();
}

void main_resume_action(void) {
    main_b_cdc_enable = true;
//    led_test.disable();
}

void main_sof_action(void) {
    if (!main_b_cdc_enable)
        return;
}

#ifdef USB_DEVICE_LPM_SUPPORT
void main_suspend_lpm_action(void)
{
    ui_powerdown();
}

void main_remotewakeup_lpm_disable(void)
{
    ui_wakeup_disable();
}

void main_remotewakeup_lpm_enable(void)
{
    ui_wakeup_enable();
}
#endif

bool main_cdc_enable(uint8_t port) {
    main_b_cdc_enable = true;
    // Open communication
    //uart_open(port);

    return true;
}

void main_cdc_disable(uint8_t port) {
    main_b_cdc_enable = false;
    // Close communication
    //uart_close(port);

}

void main_cdc_set_dtr(uint8_t port, bool b_enable) {
    if (b_enable) {
        // Host terminal has open COM
        //uart_open(port);
    } else {
        // Host terminal has close COM
    }
}

void cdc_rx_notify(int port) {
    while (uart_ireceive(0) == 0);
        //PORTC.OUTTGL = (1<<7);
}

bool main_cdc_is_enabled() {
    return main_b_cdc_enable;
}

/**
 * \mainpage ASF USB Device CDC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device CDC
 * on Atmel MCU with USB module.
 * The application note AVR4907 provides more information
 * about this implementation.
 *
 * \section desc Description of the Communication Device Class (CDC)
 * The Communication Device Class (CDC) is a general-purpose way to enable all
 * types of communications on the Universal Serial Bus (USB).
 * This class makes it possible to connect communication devices such as
 * digital telephones or analog modems, as well as networking devices
 * like ADSL or Cable modems.
 * While a CDC device enables the implementation of quite complex devices,
 * it can also be used as a very simple method for communication on the USB.
 * For example, a CDC device can appear as a virtual COM port, which greatly
 * simplifies application development on the host side.
 *
 * \section startup Startup
 * The example is a bridge between a USART from the main MCU
 * and the USB CDC interface.
 *
 * In this example, we will use a PC as a USB host:
 * it connects to the USB and to the USART board connector.
 * - Connect the USART peripheral to the USART interface of the board.
 * - Connect the application to a USB host (e.g. a PC)
 *   with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a virtual COM (see Windows Device Manager).
 * - Open a HyperTerminal on both COM ports (RS232 and Virtual COM)
 * - Select the same configuration for both COM ports up to 115200 baud.
 * - Type a character in one HyperTerminal and it will echo in the other.
 *
 * \note
 * On the first connection of the board on the PC,
 * the operating system will detect a new peripheral:
 * - This will open a new hardware installation window.
 * - Choose "No, not this time" to connect to Windows Update for this installation
 * - click "Next"
 * - When requested by Windows for a driver INF file, select the
 *   atmel_devices_cdc.inf file in the directory indicated in the Atmel Studio
 *   "Solution Explorer" window.
 * - click "Next"
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and CDC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/cdc/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *      <br>
 *    - uart_xmega.c,
 *      <br>implementation of RS232 bridge for XMEGA parts
 *    - uart_uc3.c,
 *      <br>implementation of RS232 bridge for UC3 parts
 *    - uart_sam.c,
 *      <br>implementation of RS232 bridge for SAM parts
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (leds,buttons...)
 */