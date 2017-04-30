//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_TDLACTION_H
#define WS_OST_TDLACTION_H


#include "TdlActivator.h"
#include "TdlRuleState.h"

class TdlAction {
public:
    TdlAction();

    TdlAction(TdlActivatorState_t activator_state_when_running, TdlActivator activator)
            : activator_(activator),
              activator_state_when_running_(activator_state_when_running)
    {
        state_ = TDLRULESTATE_INACTIVE;
    }

    void start(DateTime now);
    void stop(DateTime now);
    void toggle(DateTime now);

    TdlRuleState_t getState() { return state_; };
    TdlActivatorState_t getActivatorState() { return activator_.getState(); };

    static const uint8_t ACTION_TARGET_CHANNEL = 0;
    static const uint8_t ACTION_TARGET_RULE = 1;
    static const uint8_t ACTION_COMPILED_LENGTH = 5;

    int getActivatorType() { return activator_.getActivatorType(); };

    TdlActivator& getActivator() { return activator_; };

    static TdlAction Decompile(uint8_t* data) { return TdlAction(data); }

private:
    TdlAction(uint8_t* data);

private:
    TdlActivator activator_;
    TdlActivatorState_t activator_state_when_running_;
    TdlRuleState_t state_;
};

//needs to be declared at the end..
#include "TdlRule.h"

#endif //WS_OST_TDLACTION_H
