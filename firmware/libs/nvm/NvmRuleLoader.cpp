//
// Created by mcochrane on 22/05/17.
//

#include "NvmRuleLoader.h"

/*
 * NvmRuleLoader functions
 */

NvmRuleLoader::NvmRuleLoader(nvm_address_t nvm_start_address, nvm_address_t nvm_end_address, NvmDriverInterface *driver)
        : initialised_(false),
          nvm_start_address_(nvm_start_address),
          nvm_end_address_(nvm_end_address),
          driver_(driver)
{
}

NvmRuleLoader::NvmRuleLoader(uint8_t id, nvm_address_t nvm_start_address, nvm_address_t nvm_end_address, NvmDriverInterface* driver)
        : initialised_(false),
          nvm_start_address_(nvm_start_address),
          nvm_end_address_(nvm_end_address),
          driver_(driver)
{
    getRule(id);
}

void NvmRuleLoader::readHeaderFromNvm() {
    driver_->read(&length_, 1, nvm_address_);
    if (length_ == 0xFF) initialised_ = false;
}

void NvmRuleLoader::getNextRule() {
    nvm_address_ = getNextRuleNvmAddress();
    rule_id_++;
    readHeaderFromNvm();
}

nvm_address_t NvmRuleLoader::getNextRuleNvmAddress() {
    return (nvm_address_t) (nvm_address_ + (length_ + 1));
}

void NvmRuleLoader::getData(uint8_t* data) {
    driver_->read(data, length_, (uint16_t) (nvm_address_ + 1));
}

void NvmRuleLoader::getRule(uint8_t rule_id) {
    rule_id_ = 0;
    nvm_address_ = nvm_start_address_;
    initialised_ = true;
    readHeaderFromNvm();

    // Keep getting the next target until we get to the one we want.
    while (rule_id_ < rule_id && initialised_)
        getNextRule();
}