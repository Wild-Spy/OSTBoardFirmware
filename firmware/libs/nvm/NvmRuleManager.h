//
// Created by mcochrane on 1/04/17.
//

#ifndef WS_OST_RULER_WRITER_H
#define WS_OST_RULER_WRITER_H

#include <stdint.h>
#include "exception/ExceptionValues.h"

#ifdef __cplusplus

#include "NvmDriverInterface.h"
#include "NvmRuleLoader.h"

/**
 * This class handles saving and loading rules to/from NVM.
 *
 * When the class is constructed it reads the rule count from the NVM.
 *
 * When a rule is read the class fetches it from the NVM.  Rules are
 * stored in NVM as a forward linked list.
 *
 * As the number of rules is likely to be small and read infrequently, the
 * NvmRuleManager does not keep a table of rule addresses. Therefore to retrieve
 * a rule from NVM, the it must traverse the list from the start.
 *
 * The manager keeps a link to the last node read so reading read that node again
 * or reading the next node, is cheap.  Ie. the manager doesn't need to traverse
 * all previous nodes to find it's address.
 * This makes reading out rules sequentially fast but reading out rules in reverse
 * order expensive.
 *
 * The list of saved rules can only be appended to or cleared.  Rules cannot be
 * deleted individually.
 *
 * The manager is bounded by it's start and end address in NVM, it will throw an
 * exception if it attempts to read/write outside of this range.
 *
 * This means that a specific region can be allocated for storing rules and the
 * user can be confident that the NvmRuleManager will not operate outside that
 * region, corrupting other data or reading invalid data.
 */
class NvmRuleManager {
public:
    /**
     * Creates an NvmRuleManager bound by the specified region
     * @param nvm_start_address
     * @param nvm_end_address
     */
    NvmRuleManager(nvm_address_t nvm_start_address, nvm_address_t nvm_end_address, NvmDriverInterface& driver);

    /**
     * Get's the number of rules currently saved to
     * the NVM.
     * @return  number of rules in NVM
     */
    uint8_t getRuleCount();

    /**
     * Adds a new rule to the NVM.  The rule is saved
     * after the last rule that currently exists.
     * @param buf       the rule's data
     * @param length    the length of the rule's data
     *                  (not including the length byte)
     * @return          the new rule's id
     */
    uint8_t addNewRule(uint8_t buf[], uint8_t length);

    /**
     * Retrieves a rule from NVM.
     * @param   id the id of the rule to retrieve
     * @return  the rule as a RuleWriterRule object
     */
    void getRuleData(uint8_t rule_id, uint8_t* data, uint8_t* length);

    /**
     * Clears all the rules in the NVM.
     */
    void clearRules();

    /**
     * Saves the rule count to NVM.
     * Should be called after adding a number of new rules.
     */
    void saveRuleCountToNvm();

#ifdef TESTING
    nvm_address_t getNewRuleNvmAddressVar() { return new_rule_nvm_address_; };
#endif

protected:
    uint8_t getRuleCountFromNvm();
    nvm_address_t getNewRuleNvmAddress();
    nvm_address_t getRuleCountAddress() { return nvm_start_address_; };
    nvm_address_t getFirstRuleAddress() { return nvm_start_address_ + 1; };
    NvmRuleLoader& getRuleInfo(uint8_t rule_index);

private:
    nvm_address_t new_rule_nvm_address_;
    uint8_t rule_count_;
    nvm_address_t nvm_start_address_;
    nvm_address_t nvm_end_address_;
    NvmDriverInterface& driver_;
    NvmRuleLoader rule_loader_;
};

NvmRuleManager& NvmRuleManager_Get();

#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


EXTERNC void NvmRuleManager_Init(nvm_address_t nvm_start_address, nvm_address_t nvm_end_address, NvmDriverInterface& nvmDriver);
EXTERNC uint8_t NvmRuleManager_GetRuleCount();
EXTERNC uint8_t NvmRuleManager_AddNewRule(uint8_t *buf, uint8_t length);
EXTERNC void NvmRuleManager_ClearRules();
EXTERNC void NvmRuleManager_SaveRuleCount();

/**
 *
 * @param rule_id
 * @param data
 * @param length
 * @throws EX_OUT_OF_RANGE
 */
EXTERNC void NvmRuleManager_GetRuleData(uint8_t rule_id, uint8_t *data, uint8_t *length);

#ifdef TESTING
EXTERNC void NvmRuleManager_Destroy();
#endif

#undef EXTERNC

#endif //WS_OST_RULER_WRITER_H
