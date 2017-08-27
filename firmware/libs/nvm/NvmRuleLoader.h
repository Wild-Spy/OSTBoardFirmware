//
// Created by mcochrane on 22/05/17.
//

#ifndef OSTBOARDFIRMWARE_RULELOADER_H
#define OSTBOARDFIRMWARE_RULELOADER_H

#include <stdint.h>
#include "NvmDriverInterface.h"

class NvmRuleManager;

class NvmRuleLoader {
public:

    /**
     *
     * @param nvm_start_address
     * @param nvm_end_address
     * @param driver
     */
    NvmRuleLoader(nvm_address_t nvm_start_address, nvm_address_t nvm_end_address, NvmDriverInterface* driver);

    /**
     *
     * @param rule_id
     * @param nvm_start_address
     * @param nvm_end_address
     * @param driver
     * @throws  EX_OUT_OF_RANGE
     */
    NvmRuleLoader(uint8_t rule_id, nvm_address_t nvm_start_address, nvm_address_t nvm_end_address, NvmDriverInterface* driver);

    /**
     * Gets the rule specified by rule_id.
     * @param rule_id   the id of the rule to point to
     */
    void getRule(uint8_t rule_id);

    /**
     * Gets the next rule.
     * @throws EX_OUT_OF_RANGE
     */
    void getNextRule();

    /**
     * Reads the rule's data and stores it at the pointer passed in data
     * Note: Use getLength to determine the length of the data that will be placed
     *       into data parameter.
     * @param data      pointer to the place where the rule data will be stored
     */
    void getData(uint8_t* data);

    /**
     * Gets the length of the rule in bytes.
     * @return the rule's length in bytes
     */
    uint8_t getLength() { return length_; };

    /**
     * Gets the loaded rule's id.
     * @return the rule's id
     */
    uint8_t getId() { return rule_id_; };

    nvm_address_t getNextRuleNvmAddress();

    /**
     * @return true if this object is initialised
     */
    bool isInitialised() { return initialised_; };

protected:
    void readHeaderFromNvm();

private:
    bool initialised_;
    uint8_t rule_id_;
    uint8_t length_;
    nvm_address_t nvm_address_;
    nvm_address_t nvm_start_address_;
    nvm_address_t nvm_end_address_;
    NvmDriverInterface* driver_;
};

#endif //OSTBOARDFIRMWARE_RULELOADER_H
