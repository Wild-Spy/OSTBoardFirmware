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

static I2c* i2c = NULL;
static DS3232SN* rtc;

static volatile bool callback_triggered = false;

static void DS3232SNInterruptTestCallback(void) {
//    print_message("cb\n");
    callback_triggered = true;
    rtc->disablePinInterrupt();
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
    initRtc(*i2c, DS3232SNInterruptTestCallback);
    rtc = &getRtc();
//    delay_s(1);
    return 0;
}

static int teardown(void** state) {
    if (rtc!= NULL) rtc->disablePinInterrupt();
    rtc = NULL;
    i2c->disable();
    i2c = NULL;
    return 0;
}

static void correct_state_on_init(void** state) {
    (void) state; // unused
    // have already run init function

    // Check that we have the right pins setup -> testing the initRtc function here
    assert_int_equal(rtc->npen_pin_.getPort(), 0);
    assert_int_equal(rtc->npen_pin_.getPin(), 19);
    assert_int_equal(rtc->nint_pin_.getPort(), 0);
    assert_int_equal(rtc->nint_pin_.getPin(),  6);

    assert_ptr_equal(rtc->int_callback_, DS3232SNInterruptTestCallback);

    // Initialisation state is described in the comment for DS3232SN::init()
    assert_true(rtc->npen_pin_.getDir() == pin_dir_output);
    assert_true(rtc->npen_pin_.getValue() == false);
    assert_true(rtc->nint_pin_.getDir() == pin_dir_input);
//    assert_true(rtc->nint_pin_.getInputPull() == EXTINT_PULL_UP);
    //TODO: check the interrupt is setup correctly

    uint8_t ctrl;
    ctrl = rtc->i2cReadByte(DSRTC_CONTROL);
    ctrl &= ~((1<<RS2) | (1<<RS1)); //Don't care about these bits
    //nEOSC = 0, BBSQW = 0,CONV,RS2,RS1, INTCN = 1, A2IE = 0, A1IE = 0
    assert_int_equal(ctrl, 0b00000100);

    uint8_t status;
    status = rtc->i2cReadByte(DSRTC_STATUS);
    status &= ~((1<<BSY)); //Don't care about these bits
    //OSF = 0, BB32kHz = 0, CRATE1 = 0, CRATE0 = 0, EN32kHz = 0, BSY, A2F = 0, A1F = 0
    //Note: alarm flags should be 0 because we cleared them in the setup
    assert_int_equal(status, 0b00000000);

    assert_true(rtc->powerEnabled());

    DateTime start_time(2000, 1, 1, 0, 0, 0);
    DateTime rtc_time = rtc->get();
    assert_memory_equal(&rtc_time, &start_time, 4);
}

static void rtc_ticks(void** state) {
    (void) state; // unused

    DateTime time = DateTime(2000, 1, 1, 0, 0, 0);
    DateTime rtc_time;

    for (uint8_t i = 0; i < 5; i++) {
        rtc_time = rtc->get();
        assert_memory_equal(&rtc_time, &time, 4);
        delay_ms(1000);
        time = time.plusSeconds(1);
    }
}

/**
 * Update the time on the rtc only works once the RTC has time to update
 * its internal registers.  Therefore, you must wait some time before
 * you can read back an updated time
 */
static void rtc_set_time_get_time(void** state) {
    (void) state; // unused

    DateTime time = DateTime(2017, 3, 5, 4, 22, 51);
    DateTime rtc_time;

    rtc->set(time);

    rtc_time = rtc->get();
    assert_memory_equal(&rtc_time, &time, 4);
}

/**
 * Tests that enable and disable power works.  Should
 * set the relevant pins, enable/disable the I2C and update
 * the power_enabled_ member variable.
 */
static void rtc_enable_disable_power(void** state) {
    (void) state; // unused

    assert_true(rtc->powerEnabled() == true);

    rtc->disablePower();

    assert_true(rtc->npen_pin_.getDir() == PORT_PIN_DIR_OUTPUT);
    assert_true(rtc->npen_pin_.getValue() == true); // High -> disabled
    assert_true(rtc->powerEnabled() == false);

    rtc->enablePower();

    assert_true(rtc->npen_pin_.getDir() == PORT_PIN_DIR_OUTPUT);
    assert_true(rtc->npen_pin_.getValue() == false); // Low -> enabled
    assert_true(rtc->powerEnabled() == true);
}

/**
 * Tests that a simple alarm gets fired at the correct time.
 * In this case, 2 second from now.
 */
static void rtc_set_simple_alarm1(void** state) {
    (void) state; // unused

    DateTime alarm_time = DateTime(2000, 1, 1, 0, 0, 2);
    DateTime rtc_time;

    rtc->setAlarm(ALM1_MATCH_DATE, alarm_time);

//    rtc->readAlarm();
//    assert_memory_equal(&rtc_time, &time, 4);

    delay_s(2);

    // Make sure the alarm was triggered
    assert_true(rtc->alarm(1));

    // Check the time is right
    rtc_time = rtc->get();
    assert_memory_equal(&rtc_time, &alarm_time, 4);
}

/**
 * Tests that a simple alarm gets fired at the correct time.
 * In this case, 2 second from 'now'.
 * Note: alarm 2 doesn't have a 'seconds' element, only 'minutes'
 */
static void rtc_set_simple_alarm2(void** state) {
    (void) state; // unused

    DateTime time = DateTime(2000, 1, 1, 0, 1, 57);
    DateTime alarm_time = DateTime(2000, 1, 1, 0, 2, 0);
    DateTime rtc_time;

    rtc->set(time);

    rtc->setAlarm(ALM2_MATCH_DATE, alarm_time);

    delay_s(3);

//    uint32_t start_time = time_tick_get();
//    while (!rtc->alarm(2) && time_tick_calc_delay_ms(start_time, time_tick_get()) < 4000);

    // Check the time is right
    rtc_time = rtc->get();
    assert_memory_equal(&rtc_time, &alarm_time, 4);
    assert_int_equal(*((uint32_t*)&rtc_time), *((uint32_t*)&alarm_time));

    // Make sure the alarm was triggered
    assert_true(rtc->alarm(2));

}

/**
 * Tests that interrupts work for a simple alarm
 */
static void rtc_alarm_interrupts(void** state) {
    (void) state; // unused

    DateTime alarm_time = DateTime(2000, 1, 1, 0, 0, 2);
    DateTime rtc_time;
    callback_triggered = false;

    rtc->setAlarm(ALM1_MATCH_DATE, alarm_time);
    rtc->alarmInterrupt(1, true);

    uint32_t start_time = time_tick_get();
//    print_message("start time: %lu\n", start_time);
    rtc->enablePinInterrupt();
    while (!callback_triggered && time_tick_calc_delay_ms(start_time, time_tick_get()) < 3000) {
//        print_message("t: %lu\n", time_tick_calc_delay_ms(start_time, time_tick_get()));
    };
    uint32_t start_time1 = time_tick_get();

    if (!callback_triggered) fail_msg("Callback was not triggered!");
    print_message("Interrupt triggered in %lu ms.\n", time_tick_calc_delay_ms(start_time, time_tick_get()));

    // Make sure the alarm was triggered
    assert_true(rtc->alarm(1));

    // Check the time is right
    rtc_time = rtc->get();
    assert_memory_equal(&rtc_time, &alarm_time, 4);

    // How long does interrupt pin stay pulled low for?
    while (!rtc->nint_pin_.getValue() && time_tick_calc_delay_ms(start_time1, time_tick_get()) < 3000) {
//        print_message("t: %lu\n", time_tick_calc_delay_ms(start_time, time_tick_get()));
    };

    uint32_t interrupt_low_time = time_tick_calc_delay_ms(start_time1, time_tick_get());
    print_message("interrupt low time: %lu ms\n", interrupt_low_time);

    assert_true(interrupt_low_time < 1000);
}

/**
 * Tests that interrupts work when being set every second
 */
static void rtc_alarm_interrupt_every_second(void** state) {
    (void) state; // unused

    DateTime alarm_time = DateTime(2000, 1, 1, 0, 0, 2);
    DateTime rtc_time;


    for (uint8_t i = 0; i < 5; i++) {
        callback_triggered = false;

        rtc->setAlarm(ALM1_MATCH_DATE, alarm_time);
        rtc->alarmInterrupt(1, true);

        uint32_t start_time = time_tick_get();
//        print_message("start time: %lu\n", start_time);
        rtc->enablePinInterrupt();
        while (!callback_triggered && (time_tick_calc_delay_ms(start_time, time_tick_get()) < 3000));
        uint32_t start_time1 = time_tick_get();

        if (!callback_triggered) fail_msg("Callback was not triggered!");
        print_message("Interrupt triggered in %lu ms.\n", time_tick_calc_delay_ms(start_time, time_tick_get()));

        // Make sure the alarm was triggered
        assert_true(rtc->alarm(1));

        // Check the time is right
        rtc_time = rtc->get();
        assert_memory_equal(&rtc_time, &alarm_time, 4);

        // How long does interrupt pin stay pulled low for?
        while ((!rtc->nint_pin_.getValue()) && (time_tick_calc_delay_ms(start_time1, time_tick_get()) < 100));

        uint32_t interrupt_low_time = time_tick_calc_delay_ms(start_time1, time_tick_get());
        print_message("interrupt low time: %lu ms\n", interrupt_low_time);
        assert_true(rtc->nint_pin_.getValue());

        assert_true(interrupt_low_time < 100);
        assert_true(rtc->nint_pin_.getValue());

        //Test interrupt does not fired if we enable it again
        callback_triggered = false;
        rtc->enablePinInterrupt();
        assert_true(rtc->nint_pin_.getValue());

        delay_ms(10);
        assert_true(rtc->nint_pin_.getValue());
        assert_false(callback_triggered);

        alarm_time = alarm_time.plusSeconds(1);
    };

}

/**
 * This test forces a temperature conversion on the DS3232SN and measures it's length.
 * The datasheet says (page 13) 'A user-initiated temperature conversion does not
 * affect the BSY bit for approximately 2ms.' and 'The CONV bit remains at a 1 from
 * the time it is written until the conversion is finished at which time both CONV and
 * BSY go to 0.  The CONV bit should be used when monitoring the status of a user-
 * initiated conversion.'
 */
static void force_conv_measure_time(void** state) {
    assert_false(rtc->busy());
    assert_false(rtc->conv());

    rtc->forceTemperatureConversion();

    uint32_t start_time = time_tick_get();
    assert_true(rtc->conv());

    while (rtc->conv() && (time_tick_calc_delay_ms(start_time, time_tick_get()) < 3000));

    uint32_t time_taken = time_tick_calc_delay_ms(start_time, time_tick_get());

    // Make sure conversion is finished
    assert_false(rtc->conv());
    assert_false(rtc->busy());

    print_message("Temperature conversion time: %lu ms\n", time_taken);
}

/**
 * Determines the time taken to read a single byte register on the DS3232SN.
 */
static void read_reg_time(void** state) {
    uint32_t start_time = time_tick_get();

    // Read 1000 times
    uint16_t reads = 1000;
    for (uint16_t i = 0; i < reads; i++) rtc->conv();

    uint32_t time_taken = time_tick_calc_delay_ms(start_time, time_tick_get());

    print_message("Read CONTROL register time: %f ms\n", time_taken/((float)reads));
}

/**
 * Tests that an alarm still fires if we set the alarm and then change the rtc time (to a time that
 * is still before the alarm but closer to it).
 */
static void alarm_fires_when_time_changed(void** state) {
    (void) state; // unused

    DateTime alarm_time = DateTime(2000, 1, 10, 0, 0, 5);
    DateTime time = DateTime(2000, 1, 10, 0, 0, 4);
    DateTime rtc_time;
    callback_triggered = false;

    //Set the alarm and enable the interrupt
    rtc->setAlarm(ALM1_MATCH_DATE, alarm_time);
    rtc->alarmInterrupt(1, true);

    // Check it doesn't get fired yet
    assert_false(rtc->alarm(1));

    // Move the rtc time closer to the alarm (1 second before)
    rtc->set(time);

    // Check it doesn't get fired yet
    assert_false(rtc->alarm(1));

    // Wait for the alarm (or 2 second timeout)
    uint32_t start_time = time_tick_get();
    callback_triggered = false;
    rtc->enablePinInterrupt();
    while (!callback_triggered && time_tick_calc_delay_ms(start_time, time_tick_get()) < 2000);
    if (!callback_triggered) fail_msg("Callback was not triggered!");
    print_message("Interrupt triggered in %lu ms.\n", time_tick_calc_delay_ms(start_time, time_tick_get()));

    // Make sure the alarm fired (& clear alarm flag)
    assert_true(rtc->alarm(1));

    // Check the time is right
    rtc_time = rtc->get();
    assert_memory_equal(&rtc_time, &alarm_time, 4);
}

/**
 * Tests that an alarm does not fire if we set the alarm and then change the rtc time to a time that
 * is after the alarm time.
 * Didn't think it would work this way.  Was expecting it to trigger the alarm still.
 */
static void alarm_does_not_fire_when_time_changed_to_after(void** state) {
    (void) state; // unused

    DateTime alarm_time = DateTime(2000, 1, 10, 0, 0, 5);
    DateTime time = DateTime(2000, 1, 10, 0, 0, 6);
    DateTime rtc_time;
    callback_triggered = false;

    //Set the alarm
    rtc->setAlarm(ALM1_MATCH_DATE, alarm_time);

    // Check it doesn't get fired yet
    assert_false(rtc->alarm(1));

    // Move the rtc time to 1 second after the alarm time
    rtc->set(time);

    // Make sure the alarm does not fire!  (wait 1 second to verify)
    uint32_t start_time = time_tick_get();
    while (!rtc->alarm(1) && time_tick_calc_delay_ms(start_time, time_tick_get()) < 2000);
    assert_true(time_tick_calc_delay_ms(start_time, time_tick_get()) >= 1000);
}

/**
 * Tests that an alarm does not get fired if we set the alarm and then change the rtc time to that alarm
 * time.
 * Note: This actually makes sense.  The alarm can only be set to sec/min/hour/day not month so when
 *       an alarm is set to a time and the time is >= that, you're really setting the alarm to be triggered
 *       in one month from that time.
 */
static void alarm_does_not_fire_when_time_changed_to_alarm_time(void** state) {
    (void) state; // unused

    DateTime alarm_time = DateTime(2000, 1, 10, 0, 0, 5);
    DateTime rtc_time;
    callback_triggered = false;

    //Set the alarm
    rtc->setAlarm(ALM1_MATCH_DATE, alarm_time);

    // Check it doesn't get fired yet
    assert_false(rtc->alarm(1));

    // Move the rtc time to 1 second after the alarm time
    rtc->set(alarm_time);

    // Make sure the alarm does not fire!  (with 1 second timeout)
    uint32_t start_time = time_tick_get();
    while (!rtc->alarm(1) && time_tick_calc_delay_ms(start_time, time_tick_get()) < 1000);
    assert_true(time_tick_calc_delay_ms(start_time, time_tick_get()) >= 1000);
}

/**
 * Tests that an alarm gets fired if we set the alarm to the current rtc time.
 * Note: Interesting to note that
 *        - if the alarm is set to the current rtc time the alarm FIRES
 *        - if the rtc time is set to an alarm time the alarm DOES NOT FIRE
 */
static void alarm_fires_when_alarm_set_to_rtc_time(void** state) {
    (void) state; // unused

    DateTime time = DateTime(2000, 1, 10, 0, 0, 5);
    DateTime rtc_time;
    callback_triggered = false;

    rtc->set(time);

    delay_s(1);

    time = rtc->get();

    // Check alarm is not active
    assert_false(rtc->alarm(1));

    //Set the alarm
    rtc->setAlarm(ALM1_MATCH_DATE, time);

    // Make sure the alarm fires!  (with 1 second timeout)
    uint32_t start_time = time_tick_get();
    while (!rtc->alarm(1) && time_tick_calc_delay_ms(start_time, time_tick_get()) < 1000);
    assert_true(time_tick_calc_delay_ms(start_time, time_tick_get()) < 1000);
}

/**
 * Tests that an alarm gets fired if the alarm is set to the start of the month and we roll-
 * over to a new month
 */
static void alarm_on_end_of_month(void** state) {
    (void) state; // unused

    DateTime alarm(2000, 1, 1, 0, 0, 0); // start of month (month and year don't matter here)
    DateTime time(2000, 1, 31, 23, 59, 58); // 2 seconds before start of new month
    DateTime rtc_time;

    // Set rtc time
    rtc->set(time);

    // Set the alarm
    rtc->setAlarm(ALM1_MATCH_DATE, alarm);

    // Make sure the alarm fires!  (with 3 second timeout)
    uint32_t start_time = time_tick_get();
    while (!rtc->alarm(1) && time_tick_calc_delay_ms(start_time, time_tick_get()) < 3000);
    assert_true(time_tick_calc_delay_ms(start_time, time_tick_get()) < 3000);

    // Check the time is right
    DateTime alarm_time(2000, 2, 1, 0, 0, 0);
    rtc_time = rtc->get();
    assert_memory_equal(&rtc_time, &alarm_time, 4);
}

/**
 * Tests that alarm interrupt works when the RTC is powered down (battery backup mode)
 */
static void rtc_alarm_interrupts_when_power_disabled(void** state) {
    (void) state; // unused

    DateTime alarm_time = DateTime(2000, 1, 1, 0, 0, 2);
    DateTime rtc_time;
    callback_triggered = false;

    // Set the alarm and interrupt
    rtc->setAlarm(ALM1_MATCH_DATE, alarm_time);
    rtc->alarmInterrupt(1, true);

    // Disable power to RTC
    rtc->disablePower();

    // Enable pin interrupt and wait (3 second timeout)
    uint32_t start_time = time_tick_get();
    rtc->enablePinInterrupt();
    while (!callback_triggered && time_tick_calc_delay_ms(start_time, time_tick_get()) < 3000);
    if (!callback_triggered) fail_msg("Callback was not triggered!");
    print_message("Interrupt triggered in %lu ms.\n", time_tick_calc_delay_ms(start_time, time_tick_get()));

    rtc->enablePower();
    // Make sure the alarm was triggered and clear flag
    assert_true(rtc->alarm(1));

    // Check the time is right
    rtc_time = rtc->get();
    assert_memory_equal(&rtc_time, &alarm_time, 4);
}

/**
 * Tests that reading the time from the rtc does not lock up the mcu if
 * the rtc is powered down.  Should throw an exception instead.
 */
static void read_rtc_time_doesnt_freeze_mcu(void** state) {
    (void) state; // unused
    CEXCEPTION_T e;

    rtc->disablePower();

    Try {
        rtc->get();
        fail_msg("Should have thrown an exception!");
    } Catch(e) {
        assert_int_equal(e, EX_READ_ERROR);
    }
}

const struct CMUnitTest ds3232sn_tests[] = {
        cmocka_unit_test_setup_teardown(rtc_ticks, setup, teardown),
        cmocka_unit_test_setup_teardown(rtc_set_time_get_time, setup, teardown),
        cmocka_unit_test_setup_teardown(rtc_enable_disable_power, setup, teardown),
        cmocka_unit_test_setup_teardown(rtc_set_simple_alarm1, setup, teardown),
        cmocka_unit_test_setup_teardown(rtc_set_simple_alarm2, setup, teardown),
        cmocka_unit_test_setup_teardown(rtc_alarm_interrupts, setup, teardown),
        cmocka_unit_test_setup_teardown(correct_state_on_init, setup, teardown), // make sure we don't run this first.
        cmocka_unit_test_setup_teardown(rtc_alarm_interrupt_every_second, setup, teardown),
        cmocka_unit_test_setup_teardown(force_conv_measure_time, setup, teardown),
        cmocka_unit_test_setup_teardown(read_reg_time, setup, teardown),
        cmocka_unit_test_setup_teardown(alarm_fires_when_time_changed, setup, teardown),
        cmocka_unit_test_setup_teardown(alarm_does_not_fire_when_time_changed_to_after, setup, teardown),
        cmocka_unit_test_setup_teardown(alarm_does_not_fire_when_time_changed_to_alarm_time, setup, teardown),
        cmocka_unit_test_setup_teardown(alarm_fires_when_alarm_set_to_rtc_time, setup, teardown),
        cmocka_unit_test_setup_teardown(alarm_on_end_of_month, setup, teardown),
        cmocka_unit_test_setup_teardown(rtc_alarm_interrupts_when_power_disabled, setup, teardown),
        cmocka_unit_test_setup_teardown(read_rtc_time_doesnt_freeze_mcu, setup, teardown),
};

void run_ds3232sn_tests() {
    CEXCEPTION_T e;
//    CEXCEPTION_FILELINEDEFS;
    char c;

    print_message("Press 'y' to run DS3232SN tests or 'n' to skip... \r\n");


    while(true) {
        scanf("%c", &c);
        if (c == 'y') break;
        else return;
    }

    Try {
//        setup(NULL);
        cmocka_run_group_tests(ds3232sn_tests, NULL, NULL);
//    } CatchFL (e) {
    } Catch (e) {
//        print_error("Threw Exception: %u (%s:%u)\n", e, ex_file, ex_line);
        print_error("Threw Exception: %u\n", e);
    }
}