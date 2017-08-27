#ifndef WS_OST_TDLRULE_H
#define WS_OST_TDLRULE_H

#include "gmock/gmock.h"

#include <DateTimeSimpleMock.h>
#include <stdint.h>
//#include <datetime/PeriodInterval.h>
//#include <datetime/Period.h>
//#include <datetime/Interval.h>
//#include <simplib/array.h>
//#include "TdlRuleState.h"
//#include "TdlEvent.h"
//#include "TdlAction.h"

typedef int rule_id_t;

#define RULE_ID_NULL        -1
#define RULE_MAX_INTERVALS  5

class TdlRuleInterface {
public:
    TdlRuleInterface() {};

//    TdlRuleInterface(uint8_t* data, uint8_t len);

    TdlRuleInterface(rule_id_t id, bool enabledOnStart)
        : id_(id),
          enabled_(enabledOnStart),
          enabled_on_start_(enabledOnStart)
    {};

    bool isEnabled() { return enabled_; };
    bool isEnabledOnStart() { return enabled_on_start_; };

    void enable();
    void disable();

    int getId() { return id_; };
//    TdlRuleState_t getOutputState();
//    DateTime getLastStateChangeTime();
//    DateTime getNextStateChangeTime();

//    void update(DateTime now);
    virtual void updateThisRuleAndParentsIfNecessary(DateTime now) {};
//    void updateThisRuleOnly(DateTime now);
    void reset();
//    void setHasNotBeenRunThisStep() { has_been_run_this_step_ = false; };

//    static TdlRule Decompile(uint8_t *data, uint8_t len);

    virtual ~TdlRuleInterface() {};

    virtual void enableCalled() {};
    virtual void disableCalled() {};
    virtual void resetCalled() {};
private:
//    DateTime getLastStateChangeTimeNoEvents();
//    DateTime getNextStateChangeTimeNoEvents();
//    Interval getNextInterval();
//    Interval getLastInterval();
//    Interval getCurrentInterval();
//    void runParentRulesIfNecessary(DateTime now);
//
//    void findParentRules(sl::Array<TdlRule*, 4>* parent_rules);
//
//    void updateCurrentPeriodIntervals();
//    void resetInternalTimeKeepingVariables();
//    DateTime getEndOfCurrentPeriod();
//    void updateInternalTimeKeepingVariables(DateTime now);
//    DateTime getStartOfNextPeriodForEvent(DateTime now);
//    DateTime getStartOfPreviousPeriodForEvent(DateTime now);
//
////    DateTime findNextInList(DateTime now, sl::Array<DateTime, 10>& times);
////    DateTime findPreviousInList(DateTime now, sl::Array<DateTime, 10>& times);
//    void updateInitialStartOfCurrentPeriod(DateTime now);
//
//protected:
//    TdlAction& getAction() { return action_; };
//    sl::Array<PeriodInterval, RULE_MAX_INTERVALS>& getIntervals() { return intervals_; };
//    Period& getPeriod() { return period_; };
//    DateTime getStartOfFirstPeriod() { return start_of_first_period_; };
//    bool isTriggeredByEvent() { return (start_of_first_period_event_id_ != EVENT_ID_NULL); };
//    event_id_t getStartOfFirstPeriodEventId() { return start_of_first_period_event_id_; };
//    Period getStartOfFirstPeriodEventDelay() { return start_of_first_period_event_delay_; };

private:
    int id_;
    //char name[??];
//    TdlAction action_;
//    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals_;
//    Period period_;

    bool enabled_;
    bool enabled_on_start_;

//    DateTime start_of_first_period_;
//    Period start_of_first_period_event_delay_;
//    event_id_t start_of_first_period_event_id_;

//    bool has_been_run_this_step_;

    //Internal time keeping variables
//    sl::Array<Interval, RULE_MAX_INTERVALS> current_intervals_;
//    DateTime start_of_current_period_;
//    DateTime last_update_time_;
};

class TdlRule : public TdlRuleInterface {
public:
    TdlRule()
        : TdlRuleInterface()
    {};

    TdlRule(const TdlRule& r)
            : TdlRuleInterface()
    {};

    TdlRule(rule_id_t id, bool enabledOnStart)
        : TdlRuleInterface(id, enabledOnStart)
    {};

    MOCK_METHOD0(enableCalled, void(void));
    MOCK_METHOD0(disableCalled, void(void));
    MOCK_METHOD1(updateThisRuleAndParentsIfNecessary, void(DateTime now));
    MOCK_METHOD0(resetCalled, void(void));

    /**
     *
     * @param data          an array of bytes to be decompiled into a rule
     * @param len           the length of data
     * @param location      a pointer to the location in memory where the rule should be stored
     * @return a reference to the rule which was created
     */
    static TdlRule& Decompile(uint8_t* data, uint8_t len, TdlRule* location) {
        return *(new (location) TdlRule(data[0], (bool)data[1]));
    }

};

#endif //WS_OST_TDLRULE_H
