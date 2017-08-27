//
// Created by mcochrane on 5/04/17.
//

#define __STDC_LIMIT_MACROS
#include <stdint-gcc.h>

#if !defined(TESTING)
#include <min/min_transmit_cmds.h>
#endif

#include "TdlRule.h"


#if defined(TESTING)
#include <delayMock.h>
#include "TdlRulesMock.h"
#include "TdlEventsMock.h"
#else
#include <delay.h>
#include "TdlRules.h"
#include "TdlEvents.h"
#endif

#include <exception/CException.h>
#include <memory/CppNewDeleteOps.h>

#if defined(MCU_TESTING)
#include <cmocka.h>
#endif

void TdlRule::enable() {
    enabled_ = true;
    resetInternalTimeKeepingVariables();
}

void TdlRule::disable() {
    enabled_ = false;
    action_.stop(DateTime());
}

void TdlRule::resetInternalTimeKeepingVariables() {
    start_of_current_period_ = DateTime::Empty();
    current_intervals_.clear();
}

TdlRuleState_t TdlRule::getOutputState() {
    if (action_.getState() == TDLACTIONSTATE_ACTIVE)
        return TDLRULESTATE_ACTIVE;
    else if (action_.getState() == TDLACTIONSTATE_INACTIVE)
        return TDLRULESTATE_INACTIVE;
    return TDLRULESTATE_INACTIVE;
}

#ifndef TESTING
TdlRule TdlRule::Decompile(uint8_t *data, uint8_t len) {
    return TdlRule(data, len);
}
#endif

void TdlRule::Decompile(uint8_t *data, uint8_t len, TdlRule *addressToStoreAt) {
    new (addressToStoreAt) TdlRule(data, len);
}

TdlRule::TdlRule(uint8_t *data, uint8_t len) {
    uint16_t len_so_far = 0;
    uint8_t byte_count;

    // Decompile id
    id_ = *data;
    len_so_far++;
    if (len_so_far > len) Throw(EX_BUFFER_OVERRUN);

    enabled_on_start_ = (bool)*(data+len_so_far);
    len_so_far++;
    if (len_so_far > len) Throw(EX_BUFFER_OVERRUN);

    // Decompile action
//    action_ = TdlAction::Decompile(data+len_so_far);
    TdlAction::Decompile(data+len_so_far, &action_);
    len_so_far += TdlAction::ACTION_COMPILED_LENGTH;
    if (len_so_far > len) Throw(EX_BUFFER_OVERRUN);

    // Decompile period
    period_ = Period::Decompile(data+len_so_far, &byte_count);
    len_so_far += byte_count;
    if (len_so_far > len) Throw(EX_BUFFER_OVERRUN);

    // Decompile intervals
    uint8_t interval_count = *(data+len_so_far);

//    while(1) {
//        report_printf("interval_count = %u", interval_count);
//    }

    len_so_far++;
    for (uint8_t i = 0; i < interval_count; i++) {
        PeriodInterval periodInterval = PeriodInterval::Decompile(data+len_so_far, &byte_count);
        len_so_far += byte_count;
        if (len_so_far > len) Throw(EX_BUFFER_OVERRUN);
        intervals_.append(periodInterval);
    }

    // Decompile start of first period (datetime)
    dt_time_t t = 0;
    t += *(data+len_so_far+0);
    t += *(data+len_so_far+1)<<8;
    t += *(data+len_so_far+2)<<16;
    t += *(data+len_so_far+3)<<24;
//    DateTime ddd(*((dt_time_t*)(data+len_so_far)));
    start_of_first_period_ = DateTime(t);
    len_so_far += sizeof(dt_time_t);
    if (len_so_far > len) Throw(EX_BUFFER_OVERRUN);

    // Decompile start of first period delay
    start_of_first_period_event_delay_ = Period::Decompile(data+len_so_far, &byte_count);
    len_so_far += byte_count;
    if (len_so_far > len) Throw(EX_BUFFER_OVERRUN);

    // Decompile start of start of first period event id (1 byte)
    start_of_first_period_event_id_ = *(data+len_so_far);
    len_so_far++;
    if (len_so_far > len) Throw(EX_BUFFER_OVERRUN);

    if (len_so_far != len) {
        Throw(EX_INVALID_INPUT_VALUE);
    }

    enabled_ = enabled_on_start_;
}

//TdlRule::TdlRule(rule_id_t id, bool enabled_on_start, TdlAction &action, Period period,
//                 sl::Array<PeriodInterval, RULE_MAX_INTERVALS> &intervals, DateTime start_of_first_period,
//                 Period start_of_first_period_event_delay, event_id_t start_of_first_period_event_id) {
//    id_ = id;
//    enabled_on_start_ = enabled_on_start;
//
////    #ifdef TESTING
////    for (int i = 0; i < sizeof(TdlAction); i++) {
////        *(((uint8_t*)&action_)+i) = *(((uint8_t*)&action)+i);
////    }
////    #else
//    action_ = action;
////    #endif
//
//    period_ = period;
//
//    intervals_.clear();
//    for (uint8_t i = 0; i < intervals.getCount(); i++) {
//        intervals_.append(intervals.get(i));
//    }
//
//    start_of_first_period_ = start_of_first_period;
//    start_of_first_period_event_delay_ = start_of_first_period_event_delay;
//    start_of_first_period_event_id_ = start_of_first_period_event_id;
//
//    enabled_ = enabled_on_start_;
//}

TdlRule::TdlRule(rule_id_t id, bool enabled_on_start, Period period,
                 sl::Array<PeriodInterval, RULE_MAX_INTERVALS> &intervals, DateTime start_of_first_period,
                 Period start_of_first_period_event_delay, event_id_t start_of_first_period_event_id) {
    id_ = id;
    enabled_on_start_ = enabled_on_start;

    // For testing, no action provided, it's created on init
    // action_ = action;

    period_ = period;

    intervals_.clear();
    for (uint8_t i = 0; i < intervals.getCount(); i++) {
        intervals_.append(intervals.get(i));
    }

    start_of_first_period_ = start_of_first_period;
    start_of_first_period_event_delay_ = start_of_first_period_event_delay;
    start_of_first_period_event_id_ = start_of_first_period_event_id;

    enabled_ = enabled_on_start_;
}

DateTime TdlRule::getLastStateChangeTime() {
    return getLastStateChangeTimeNoEvents();
}

DateTime TdlRule::getLastStateChangeTimeNoEvents() {
    if (intervals_.getCount() == 0) return DateTime::Empty();

    Interval current_interval = getCurrentInterval();

    //If we're in a PeriodInterval, find when that period interval started.
    if (!current_interval.isEmpty()) {
        //If that Interval started before the current Period then return the start of the Period.
        if (current_interval.getStart() < start_of_current_period_) {
            return start_of_current_period_;
        } else {
            return current_interval.getStart();
        }
    }

    //If we're not in a PeriodInterval, return the end of the last PeriodInterval
    Interval last_interval = getLastInterval();

    if (!last_interval.isEmpty()) {
        //If that PeriodInterval starts before the current Period then return the start of the Period.
        if (last_interval.getEnd() < start_of_current_period_) {
            return start_of_current_period_;
        } else {
            return last_interval.getEnd();
        }
    }

    //Bonus: If we're in a PeriodInterval, and any PeriodInterval starts before this PeriodInterval
    //       ends, then recurse this statement until we're in a PeriodInterval that doesn't have
    //       another one starting within it and return the end of that PeriodInterval.

    //Otherwise it doesn't change during this period.  It may change in a later period though so
    //return the end of the current period.
    return start_of_current_period_;
}

/**
 * Get the interval that starts next (closest start time to lastUpdateTime)
 */
Interval TdlRule::getNextInterval() {
    int32_t min_distance_to_interval = INT32_MAX;
    Interval closest_interval = Interval::Empty();

    //Find next interval
    for (uint8_t i = 0; i < current_intervals_.getCount(); i++) {
        Interval &interval = current_intervals_.getRef(i);
        int32_t distance_to_interval = interval.getStart() - last_update_time_;
        if (distance_to_interval < min_distance_to_interval
            && distance_to_interval > 0) {
            min_distance_to_interval = distance_to_interval;
            closest_interval = interval;
        }
    }

    return closest_interval;
}

/**
 * Gets the interval that starts previous (closest end? time to lastUpdateTime)
 */
Interval TdlRule::getLastInterval() {
    int32_t min_distance_to_interval = INT32_MAX;
    Interval closest_interval = Interval::Empty();

    //Find next interval
    for (uint8_t i = 0; i < current_intervals_.getCount(); i++) {
        Interval &interval = current_intervals_.getRef(i);
        int32_t distance_to_interval = last_update_time_ - interval.getEnd();
        if (distance_to_interval < min_distance_to_interval
            && distance_to_interval > 0) {
            min_distance_to_interval = distance_to_interval;
            closest_interval = interval;
        }
    }

    return closest_interval;
}

Interval TdlRule::getCurrentInterval() {
    for (uint8_t i = 0; i < current_intervals_.getCount(); i++) {
        Interval& interval = current_intervals_.getRef(i);
        //Check if now is between calculated start/stop.
        if (last_update_time_ >= interval.getStart()
            && last_update_time_ < interval.getEnd()) {
            return interval;
        }
    }

    return Interval::Empty();
}

DateTime TdlRule::getNextStateChangeTimeNoEvents() {
    if (intervals_.getCount() == 0) return DateTime::Empty();
    Interval current_interval = getCurrentInterval();

//    while(1) {
//        char ts[20];
//        current_interval.getStart().isotime(ts);
//        report_printf("Rule intvl start: %s", ts);
//        current_interval.getEnd().isotime(ts);
//        report_printf("Rule intvl end: %s", ts);
//    }

    //If we're in a PeriodInterval, find when that period interval ends.
    if (!current_interval.isEmpty()) {
        //If that PeriodInterval ends after the current Period then return the end of the Period.
        DateTime end_of_current_period = getEndOfCurrentPeriod();
        if (current_interval.getEnd() > end_of_current_period) {
            return end_of_current_period;
        } else {
            return current_interval.getEnd();
        }
    }

    //If we're not in a PeriodInterval, return the start of the next PeriodInterval
    Interval next_interval = getNextInterval();

    if (!next_interval.isEmpty()) {
        //If that PeriodInterval ends after the current Period then return the end of the Period.
        DateTime end_of_current_period = getEndOfCurrentPeriod();
        if (next_interval.getStart() > end_of_current_period) {
            return end_of_current_period;
        } else {
            return next_interval.getStart();
        }
    }

    //Bonus: If we're in a PeriodInterval, and any PeriodInterval starts before this PeriodInterval
    //       ends, then recurse this statement until we're in a PeriodInterval that doesn't have
    //       another one starting within it and return the end of that PeriodInterval.

    //Otherwise it doesn't change during this period.  It may change in a later period though so
    //return the end of the current period.
    return getEndOfCurrentPeriod();
}

DateTime TdlRule::getNextStateChangeTime() {
    DateTime retval = getNextStateChangeTimeNoEvents();

    DateTime event_start = DateTime::Empty();
    if (isTriggeredByEvent()) {
        event_start = getStartOfNextPeriodForEvent(last_update_time_);
    }

    if (!event_start.isEmpty() && !retval.isEmpty()) return event_start;
    if (!event_start.isEmpty() && event_start < retval) return event_start;

    return retval;
}

DateTime TdlRule::getEndOfCurrentPeriod() {
    if (start_of_current_period_.isEmpty()) return DateTime::Empty();
    if (period_.isInfinite()) return DateTime::Empty();
    return start_of_current_period_ + period_;
}

void TdlRule::updateCurrentPeriodIntervals() {
    current_intervals_.clear();
    for (uint8_t i = 0; i < intervals_.getCount(); i++) {
        PeriodInterval &period_interval = intervals_.getRef(i);
        current_intervals_.append(period_interval.toInterval(start_of_current_period_));
    }
}

void TdlRule::updateInternalTimeKeepingVariables(DateTime now) {
    if (start_of_current_period_.isEmpty()) {
        updateInitialStartOfCurrentPeriod(now);
        if (!start_of_current_period_.isEmpty())
            updateCurrentPeriodIntervals();
    } else {
        if (isTriggeredByEvent()) {
            DateTime start_of_next = getStartOfNextPeriodForEvent(last_update_time_);
//            while (now >= start_of_next) {
//                start_of_current_period_ = start_of_next;
//                if (start_of_next.isEmpty()) break;
//                start_of_next = getStartOfNextPeriodForEvent(start_of_next);
//            }
            if (!start_of_next.isEmpty())
                start_of_current_period_ = start_of_next;
            updateCurrentPeriodIntervals();
        }
        DateTime end_of_current_period = getEndOfCurrentPeriod();
        if (!start_of_first_period_.isEmpty() && now < start_of_first_period_) {
            action_.stop(now);
        } else if (now < start_of_current_period_) {
            // ie there's a delay
            return;
            //should this really be possible???
            //if (startOfCurrentPeriod.getYear() >= 1000000) return;
            //TODO: handle this!
            Throw(EX_OUT_OF_RANGE);
            //updateCurrentPeriodIntervals
        } else if (now >= end_of_current_period) {
            //TODO: break out/abort if it's taking too long?
            while (now >= end_of_current_period) {
                start_of_current_period_ = end_of_current_period;
                end_of_current_period = start_of_current_period_ + period_;
            }
            updateCurrentPeriodIntervals();
        }
    }
}

void TdlRule::updateInitialStartOfCurrentPeriod(DateTime now) {
    if (!start_of_first_period_.isEmpty()) {
        start_of_current_period_ = start_of_first_period_;
        action_.stop(DateTime::Empty());
        return;
    }

    if (isTriggeredByEvent()) {
//        start_of_current_period_ = getStartOfPreviousPeriodForEvent(now);
        start_of_current_period_ = getStartOfNextPeriodForEvent(now);
        return;
    }

    if (period_.isInfinite()) {
        start_of_current_period_ = now;
        return;
    }

    PeriodFieldType_t period_field_type = period_.getLongestFieldType();

    if (period_field_type == PERIODFIELD_YEARS) {
        start_of_current_period_ = DateTime(now.getYear(), 1, 1, 0, 0, 0);
    } else if (period_field_type == PERIODFIELD_MONTHS) {
        start_of_current_period_ = DateTime(now.getYear(), now.getMonthOfYear(), 1, 0, 0, 0);
    } else if (period_field_type == PERIODFIELD_WEEKS) {
        start_of_current_period_ = DateTime(now.toTimet() - ((uint32_t)now.getSecondOfDay() + (uint32_t)(now.getDayOfWeek() - 1) * ONE_DAY));
    } else if (period_field_type == PERIODFIELD_DAYS) {
        start_of_current_period_ = DateTime(now.getYear(), now.getMonthOfYear(),
                                            now.getDayOfMonth(), 0, 0, 0);
    } else if (period_field_type == PERIODFIELD_HOURS) {
        start_of_current_period_ = DateTime(now.getYear(), now.getMonthOfYear(),
                                            now.getDayOfMonth(), now.getHourOfDay(), 0, 0);
    } else if (period_field_type == PERIODFIELD_MINUTES) {
        start_of_current_period_ = DateTime(now.getYear(), now.getMonthOfYear(),
                                            now.getDayOfMonth(), now.getHourOfDay(),
                                            now.getMinuteOfHour(), 0);
    } else if (period_field_type == PERIODFIELD_SECONDS) {
        start_of_current_period_ = DateTime(now.getYear(), now.getMonthOfYear(),
                                            now.getDayOfMonth(), now.getHourOfDay(),
                                            now.getMinuteOfHour(), now.getSecondOfMinute());
    } else  {
        start_of_current_period_ = now;
    }

//    while(1) {
//        char ts[20];
//        start_of_current_period_.isotime(ts);
//        report_printf("Rule socp (%u, moy: %u) %s", period_field_type, now.getMonthOfYear(), ts);
//        now.isotime(ts);
//        report_printf("Now.. %s", ts);
//    }
}

void TdlRule::reset() {
    resetInternalTimeKeepingVariables();
    action_.stop(DateTime::Empty());
    enabled_ = enabled_on_start_;
    has_been_run_this_step_ = false;
}

void TdlRule::findParentRules(sl::Array<TdlRule *, 4> *parent_rules) {
    parent_rules->clear();

    for (uint8_t i = 0; i < TdlRules_GetInstance().getCount(); i++) {
        TdlRule& r = TdlRules_GetInstance().get(i);
        if (r.getAction().getActivatorType() == ACTIVATOR_TYPE_RULE) {
            TdlActivator* activator = &r.getAction().getActivator();
            TdlRule* rule = activator->getRule();
            if (rule == this) parent_rules->append(&r);
        }
//        if (TdlRuleActivator* rule_activator = dynamic_cast<TdlRuleActivator*>(&r.getAction().getActivator())) {
//            TdlRule& rule = rule_activator->getRule();
//            if (&rule == this) parent_rules->append(&r);
//        }
    }
}

void TdlRule::runParentRulesIfNecessary(DateTime now) {
    sl::Array<TdlRule*, 4> parent_rules;
    findParentRules(&parent_rules);
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 2 && !defined(MCU_TESTING)
    report_printf("Parent Rules(%u): %u", this->id_, parent_rules.getCount());
    #elif defined(MCU_TESTING) && defined(DEBUGPRINTS) && DEBUGPRINTS > 2
    print_message("Parent Rules(%u): %u\n", this->id_, parent_rules.getCount());
    #endif
    for (uint8_t i = 0; i < parent_rules.getCount(); i++) {
        TdlRule& rule = *parent_rules.get(i);
        if (!rule.has_been_run_this_step_) rule.update(now);
    }
}

void TdlRule::update(DateTime now) {
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 2 && !defined(MCU_TESTING)
    report_printf("Update Rule %u (%senabled)", this->id_, isEnabled()?"":"not ");
    #elif defined(MCU_TESTING) && defined(DEBUGPRINTS) && DEBUGPRINTS > 2
    print_message("Update Rule %u (%senabled)\n", this->id_, isEnabled()?"":"not ");
    #endif
    if (isEnabled() && !has_been_run_this_step_) {
        runParentRulesIfNecessary(now);
        updateThisRuleOnly(now);
    }
}

void TdlRule::updateThisRuleAndParentsIfNecessary(DateTime now) {
    runParentRulesIfNecessary(now);
    updateThisRuleOnly(now);
}

void TdlRule::updateThisRuleOnly(DateTime now) {
    bool is_active = false;
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 1 && !defined(MCU_TESTING)
    report_printf("Run Rule %u", this->id_);
    #elif defined(MCU_TESTING) && defined(DEBUGPRINTS) && DEBUGPRINTS > 2
    print_message("Run Rule %u\n", this->id_);
    #endif
//    char nts[20];
//    now.isotime(nts);
    updateInternalTimeKeepingVariables(now);
    for (uint8_t i = 0; i < current_intervals_.getCount(); i++) {
        Interval& interval = current_intervals_.getRef(i);
//        char sts[20];
//        interval.getStart().isotime(sts);
//        report_printf_P(PSTR("Rule %u interval %i = %s to %s (now=%s)"), id_, i,
//                        sts, interval.getEnd().isotime(), nts);
        //Check if now is between calculated start/stop
        if (now >= interval.getStart() && now < interval.getEnd()) {
            is_active = true;
            break;
        }
    }
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 1 && !defined(MCU_TESTING)
    report_printf("Rule %u is %sactive.", id_, is_active?"":"not ");
    #elif defined(MCU_TESTING) && defined(DEBUGPRINTS) && DEBUGPRINTS > 2
    print_message("Rule %u is %sactive.\n", id_, is_active?"":"not ");
    #endif
    // TODO: shouldn't last_update_time_ and has_been_run_this_step_ be updated before setting the action?
    //       The reason for this is, we might otherwise run the rule twice. If the action is a rule then
    //       you will update that rul when you start/stop the action!  If that rule has this rule as a parent
    //       then this rule will be run twice.

    if (is_active) {
        action_.start(now);
    } else {
        action_.stop(now);
    }
    last_update_time_ = now;
    has_been_run_this_step_ = true;
}

DateTime TdlRule::getStartOfNextPeriodForEvent(DateTime now) {
    if (!TdlEvents_GetInstance().isUnprocessedEvent() ||
            TdlEvents_GetInstance().getCurrentEvent().getId() != start_of_first_period_event_id_) {
        //There are no unprocessed events for this rule!
        return DateTime::Empty();
    }

    if (!start_of_first_period_event_delay_.isEmpty()) {
        return TdlEvents_GetInstance().getCurrentEvent().getTime() + start_of_first_period_event_delay_;
    } else {
        return TdlEvents_GetInstance().getCurrentEvent().getTime();
    }

    // TODO: did an event just get triggered???
    // this is weird and needs to be rewritten for the microcontroller.
    // in the java code (on the PC), the events are just stored in a
    // 'simulated event list'.  Here they get generated asynchronously.
    //return DateTime::Empty();
}

DateTime TdlRule::getStartOfPreviousPeriodForEvent(DateTime now) {
    if (TdlEvents_GetInstance().getLastEvent().isEmpty() ||
        TdlEvents_GetInstance().getLastEvent().getId() != start_of_first_period_event_id_) {
        //There are no unprocessed events for this rule!
        return DateTime::Empty();
    }

    if (!start_of_first_period_event_delay_.isEmpty()) {
        return TdlEvents_GetInstance().getLastEvent().getTime() + start_of_first_period_event_delay_;
    } else {
        return TdlEvents_GetInstance().getLastEvent().getTime();
    }

    // find next event
    // TODO: did an event just get triggered???
    // this is weird and needs to be rewritten for the microcontroller.
    // in the java code (on the PC), the events are just stored in a
    // 'simulated event list'.  Here they get generated asynchronously.
    return DateTime::Empty();
}
