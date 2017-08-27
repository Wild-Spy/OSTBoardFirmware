//
// Created by mcochrane on 25/05/17.
//

#include <hal/Pin.h>
#include <hw/hal_gpio.h>
#include <serial/usb_serial.h>
#include <libs/conf/conf_board.h>
#include <asf/common/services/usb/udc/udc.h>
#include <libs/min/layer2.h>
#include "usb_handlers.h"

static bool _vbus_high = false;
//static bool main_b_vbus_event = false;
static bool _usb_enabled = false;
static bool _sof_event = false;
static bool _cdc_configured = false;
static bool _cdc_enumerated = false;
static bool _usb_ready = false;

static Pin led(HAL_GPIO_PORTA, 27);

static void _uhd_vbus_handler(void);

# define is_usb_vbus_high()           port_pin_get_input_level(USB_VBUS_PIN)

void main_suspend_action(void) {
    _usb_enabled = false;
//    led_test.enable();
}

void main_resume_action(void) {
    _usb_enabled = true;
//    led_test.disable();
}

void main_sof_action(void) {
    _sof_event = true;
}

bool main_cdc_enable(uint8_t port) {
//    Pin led(HAL_GPIO_PORTA, 27);
//    led.setDirOutput();
//    led.setOutputHigh();
    _cdc_enumerated = true;
    return true;
}

void main_cdc_disable(uint8_t port) {
    _cdc_enumerated = false;
}

//void cdc_rx_notify(uint8_t port) {
//    serial_ireceive(port);
//}

void main_cdc_set_coding(uint8_t port) {
    _cdc_configured = true;
}

void main_cdc_set_dtr(uint8_t port, bool set) {

}

void main_vbus_action(bool vbus_high) {
    _vbus_high = vbus_high;
}

void main_suspend_lpm_action() {

}

void main_remotewakeup_lpm_enable() {

}

void main_remotewakeup_lpm_disable() {

}



bool get_vbus_state(void) {
    return _vbus_high;
}

bool get_usb_enabled() {
    return _usb_enabled;
}

bool get_sof_event() {
    return _sof_event;
}

bool get_cdc_configured() {
    return _cdc_configured;
}

bool get_cdc_enumerated() {
    return _cdc_enumerated;
}

static void setup_vbus_interrupt(enum extint_detect detect_criteria, bool enable_now) {
    /* Initialize EIC for vbus checking */
    struct extint_chan_conf eint_chan_conf;
    extint_chan_get_config_defaults(&eint_chan_conf);

    eint_chan_conf.gpio_pin           = USB_VBUS_PIN;
    eint_chan_conf.gpio_pin_mux       = USB_VBUS_EIC_MUX;
    eint_chan_conf.gpio_pin_pull      = EXTINT_PULL_NONE;
//    eint_chan_conf.detection_criteria = EXTINT_DETECT_HIGH;
    eint_chan_conf.detection_criteria = detect_criteria;
    eint_chan_conf.filter_input_signal = false;
    eint_chan_conf.wake_if_sleeping   = true;

    extint_chan_disable_callback(USB_VBUS_EIC_LINE,
                                 EXTINT_CALLBACK_TYPE_DETECT);
    extint_chan_set_config(USB_VBUS_EIC_LINE, &eint_chan_conf);
    extint_register_callback(_uhd_vbus_handler,
                             USB_VBUS_EIC_LINE,
                             EXTINT_CALLBACK_TYPE_DETECT);
    if (enable_now) {
        extint_chan_enable_callback(USB_VBUS_EIC_LINE,
                                    EXTINT_CALLBACK_TYPE_DETECT);
    }
}

static void set_usb_pins_input_and_pullup() {
//    From Datasheet:
//    If the PA24 and PA25 pins are not connected,
//    it is recommended to enable a pull-up on PA24 and
//    PA25 through input GPIO mode. The aim is to avoid
//    an eventually extract power consumption (<1mA)
//    due to a not stable level on pad. The port PA24 and
//    PA25 doesn't have Drive Strength option.
    struct port_config config;
    port_get_config_defaults(&config);
    config.direction = PORT_PIN_DIR_INPUT;
    config.input_pull = PORT_PIN_PULL_UP;
    port_pin_set_config(24, &config);
    port_pin_set_config(25, &config);
}

static void _uhd_vbus_handler(void)
{
    extint_chan_disable_callback(USB_VBUS_EIC_LINE,
                                 EXTINT_CALLBACK_TYPE_DETECT);

    if (is_usb_vbus_high()) {
        led.setOutputHigh();
        udc_start();
        udc_attach();
        init_min();
//        _usb_enabled = true;
        setup_vbus_interrupt(EXTINT_DETECT_LOW, false);
    } else {
        led.setOutputLow();
        udc_detach();
        udc_stop();
        setup_vbus_interrupt(EXTINT_DETECT_HIGH, false);
        set_usb_pins_input_and_pullup();
//        _usb_enabled = false;
    }
    extint_chan_enable_callback(USB_VBUS_EIC_LINE,
                                EXTINT_CALLBACK_TYPE_DETECT);
}

void usb_vbus_config(void)
{
    if (is_usb_vbus_high()) {
        led.setOutputHigh();
        udc_start();
        udc_attach();
        init_min();
//        _usb_enabled = true;
        setup_vbus_interrupt(EXTINT_DETECT_LOW, true);
    } else {
        setup_vbus_interrupt(EXTINT_DETECT_HIGH, true);
    }



}