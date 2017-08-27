//
// Created by mcochrane on 1/04/17.
//

#include "NvmRuleManager.h"
#include <string.h>

//#ifdef TESTING
//#include <new>
//#endif

static NvmRuleManager* nvmRuleManager = NULL;

/*
 * NvmRuleManager functions
 */

NvmRuleManager::NvmRuleManager(nvm_address_t nvm_start_address, nvm_address_t nvm_end_address, NvmDriverInterface& driver)
        : rule_count_(0),
          driver_(driver),
          nvm_start_address_(nvm_start_address),
          nvm_end_address_(nvm_end_address),
          rule_loader_(getFirstRuleAddress(), nvm_end_address_, &driver_)
{
    if (nvm_start_address_ >= nvm_end_address_) Throw(EX_OUT_OF_RANGE);
    rule_count_ = getRuleCountFromNvm();
    if (rule_count_ == 0) {
        new_rule_nvm_address_ = getFirstRuleAddress();
    } else {
        new_rule_nvm_address_ = getNewRuleNvmAddress();
    }
};

//TODO: want to invalidate rule_ somehow, have it be invalid on NvmRuleManager creation, then
//      in getRuleInfo, we check if it's invalidated or not, if so, create a new NvmRuleLoader,
//      if not, we might be able to use the current on.  We also want to invalidate when we
//      clear all rules and when we add a new rule.

uint8_t NvmRuleManager::getRuleCount() {
    return rule_count_;
}

uint8_t NvmRuleManager::addNewRule(uint8_t *buf, uint8_t length) {
    if (new_rule_nvm_address_ + 1 + length > nvm_end_address_) Throw(EX_OUT_OF_RANGE);

    driver_.write(&length, 1, new_rule_nvm_address_); // put length byte
    driver_.write(buf, length, new_rule_nvm_address_+1); // put data
    driver_.flush();

    rule_count_++;
    new_rule_nvm_address_ += (length + 1);
    return (uint8_t) (rule_count_ - 1);
}

nvm_address_t NvmRuleManager::getNewRuleNvmAddress() {
    NvmRuleLoader rule(rule_count_ - (uint8_t)1u,
                    getFirstRuleAddress(),
                    nvm_end_address_,
                    &driver_);
    return rule.getNextRuleNvmAddress();
}

uint8_t NvmRuleManager::getRuleCountFromNvm() {
    driver_.read(&(rule_count_), 1, getRuleCountAddress());
    if (rule_count_ == 0xFF) return 0;
    else return rule_count_;
}

void NvmRuleManager::getRuleData(uint8_t rule_id, uint8_t *data, uint8_t *length) {
    *length = getRuleInfo(rule_id).getLength();
    getRuleInfo(rule_id).getData(data);
}

NvmRuleLoader& NvmRuleManager::getRuleInfo(uint8_t rule_id) {
    if (rule_id >= rule_count_) Throw(EX_OUT_OF_RANGE);

    if (!rule_loader_.isInitialised()) {
        rule_loader_.getRule(rule_id);
        return rule_loader_;
    }

    if (rule_id == rule_loader_.getId()) {
        // do nothing
    } else if (rule_id == rule_loader_.getId() + 1) {
        rule_loader_.getNextRule();
    } else {
//        rule_loader_ = NvmRuleLoader(rule_id, getFirstRuleAddress(), nvm_end_address_, &driver_);
        rule_loader_.getRule(rule_id);
        //Or we could use in-place new (but this doesn't call the destructor)
//        new (&rule_loader_) NvmRuleLoader(rule_id, getFirstRuleAddress(), nvm_end_address_, &driver_);
    }

    return rule_loader_;
}

void NvmRuleManager::clearRules() {
    if (rule_count_ == 0) return; // Avoid extra writes to NVM if possible.
    rule_count_ = 0;
    saveRuleCountToNvm();
    new_rule_nvm_address_ = getFirstRuleAddress();
}

void NvmRuleManager::saveRuleCountToNvm() {
    driver_.write(&rule_count_, 1, getRuleCountAddress());
    driver_.flush();
}


/*
 * Non class functions
 */

void check_valid_pointer() {
    if (nvmRuleManager == NULL) Throw(EX_NOT_INITIALISED);
}

NvmRuleManager& NvmRuleManager_Get() {
    check_valid_pointer();
    return *nvmRuleManager;
}


/*
 * C accessible functions
 */

uint8_t NvmRuleManager_GetRuleCount() {
    return NvmRuleManager_Get().getRuleCount();
}

uint8_t NvmRuleManager_AddNewRule(uint8_t *buf, uint8_t length) {
    return NvmRuleManager_Get().addNewRule(buf, length);
}

void NvmRuleManager_ClearRules() {
    NvmRuleManager_Get().clearRules();
}

void NvmRuleManager_GetRuleData(uint8_t rule_id, uint8_t *data, uint8_t *length) {
    NvmRuleManager_Get().getRuleData(rule_id, data, length);
//    NvmRuleLoader rule = NvmRuleManager_Get().GetRule(rule_id);
//
//    *length = rule.GetLength();
//
//    memcpy(data, rule.GetData(), *length);
}

void NvmRuleManager_SaveRuleCount() {
    NvmRuleManager_Get().saveRuleCountToNvm();
}

void NvmRuleManager_Init(nvm_address_t nvm_start_address, nvm_address_t nvm_end_address, NvmDriverInterface& nvmDriver) {
    if (nvmRuleManager != NULL) Throw(EX_ALREADY_INITIALISED);
    nvmRuleManager = new NvmRuleManager(nvm_start_address, nvm_end_address, nvmDriver);
}

#ifdef TESTING
void NvmRuleManager_Destroy() {
    check_valid_pointer();
    delete nvmRuleManager;
    nvmRuleManager = NULL;
}
#endif
