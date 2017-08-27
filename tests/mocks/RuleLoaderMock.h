//
// Created by mcochrane on 22/05/17.
//

#ifndef OSTBOARDFIRMWARE_RULELOADER_H
#define OSTBOARDFIRMWARE_RULELOADER_H

#include <gmock/gmock.h>

//class RuleLoaderMockInterface {
//public:
//    /**
//     *
//     * @param   writer the RuleWriter object
//     * @param   id the id of the rule to read
//     * @throws  EX_OUT_OF_RANGE
//     */
//    RuleLoader(uint8_t id, uint32_t eeprom_start_address, uint32_t eeprom_end_address, NvmDriverInterface& driver);
//
//    /**
//     * Gets the next rule.
//     * @throws EX_OUT_OF_RANGE
//     */
//    void getNextRule();
//
//    /**
//     * Reads the rule's data and stores it at the pointer passed in data
//     * Note: Use getLength to determine the length of the data that will be placed
//     *       into data parameter.
//     * @param data      pointer to the place where the rule data will be stored
//     */
//    void getData(uint8_t* data);
//
//    /**
//     * Gets the length of the rule in bytes.
//     * @return the rule's length in bytes
//     */
//    uint8_t getLength() { return length_; };
//
//    /**
//     * Gets the loaded rule's id.
//     * @return the rule's id
//     */
//    uint8_t getId() { return id_; };
//
//    uint16_t getNextRuleNvmAddress();
//};

class RuleLoaderlll { //}; : public RuleLoaderMockInterface {
public:

    RuleLoader(uint8_t id, uint32_t eeprom_start_address, uint32_t eeprom_end_address, NvmDriverInterface& driver) {

    }

    MOCK_METHOD0(getNextRule, void(void));
    MOCK_METHOD1(getData, void(uint8_t* data));
    MOCK_METHOD0(getLength, uint8_t(void));
    MOCK_METHOD0(getId, uint8_t(void));
    MOCK_METHOD0(getNextRuleNvmAddress, uint16_t(void));
};

#endif //OSTBOARDFIRMWARE_RULELOADER_H
