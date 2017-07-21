//
// Created by mcochrane on 5/04/17.
//

#include "TdlChannel.h"

void TdlChannel::disable() {
    state_ = TDLCHANNELSTATE_DISABLED;
    state_change_callback_(id_);
//#if defined(DEBUGPRINTS) && DEBUGPRINTS > 0
//    report_printf("Chan %u OFF", id_);
//#endif
}

void TdlChannel::enable() {
    state_ = TDLCHANNELSTATE_ENABLED;
    state_change_callback_(id_);
//#if defined(DEBUGPRINTS) && DEBUGPRINTS > 0
//    report_printf("Chan %u ON", id_);
//#endif
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
}
