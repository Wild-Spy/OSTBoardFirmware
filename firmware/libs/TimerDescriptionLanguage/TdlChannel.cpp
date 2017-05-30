//
// Created by mcochrane on 5/04/17.
//

#include "TdlChannel.h"
//#include <asf.h>
#include <min/min_transmit_cmds.h>

void TdlChannel::disable() {
//    port_->OUTCLR = (uint8_t)(1<<pin_);
    pin_->setOutputLow();
    state_ = TDLCHANNELSTATE_DISABLED;
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0
    report_printf("Chan %u OFF", id_);
    #endif
}

void TdlChannel::enable() {
//    port_->OUTSET = (uint8_t)(1<<pin_);
    pin_->setOutputHigh();
    state_ = TDLCHANNELSTATE_ENABLED;
    #if defined(DEBUGPRINTS) && DEBUGPRINTS > 0
    report_printf("Chan %u ON", id_);
    #endif
}

void TdlChannel::setState(TdlChannelState_t state) {
    switch (state) {
        case TDLCHANNELSTATE_DISABLED:
            disable();
            break;
        case TDLCHANNELSTATE_ENABLED:
            enable();
            break;
    }
}

void TdlChannel::toggle() {
    if (getState() == TDLCHANNELSTATE_ENABLED) {
        disable();
    } else {
        enable();
    }
//    setState((TdlChannelState_t)!((bool)getState()));
}
