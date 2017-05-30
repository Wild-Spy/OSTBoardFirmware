//
// Created by mcochrane on 25/05/17.
//

#ifndef WS_OST_USB_HANDLERS_H
#define WS_OST_USB_HANDLERS_H

#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void main_suspend_action(void);
void main_resume_action(void);
void main_sof_action(void);
bool main_cdc_enable(uint8_t port);
void main_cdc_disable(uint8_t port);
//int8_t serial_ireceive(uint8_t port);
//void cdc_rx_notify(uint8_t port);
//int8_t serial_isend(uint8_t port);
void main_cdc_set_coding(uint8_t port);
void main_cdc_set_dtr(uint8_t port, bool set);
void main_vbus_action(bool vbus_high);
bool get_vbus_state(void);
bool get_usb_enabled();
bool get_sof_event();
bool get_cdc_configured();
bool get_cdc_enumerated();
void main_suspend_lpm_action();
void main_remotewakeup_lpm_enable();
void main_remotewakeup_lpm_disable();

#ifdef __cplusplus
}
#endif

#endif //WS_OST_USB_HANDLERS_H
