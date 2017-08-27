//
// Created by mcochrane on 5/04/17.
//
#include "TdlAction.h"

#ifdef TESTING
#include "delayMock.h"
#include "SimplePinMock.h"
#else
#include <delay.h>
#endif

extern "C" {
#include "min/encode_decode.h"
}

#include "TdlChannels.h"
#include "TdlRules.h"

void TdlAction::start(DateTime now) {
    if (state_ == TDLACTIONSTATE_ACTIVE) return;
//    report_printf("actn sta");
    state_ = TDLACTIONSTATE_ACTIVE;
    if (activator_state_when_running_ == TDLACTIVATORSTATE_ENABLED) {
        if (activator_.getActivatorType() == ACTION_TARGET_CHANNEL) {
            activator_.enable(now);
        } else if (activator_.getActivatorType() == ACTION_TARGET_RULE) {
            activator_.enable(now);
        } else {
            activator_.enable(now);
        }
    } else {
        if (activator_.getActivatorType() == ACTION_TARGET_CHANNEL) {
            activator_.disable();
        } else if (activator_.getActivatorType() == ACTION_TARGET_RULE) {
            activator_.disable();
        } else {
            activator_.disable();
        }
    }
}

void TdlAction::stop(DateTime now) {
    if (state_ == TDLACTIONSTATE_INACTIVE) return;
//    report_printf("actn stp");
    state_ = TDLACTIONSTATE_INACTIVE;
    if (activator_state_when_running_ == TDLACTIVATORSTATE_ENABLED) {
        activator_.disable();
    } else {
        activator_.enable(now);
    }
}

void TdlAction::toggle(DateTime now) {
    if (state_ == TDLACTIONSTATE_ACTIVE) {
        stop(now);
    } else {
        start(now);
    }
}

TdlAction::TdlAction(uint8_t *data)
{
    uint8_t activator_type = data[0];
    uint16_t target_id = decode_u16(data+1);
    TdlActivatorState_t default_state = TDLACTIVATORSTATE_DISABLED;
    if (data[3] == 0) default_state = TDLACTIVATORSTATE_DISABLED;
    else if (data[3] == 1) default_state = TDLACTIVATORSTATE_ENABLED;
    else Throw(EX_INVALID_INPUT_VALUE);

    if (data[4] == 0) activator_state_when_running_ = TDLACTIVATORSTATE_DISABLED;
    else if (data[4] == 1) activator_state_when_running_ = TDLACTIVATORSTATE_ENABLED;
    else Throw(EX_INVALID_INPUT_VALUE);

    if (activator_type == ACTION_TARGET_CHANNEL) {
        activator_ = TdlActivator(&TdlChannels_GetInstance().get(target_id), default_state);
    } else if (activator_type == ACTION_TARGET_RULE) {
        activator_ = TdlActivator(&TdlRules_GetInstance().get(target_id), default_state);
    } else {
        Throw(EX_INVALID_INPUT_VALUE);
    }
}

TdlAction::TdlAction()
        : activator_()
{}
