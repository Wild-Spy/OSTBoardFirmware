//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_TDLRULE_H
#define WS_OST_TDLRULE_H

#include <datetime/PeriodInterval.h>
#include <datetime/Period.h>
#include <datetime/DateTime.h>
#include <datetime/Interval.h>
#include <simplib/array.h>
#include "TdlRuleState.h"
#include "TdlEvent.h"

#if defined(RULETESTING)
#include <TdlActionMock.h>
#else
#include "TdlAction.h"
#endif

//class TdlAction;

typedef int rule_id_t;

#define RULE_ID_NULL        -1
#define RULE_MAX_INTERVALS  5

class TdlRule {
public:
//    TdlRule() {};

    TdlRule(uint8_t* data, uint8_t len);

//    TdlRule(rule_id_t id, bool enabled_on_start, TdlAction& action, Period period,
//            sl::Array<PeriodInterval, RULE_MAX_INTERVALS>& intervals, DateTime start_of_first_period,
//            Period start_of_first_period_event_delay, event_id_t start_of_first_period_event_id);

    TdlRule(rule_id_t id, bool enabled_on_start, Period period,
            sl::Array<PeriodInterval, RULE_MAX_INTERVALS>& intervals, DateTime start_of_first_period,
            Period start_of_first_period_event_delay, event_id_t start_of_first_period_event_id);

    bool isEnabled() { return enabled_; };

    void enable();
    void disable();

    int getId() { return id_; };
    TdlRuleState_t getOutputState();
    //string getName();
    DateTime getLastStateChangeTime();
    DateTime getNextStateChangeTime();

    void update(DateTime now);
    void updateThisRuleAndParentsIfNecessary(DateTime now);
    void updateThisRuleOnly(DateTime now);
    void reset();
    void setHasNotBeenRunThisStep() { has_been_run_this_step_ = false; };

    static TdlRule Decompile(uint8_t *data, uint8_t len);
    static void Decompile(uint8_t *data, uint8_t len, TdlRule* addressToStoreAt);
    TdlAction& getAction() { return action_; };
    void setAction(TdlAction action) { action_ = action; };

private:
    DateTime getLastStateChangeTimeNoEvents();
    DateTime getNextStateChangeTimeNoEvents();
    Interval getNextInterval();
    Interval getLastInterval();
    Interval getCurrentInterval();
    void runParentRulesIfNecessary(DateTime now);

    void findParentRules(sl::Array<TdlRule*, 4>* parent_rules);

    void updateCurrentPeriodIntervals();
    void resetInternalTimeKeepingVariables();
    DateTime getEndOfCurrentPeriod();
    void updateInternalTimeKeepingVariables(DateTime now);
    DateTime getStartOfNextPeriodForEvent(DateTime now);
    DateTime getStartOfPreviousPeriodForEvent(DateTime now);

//    DateTime findNextInList(DateTime now, sl::Array<DateTime, 10>& times);
//    DateTime findPreviousInList(DateTime now, sl::Array<DateTime, 10>& times);
    void updateInitialStartOfCurrentPeriod(DateTime now);

protected:
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS>& getIntervals() { return intervals_; };
    Period& getPeriod() { return period_; };
    DateTime getStartOfFirstPeriod() { return start_of_first_period_; };
    bool isTriggeredByEvent() { return (start_of_first_period_event_id_ != EVENT_ID_NULL); };
    event_id_t getStartOfFirstPeriodEventId() { return start_of_first_period_event_id_; };
    Period getStartOfFirstPeriodEventDelay() { return start_of_first_period_event_delay_; };

private:
    int id_;
    //char name[??];
    TdlAction action_;
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals_;
    Period period_;

    bool enabled_;
    bool enabled_on_start_;

    DateTime start_of_first_period_;
    Period start_of_first_period_event_delay_;
    event_id_t start_of_first_period_event_id_;

    bool has_been_run_this_step_;

    //Internal time keeping variables
    sl::Array<Interval, RULE_MAX_INTERVALS> current_intervals_;
    DateTime start_of_current_period_;
    DateTime last_update_time_;
};

#endif //WS_OST_TDLRULE_H
