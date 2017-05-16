//
// Created by mcochrane on 6/04/17.
//

#ifndef WS_OST_TDLRULES_H
#define WS_OST_TDLRULES_H

#include <helpers/compile_time_sizeof.h>
#include "TdlRule.h"
extern "C" {
    #include "exception/ExceptionValues.h"
};

#define MAX_RULES   2

class TdlRules {
public:
    TdlRules(uint8_t max_rules);

    void disableAll();

    /**
     * Gets a channel from it's index
     * @param index     the channel's index
     * @return  a reference to the channel
     * @throws  EX_OUT_OF_RANGE if the index is out of range
     */
    TdlRule& get(int index);

    uint8_t getCount() { return rule_count_; };

    void loadFromEeprom();

private:
    uint8_t max_rules_;
    uint8_t rule_count_;
    TdlRule rules_[MAX_RULES];
};

//COMPILE_TIME_SIZEOF(TdlRule);
//COMPILE_TIME_SIZEOF(long);

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void TdlRules_Init();
EXTERNC TdlRules& TdlRules_GetInstance();

#undef EXTERNC

#endif //WS_OST_TDLRULES_H