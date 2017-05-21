//
// Created by mcochrane on 20/05/17.
//

#include "TdlChannelMock.h"

void TdlChannelInterface::disable() {
    disableCalled();
    state_ = TDLCHANNELSTATE_DISABLED;
}

void TdlChannelInterface::enable() {
    enableCalled();
    state_ = TDLCHANNELSTATE_ENABLED;
}

void TdlChannelInterface::setState(TdlChannelState_t state) {
    switch (state) {
        case TDLCHANNELSTATE_DISABLED:
            disable();
            break;
        case TDLCHANNELSTATE_ENABLED:
            enable();
            break;
    }
}

void TdlChannelInterface::toggle() {
    if (getState() == TDLCHANNELSTATE_ENABLED) {
        disable();
    } else {
        enable();
    }
//    setState((TdlChannelState_t)!((bool)getState()));
}