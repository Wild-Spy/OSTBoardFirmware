//
// Created by mcochrane on 21/05/17.
//

#ifndef WS_OST_RULER_WRITER_H
#define WS_OST_RULER_WRITER_H

#include <gmock/gmock.h>
#include <exception/CException.h>
#include <stdint.h>

#ifdef __cplusplus
//class NvmRuleManager;

//class RuleWriterRuleInterface {
//public:
//    /**
//     *
//     * @param   writer the NvmRuleManager object
//     * @param   id the id of the rule to read
//     * @throws  EX_OUT_OF_RANGE
//     */
//    NvmRuleLoader(NvmRuleManager& writer, uint8_t id);
//
//    /**
//     * Gets the next rule.
//     * @throws EX_OUT_OF_RANGE
//     */
//    void GetNextRule();
////    void GetPreviousRule();
//
//    /**
//     * Returns a pointer to the rule's data.
//     * If the data hasn't been read from eeprom yet, this
//     * function reads it into memory, which could take
//     * a while.
//     * @return a pointer to the data
//     */
//    uint8_t* GetData();
//
//    /**
//     * Gets the length of the rule in bytes.
//     * @return the rule's length in bytes
//     */
//    uint8_t GetLength();
//
//    /**
//     * Gets the loaded rule's id.
//     * @return the rule's id
//     */
//    uint8_t GetId();
//
//    uint16_t GetNextRuleEepromAddress();
//
//protected:
//    void ReadDataFromEeprom();
//    void ReadHeaderFromEeprom();
//
//private:
//    bool have_read_data_;
//    uint8_t rule_id;
//    uint8_t length_;
//    NvmRuleManager& writer_;
//    uint8_t data_[128];
//    uint16_t nvm_address_;
//};

class NvmRuleManagerMockInterface {
public:
    NvmRuleManagerMockInterface() {};

    /**
     * Initialises the rule writer.  Gets the rule count from EEPROM, etc.
     */
    void initialise();

    /**
     * @return true if the class has been initialised
     */
    bool initialised();

    /**
     * Get's the number of rules currently saved to
     * the EEPROM.
     * @return  number of rules in eeprom
     */
    uint8_t getRuleCount();

    /**
     * Adds a new rule to the EEPROM.  The rule is saved
     * after the last rule that currently exists.
     * @param buf       the rule's data
     * @param length    the length of the rule's data
     *                  (not includeing length byte)
     * @return          the new rule's id
     */
    uint8_t addNewRule(uint8_t buf[], uint8_t length);

    /**
     * Retrieves a rule from EEPROM.
     * @param   id the id of the rule to retrieve
     * @return  the rule as a RuleWriterRule object
     */
    void getRuleData(uint8_t id, uint8_t* data, uint8_t* length);


    /**
     * Clears all the rules in the EEPROM
     */
    void clearRules();

    /**
     * Saves the rule count to EEPROM.
     * Should be called after adding a number of new rules.
     */
    void saveRuleCountToEeprom();
};

class NvmRuleManager : public NvmRuleManagerMockInterface {
public:
    NvmRuleManager() {};

    MOCK_METHOD0(getRuleCount, uint8_t(void));
    MOCK_METHOD2(addNewRule, uint8_t(uint8_t* buf, uint8_t length));
    MOCK_METHOD3(getRuleData, void(uint8_t id, uint8_t* data, uint8_t* length));
    MOCK_METHOD0(clearRules, void(void));

    MOCK_METHOD0(saveRuleCountToNvm, void(void));
    MOCK_METHOD0(initialise, void(void));
    MOCK_METHOD0(initialised, bool(void));
};

NvmRuleManager& NvmRuleManager_Get();

#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


EXTERNC void NvmRuleManager_Init();
EXTERNC uint8_t NvmRuleManager_GetRuleCount();
EXTERNC uint8_t NvmRuleManager_AddNewRule(uint8_t *buf, uint8_t length);
EXTERNC void NvmRuleManager_ClearRules();
EXTERNC void NvmRuleManager_SaveRuleCount();

// Not available in the real NvmRuleManager, only for testing
EXTERNC void NvmRuleManager_Destroy();

/**
 *
 * @param rule_id
 * @param data
 * @param length
 * @throws EX_OUT_OF_RANGE
 */
EXTERNC void NvmRuleManager_GetRuleData(uint8_t rule_id, uint8_t *data, uint8_t *length);

#undef EXTERNC

#endif //WS_OST_RULER_WRITER_H

