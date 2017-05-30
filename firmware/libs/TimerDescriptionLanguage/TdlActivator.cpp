//
// Created by mcochrane on 5/04/17.
//

#include <min/min_transmit_cmds.h>
#include "TdlActivator.h"
#include "TdlRule.h"

void TdlActivator::disable() {
    currentState_ = TDLACTIVATORSTATE_DISABLED;
    if (target_ == NULL) return;
    if (act_type_ == ACTIVATOR_TYPE_CHANNEL) {
        getChannel()->disable();
//        report_printf("chan %u disabled", getChannel()->getId());
    } else if (act_type_ == ACTIVATOR_TYPE_RULE) {
        getRule()->disable();
    }
}

void TdlActivator::enable(DateTime now) {
    currentState_ = TDLACTIVATORSTATE_ENABLED;
    if (target_ == NULL) return;
    if (act_type_ == ACTIVATOR_TYPE_CHANNEL) {
        getChannel()->enable();
//        report_printf("chan %u enabled", getChannel()->getId());
    } else if (act_type_ == ACTIVATOR_TYPE_RULE) {
        getRule()->enable();
        getRule()->updateThisRuleAndParentsIfNecessary(now);
    }
}

int TdlActivator::getTargetId() {
    if (target_ == NULL) return -2;
    if (act_type_ == ACTIVATOR_TYPE_CHANNEL) {
        getChannel()->getId();
    } else if (act_type_ == ACTIVATOR_TYPE_RULE) {
        getRule()->getId();
    }
}

bool TdlActivator::operator==(const TdlActivator &rhs) const {
    return (defaultState_ == rhs.defaultState_ &&
            currentState_ == rhs.currentState_ &&
            act_type_ == rhs.act_type_ &&
            target_ == rhs.target_);
}
