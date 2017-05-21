//
// Created by mcochrane on 1/04/17.
//

#ifndef WS_OST_RULER_WRITER_H
#define WS_OST_RULER_WRITER_H

#include <stdint.h>

#ifdef __cplusplus
class RuleWriter;

class RuleWriterRule {
public:
//    RuleWriterRule(RuleWriter writer);
    /**
     *
     * @param   writer the RuleWriter object
     * @param   id the id of the rule to read
     * @throws  EX_OUT_OF_RANGE
     */
    RuleWriterRule(RuleWriter& writer, uint8_t id);

    /**
     * Gets the next rule.
     * @throws EX_OUT_OF_RANGE
     */
    void GetNextRule();
//    void GetPreviousRule();

    /**
     * Returns a pointer to the rule's data.
     * If the data hasn't been read from eeprom yet, this
     * function reads it into memory, which could take
     * a while.
     * @return a pointer to the data
     */
    uint8_t* GetData();

    /**
     * Gets the length of the rule in bytes.
     * @return the rule's length in bytes
     */
    uint8_t GetLength();

    /**
     * Gets the loaded rule's id.
     * @return the rule's id
     */
    uint8_t GetId();

    uint16_t GetNextRuleEepromAddress();

protected:
    void ReadDataFromEeprom();
    void ReadHeaderFromEeprom();

private:
    bool have_read_data_;
    uint8_t id_;
    uint8_t length_;
    RuleWriter& writer_;
    uint8_t data_[128];
    uint16_t eeprom_address_;
};

class RuleWriter {
public:
    RuleWriter() : rule_count_(0) {};

    RuleWriter(uint8_t start);

    /**
     * Get's the number of rules currently saved to
     * the EEPROM.
     * @return  number of rules in eeprom
     */
    uint8_t GetRuleCount();

    /**
     * Adds a new rule to the EEPROM.  The rule is saved
     * after the last rule that currently exists.
     * @param buf       the rule's data
     * @param length    the length of the rule's data
     *                  (not includeing length byte)
     * @return          the new rule's id
     */
    uint8_t AddNewRule(uint8_t buf[], uint8_t length);
//    void DeleteRule(RuleWriterRule rule);

    /**
     * Retrieves a rule from EEPROM.
     * @param   id the id of the rule to retrieve
     * @return  the rule as a RuleWriterRule object
     */
    RuleWriterRule GetRule(uint8_t id);


    /**
     * Clears all the rules in the EEPROM
     */
    void ClearRules();

    /**
     * Saves the rule count to EEPROM.
     * Should be called after adding a number of new rules.
     */
    void SaveRuleCountToEeprom();

protected:
    uint16_t GetNewRuleEepromAddress();
    uint8_t GetRuleCountFromEeprom();


private:
    uint16_t new_rule_eeprom_address_;
    uint8_t rule_count_;


};

RuleWriter& RuleWriter_GetRuleWriter();

#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


EXTERNC void RuleWriter_Init();
EXTERNC uint8_t RuleWriter_GetRuleCount();
EXTERNC uint8_t RuleWriter_AddNewRule(uint8_t *buf, uint8_t length);
EXTERNC void RuleWriter_ClearRules();
EXTERNC void RuleWriter_SaveRuleCount();

/**
 *
 * @param rule_id
 * @param data
 * @param length
 * @throws EX_OUT_OF_RANGE
 */
EXTERNC void RuleWriter_GetRuleData(uint8_t rule_id, uint8_t* data, uint8_t* length);

#undef EXTERNC

#endif //WS_OST_RULER_WRITER_H
