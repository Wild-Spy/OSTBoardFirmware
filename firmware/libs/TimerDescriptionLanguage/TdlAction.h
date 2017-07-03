//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_TDLACTION_H
#define WS_OST_TDLACTION_H

#include "TdlRuleState.h"
#include "TdlActivator.h"


typedef enum ActionState_enum {
    TDLACTIONSTATE_ACTIVE,
    TDLACTIONSTATE_INACTIVE
} TdlActionState_t;

class TdlAction {
public:
    TdlAction();

    TdlAction(TdlActivatorState_t activator_state_when_running, TdlActivator activator)
            : activator_(activator),
              activator_state_when_running_(activator_state_when_running)
    {
        state_ = TDLACTIONSTATE_INACTIVE;
    }

    void start(DateTime now);
    void stop(DateTime now);
    void toggle(DateTime now);

    TdlActivatorState_t getActivatorStateWhenRunning() { return activator_state_when_running_; };

    TdlActionState_t getState() { return state_; };
    TdlActivatorState_t getActivatorState() { return activator_.getState(); };

    static const uint8_t ACTION_TARGET_CHANNEL = 0;
    static const uint8_t ACTION_TARGET_RULE = 1;
    static const uint8_t ACTION_COMPILED_LENGTH = 5;

    TdlActivatorType_t getActivatorType() { return activator_.getActivatorType(); };

    TdlActivator& getActivator() { return activator_; };

    static TdlAction Decompile(uint8_t* data) { return TdlAction(data); }
    static void Decompile(uint8_t* data, TdlAction* address_to_store) { new (address_to_store) TdlAction(data); }

    bool isEmpty() { return activator_.isEmpty(); };

private:
    TdlAction(uint8_t* data);

private:
    TdlActivator activator_;
    TdlActivatorState_t activator_state_when_running_;
    TdlActionState_t state_;
};

//needs to be declared at the end..
#ifdef TESTING
#include "TdlRuleMock.h"
#else
#include "TdlRule.h"
#endif

#endif //WS_OST_TDLACTION_H
