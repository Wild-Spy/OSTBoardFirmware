//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_TDLACTIVATOR_H
#define WS_OST_TDLACTIVATOR_H

#include <datetime/DateTime.h>
#include "TdlActivatorState.h"
#include "TdlChannel.h"
//#include "TdlRule.h"

class TdlRule;

typedef enum TdlActivatorType_enum {
    ACTIVATOR_TYPE_CHANNEL,
    ACTIVATOR_TYPE_RULE
} TdlActivatorType_t;

class TdlActivator {
public:

    TdlActivator(TdlChannel* channel, TdlActivatorState_t defaultState) {
        defaultState_ = defaultState;
        currentState_ = defaultState;
        target_ = channel;
        act_type_ = ACTIVATOR_TYPE_CHANNEL;
    }

    TdlActivator(TdlRule* rule, TdlActivatorState_t defaultState) {
        defaultState_ = defaultState;
        currentState_ = defaultState;
        target_ = rule;
        act_type_ = ACTIVATOR_TYPE_RULE;
    }

    TdlActivator() {
        defaultState_ = TDLACTIVATORSTATE_DISABLED;
        currentState_ = TDLACTIVATORSTATE_DISABLED;
        target_ = NULL;
    };

    /**
     * Enables the rule or channel (ie on)
     */
    void enable(DateTime now);

    /**
     * Disables the rule or channel (ie off)
     */
    void disable();

    inline TdlActivatorState_t getDefaultState() {
        return defaultState_;
    }

    inline TdlActivatorState_t getState() {
        return currentState_;
    }

    inline bool isInDefaultState() {
        return currentState_ == defaultState_;
    }

    TdlChannel* getChannel() {
        if (act_type_ != ACTIVATOR_TYPE_CHANNEL) return NULL;
        return (TdlChannel*) target_;
    }

    TdlRule* getRule() {
        if (act_type_ != ACTIVATOR_TYPE_RULE) return NULL;
        return (TdlRule*) target_;
    }

    int getTargetId();

    TdlActivatorType_t getActivatorType() { return act_type_; };

private:
    TdlActivatorState_t defaultState_;
    TdlActivatorState_t currentState_;
    TdlActivatorType_t act_type_;
    void* target_;
};

#include "TdlRule.h"

#endif //WS_OST_TDLACTIVATOR_H