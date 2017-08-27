//
// Created by mcochrane on 21/05/17.
//

#include "NvmRuleManagerMock.h"

static NvmRuleManager* ruleWriter = NULL;

void NvmRuleManager_Init() {
    ruleWriter = new NvmRuleManager();
}

uint8_t NvmRuleManager_GetRuleCount() {
    return NvmRuleManager_Get().getRuleCount();
}

uint8_t NvmRuleManager_AddNewRule(uint8_t *buf, uint8_t length) {
    return NvmRuleManager_Get().addNewRule(buf, length);
}

void NvmRuleManager_ClearRules() {
    NvmRuleManager_Get().clearRules();
}

void NvmRuleManager_SaveRuleCount() {
    NvmRuleManager_Get().saveRuleCountToNvm();
}

void NvmRuleManager_GetRuleData(uint8_t rule_id, uint8_t *data, uint8_t *length) {
    NvmRuleManager_Get().getRuleData(rule_id, data, length);
}

NvmRuleManager& NvmRuleManager_Get() {
    if (ruleWriter == NULL) Throw(EX_NULL_POINTER);
    return *ruleWriter;
}

void NvmRuleManager_Destroy() {
    delete ruleWriter;
    ruleWriter = NULL;
}

