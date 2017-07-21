/**
 * Matthew Cochrane
 * 14/7/2017
 */

#include "time_tick.h"
#include <clock/gclk.h>
#include <samd21.h>
#include <tc.h>
#include <tc_interrupt.h>

/** Counts for 1ms time ticks. */
static volatile uint32_t g_ms_ticks = 0;
static struct tc_module tc_instance;

#define TIMER_INTERVAL_US 1000 // HW timer interval in us

/**
 * \brief Callback for TC interrupt
 */
static void tc_callback_overflow(struct tc_module *const module_inst)
//void SysTick_Handler_sub(void)
{
    g_ms_ticks++;
}

void time_tick_init(void)
{
    g_ms_ticks = 0;

    struct tc_config config_tc;
    tc_get_config_defaults(&config_tc);

    // time_s = ticks / system_clk_hz
    // ticks = time_s * system_clk_hz
    // ticks = time_us * system_clk_hz / 1000000

    // set up a timer - period will be TIMER_INTERVAL_US
    uint16_t ticks = (uint16_t) (TIMER_INTERVAL_US * (system_gclk_gen_get_hz(0) / 1000000));

    config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
    config_tc.clock_source = GCLK_GENERATOR_0;  // system clock
    config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
    config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
    config_tc.counter_16_bit.compare_capture_channel[0] = ticks;

    tc_init(&tc_instance, TC3, &config_tc);

    //enabled by default
    tc_enable(&tc_instance);

    // Register and enable callback
    tc_register_callback(&tc_instance, tc_callback_overflow, TC_CALLBACK_OVERFLOW);
    tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
}

uint32_t time_tick_get(void)
{
    return g_ms_ticks;
    //can return partial if necessary
}

uint32_t time_tick_calc_delay_ms(uint32_t tick_start, uint32_t tick_end)
{
    if (tick_end >= tick_start) {
        return (tick_end - tick_start) * (1000 / TIMER_INTERVAL_US);
    } else {
        /* In the case of 32-bit couter number overflow */
        return (tick_end + (0xFFFFFFFF - tick_start)) * (1000 / TIMER_INTERVAL_US);
    }
}

void time_tick_start(void) {
    tc_enable(&tc_instance);
}

void time_tick_stop(void) {
    tc_disable(&tc_instance);
}
