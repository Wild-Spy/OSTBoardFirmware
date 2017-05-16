//
// Created by mcochrane on 6/04/17.
//

extern "C" {
#include <util/delay.h>
}

#include <config_comms/ruler_writer_cpp.h>
#include <min/min_transmit_cmds.h>
#include <avr/interrupt.h>
#include "TdlRules.h"
#include "TdlChannels.h"

static TdlRules tdl_rules = TdlRules(MAX_RULES);

TdlRules::TdlRules(uint8_t max_rules)
        : max_rules_(max_rules),
          rule_count_(0)
{
//    for (uint8_t i = 0; i < channel_count_; i++) {
//        channels_[i] = TdlChannel(i, default_state, PORTB, i);
//    }
}

void TdlRules::disableAll() {
//    for (uint8_t i = 0; i < channel_count_; i++) {
//        channels_[i].disable();
//    }
}

TdlRule& TdlRules::get(int index) {
    if (index < 0 || index >= rule_count_)
        Throw(EX_OUT_OF_RANGE);

    return rules_[index];
}

void TdlRules::loadFromEeprom() {
    rule_count_ = RuleWriter_GetRuleCount();
    if (rule_count_ == 0) return;

    RuleWriterRule ruleWriterRule = RuleWriter_GetRuleWriter().GetRule(0);
    for (uint8_t i = 0; i < rule_count_; i++) {
        rules_[i] = TdlRule::Decompile(ruleWriterRule.GetData(), ruleWriterRule.GetLength());

        if (i < rule_count_-1)
            ruleWriterRule.GetNextRule();
    }
}

TdlRules& TdlRules_GetInstance() {
    return tdl_rules;
}

void TdlRules_Init() {
    RuleWriter_Init();
    tdl_rules = TdlRules(MAX_RULES);
    tdl_rules.loadFromEeprom();
}
