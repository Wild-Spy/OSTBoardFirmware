//
// Created by mcochrane on 21/05/17.
//


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <TimerDescriptionLanguage/TdlRule.h>
#include <TdlRulesMock.h>
#include <time_test.h>
#include <hash_map>

using ::testing::_;
using ::testing::Return;
using ::testing::Sequence;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;
using ::sl::Array;
using std::map;

namespace testing
{
    namespace internal
    {
        enum GTestColor {
            COLOR_DEFAULT,
            COLOR_RED,
            COLOR_GREEN,
            COLOR_YELLOW
        };

        extern void ColoredPrintf(GTestColor color, const char* fmt, ...);
    }
}
#define PRINTF(...)  do { testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] "); testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__); } while(0)

// C++ stream interface
class TestCout : public std::stringstream
{
public:
    ~TestCout()
    {
        PRINTF("%s",str().c_str());
    }
};

#define TEST_COUT  TestCout()

MATCHER_P(EqualsDate, date, "") {
//    if (arg.toTimet() == date.toTimet()) return true;
    if (arg == date) return true;

    char s1[30];
    char s2[30];
    arg.isotime(s1);
    date.isotime(s2);

    *result_listener << s1 << " != " << s2;

    return false;
}

class TdlRuleTestFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        TdlRules_Init();
    }

    virtual void TearDown() {
        TdlRules_Destroy();
    }
};

/**
 * Function for testing transition timing
 * @param rule              the rule being tested
 * @param start_time        the time the device is 'deployed'/started
 * @param condition_map     map of time and state (true=on, false=off)
 */
void test_transitions(TdlRule& rule, DateTime start_time, bool enabled_on_start, map<DateTime, bool> condition_map) {
    Sequence seq;
    TdlAction& action = rule.getAction();

    EXPECT_CALL(action, stop(DateTime()))
            .Times(1)
            .InSequence(seq);
    if (enabled_on_start) {
        EXPECT_CALL(action, start(start_time))
                .Times(1)
                .InSequence(seq);
    } else {
        EXPECT_CALL(action, stop(start_time))
                .Times(1)
                .InSequence(seq);
    }

    for (auto i : condition_map) {
        if (i.second) {
            EXPECT_CALL(action, start(i.first))
                    .Times(1)
                    .InSequence(seq);
        } else {
            EXPECT_CALL(action, stop(i.first))
                    .Times(1)
                    .InSequence(seq);
        }
    }

    rule.reset();
    TEST_COUT << "Rule reset" << std::endl;

    rule.update(start_time);
    TEST_COUT << "Rule update to start time: " << start_time.isotime() << std::endl;

    char nsctStr[20];
    char iFirstStr[20];
    for (auto i : condition_map) {
        DateTime nextStateChangeTime = rule.getNextStateChangeTime();

        nextStateChangeTime.isotime(nsctStr);
        i.first.isotime(iFirstStr);
        TEST_COUT << "Next state change time/expected time: " << nsctStr << " / " << iFirstStr << std::endl;
        EXPECT_THAT(nextStateChangeTime, EqualsDate(i.first));
        rule.setHasNotBeenRunThisStep();
        rule.update(i.first);
    }
}

/**
 * Ensure creating a simple rule (on 30 mins every 2 hours) doesn't fail.
 */
TEST(TdlRuleTestNoFixture, create_rule) {
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(30, PERIODFIELD_MINUTES)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(2, PERIODFIELD_HOURS), intervals, DateTime(), Period(), EVENT_ID_NULL);
}

/**
 * Expect action.start() to be called when we first run rule.update().
 */
TEST_F(TdlRuleTestFixture, expect_on_at_start) {
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(30, PERIODFIELD_MINUTES)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(2, PERIODFIELD_HOURS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction& action = rule.getAction();

    EXPECT_CALL(action, start(DateTime(2000, 1, 1, 0, 0, 0)))
                .Times(1);

    rule.update(DateTime(2000, 1, 1, 0, 0, 0));
}

/**
 * When we call rule.reset(), action.stop() should get called.
 */
TEST_F(TdlRuleTestFixture, reset_calls_stop) {
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(30, PERIODFIELD_MINUTES)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(2, PERIODFIELD_HOURS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction& action = rule.getAction();

    EXPECT_CALL(action, stop(DateTime()))
            .Times(1);

    rule.reset();
}

/**
 * Call rule.update, then rule.reset, then rule.update again.
 * We should see action.start then action.stop, then action.start again.
 */
TEST_F(TdlRuleTestFixture, start_twice) {
    Sequence seq;
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(30, PERIODFIELD_MINUTES)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(2, PERIODFIELD_HOURS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction& action = rule.getAction();

    EXPECT_CALL(action, start(DateTime(2000, 1, 1, 0, 0, 0)))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(action, stop(DateTime()))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(action, start(DateTime(2000, 4, 2, 1, 12, 0)))
            .Times(1)
            .InSequence(seq);

    rule.update(DateTime(2000, 1, 1, 0, 0, 0));
    rule.reset();
    rule.update(DateTime(2000, 4, 2, 1, 12, 0));
}

/**
 * Test transition times of simple rule:
 * Rule on for 30 minutes every 2 hours
 * Start time is 1/1/2000 at 00:00:00
 */
TEST_F(TdlRuleTestFixture, simple_rule_transition_times) {
    Sequence seq;
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(30, PERIODFIELD_MINUTES)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(2, PERIODFIELD_HOURS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction &action = rule.getAction();

    // Transition times
    DateTime start_time = DateTime(2000, 1, 1, 0, 0, 0);

    map<DateTime, bool> map;
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 0, 30, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 2,  0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 2, 30, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 4,  0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 4, 30, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 6,  0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 6, 30, 0), false));

    test_transitions(rule, start_time, true, map);
}

/**
 * Test transition times of another simple rule:
 * Rule on for 5 days every 10 days
 * Start time is 1/1/2000 at 00:00:00
 */
TEST_F(TdlRuleTestFixture, simple_rule_transition_times2) {
    Sequence seq;
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(5, PERIODFIELD_DAYS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(10, PERIODFIELD_DAYS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction &action = rule.getAction();

    // Transition times
    DateTime start_time = DateTime(2000, 1, 1, 0, 0, 0);

    map<DateTime, bool> map;
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1,  6, 0, 0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 11, 0, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 16, 0, 0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 21, 0, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 26, 0, 0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 31, 0, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 2, 05, 0, 0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 2, 10, 0, 0, 0),  true));

    test_transitions(rule, start_time, true, map);
}

/**
 * Test transition times of rule with overflow in seconds in it's interval.
 * Rule on for 125 seconds (2 minutes and 5 seconds) every month.
 * Start time is 1/1/2000 at 00:00:00
 */
TEST_F(TdlRuleTestFixture, rule_transition_times_overflow_interval) {
    Sequence seq;
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(125, PERIODFIELD_SECONDS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(1, PERIODFIELD_MONTHS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction &action = rule.getAction();

    // Transition times
    DateTime start_time = DateTime(2000, 1, 1, 0, 0, 0);

    map<DateTime, bool> map;
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 0, 2, 5), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 2, 1, 0, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 2, 1, 0, 2, 5), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 3, 1, 0, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 3, 1, 0, 2, 5), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 4, 1, 0, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 4, 1, 0, 2, 5), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 5, 1, 0, 0, 0),  true));

    test_transitions(rule, start_time, true, map);
}

/**
 * Testing a rule which has an overflowing number of minutes for it's period.  Ie it has a period of > 60 minutes,
 * specified in minutes.
 * Rule on for 1 hour every 125 minutes (2 hours and 5 minutes)
 */
TEST_F(TdlRuleTestFixture, rule_transition_times_overflow_period) {
    Sequence seq;
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(1, PERIODFIELD_HOURS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(125, PERIODFIELD_MINUTES), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction &action = rule.getAction();

    // Transition times
    DateTime start_time = DateTime(2000, 1, 1, 0, 0, 0);

    map<DateTime, bool> map;
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 1,  0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 2,  5, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 3,  5, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 4, 10, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 5, 10, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 6, 15, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 7, 15, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 8, 20, 0),  true));

    test_transitions(rule, start_time, true, map);
}

/**
 * Test a rule where the interval doesn't get enabled at the start of the period.
 * In this rule, the period is 3 hours and the rule is enabled between the end of the 1st hour and the start of the
 * second hour.
 */
TEST_F(TdlRuleTestFixture, rule_transition_times_offest_interval) {
    Sequence seq;
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::MakeCustomPeriod(1, PERIODFIELD_HOURS), Period::MakeCustomPeriod(2, PERIODFIELD_HOURS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(3, PERIODFIELD_HOURS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction &action = rule.getAction();

    // Transition times
    DateTime start_time = DateTime(2000, 1, 1, 0, 0, 0);

    map<DateTime, bool> map;
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 1, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 2, 0, 0), false));

    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 3, 0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 4, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 5, 0, 0), false));
//
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 6, 0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 7, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 8, 0, 0), false));

    test_transitions(rule, start_time, false, map);
}


/**
 * Test a rule with multiple intervals
 * In this rule, the period is 1 day and the rule is enabled between 9am and 11am, and also between 6pm and 10pm.
 */
TEST_F(TdlRuleTestFixture, rule_transition_times_multiple_intervals) {
    Sequence seq;
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::MakeCustomPeriod( 9, PERIODFIELD_HOURS), Period::MakeCustomPeriod(11, PERIODFIELD_HOURS)));
    intervals.append(PeriodInterval(Period::MakeCustomPeriod(18, PERIODFIELD_HOURS), Period::MakeCustomPeriod(22, PERIODFIELD_HOURS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(1, PERIODFIELD_DAYS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction &action = rule.getAction();

    // Transition times
    DateTime start_time = DateTime(2000, 1, 1, 0, 0, 0);

    map<DateTime, bool> map;
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1,  9, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 11, 0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 18, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 22, 0, 0), false));
//
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2,  0, 0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2,  9, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2, 11, 0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2, 18, 0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2, 22, 0, 0), false));

    test_transitions(rule, start_time, false, map);
}

/**
 * Test a rule with three intervals
 * In this rule, the period is 1 day and the rule is enabled between 9am and 11am, between 1pm and 2pm, and between
 * 8:30pm and 9pm.
 * Also note that the third interval has it's starting time expressed in minutes
 */
TEST_F(TdlRuleTestFixture, rule_transition_times_three_intervals) {
    Sequence seq;
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::MakeCustomPeriod( 9, PERIODFIELD_HOURS), Period::MakeCustomPeriod(11, PERIODFIELD_HOURS)));
    intervals.append(PeriodInterval(Period::MakeCustomPeriod(13, PERIODFIELD_HOURS), Period::MakeCustomPeriod(14, PERIODFIELD_HOURS)));
    intervals.append(PeriodInterval(Period::MakeCustomPeriod(20*60 + 30, PERIODFIELD_MINUTES), Period::MakeCustomPeriod(21, PERIODFIELD_HOURS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(1, PERIODFIELD_DAYS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction &action = rule.getAction();

    // Transition times
    DateTime start_time = DateTime(2000, 1, 1, 0, 0, 0);

    map<DateTime, bool> map;
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1,  9,  0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 11,  0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 13,  0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 14,  0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 20, 30, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 1, 21,  0, 0), false));
//
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2,  0,  0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2,  9,  0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2, 11,  0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2, 13,  0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2, 14,  0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2, 20, 30, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 2, 21,  0, 0), false));

    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 3,  0,  0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 3,  9,  0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 3, 11,  0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 3, 13,  0, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 3, 14,  0, 0), false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 3, 20, 30, 0),  true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 1, 3, 21,  0, 0), false));

    test_transitions(rule, start_time, false, map);
}


/**
 * Test a rule with strange interval
 * In this rule, the period is 3 months day and the rule is enabled between the second month (month 1) and
 * 24*31 + 24*2 + 10 = 802 hours.
 */
TEST_F(TdlRuleTestFixture, rule_transition_times_strange_interval) {
    Sequence seq;
    Array<PeriodInterval, 5> intervals;
    intervals.append(PeriodInterval(Period::MakeCustomPeriod( 1, PERIODFIELD_MONTHS), Period::MakeCustomPeriod(802, PERIODFIELD_HOURS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(3, PERIODFIELD_MONTHS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    TdlAction &action = rule.getAction();

    // Transition times
    DateTime start_time = DateTime(2000, 1, 1, 0, 0, 0);

    map<DateTime, bool> map;
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 2, 1,  0, 0, 0),   true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 2, 3, 10, 0, 0),  false));

    map.insert(std::pair<DateTime, bool>(DateTime(2000, 4, 1,  0, 0, 0),  false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 5, 1,  0, 0, 0),   true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 5, 4, 10, 0, 0),  false));

    map.insert(std::pair<DateTime, bool>(DateTime(2000, 7, 1,  0, 0, 0),  false));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 8, 1,  0, 0, 0),   true));
    map.insert(std::pair<DateTime, bool>(DateTime(2000, 8, 3, 10, 0, 0),  false));

    test_transitions(rule, start_time, false, map);
}

//TEST_F(TdlRulesDefaultInitTestFixture, get_out_of_range_throws_exception) {
//    CEXCEPTION_T e;
//    Try {
//        TdlRules_GetInstance().get(10);
//        FAIL() << "Should have thrown an exception.";
//    } Catch(e) {
//        EXPECT_EQ(e, EX_OUT_OF_RANGE);
//    }
//}
