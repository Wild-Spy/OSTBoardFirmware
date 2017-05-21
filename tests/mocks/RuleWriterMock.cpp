//
// Created by mcochrane on 21/05/17.
//

#include "RuleWriterMock.h"

static RuleWriter* ruleWriter = NULL;

void RuleWriter_Init() {
    ruleWriter = new RuleWriter();
}

uint8_t RuleWriter_GetRuleCount() {
    return RuleWriter_GetRuleWriter().getRuleCount();
}

uint8_t RuleWriter_AddNewRule(uint8_t *buf, uint8_t length) {
    return RuleWriter_GetRuleWriter().addNewRule(buf, length);
}

void RuleWriter_ClearRules() {
    RuleWriter_GetRuleWriter().clearRules();
}

void RuleWriter_SaveRuleCount() {
    RuleWriter_GetRuleWriter().saveRuleCountToEeprom();
}

void RuleWriter_GetRuleData(uint8_t rule_id, uint8_t *data, uint8_t *length) {
    RuleWriter_GetRuleWriter().getRuleData(rule_id, data, length);
}

RuleWriter& RuleWriter_GetRuleWriter() {
    if (ruleWriter == NULL) Throw(EX_NULL_POINTER);
    return *ruleWriter;
}

void RuleWriter_Destroy() {
    delete ruleWriter;
    ruleWriter = NULL;
}

