//
// Created by mcochrane on 6/04/17.
//

#ifndef WS_OST_TDLRULES_H
#define WS_OST_TDLRULES_H

#ifdef TESTING
#include "TdlRuleMock.h"
#include "NvmRuleManagerMock.h"
#else
#include "TdlRule.h"
#include "nvm/NvmRuleManager.h"
#endif

#include <helpers/compile_time_sizeof.h>
#include "exception/CException.h"

class TdlRules {
public:
    TdlRules(uint8_t max_rules, NvmRuleManager& ruleWriter);

//    void disableAll();

    /**
     * Gets a channel from it's index
     * @param index     the channel's index
     * @return  a reference to the channel
     * @throws  EX_OUT_OF_RANGE if the index is out of range
     */
    TdlRule& get(int index);

    uint8_t getMaxRules() { return max_rules_; };

    uint8_t getCount() { return rule_count_; };

    void loadFromEeprom();

    #ifdef TESTING
    TdlRules(TdlRule rules[], uint8_t count);
    ~TdlRules();
    #endif

private:
    uint8_t max_rules_;
    uint8_t rule_count_;
    TdlRule* rules_;//[MAX_RULES];
    NvmRuleManager& rule_writer_;
};

//COMPILE_TIME_SIZEOF(TdlRule);
//COMPILE_TIME_SIZEOF(long);

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void TdlRules_Init(uint8_t max_rules, NvmRuleManager& ruleWriter);
EXTERNC TdlRules& TdlRules_GetInstance();

#ifdef TESTING
EXTERNC void TdlRules_Init_Test(TdlRule rules[], uint8_t count);
EXTERNC void TdlRules_Destroy();
#endif

#undef EXTERNC

#endif //WS_OST_TDLRULES_H