//
// Created by mcochrane on 18/04/17.
//

extern "C" {
#include <util/delay.h>
}
#include <DS3232SN/DS3232SN.h>
#include <min/min_transmit_cmds.h>
#include "TdlRuleRunner.h"
#include "TdlRules.h"
#include "TdlChannels.h"

static TdlRuleRunner rule_runner;

void TdlRuleRunner::start(DateTime starting_time) {
    rtc_->enablePower();
    rtc_->set(starting_time);
    start();
}

void TdlRuleRunner::start() {
    resetAll();
    rtc_->enablePower();
    start_time_ = getNow();
    running_ = true;
    stepTo(start_time_);
    next_update_time_ = start_time_;
    update();
    rtc_->disablePower();
}

void TdlRuleRunner::stop() {
    resetAll();
    rtc_->enablePower();
    rtc_->alarmInterrupt(1, false);
    rtc_->alarm(1); //Clear interrupt flag
    rtc_->disablePower();
}

void TdlRuleRunner::resetAll() {
    for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
        TdlRule& rule = TdlRules_GetInstance().get(i);
        rule.reset();
    }
    start_time_ = DateTime::Empty();
//    deployment_time_ = deployment_time;
    running_ = false;
}

//DateTime TdlRuleRunner::step() {
//    DateTime next_state_change_time = findOverallNextStateChangeTime();
//    setAllRulesNotRunThisStep();
//    //Not working, need to run the correct order for chains?
//    //Ie, if a rule depends on another rule, you MUST run that rule before running it.
//    //find rules
//    for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
//        TdlRule& rule = TdlRules_GetInstance().get(i);
//        rule.update(next_state_change_time);
//    }
//    next_update_time_ = next_state_change_time;
//
//    return next_state_change_time;
//}

void TdlRuleRunner::setAllRulesNotRunThisStep() {
    for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
        TdlRule& rule = TdlRules_GetInstance().get(i);
        rule.setHasNotBeenRunThisStep();
    }
}


void TdlRuleRunner::stepTo(DateTime now) {
    setAllRulesNotRunThisStep();
    for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
        TdlRule& rule = TdlRules_GetInstance().get(i);
        rule.update(now);
    }
    current_time_ = now;
}

DateTime TdlRuleRunner::findOverallNextStateChangeTime() {
    DateTime min_next = DateTime(0xFFFFFFFF);
//    while (1) {
        for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
            TdlRule &rule = TdlRules_GetInstance().get(i);
            if (rule.isEnabled()) {
                DateTime next = rule.getNextStateChangeTime();
//                char ts[20];
//                next.isotime(ts);
//                report_printf("rule %u/%u nsct = %s", i, TdlRules_GetInstance().getCount(), ts);
//                _delay_ms(10);
//                report_printf("..a");
//            while(1) {
//                char ts[20];
//                next.isotime(ts);
//                report_printf("Rule %u next: %s", i, ts);
//            }
                if (next.isEmpty()) continue;
                if (next < min_next) {
                    min_next = next;
                }
//            } else {
//                report_printf("rule %u/%u not enabled", i, TdlRules_GetInstance().getCount());

            }
        }
//    }
    return min_next;
}

DateTime TdlRuleRunner::findOverallEarliestPreviousStateChangeTime() {
    DateTime min_last = DateTime(0xFFFFFFFF);
    for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
        TdlRule& rule = TdlRules_GetInstance().get(i);
        if (rule.isEnabled()) {
            DateTime last = rule.getLastStateChangeTime();
            if (last.isEmpty()) continue;
            if (last < min_last) {
                min_last = last;
            }
        }
    }
    if (min_last.toTimet() == 0xFFFFFFFF) return DateTime::Empty();
    return min_last;
}

DateTime TdlRuleRunner::getNow() {
    if (rtc_->powerEnabled())
        return rtc_->get();
    else {
        rtc_->enablePower();
        DateTime now = rtc_->get();
        rtc_->disablePower();
        return now;
    }
}

bool TdlRuleRunner::update() {
    current_time_ = getNow();
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0
    report_printf_P(PSTR("Handle Interrupt %s"), current_time_.isotime());
    #endif

    //TODO: handle events! - how to do it?
    if (current_time_ < next_update_time_) {
        #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0
        report_printf_P(PSTR("Not Time Yet!"));
        #endif
        // Clear interrupt flag
//        rtc_->alarm(1);
        // If we're not at the next update time yet just set the
        // interrupt again.
        setRtcInterrupt();
        //TdlChannels_GetInstance().get(0).disable();
        return false;
    }

//    while(1) report_printf("nst = %s", next_state_change_time.isotime());
    setAllRulesNotRunThisStep();
    //Not working, need to run the correct order for chains?
    //Ie, if a rule depends on another rule, you MUST run that rule before running it.
    //find rules
//    for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
    for (int16_t i = TdlRules_GetInstance().getCount()-1; i >= 0; i--) {
        #if defined(DEBUGPRINTS) && DEBUGPRINTS > 1
        report_printf_P(PSTR("urg: %u"), i);
        #endif
        TdlRule& rule = TdlRules_GetInstance().get(i);
        rule.update(current_time_);
    }
    DateTime next_state_change_time = findOverallNextStateChangeTime();
    last_update_time_ = current_time_;
    next_update_time_ = next_state_change_time;

//    rtc_->alarm(1);
    setRtcInterrupt();

//    TdlChannels_GetInstance().get(0).disable();
    return true;
}

/**
 * Sets up the RTC interrupt to wake the device at the next update time
 */
void TdlRuleRunner::setRtcInterrupt() {
    DateTime now = current_time_;//getNow();
    DateTime alarm_time;
    if (now.getMonthOfYear() == next_update_time_.getMonthOfYear() &&
            now.getYear() == next_update_time_.getYear()) {
        alarm_time = next_update_time_;
    } else {
        // Next update is not this month...
        // Set the interrupt to be on the first day of next month, and we'll
        // update the alarm again then.
//        rtc_->setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
//        alarm_time = DateTime(2000, 1, 1, 0, 0, 0);
        alarm_time = now;
        alarm_time.setSecondOfMinute(0);
        alarm_time.setMinuteOfHour(0);
        alarm_time.setDayOfMonth(1);
        alarm_time.addMonths(1);
    }

    //If the RTC is busy, wait.
    //TODO: maybe actually test this..
    while(rtc_->busy());

    cli();
    //clear the alarm if it's active.
    uint8_t en = (uint8_t) rtc_->alarm(1);
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0
    report_printf_P(PSTR("Set Interrupt: %u %s"), en, alarm_time.isotime());
    #endif
    rtc_->setAlarm(ALM1_MATCH_DATE, alarm_time);
    rtc_->alarmInterrupt(1, true);
    rtc_->enablePinInterrupt();
    sei();

//    TdlChannels_GetInstance().get(0).disable();
}

void TdlRuleRunner::reloadRules() {
    TdlRules_GetInstance().loadFromEeprom();
}

void TdlRuleRunner::mainLoopCallback() {
    // The DS3232SN only updates the interrupt pin output value on the
    // 'once-per-second' update so even if we clear it we'll still have
    // our MCU waking up for the whole next second by the interrupt.
    // Here we just check to see if (and to get here the MCU has fired an
    // interrupt) the alarm is still active on the DS3232SN.  We clear the
    // alarm 'active' bit the first time we get an interrupt so if
    // rtc_->alarm(1) is false then we know we're in that weird period when
    // the interrupt is constantly triggering but it's a 'false alarm'
    // pardon the pun.

    if (!pin_interrupt_triggered_) return;

    if (isRunning()) {
        rtc_->enablePower();
        if (rtc_->alarm(1)) {
            update();
        } else {
            #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0
            report_printf_P(PSTR("No Alarm"));
            #endif
        }
        rtc_->disablePower();
    }

    rtc_->enablePinInterrupt();
    pin_interrupt_triggered_ = false;
}

/**
 * Initialises the RuleRunner along with it's dependencies
 * - Initial state is stopped.
 * - Initialises TdlRules (loads rules from eeprom here)
 * - Sets up and initialises RTC
 */
void TdlRuleRunner_Init() {
    initRtc();
    // clear oscillator stopped flag if it's set
    getRtc().oscStopped(true);
    getRtc().set(DateTime(2000, 1, 1, 0, 0, 0));
    TdlRules_Init();
    rule_runner = TdlRuleRunner(getRtc());
}

TdlRuleRunner& TdlRuleRunner_GetInstance() {
    return rule_runner;
}