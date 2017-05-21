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

#include <config_comms/ruler_writer.h>
#include <min/min_transmit_cmds.h>

#include "TdlRules.h"

static TdlRules* tdl_rules = NULL;

TdlRules::TdlRules(uint8_t max_rules, RuleWriter& ruleWriter)
    : max_rules_(max_rules),
      rule_count_(0),
      rule_writer_(ruleWriter)
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

#ifdef TESTING
TdlRules::TdlRules(TdlRule rules[], uint8_t count)
        : max_rules_(count),
          rule_count_(count),
          rule_writer_(*(RuleWriter*)0)
{
    if (rule_count_ == 0) return;
    rules_ = rules;
}

TdlRules::~TdlRules() {
    for (uint8_t i = 0; i < rule_count_; i++) {
        rules_[i].~TdlRule();
    }
    free(rules_);
}
#endif

void TdlRules::loadFromEeprom() {
    rule_count_ = RuleWriter_GetRuleCount();
    if (rule_count_ == 0) return;

    uint8_t data[256];
    uint8_t length = 0;

    for (uint8_t i = 0; i < rule_count_; i++) {
        RuleWriter_GetRuleData(i, data, &length);
        TdlRule::Decompile(data, length, (rules_ + i));
    }

//    RuleWriterRule ruleWriterRule = RuleWriter_GetRuleWriter().GetRule(0);
//    for (uint8_t i = 0; i < rule_count_; i++) {
//        rules_[i] = TdlRule::Decompile(ruleWriterRule.GetData(), ruleWriterRule.GetLength());
//
//        if (i < rule_count_-1)
//            ruleWriterRule.GetNextRule();
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
void TdlRules_Init(uint8_t max_rules, RuleWriter& ruleWriter) {
    tdl_rules = new TdlRules(max_rules, ruleWriter);
    TdlRules_GetInstance().loadFromEeprom();
}

#ifdef TESTING
void TdlRules_Init_Test(TdlRule rules[], uint8_t count) {
    tdl_rules = new TdlRules(rules, count);
}

void TdlRules_Destroy() {
    TdlRules_GetInstance().~TdlRules();
    tdl_rules = NULL;
}
#endif
