//
// Created by mcochrane on 8/04/17.
//

#include "Pin.h"

void Pin::setDirOutput() {
    port_->DIRSET = pin_mask_;
}

void Pin::setDirInput() {
    port_->DIRCLR = pin_mask_;
}

bool Pin::getValue() const {
    if (dir_ == pin_dir_output) {
        return (port_->OUT & pin_mask_) > 0;
    } else {
        return (port_->IN & pin_mask_) > 0;
    }
}

void Pin::setDir(pin_dir_t dir) {
    dir_ = dir;
    if (dir_ == pin_dir_output) {
        setDirOutput();
    } else {
        setDirInput();
    }
}

pin_dir_t Pin::getDir() const {
    return dir_;
}

void Pin::setOutput(bool value) {
    if (value) {
        setOutputHigh();
    } else {
        setOutputLow();
    }
}

void Pin::setOutputHigh() {
    port_->OUTSET = pin_mask_;
}

void Pin::setOutputLow() {
    port_->OUTCLR = pin_mask_;
}

void Pin::toggleOutput() {
    port_->OUTTGL = pin_mask_;
}

void Pin::setPinCtrlReg(uint8_t value) {
    pin_ctrl_reg_ = value;
    /* The MPCMASK register enables configuration of several pins of a port at the same time.
     * Writing a one to bit n makes pin n part of the multi-pin configuration. When one or
     * more bits in the MPCMASK register is set, writing any of the PINnCTRL registers will
     * update only the PINnCTRL registers matching the mask in the MPCMASK register for that
     * port. The MPCMASK register is automatically cleared after any PINnCTRL register is
     * written.
     */
    PORTCFG.MPCMASK = pin_mask_;
    // only sets the pins specified in pin_mask, pin0 is not necessarily modified..
    port_->PIN0CTRL = pin_ctrl_reg_;

}

void Pin::setInverted(bool inverted) {
    if (inverted) {
        setPinCtrlReg(pin_ctrl_reg_ | PORT_INVEN_bm);
    } else {
        setPinCtrlReg(pin_ctrl_reg_ & ~PORT_INVEN_bm);
    }
}

void Pin::setOutputPullConfiguration(PORT_OPC_t opc) {
    uint8_t new_ctrl_reg = pin_ctrl_reg_;
    new_ctrl_reg &= ~PORT_OPC_gm; //clear opc bits
    new_ctrl_reg |= opc;
    setPinCtrlReg(new_ctrl_reg);
}

void Pin::setInputSenseConfiguration(PORT_ISC_t isc) {
    uint8_t new_ctrl_reg = pin_ctrl_reg_;
    new_ctrl_reg &= ~PORT_ISC_gm; //clear isc bits
    new_ctrl_reg |= isc;
    setPinCtrlReg(new_ctrl_reg);
}

bool Pin::interruptEnabled(uint8_t interrupt_number) {
    if (interrupt_number == 0) {
        return (bool) (port_->INT0MASK && pin_mask_);
    } else if (interrupt_number == 1) {
        return (bool) (port_->INT1MASK && pin_mask_);
    } else {
        return false;
    }
}

void Pin::enableInterrupt(uint8_t interrupt_number) {
    if (interrupt_number == 0) {
        port_->INT0MASK |= pin_mask_;
    } else if (interrupt_number == 1) {
        port_->INT1MASK |= pin_mask_;
    }
}

void Pin::disableInterrupt(uint8_t interrupt_number) {
    if (interrupt_number == 0) {
        port_->INT0MASK &= ~pin_mask_;
    } else if (interrupt_number == 1) {
        port_->INT1MASK &= ~pin_mask_;
    }
}

void Pin::setInterruptLevel(uint8_t interrupt_number, PORT_INT0LVL_t level) {
    uint8_t intctrl = port_->INTCTRL;
    if (interrupt_number == 0) {
        intctrl &= ~PORT_INT0LVL_gm;
        port_->INTCTRL = intctrl | level;
    } else if (interrupt_number == 1) {
        intctrl &= ~PORT_INT1LVL_gm;
        port_->INT1MASK = intctrl | (level<<2); //shift left two bits to convert a int0lvl to an int1lvl
    }
}
