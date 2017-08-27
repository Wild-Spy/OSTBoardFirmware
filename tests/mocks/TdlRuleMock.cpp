//
// Created by mcochrane on 20/05/17.
//

#include "TdlRuleMock.h"



void TdlRuleInterface::enable() {
    enabled_ = true;
    enableCalled();
//    resetInternalTimeKeepingVariables();
}

void TdlRuleInterface::disable() {
    enabled_ = false;
    disableCalled();
//    action_.stop(DateTime());
}

void TdlRuleInterface::reset() {
    enabled_ = enabled_on_start_;
    resetCalled();
}

//void TdlRuleInterface::updateThisRuleAndParentsIfNecessary(DateTime now) {
////    runParentRulesIfNecessary(now);
////    updateThisRuleOnly(now);
//}