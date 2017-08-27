//
// Created by mcochrane on 5/06/17.
//

#ifndef OSTBOARDFIRMWARE_TDLRULES_H
#define OSTBOARDFIRMWARE_TDLRULES_H

#include <gmock/gmock.h>
#include <TimerDescriptionLanguage/TdlRule.h>

class TdlRules {
public:
    TdlRules()
        : count_(0)
    {};

    /**
     * Gets a rule from it's index
     * @param index     the rule's index
     * @return  a reference to the rule
     * @throws  EX_OUT_OF_RANGE if the index is out of range
     */
//    TdlRule& get(int index);

//    uint8_t getMaxRules() { return max_rules_; };

//    uint8_t getCount() { return rule_count_; };

//    void loadFromEeprom();

    MOCK_METHOD1(get, TdlRule&(int));
    MOCK_METHOD0(getMaxRules, uint8_t(void));
//    MOCK_METHOD0(getCount, uint8_t(void));
    MOCK_METHOD0(loadFromEeprom, void(void));

    uint8_t getCount() { return count_; };
    void setCount(uint8_t count) { count_ = count; };

//    TdlRules(TdlRule rules[], uint8_t count);
    ~TdlRules() {};

private:
    uint8_t count_;
};

TdlRules& TdlRules_GetInstance();

void TdlRules_Init();
void TdlRules_Destroy();

#endif //OSTBOARDFIRMWARE_TDLRULES_H
