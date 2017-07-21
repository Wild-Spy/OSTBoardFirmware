//
// Created by mcochrane on 6/04/17.
//

//#ifdef TESTING
//#include "delayMock.h"
//#else
//extern "C" {
//#include <util/delay.h>
//}
//#include <avr/interrupt.h>
//#endif

#include <nvm/NvmRuleManager.h>
#include <min/min_transmit_cmds.h>
#include <cstring>

#include "TdlRules.h"

static TdlRules* tdl_rules = NULL;

TdlRules::TdlRules(uint8_t max_rules, NvmRuleManager& ruleWriter)
        : max_rules_(max_rules),
          rule_count_(0)
{
    // Allocate space for all rules
    rules_ = (TdlRule*)malloc(sizeof(TdlRule)*max_rules_);
}

//void TdlRules::disableAll() {
////    for (uint8_t i = 0; i < channel_count_; i++) {
////        get(i).disable();
////    }
//}

TdlRule& TdlRules::get(int index) {
    if (index < 0 || index >= rule_count_) Throw(EX_OUT_OF_RANGE);
    return rules_[index];
}

TdlRules::~TdlRules() {
    for (uint8_t i = 0; i < rule_count_; i++) {
        rules_[i].~TdlRule();
    }
    free(rules_);
}

/**
 * Loads a rule into the mock
 * @param rule  rule to load in
 * @return      the position in the list that the rule is stored
 */
int TdlRules::loadInRule(TdlRule& rule) {
    if (rule_count_ == max_rules_) return -1;
    memcpy_ram2ram(&rules_[rule_count_], &rule, sizeof(TdlRule));
    return rule_count_++;
}

/*
 * LoadFromEeprom does essentially nothing now.
 * We'll load them in manually
 */
void TdlRules::loadFromEeprom() {
//    rule_count_ = NvmRuleManager_GetRuleCount();
//    if (rule_count_ == 0) return;
//
//    uint8_t data[256];
//    uint8_t length = 0;
//
//    for (uint8_t i = 0; i < rule_count_; i++) {
//        NvmRuleManager_GetRuleData(i, data, &length);
//        TdlRule::Decompile(data, length, (rules_ + i));
////        new (rules_ + i) TdlRule(data[0], (bool)data[1]);
//    }
}

TdlRules& TdlRules_GetInstance() {
    if (tdl_rules == NULL) Throw(EX_NULL_POINTER);
    return *tdl_rules;
}

/**
 * RuleWriter must have been initialised already.
 * @param max_rules     number of rules to allocate on the heap
 * @param ruleWriter    the rule writer from which we will retrieve rules
 */
void TdlRules_Init(uint8_t max_rules, NvmRuleManager& ruleWriter) {
    tdl_rules = new TdlRules(max_rules, ruleWriter);
    TdlRules_GetInstance().loadFromEeprom();
}

void TdlRules_Destroy() {
    TdlRules_GetInstance().~TdlRules();
    tdl_rules = NULL;
}