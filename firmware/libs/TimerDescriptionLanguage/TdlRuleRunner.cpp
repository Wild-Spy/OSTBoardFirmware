//
// Created by mcochrane on 18/04/17.
//

#if defined(TESTING)

#else
#include <delay.h>
#include <DS3232SN/DS3232SN.h>
#endif

#include <min/min_transmit_cmds.h>
#include "TdlRuleRunner.h"
#include "TdlRules.h"
#include "TdlChannels.h"
#include "TdlEvents.h"


static TdlRuleRunner* rule_runner = NULL;

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
    for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
        TdlRule &rule = TdlRules_GetInstance().get(i);
        if (rule.isEnabled()) {
            DateTime next = rule.getNextStateChangeTime();
            if (next.isEmpty()) continue;
            if (next < min_next) {
                min_next = next;
            }
        }
    }
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
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0 && !defined(MCU_TESTING)
    report_printf("Handle Interrupt/event %s", current_time_.isotime());
    #elif defined(MCU_TESTING)
    print_message("Handle Interrupt/event %s\n", current_time_.isotime());
    #endif

    if (current_time_ < next_update_time_ && !TdlEvents_GetInstance().isUnprocessedEvent()) {
        #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0 && !defined(MCU_TESTING)
            report_printf("Not Time Yet!");
        #elif defined(MCU_TESTING)
            print_message("Not Time Yet!\n");
        #endif
        // Clear interrupt flag
//        rtc_->alarm(1);
        // If we're not at the next update time yet just set the
        // interrupt again.
        setRtcInterrupt();
        return false;
    }

    setAllRulesNotRunThisStep();
    //Not working, need to run the correct order for chains?
    //Ie, if a rule depends on another rule, you MUST run that rule before running it.
    //find rules
//    for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
    for (int16_t i = TdlRules_GetInstance().getCount()-1; i >= 0; i--) {
        #if defined(DEBUGPRINTS) && DEBUGPRINTS > 1 && !defined(MCU_TESTING)
        report_printf("urg: %u", i);
        #elif defined(MCU_TESTING) && DEBUGPRINTS > 1
        print_message("urg: %u\n", i);
        #endif
        TdlRule& rule = TdlRules_GetInstance().get(i);
        rule.update(current_time_);
    }

    if (TdlEvents_GetInstance().isUnprocessedEvent()) {
        TdlEvents_GetInstance().setCurrentEventAsProcessed();
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
        alarm_time = DateTime(2000, 1, 1, 0, 0, 0);

//        alarm_time = now;
//        alarm_time.setSecondOfMinute(0);
//        alarm_time.setMinuteOfHour(0);
//        alarm_time.setHourOfDay(0);
//        alarm_time.setDayOfMonth(1);
//        alarm_time = alarm_time.plusMonths(1);
    }

    //If the RTC is busy (doing a temperature conversion), wait.
    //TODO: maybe actually test this..
    //TODO: Should add timeout on this!!!!!!! - nah, the i2c has a timeout already..? hmm
    while(rtc_->busy());

    cpu_irq_enter_critical();
    //clear the alarm if it's active.
    uint8_t en = (uint8_t) rtc_->alarm(1);
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0 && !defined(MCU_TESTING)
        report_printf("Set Interrupt: %u %s", en, alarm_time.isotime());
    #elif defined(MCU_TESTING)
        print_message("Set Interrupt: %u %s\n", en, alarm_time.isotime());
    #endif
    rtc_->setAlarm(ALM1_MATCH_DATE, alarm_time);
    rtc_->alarmInterrupt(1, true);
    rtc_->enablePinInterrupt();
    cpu_irq_leave_critical();

//    TdlChannels_GetInstance().get(0).disable();
}

void TdlRuleRunner::reloadRules() {
    TdlRules_GetInstance().loadFromEeprom();
}

void TdlRuleRunner::mainLoopCallback() {
    if (!pin_interrupt_triggered_ && !TdlEvents_GetInstance().isUnprocessedEvent()) return;

    if (isRunning()) {
        rtc_->enablePower();
        if (rtc_->alarm(1)) {
            update();
        } else if (TdlEvents_GetInstance().isUnprocessedEvent()) {
            update();
        } else {
            #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0 && !defined(MCU_TESTING)
                report_printf("No Alarm");
            #elif defined(MCU_TESTING)
                print_message("No Alarm\n");
            #endif
        }
        rtc_->disablePower();
    }

    rtc_->enablePinInterrupt();
    pin_interrupt_triggered_ = false;
}

static void DS3232SNInterruptCallback(void) {
    TdlRuleRunner_GetInstance().interruptCallback();
}

/**
 * Initialises the RuleRunner along with it's dependencies
 * - Initial state is stopped.
 * - Initialises TdlRules (loads rules from eeprom here)
 * - Sets up and initialises RTC
 */
void TdlRuleRunner_Init(I2c& i2c, NvmRuleManager& ruleManager) {
    initRtc(i2c, DS3232SNInterruptCallback);
    TdlRules_Init(MAX_RULES, ruleManager);
    rule_runner = new TdlRuleRunner(getRtc());
    getRtc().enablePinInterrupt();
}

TdlRuleRunner& TdlRuleRunner_GetInstance() {
    if (rule_runner == NULL) Throw(EX_NULL_POINTER);
    return *rule_runner;
}
