//
// Created by mcochrane on 18/04/17.
//

#ifndef WS_OST_TDLRULERUNNER_H
#define WS_OST_TDLRULERUNNER_H

#include "DS3232SN/DS3232SN.h"
#include <datetime/DateTime.h>
#include <libs/nvm/NvmRuleManager.h>

#define MAX_RULES   3

class TdlRuleRunner {
public:
    TdlRuleRunner()
            : rtc_(NULL),
              running_(false),
              pin_interrupt_triggered_(false)
    {};

    TdlRuleRunner(DS3232SN& rtc)
            : rtc_(&rtc),
              running_(false),
              pin_interrupt_triggered_(false)
    {};

    /**
     * Start using the current RTC time.
     */
    void start();

    /**
     * Set the current RTC time to starting_time and start.
     * @param starting_time     the current time
     */
    void start(DateTime starting_time);

    /**
     * Returns the current time based on the RTC.
     */
    DateTime getNow();

    bool isRunning() { return running_; };

    /**
     * Stops the rule runner.
     */
    void stop();

    /**
     * Call this function in the main loop of your program
     */
    void mainLoopCallback();

    /**
     * Execute this function within an the MCU alarm pin interrupt
     * For example:
     *   ISR(PORTE_INT0_vect) {
     *       TdlRuleRunner_GetInstance().interruptCallback();
     *   }
     */
    inline void interruptCallback() {
        pin_interrupt_triggered_ = true;
        rtc_->disablePinInterrupt();
        //note that the pin will stay low until rtc_->alarm(?) is called for the specified alarm
        //ie you need to clear the interrupt flag on the rtc!!
    };

    /**
     * Reloads all rules from EEPROM.
     */
    void reloadRules();

    /**
     * Check to see if the module has been initialised yet...
     */
    bool isInitialised() { return rtc_ != NULL; };

    /**
     * @return  the time when the device was started?
     */
    DateTime getStartTime() { return start_time_; };
    DateTime getNextWakeupTime() { return next_update_time_; };
    DateTime getCurrentTime() { return current_time_; };

private:
    bool update();
    void setRtcInterrupt();

    void resetAll();
//    DateTime step();

//    void disableAll();

    void stepTo(DateTime now);
    void setAllRulesNotRunThisStep();
    DateTime findOverallNextStateChangeTime();
    DateTime findOverallEarliestPreviousStateChangeTime();

private:
    DS3232SN* rtc_;
    DateTime start_time_;
    bool running_;

    DateTime current_time_;
//    DateTime deployment_time_;
    DateTime last_update_time_;
    DateTime next_update_time_;

    bool pin_interrupt_triggered_;
};

void TdlRuleRunner_Init(I2c& i2c, NvmRuleManager& ruleManager);
TdlRuleRunner& TdlRuleRunner_GetInstance();

#endif //WS_OST_TDLRULERUNNER_H
