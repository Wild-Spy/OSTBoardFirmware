//
// Created by mcochrane on 8/04/17.
//

#include <libs/exception/CException.h>
#include "Pin.h"

void Pin::setDirOutput() {
    if (config_.direction == PORT_PIN_DIR_OUTPUT) return;

    config_.direction = PORT_PIN_DIR_OUTPUT;
    setConfig();
}

void Pin::setDirInput() {
    if (config_.direction == PORT_PIN_DIR_INPUT) return;

    config_.direction = PORT_PIN_DIR_INPUT;
    setConfig();
}

bool Pin::getValue() const {
    if (config_.direction == PORT_PIN_DIR_OUTPUT) {
        return port_pin_get_input_level(pin_);
    } else {
        return port_pin_get_output_level(pin_);
    }
}

void Pin::setDir(pin_dir_t dir) {
    if (dir == pin_dir_output) {
        setDirOutput();
    } else if (dir == pin_dir_input) {
        setDirInput();
    }
}

pin_dir_t Pin::getDir() const {

    if (config_.direction == PORT_PIN_DIR_OUTPUT) {
        return pin_dir_output;
    } else if (config_.direction == PORT_PIN_DIR_INPUT) {
        return pin_dir_input;
    }

    return pin_dir_input;
}

void Pin::setOutput(bool value) {
    if (value) {
        setOutputHigh();
    } else {
        setOutputLow();
    }
}

void Pin::setOutputHigh() {
    port_pin_set_output_level(pin_, true);
}

void Pin::setOutputLow() {
    port_pin_set_output_level(pin_, false);
}

void Pin::toggleOutput() {
    port_pin_toggle_output_level(pin_);
}

enum port_pin_pull Pin::getInputPull() {
    return config_.input_pull;
}

void Pin::setInputPull(enum port_pin_pull input_pull) {
    config_.input_pull = input_pull;
    setConfig();
}

void Pin::setConfig() {
    port_pin_set_config(pin_, &config_);
}

void Pin::setupInterrupt(enum extint_detect detection_criteria,
                         enum extint_pull gpio_pin_pull,
                         bool wake_if_sleeping,
                         bool filter_input_signal) {
    extint_chan_get_config_defaults(&config_extint_chan_);
    config_extint_chan_.gpio_pin = pin_;
//    config_extint_chan.gpio_pin_mux = SYSTEM_PINMUX_GPIO; //default anyway
    config_extint_chan_.gpio_pin_pull = EXTINT_PULL_UP;
    config_extint_chan_.detection_criteria = EXTINT_DETECT_LOW;
    extint_chan_set_config(pin_, &config_extint_chan_);
}

/**
 * Also enables the callback.
 * @param callback
 */
void Pin::registerCallback(extint_callback_t callback) {
    if (callback_ != NULL) Throw(EX_ALREADY_INITIALISED);
    callback_ = callback;
    enum status_code result = extint_unregister_callback(callback_, pin_, EXTINT_CALLBACK_TYPE_DETECT);
    if (result == STATUS_ERR_ALREADY_INITIALIZED) {
        Throw(EX_ALREADY_INITIALISED);
    }
    extint_chan_enable_callback(pin_, EXTINT_CALLBACK_TYPE_DETECT);
}

/**
 * Also disables the callback.
 */
void Pin::unregisterCallback() {
    extint_chan_disable_callback(pin_, EXTINT_CALLBACK_TYPE_DETECT);
    extint_unregister_callback(callback_, pin_, EXTINT_CALLBACK_TYPE_DETECT);
    callback_ = NULL;
}



//void Pin::setPinCtrlReg(uint8_t value) {
//    pin_ctrl_reg_ = value;
//    /* The MPCMASK register enables configuration of several pins of a port at the same time.
//     * Writing a one to bit n makes pin n part of the multi-pin configuration. When one or
//     * more bits in the MPCMASK register is set, writing any of the PINnCTRL registers will
//     * update only the PINnCTRL registers matching the mask in the MPCMASK register for that
//     * port. The MPCMASK register is automatically cleared after any PINnCTRL register is
//     * written.
//     */
//    PORTCFG.MPCMASK = pin_mask_;
//    // only sets the pins specified in pin_mask, pin0 is not necessarily modified..
//    port_->PIN0CTRL = pin_ctrl_reg_;
//}
//
//void Pin::setInverted(bool inverted) {
//    if (inverted) {
//        setPinCtrlReg(pin_ctrl_reg_ | PORT_INVEN_bm);
//    } else {
//        setPinCtrlReg(pin_ctrl_reg_ & ~PORT_INVEN_bm);
//    }
//}
//
//void Pin::setOutputPullConfiguration(PORT_OPC_t opc) {
//    uint8_t new_ctrl_reg = pin_ctrl_reg_;
//    new_ctrl_reg &= ~PORT_OPC_gm; //clear opc bits
//    new_ctrl_reg |= opc;
//    setPinCtrlReg(new_ctrl_reg);
//}
//
//void Pin::setInputSenseConfiguration(PORT_ISC_t isc) {
//    uint8_t new_ctrl_reg = pin_ctrl_reg_;
//    new_ctrl_reg &= ~PORT_ISC_gm; //clear isc bits
//    new_ctrl_reg |= isc;
//    setPinCtrlReg(new_ctrl_reg);
//}
//
//bool Pin::interruptEnabled(uint8_t interrupt_number) {
//    if (interrupt_number == 0) {
//        return (bool) (port_->INT0MASK && pin_mask_);
//    } else if (interrupt_number == 1) {
//        return (bool) (port_->INT1MASK && pin_mask_);
//    } else {
//        return false;
//    }
//}
//
//void Pin::enableInterrupt(uint8_t interrupt_number) {
//    if (interrupt_number == 0) {
//        port_->INT0MASK |= pin_mask_;
//    } else if (interrupt_number == 1) {
//        port_->INT1MASK |= pin_mask_;
//    }
//}
//
//void Pin::disableInterrupt(uint8_t interrupt_number) {
//    if (interrupt_number == 0) {
//        port_->INT0MASK &= ~pin_mask_;
//    } else if (interrupt_number == 1) {
//        port_->INT1MASK &= ~pin_mask_;
//    }
//}
//
//void Pin::setInterruptLevel(uint8_t interrupt_number, PORT_INT0LVL_t level) {
//    uint8_t intctrl = port_->INTCTRL;
//    if (interrupt_number == 0) {
//        intctrl &= ~PORT_INT0LVL_gm;
//        port_->INTCTRL = intctrl | level;
//    } else if (interrupt_number == 1) {
//        intctrl &= ~PORT_INT1LVL_gm;
//        port_->INT1MASK = intctrl | (level<<2); //shift left two bits to convert a int0lvl to an int1lvl
//    }
//}
