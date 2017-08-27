//
// Created by mcochrane on 26/05/17.
//

#include <stdio.h>
#include <cmocka.h>
#include <stdint.h>
#include "DS3232SN/DS3232SN.h"
#include "DS3232SN/DS3232SN_defs.h"
#include <../libs/hw/hal_gpio.h>
#include <exception/CException.h>
#include <delay.h>
#include <time_tick/time_tick.h>
#include <TimerDescriptionLanguage/TdlRuleRunner.h>
#include <TimerDescriptionLanguage/TdlRule.h>
#include <TimerDescriptionLanguage/TdlChannels.h>
#include <tests/mocks/TdlRules.h>
#include <libs/exception/CException.h>
#include <libs/TimerDescriptionLanguage/TdlEvents.h>

static I2c* i2c = NULL;
static DS3232SN* rtc = NULL;
static NvmDriverInterface* eepromDriver = NULL;
static TdlRuleRunner* rr = NULL;

//static volatile bool callback_triggered = false;
static volatile int channel_triggered = -1;

static void chanStateChangeCallback(int chan) {
//    callback_triggered = true;
    channel_triggered = chan;
}

static void i2cInit() {
    struct i2c_master_config i2c_cfg;
    i2c_master_get_config_defaults(&i2c_cfg);
    i2c_cfg.baud_rate = I2C_MASTER_BAUD_RATE_100KHZ;
    i2c_cfg.buffer_timeout = 1000;
    i2c_cfg.pinmux_pad0 = PINMUX_PA16C_SERCOM1_PAD0;
    i2c_cfg.pinmux_pad1 = PINMUX_PA17C_SERCOM1_PAD1;
    i2c_cfg.generator_source = GCLK_GENERATOR_3;
    i2c = new I2c(SERCOM1, &i2c_cfg);
}

static int setup(void** state) {
    i2cInit();
    Pin* channel_pins[4];

    // They're not actually used
    channel_pins[0] = NULL;
    channel_pins[1] = NULL;
    channel_pins[2] = NULL;
    channel_pins[3] = NULL;
//    channel_pins[0] = new Pin(HAL_GPIO_PORTA, 1);
//    channel_pins[1] = new Pin(HAL_GPIO_PORTA, 2);
//    channel_pins[2] = new Pin(HAL_GPIO_PORTA, 3);
//    channel_pins[3] = new Pin(HAL_GPIO_PORTA, 4);

    TdlChannels_Init(4, TDLCHANNELSTATE_DISABLED, channel_pins);
    TdlRuleRunner_Init(*i2c, *((NvmRuleManager*)NULL));  // Very naughty.. Trickery. New TdlRules doesn't care about the RuleManager, just ignores it.
    rr = &TdlRuleRunner_GetInstance();
    rtc = &getRtc();
    return 0;
}

static int teardown(void** state) {
    TdlRules_Destroy();
    TdlChannels_Destroy();
    if (rtc!= NULL) rtc->disablePinInterrupt();
    rtc = NULL;
    i2c->disable();
    i2c = NULL;
    eepromDriver = NULL;
    rr = NULL;
    TdlEvents_ResetInstance();
    return 0;
}

/**
 * Tests basic properties are set correctly on initialisation.
 *
 * Test rtc time is 1/1/2000 at 00:00:00
 * There should be 0 rules in TdlRules_GetInstance()
 * The next wakeup time of the rule runner should be an empty DateTime
 */
static void correct_state_on_init(void** state) {
    (void) state; // unused

    DateTime rtc_now = rr->getNow();
    DateTime expected_now(2000, 1, 1, 0, 0, 0);
    assert_memory_equal(&expected_now, &rtc_now, 4);

    assert_int_equal(TdlRules_GetInstance().getCount(), 0);

    DateTime next_wakeup_time = rr->getNextWakeupTime();
    DateTime expected_next_wakeup_time = DateTime::Empty();
    assert_memory_equal(&next_wakeup_time, &expected_next_wakeup_time, 4);
}

/**
 *  Tests that the next wakeup time of a simple rule (on 1s off 1s) is correct.
 */
static void simple_rule_next_wakeup_time(void** state) {
    (void) state; // unused

    // Build the rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(1, PERIODFIELD_SECONDS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    // Load the rule into the Rules class
    TdlRules_GetInstance().loadInRule(rule);
    assert_int_equal(TdlRules_GetInstance().getCount(), 1);

    // Start the RuleRunner
    DateTime start_time(2000, 1, 1, 0, 0, 0);
    rr->start(start_time);

    // Test that the RTC is set to the correct time
    DateTime rtc_now = rr->getNow();
    assert_memory_equal(&start_time, &rtc_now, 4);

    // Test that the next wakeup time is correct
    DateTime next_wakeup_time = rr->getNextWakeupTime();
    DateTime expected_next_wakeup_time(2000, 1, 1, 0, 0, 1);
    assert_memory_equal(&next_wakeup_time, &expected_next_wakeup_time, 4);
}

void toggle_channel_state(TdlChannelState_t& state) {
    if (state == TDLCHANNELSTATE_DISABLED) {
        state = TDLCHANNELSTATE_ENABLED;
    } else {
        state = TDLCHANNELSTATE_DISABLED;
    }
}

/**
 *  Run a simple rule in the rule runner for a number of periods and ensure the correct
 *  outputs are set at the correct times.
 *  This rule just toggles channel 1 every second starting at 1/1/2000 00:00:00
 */
static void run_simple_rule(void** state) {
    (void) state; // unused

    // Build the rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(1, PERIODFIELD_SECONDS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    // Setup the channel and rule action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction myAction = TdlAction(TDLACTIVATORSTATE_ENABLED, TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    rule.setAction(myAction);

    // Load the rule
    TdlRules_GetInstance().loadInRule(rule);
    assert_int_equal(TdlRules_GetInstance().getCount(), 1);

    // Start the RuleRunner
    DateTime start_time(2000, 1, 1, 0, 0, 0);
    rr->start(start_time);

    // Set up expected variables
    DateTime expected_next_wakeup_time = start_time;
    TdlChannelState_t expected_chan_state = TDLCHANNELSTATE_ENABLED;
    int toggles = 5;

    // Test that the RTC is set to the correct time
    DateTime rtc_now = rr->getNow();
    assert_memory_equal(&start_time, &rtc_now, 4);
    assert_int_equal(chan.getState(), expected_chan_state);

    while(1) {
        rr->mainLoopCallback();

        if (channel_triggered >= 0) {
            // Check the time is right
            rtc_now = rr->getNow();
            assert_memory_equal(&expected_next_wakeup_time, &rtc_now, 4);
            assert_int_equal(chan.getState(), expected_chan_state);
            print_message("Chan State @ t=%lu: %u\n", rtc_now, chan.getState());

            // Update values to what they should be for the next iteration
            toggle_channel_state(expected_chan_state);
            expected_next_wakeup_time = expected_next_wakeup_time.plusSeconds(1);

            // Reset flag
            channel_triggered = -1;

            // Check if we're done
            if (--toggles <= 0) break;
        }
    }
}

/**
 * Runs a RuleRunner through a scenario.
 *
 * Note: the RuleRunner is started at time times.get(0).
 *
 * @tparam ITEMS
 * @param chan          the channel that should be toggled
 * @param times         a list of times that the state changes
 * @param states        the states at those times in the times parameter above
 * @param timeout_ms    the timeout in milliseconds
 */
template <const size_t STATE_CHANGES>
void runRules(TdlChannel& chan, sl::Array<DateTime, STATE_CHANGES>& times, sl::Array<TdlChannelState_t, STATE_CHANGES>& states, uint32_t timeout_ms) {
    size_t current_index = 0;

    // Set up startup and expected variables
    DateTime start_time = times.getRef(current_index);
    DateTime expected_next_wakeup_time = start_time;
    TdlChannelState_t expected_chan_state = states.get(current_index);

    // Start the RuleRunner
    rr->start(start_time);

    // Test the initial conditions
    DateTime rtc_now = rr->getNow();
    assert_memory_equal(&start_time, &rtc_now, 4);
    assert_int_equal(chan.getState(), expected_chan_state);
    if (expected_chan_state == TDLCHANNELSTATE_DISABLED) {
        current_index++;

        // Check if we're done
        if (current_index >= STATE_CHANGES) return;

        // Update values to what they should be for the next iteration
        expected_next_wakeup_time = times.getRef(current_index);
        expected_chan_state = states.get(current_index);
    }

    uint32_t tt_start = time_tick_get();
    while(time_tick_calc_delay_ms(tt_start, time_tick_get()) < timeout_ms) {
        rr->mainLoopCallback();

        if (channel_triggered >= 0) {
            // Ensure extra events aren't triggered.  Wait for timeout before finishing!
            assert_false(current_index >= STATE_CHANGES);

            // Check the right channel was triggered
            assert_int_equal(chan.getId(), channel_triggered);

            // Check the time is right
            rtc_now = rr->getNow();
            assert_memory_equal(&expected_next_wakeup_time, &rtc_now, 4);

            //Check the state is right
            assert_int_equal(chan.getState(), expected_chan_state);
            print_message("Chan State @ t=%lu: %u\n", rtc_now, chan.getState());

            // Reset flag
            channel_triggered = -1;

            // Update event count
            current_index++;

            // Only update if there are state changes left.
            if (current_index < STATE_CHANGES) {
                // Update values to what they should be for the next iteration
                expected_next_wakeup_time = times.getRef(current_index);
                expected_chan_state = states.get(current_index);
            }
        }
    }

    assert_true(current_index == STATE_CHANGES);

//    assert_true(time_tick_calc_delay_ms(tt_start, time_tick_get()) < timeout_ms);
}

/**
 * Runs a RuleRunner through a scenario.
 *
 * Note: the RuleRunner is started at time times.get(0).
 *
 * @tparam ITEMS
 * @param chan          the channel that should be toggled
 * @param times         a list of times that the state changes
 * @param states        the states at those times in the times parameter above
 * @param timeout_ms    the timeout in milliseconds
 */
template <const size_t STATE_CHANGES, const size_t EVENT_COUNT>
void runRulesAndTriggerEvents(TdlChannel& chan,
                              sl::Array<DateTime, STATE_CHANGES>& times,
                              sl::Array<TdlChannelState_t, STATE_CHANGES>& states,
                              sl::Array<TdlEvent, EVENT_COUNT>& events,
                              uint32_t timeout_ms) {
    size_t current_index = 0;

    // Set up startup and expected variables
    DateTime start_time = times.getRef(current_index);
    DateTime expected_next_wakeup_time = start_time;
    TdlChannelState_t expected_chan_state = states.get(current_index);

    // Start the RuleRunner
    rr->start(start_time);

    // Test the initial conditions
    DateTime rtc_now = rr->getNow();
    assert_memory_equal(&start_time, &rtc_now, 4);
    assert_int_equal(chan.getState(), expected_chan_state);
    if (expected_chan_state == TDLCHANNELSTATE_DISABLED) {
        current_index++;

        // Check if we're done
        if (current_index >= STATE_CHANGES) return;

        // Update values to what they should be for the next iteration
        expected_next_wakeup_time = times.getRef(current_index);
        expected_chan_state = states.get(current_index);
    }

    uint32_t tt_start = time_tick_get();
    while(time_tick_calc_delay_ms(tt_start, time_tick_get()) < timeout_ms) {
        rr->mainLoopCallback();

        rtc_now = rr->getNow();
        for (uint8_t ei = 0; ei < events.getCount(); ei++) {
            TdlEvent e = events.getRef(ei);
            if (e.getTime() == rtc_now) {
                TdlEvents_GetInstance().triggerEvent(e);
                print_message("Set Current Event #%u @ t=%lu\n", e.getId(), e.getTime());
                events.remove(ei);
                ei--;
                if (rtc_now == 5) {
                    rtc_now = 50;
                }
            }
        }

        if (channel_triggered >= 0) {
            // Ensure extra events aren't triggered.  Wait for timeout before finishing!
            assert_false(current_index >= STATE_CHANGES);

            // Check the right channel was triggered
            assert_int_equal(chan.getId(), channel_triggered);

            // Check the time is right
            rtc_now = rr->getNow();
            assert_memory_equal(&expected_next_wakeup_time, &rtc_now, 4);

            //Check the state is right
            assert_int_equal(chan.getState(), expected_chan_state);
            print_message("Chan State @ t=%lu: %u\n", rtc_now, chan.getState());

            // Reset flag
            channel_triggered = -1;

            // Update event count
            current_index++;

            // Only update if there are state changes left.
            if (current_index < STATE_CHANGES) {
                // Update values to what they should be for the next iteration
                expected_next_wakeup_time = times.getRef(current_index);
                expected_chan_state = states.get(current_index);
            }
        }
    }

    assert_true(current_index == STATE_CHANGES);

//    assert_true(time_tick_calc_delay_ms(tt_start, time_tick_get()) < timeout_ms);
}


/**
 *  Run a simple rule in the rule runner.  Test moving over a month boundary.
 *  This is important because of how alarms on the DS3232SN work.  You can't a month for the
 *  alarm.
 *  outputs are set at the correct times.
 *  This rule just toggles channel 1 every second starting at 1/1/2000 00:00:00
 */
static void run_rule_over_month_boundary(void** state) {
    (void) state; // unused

    // Build the rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(3, PERIODFIELD_SECONDS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    // Setup the channel and rule action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction myAction = TdlAction(TDLACTIVATORSTATE_ENABLED, TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    rule.setAction(myAction);

    // Load the rule
    TdlRules_GetInstance().loadInRule(rule);
    assert_int_equal(TdlRules_GetInstance().getCount(), 1);

    const size_t item_count = 3;
    sl::Array<DateTime, item_count> times;
    sl::Array<TdlChannelState_t, item_count> states;
    // Start
    times.append(DateTime(2000, 1, 31, 23, 59, 59));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 1
    times.append(DateTime(2000, 2, 1, 0, 0, 1));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 2
    times.append(DateTime(2000, 2, 1, 0, 0, 2));
    states.append(TDLCHANNELSTATE_ENABLED);

    runRules<item_count>(chan, times, states, 3000);
}

/**
 *  Test if a wakeup multiple months away is triggered correctly.
 *  Do some trickery with the rtc to avoid having to actually wait multiple months for this test.
 */
static void run_rule_multiple_months_away(void** state) {
    (void) state; // unused

    // Build the rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(3, PERIODFIELD_MONTHS)));
    TdlRule rule(0, true, Period::MakeCustomPeriod(1, PERIODFIELD_YEARS), intervals, DateTime(), Period(), EVENT_ID_NULL);

    // Setup the channel and rule action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction act = TdlAction(TDLACTIVATORSTATE_ENABLED, TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    rule.setAction(act);

    // Load the rule
    TdlRules_GetInstance().loadInRule(rule);
    assert_int_equal(TdlRules_GetInstance().getCount(), 1);

    // Start the RuleRunner
    DateTime start_time(2000, 1, 1, 0, 0, 0);
    rr->start(start_time);

    // Set up expected variables
    DateTime expected_next_wakeup_time = start_time;  //.plusSeconds(1);
    TdlChannelState_t expected_chan_state = TDLCHANNELSTATE_ENABLED;
    int toggles = 2; // wait for 1 transition (toggles == 2 because channel_triggered is set once on init as well)

    // Test that the RTC is set to the correct time
    DateTime rtc_now = rr->getNow();
    assert_memory_equal(&start_time, &rtc_now, 4);
    assert_int_equal(chan.getState(), expected_chan_state);
//    toggle_channel_state(expected_chan_state);

    while(1) {
        rr->mainLoopCallback();

//        rtc_now = rr->getNow();
//        print_message("t=%lu\n", rtc_now);

        if (rr->getNow() == DateTime(2000, 1, 1, 0, 0, 2)) {
            // Jump from 1/1/2000 to 31/1/2000
            rtc_now = DateTime(2000, 1, 31, 23, 59, 59);
            rtc->enablePower();
            rtc->set(rtc_now);
            rtc->disablePower();
            print_message("jump to t=%lu\n", rtc_now);
        }

        if (rr->getNow() == DateTime(2000, 2, 1, 0, 0, 1)) {
            // Jump from 1/2/2000 to 29/2/2000
            rtc_now = DateTime(2000, 2, 29, 23, 59, 59);
            rtc->enablePower();
            rtc->set(rtc_now);
            rtc->disablePower();
            print_message("jump to t=%lu\n", rtc_now);
        }

        if (rr->getNow() == DateTime(2000, 3, 1, 0, 0, 1)) {
            // Jump from 1/3/2000 to 31/3/2000
            rtc_now = DateTime(2000, 3, 31, 23, 59, 59);
            rtc->enablePower();
            rtc->set(rtc_now);
            rtc->disablePower();
            print_message("jump to t=%lu\n", rtc_now);
        }

        if (channel_triggered >= 0) {
            // Check the time is right
            rtc_now = rr->getNow();
            assert_memory_equal(&expected_next_wakeup_time, &rtc_now, 4);
            assert_int_equal(chan.getState(), expected_chan_state);
            print_message("Chan State @ t=%lu: %u\n", rtc_now, chan.getState());

            DateTime calculated_next_wakeup_time = rr->getNextWakeupTime();
            print_message("Next Wakeup = %lu\n", calculated_next_wakeup_time);

            // Update values to what they should be for the next iteration
            toggle_channel_state(expected_chan_state);
            expected_next_wakeup_time = expected_next_wakeup_time.plusMonths(3); // See 'Build the rule' section -> MakeCustomPeriod

            // Reset flag
            channel_triggered = -1;

            // Check if we're done
            if (--toggles <= 0) break;
        }
    }
}

/**
 *  Run a multi level rule (2 levels)
 */
static void run_multi_level_rule(void** state) {
    (void) state; // unused

    // Build the first rule (on for 1 second every 2 seconds)
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals1;
    intervals1.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(1, PERIODFIELD_SECONDS)));
    TdlRule rule0(0, false, Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS), intervals1, DateTime(), Period(), EVENT_ID_NULL);

    //Build the second rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals2;
    intervals2.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(5, PERIODFIELD_SECONDS)));
    TdlRule rule1(1, true, Period::MakeCustomPeriod(10, PERIODFIELD_SECONDS), intervals2, DateTime(), Period(), EVENT_ID_NULL);

    // Load the rules
    TdlRules_GetInstance().loadInRule(rule0);
    TdlRules_GetInstance().loadInRule(rule1);
    assert_int_equal(TdlRules_GetInstance().getCount(), 2);

    // Setup the channel and rule0 action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction act1 = TdlAction(TDLACTIVATORSTATE_ENABLED, TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(0)
            .setAction(act1);

    // Setup rule1 action
    TdlAction act2 = TdlAction(TDLACTIVATORSTATE_ENABLED,
                               TdlActivator(&TdlRules_GetInstance().get(0), TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(1)
            .setAction(act2);


    const size_t item_count = 8;
    sl::Array<DateTime, item_count> times;
    sl::Array<TdlChannelState_t, item_count> states;
    // Start
    times.append(DateTime(2000, 1, 1, 0, 0, 0));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 1
    times.append(DateTime(2000, 1, 1, 0, 0, 1));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 2
    times.append(DateTime(2000, 1, 1, 0, 0, 2));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 3
    times.append(DateTime(2000, 1, 1, 0, 0, 3));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 4
    times.append(DateTime(2000, 1, 1, 0, 0, 4));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 5
    times.append(DateTime(2000, 1, 1, 0, 0, 5));
    states.append(TDLCHANNELSTATE_DISABLED);

    //Then long break...

    // Transition 6
    times.append(DateTime(2000, 1, 1, 0, 0, 10));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 7
    times.append(DateTime(2000, 1, 1, 0, 0, 11));
    states.append(TDLCHANNELSTATE_DISABLED);
    //etc.

    runRules<item_count>(chan, times, states, 11500);
}

/**
 *  Run a multi level rule (2 levels)
 *  The first rule has infinite period
 *  A: enable channel 1 for 2 seconds
 *  B: enable rule a on minute 20 every hour
 *
 *  Was having a problem with this kind of rule.  The channel would just stay on forever
 *  after the first activation.  I think it's because it was adding the infinite period
 *  which just overflows and the ends up in a different year/month so next alarm time is
 *  set to the start of the next month.  I think.  It only seems to fail with certain
 *  dates/times.  For example starting at 1/1/2017 didn't fail, then on 13/8/2017 did
 *  fail.  Anyway, adding the line
 *    if (period_.isInfinite()) return DateTime::Empty();
 *  into the function TdlRule::getEndOfCurrentPeriod() fixed the problem and now this
 *  test passes.
 */
static void run_multi_level_rule_first_rule_infinite(void** state) {
    (void) state; // unused

    // Build the first rule (on for 2 seconds)
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals1;
    intervals1.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS)));
    TdlRule rule0(0, false, Period::Infinite(), intervals1, DateTime(), Period(), EVENT_ID_NULL);

    //Build the second rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals2;
    intervals2.append(PeriodInterval(Period::MakeCustomPeriod(20, PERIODFIELD_MINUTES), Period::MakeCustomPeriod(21, PERIODFIELD_MINUTES)));
    TdlRule rule1(1, true, Period::MakeCustomPeriod(1, PERIODFIELD_HOURS), intervals2, DateTime(), Period(), EVENT_ID_NULL);

    // Load the rules
    TdlRules_GetInstance().loadInRule(rule0);
    TdlRules_GetInstance().loadInRule(rule1);
    assert_int_equal(TdlRules_GetInstance().getCount(), 2);

    // Setup the channel and rule0 action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction act1 = TdlAction(TDLACTIVATORSTATE_ENABLED,
                               TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(0).setAction(act1);

    // Setup rule1 action
    TdlAction act2 = TdlAction(TDLACTIVATORSTATE_ENABLED,
                               TdlActivator(&TdlRules_GetInstance().get(0), TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(1).setAction(act2);


    const size_t item_count = 3;
    sl::Array<DateTime, item_count> times;
    sl::Array<TdlChannelState_t, item_count> states;
    // Start
    times.append(DateTime(2017, 8, 13, 1, 19, 58));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 1
    times.append(DateTime(2017, 8, 13, 1, 20, 0));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 2
    times.append(DateTime(2017, 8, 13, 1, 20, 2));
    states.append(TDLCHANNELSTATE_DISABLED);

    runRules<item_count>(chan, times, states, 11500);
}

/**
 *  Run a multi level rule (3 levels)
 *  This is quite a long test! (18 seconds)
 *  Equivalent to the below rules in the Java App:
 *  a: enable channel 1 for 1 second every 2 seconds
 *  b: enable rule a for 4 seconds every 8 seconds
 *  C: enable rule b for 12 seconds every 15 seconds
 */
static void run_multi_level_rule_three_levels(void** state) {
    (void) state; // unused

    // Build the first rule (on for 1 second every 2 seconds)
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals1;
    intervals1.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(1, PERIODFIELD_SECONDS)));
    TdlRule rule0(0, false, Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS), intervals1, DateTime(), Period(), EVENT_ID_NULL);

    //Build the second rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals2;
    intervals2.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(4, PERIODFIELD_SECONDS)));
    TdlRule rule1(1, false, Period::MakeCustomPeriod(8, PERIODFIELD_SECONDS), intervals2, DateTime(), Period(), EVENT_ID_NULL);

    //Build the second rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals3;
    intervals3.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(12, PERIODFIELD_SECONDS)));
    TdlRule rule2(2, true, Period::MakeCustomPeriod(15, PERIODFIELD_SECONDS), intervals3, DateTime(), Period(), EVENT_ID_NULL);

    // Load the rules
    TdlRules_GetInstance().loadInRule(rule0);
    TdlRules_GetInstance().loadInRule(rule1);
    TdlRules_GetInstance().loadInRule(rule2);
    assert_int_equal(TdlRules_GetInstance().getCount(), 3);

    // Setup the channel and rule0 action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction act1 = TdlAction(TDLACTIVATORSTATE_ENABLED,
                               TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(0).setAction(act1);

    // Setup rule1 action
    TdlAction act2 = TdlAction(TDLACTIVATORSTATE_ENABLED,
                               TdlActivator(&TdlRules_GetInstance().get(0), TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(1).setAction(act2);

    // Setup rule2 action
    TdlAction act3 = TdlAction(TDLACTIVATORSTATE_ENABLED,
                               TdlActivator(&TdlRules_GetInstance().get(1), TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(2).setAction(act3);


    const size_t item_count = 12;
    sl::Array<DateTime, item_count> times;
    sl::Array<TdlChannelState_t, item_count> states;
    // Start
    times.append(DateTime(2000, 1, 1, 0, 0, 0));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 1
    times.append(DateTime(2000, 1, 1, 0, 0, 1));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 2
    times.append(DateTime(2000, 1, 1, 0, 0, 2));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 3
    times.append(DateTime(2000, 1, 1, 0, 0, 3));
    states.append(TDLCHANNELSTATE_DISABLED);

    //Then long break...

    // Transition 4
    times.append(DateTime(2000, 1, 1, 0, 0, 8));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 5
    times.append(DateTime(2000, 1, 1, 0, 0, 9));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 6
    times.append(DateTime(2000, 1, 1, 0, 0, 10));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 7
    times.append(DateTime(2000, 1, 1, 0, 0, 11));
    states.append(TDLCHANNELSTATE_DISABLED);

    //Then long break...

    // Transition 8
    times.append(DateTime(2000, 1, 1, 0, 0, 15));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 9
    times.append(DateTime(2000, 1, 1, 0, 0, 16));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 10
    times.append(DateTime(2000, 1, 1, 0, 0, 17));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 11
    times.append(DateTime(2000, 1, 1, 0, 0, 18));
    states.append(TDLCHANNELSTATE_DISABLED);
    //etc.

    runRules<item_count>(chan, times, states, 20000);
}

/**
 *  Test if a rule triggered by an event works.
 *  Rule: enable chan0 for 2 seconds starting on event 1
 */
static void event_triggered_rule(void** state) {
    (void) state; // unused
    const uint8_t EVENT_ID = 1;

    // Build the rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS)));
    TdlRule rule(0, true, Period::Infinite(), intervals, DateTime(), Period(), EVENT_ID);

    // Setup the channel and rule action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction act = TdlAction(TDLACTIVATORSTATE_ENABLED, TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    rule.setAction(act);

    // Load the rule
    TdlRules_GetInstance().loadInRule(rule);
    assert_int_equal(TdlRules_GetInstance().getCount(), 1);

    const size_t event_count = 2;
    const size_t item_count = event_count*2+1;
    sl::Array<DateTime, item_count> times;
    sl::Array<TdlChannelState_t, item_count> states;
    sl::Array<TdlEvent, event_count> events;

    // Start
    times.append(DateTime(2000, 1, 1, 0, 0, 0));
    states.append(TDLCHANNELSTATE_DISABLED);

    // Event
    events.append(TdlEvent(EVENT_ID, DateTime(2000, 1, 1, 0, 0, 2)));
    // Transition 1
    times.append(DateTime(2000, 1, 1, 0, 0, 2));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 2
    times.append(DateTime(2000, 1, 1, 0, 0, 4));
    states.append(TDLCHANNELSTATE_DISABLED);

    // Event
    events.append(TdlEvent(EVENT_ID, DateTime(2000, 1, 1, 0, 0, 5)));
    // Transition 3
    times.append(DateTime(2000, 1, 1, 0, 0, 5));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 4
    times.append(DateTime(2000, 1, 1, 0, 0, 7));
    states.append(TDLCHANNELSTATE_DISABLED);


    runRulesAndTriggerEvents(chan, times, states, events, 10000);
}

/**
 *  Test if a rule triggered by an event works.
 *  Rule: enable chan0 for 2 seconds starting 1 second after event 1
 */
static void event_triggered_rule_with_delay(void** state) {
    (void) state; // unused
    const uint8_t EVENT_ID = 1;

    // Build the rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals;
    intervals.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS)));
    TdlRule rule(0, true, Period::Infinite(), intervals, DateTime(), Period::MakeCustomPeriod(1, PERIODFIELD_SECONDS), EVENT_ID);

    // Setup the channel and rule action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction act = TdlAction(TDLACTIVATORSTATE_ENABLED, TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    rule.setAction(act);

    // Load the rule
    TdlRules_GetInstance().loadInRule(rule);
    assert_int_equal(TdlRules_GetInstance().getCount(), 1);

    const size_t event_count = 2;
    const size_t item_count = event_count*2+1;
    sl::Array<DateTime, item_count> times;
    sl::Array<TdlChannelState_t, item_count> states;
    sl::Array<TdlEvent, event_count> events;

    // Start
    times.append(DateTime(2000, 1, 1, 0, 0, 0));
    states.append(TDLCHANNELSTATE_DISABLED);

    // Event
    events.append(TdlEvent(EVENT_ID, DateTime(2000, 1, 1, 0, 0, 2)));
    // Transition 1
    times.append(DateTime(2000, 1, 1, 0, 0, 3));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 2
    times.append(DateTime(2000, 1, 1, 0, 0, 5));
    states.append(TDLCHANNELSTATE_DISABLED);

    // Event
    events.append(TdlEvent(EVENT_ID, DateTime(2000, 1, 1, 0, 0, 5)));
    // Transition 3
    times.append(DateTime(2000, 1, 1, 0, 0, 6));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 4
    times.append(DateTime(2000, 1, 1, 0, 0, 8));
    states.append(TDLCHANNELSTATE_DISABLED);


    runRulesAndTriggerEvents(chan, times, states, events, 10000);
}

/**
 *  Test if a multi level rule triggered by an event works.
 *  a: enable chan0 for 1 second every 2 seconds
 *  B: enable rule a for 5 seconds starting on event 1
 */
static void event_triggered_multi_level_rule(void** state) {
    (void) state; // unused
    const uint8_t EVENT_ID = 1;

    // Build the first rule (on for 1 second every 2 seconds)
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals1;
    intervals1.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(1, PERIODFIELD_SECONDS)));
    TdlRule rule0(0, false, Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS), intervals1, DateTime(), Period(), EVENT_ID_NULL);

    //Build the second rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals2;
    intervals2.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(5, PERIODFIELD_SECONDS)));
    TdlRule rule1(1, true, Period::MakeCustomPeriod(10, PERIODFIELD_SECONDS), intervals2, DateTime(), Period(), EVENT_ID);

    // Load the rules
    TdlRules_GetInstance().loadInRule(rule0);
    TdlRules_GetInstance().loadInRule(rule1);
    assert_int_equal(TdlRules_GetInstance().getCount(), 2);

    // Setup the channel and rule0 action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction act1 = TdlAction(TDLACTIVATORSTATE_ENABLED,
                               TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(0).setAction(act1);

    // Setup rule1 action
    TdlAction act2 = TdlAction(TDLACTIVATORSTATE_ENABLED,
                               TdlActivator(&TdlRules_GetInstance().get(0), TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(1).setAction(act2);


    const size_t event_count = 2;
    const size_t item_count = 13;
    sl::Array<DateTime, item_count> times;
    sl::Array<TdlChannelState_t, item_count> states;
    sl::Array<TdlEvent, event_count> events;

    // Start
    times.append(DateTime(2000, 1, 1, 0, 0, 0));
    states.append(TDLCHANNELSTATE_DISABLED);

    // Event
    events.append(TdlEvent(EVENT_ID, DateTime(2000, 1, 1, 0, 0, 2)));
    // Transition 1
    times.append(DateTime(2000, 1, 1, 0, 0, 2));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 2
    times.append(DateTime(2000, 1, 1, 0, 0, 3));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 3
    times.append(DateTime(2000, 1, 1, 0, 0, 4));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 4
    times.append(DateTime(2000, 1, 1, 0, 0, 5));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 5
    times.append(DateTime(2000, 1, 1, 0, 0, 6));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 6
    times.append(DateTime(2000, 1, 1, 0, 0, 7));
    states.append(TDLCHANNELSTATE_DISABLED);

    // Event
    events.append(TdlEvent(EVENT_ID, DateTime(2000, 1, 1, 0, 0, 11)));
    // Transition 7
    times.append(DateTime(2000, 1, 1, 0, 0, 11));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 8
    times.append(DateTime(2000, 1, 1, 0, 0, 12));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 9
    times.append(DateTime(2000, 1, 1, 0, 0, 13));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 10
    times.append(DateTime(2000, 1, 1, 0, 0, 14));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 11
    times.append(DateTime(2000, 1, 1, 0, 0, 15));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 12
    times.append(DateTime(2000, 1, 1, 0, 0, 16));
    states.append(TDLCHANNELSTATE_DISABLED);


    runRulesAndTriggerEvents(chan, times, states, events, 18000);
}

/**
 *  Test if a multi level rule triggered by an event works.
 *  a: enable chan0 for 1 second every 2 seconds
 *  B: enable rule a for 5 seconds starting on event 1
 *  But we arrange the event times such that the second event is triggered
 *  before the first event finishes.
 */
static void overlapping_event_triggered_multi_level_rules(void** state) {
    (void) state; // unused
    const uint8_t EVENT_ID = 1;

    // Build the first rule (on for 1 second every 2 seconds)
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals1;
    intervals1.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(1, PERIODFIELD_SECONDS)));
    TdlRule rule0(0, false, Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS), intervals1, DateTime(), Period(), EVENT_ID_NULL);

    //Build the second rule
    sl::Array<PeriodInterval, RULE_MAX_INTERVALS> intervals2;
    intervals2.append(PeriodInterval(Period::Empty(), Period::MakeCustomPeriod(5, PERIODFIELD_SECONDS)));
    TdlRule rule1(1, true, Period::MakeCustomPeriod(10, PERIODFIELD_SECONDS), intervals2, DateTime(), Period(), EVENT_ID);

    // Load the rules
    TdlRules_GetInstance().loadInRule(rule0);
    TdlRules_GetInstance().loadInRule(rule1);
    assert_int_equal(TdlRules_GetInstance().getCount(), 2);

    // Setup the channel and rule0 action
    TdlChannel& chan = TdlChannels_GetInstance().get(0);
    chan.setStateChangeCallback(chanStateChangeCallback);
    TdlAction act1(TDLACTIVATORSTATE_ENABLED,
              TdlActivator(&chan, TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(0).setAction(act1);

    // Setup rule1 action
    TdlAction act2(TDLACTIVATORSTATE_ENABLED,
              TdlActivator(&TdlRules_GetInstance().get(0), TDLACTIVATORSTATE_DISABLED));
    TdlRules_GetInstance().get(1).setAction(act2);


    const size_t event_count = 2;
    const size_t item_count = 9;
    sl::Array<DateTime, item_count> times;
    sl::Array<TdlChannelState_t, item_count> states;
    sl::Array<TdlEvent, event_count> events;

    // Start
    times.append(DateTime(2000, 1, 1, 0, 0, 0));
    states.append(TDLCHANNELSTATE_DISABLED);

    // Event
    events.append(TdlEvent(EVENT_ID, DateTime(2000, 1, 1, 0, 0, 2)));
    // Transition 1
    times.append(DateTime(2000, 1, 1, 0, 0, 2));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 2
    times.append(DateTime(2000, 1, 1, 0, 0, 3));
    states.append(TDLCHANNELSTATE_DISABLED);

    // Event
    events.append(TdlEvent(EVENT_ID, DateTime(2000, 1, 1, 0, 0, 4)));
    // Transition 3
    times.append(DateTime(2000, 1, 1, 0, 0, 4));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 4
    times.append(DateTime(2000, 1, 1, 0, 0, 5));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 5
    times.append(DateTime(2000, 1, 1, 0, 0, 6));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 6
    times.append(DateTime(2000, 1, 1, 0, 0, 7));
    states.append(TDLCHANNELSTATE_DISABLED);
    // Transition 7
    times.append(DateTime(2000, 1, 1, 0, 0, 8));
    states.append(TDLCHANNELSTATE_ENABLED);
    // Transition 8
    times.append(DateTime(2000, 1, 1, 0, 0, 9));
    states.append(TDLCHANNELSTATE_DISABLED);


    runRulesAndTriggerEvents(chan, times, states, events, 11000);
}

//TODO: test events

const struct CMUnitTest rule_runner_tests[] = {
        cmocka_unit_test_setup_teardown(correct_state_on_init, setup, teardown),
        cmocka_unit_test_setup_teardown(simple_rule_next_wakeup_time, setup, teardown),
        cmocka_unit_test_setup_teardown(run_simple_rule, setup, teardown),
        cmocka_unit_test_setup_teardown(run_rule_over_month_boundary, setup, teardown),
        cmocka_unit_test_setup_teardown(run_rule_multiple_months_away, setup, teardown),
        cmocka_unit_test_setup_teardown(run_multi_level_rule, setup, teardown),
        cmocka_unit_test_setup_teardown(run_multi_level_rule_first_rule_infinite, setup, teardown),
        cmocka_unit_test_setup_teardown(run_multi_level_rule_three_levels, setup, teardown),
        cmocka_unit_test_setup_teardown(event_triggered_rule, setup, teardown),
        cmocka_unit_test_setup_teardown(event_triggered_rule_with_delay, setup, teardown),
        cmocka_unit_test_setup_teardown(event_triggered_multi_level_rule, setup, teardown),
        cmocka_unit_test_setup_teardown(overlapping_event_triggered_multi_level_rules, setup, teardown),
};

const struct CMUnitTest rule_runner_tests_last[] = {
        cmocka_unit_test_setup_teardown(run_multi_level_rule_first_rule_infinite, setup, teardown),
};

void run_rule_runner_tests() {
    CEXCEPTION_T e;
    CEXCEPTION_FILELINEDEFS;
    char c;

    print_message("Press 'y' to run Rule Runner tests or 'n' to skip or 'l' to run only the last test... \r\n");

    while(true) {
        scanf("%c", &c);
        if (c == 'y') break;
        if (c == 'l') break;
        else return;
    }

    Try {
        if (c == 'y') {
            cmocka_run_group_tests(rule_runner_tests, NULL, NULL);
        } else if (c == 'l') {
            cmocka_run_group_tests(rule_runner_tests_last, NULL, NULL);
        }
    } CatchFL (e) {
//    } Catch (e) {
        print_error("Threw Exception: %u (%s:%u)\n", e, ex_file, ex_line);
//        print_error("Threw Exception: %u\n", e);
    }
}