//
// Created by mcochrane on 25/05/17.
//

#include <hal/Pin.h>
#include <hw/hal_gpio.h>
#include <serial/usb_serial.h>
#include "usb_handlers.h"

static bool _vbus_high = false;
//static bool main_b_vbus_event = false;
static bool _usb_enabled = false;
static bool _sof_event = false;
static bool _cdc_configured = false;
static bool _cdc_enumerated = false;

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