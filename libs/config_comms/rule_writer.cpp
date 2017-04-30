//
// Created by mcochrane on 1/04/17.
//

#include <min/min_transmit_cmds.h>
#include "ruler_writer_cpp.h"

extern "C" {
#include "eeprom_driver.h"
#include "exception/ExceptionValues.h"
#include <string.h>
}

#define RULE_ZERO_EEPROM_ADDRESS  1

static RuleWriter ruleWriter = RuleWriter();



/*
 * RuleWriterRule functions
 */

//RuleWriterRule::RuleWriterRule(RuleWriter writer)
//        : writer_(writer),
//          valid_(false) {
//}

RuleWriterRule::RuleWriterRule(RuleWriter& writer, uint8_t id)
        : writer_(writer),
          have_read_data_(false)
{
    this->id_ = 0;
    this->eeprom_address_ = RULE_ZERO_EEPROM_ADDRESS;
    ReadHeaderFromEeprom();

    // Keep getting the next target until we get to the one we want.
    while (this->id_ < id)
        this->GetNextRule();
}

void RuleWriterRule::ReadDataFromEeprom() {
    ReadEEPROM(this->data_, this->length_, this->eeprom_address_+1);
}

void RuleWriterRule::ReadHeaderFromEeprom() {
    ReadEEPROM(&this->length_, 1, this->eeprom_address_);
}

void RuleWriterRule::GetNextRule() {
    if (this->id_+1 >= this->writer_.GetRuleCount())
        Throw(EX_OUT_OF_RANGE);
    this->eeprom_address_ = this->GetNextRuleEepromAddress();
    this->id_++;
    this->ReadHeaderFromEeprom();
    this->have_read_data_ = false;
}

uint16_t RuleWriterRule::GetNextRuleEepromAddress() {
    return this->eeprom_address_ + (this->length_ + 1);
}

//void RuleWriterRule::GetPreviousRule() {
//    this->eeprom_address_ -= (this->previous_rule_length_ + 1);
//    //... readheader, valid = false
//}

uint8_t* RuleWriterRule::GetData() {
    if (!this->have_read_data_)
        this->ReadDataFromEeprom();
    return this->data_;
}

uint8_t RuleWriterRule::GetId() {
    return this->id_;
}

uint8_t RuleWriterRule::GetLength() {
    return this->length_;
}


/*
 * RuleWriter functions
 */

RuleWriterRule RuleWriter::GetRule(uint8_t id) {
    if (id >= rule_count_)
        Throw(EX_OUT_OF_RANGE);
    return RuleWriterRule(*this, id);
}

uint8_t RuleWriter::GetRuleCount() {
    return rule_count_;
}

uint8_t RuleWriter::AddNewRule(uint8_t *buf, uint8_t length) {
    WriteEEPROM(&length, 1, this->new_rule_eeprom_address_); // put length byte
    EEPROM_FlushBuffer();
    WriteEEPROM(buf, length, this->new_rule_eeprom_address_+1); // put data
    EEPROM_FlushBuffer();

    this->rule_count_++;
    this->new_rule_eeprom_address_ += (length + 1);
    return this->rule_count_-1;
}

uint16_t RuleWriter::GetNewRuleEepromAddress() {
    RuleWriterRule rule = GetRule(this->rule_count_-1);
    return rule.GetNextRuleEepromAddress();
}

uint8_t RuleWriter::GetRuleCountFromEeprom() {
    uint8_t rule_count = EEPROM_ReadByte(0, 0);
    if (rule_count == 0xFF) return 0;
    else return rule_count;
}

void RuleWriter::ClearRules() {
    this->rule_count_ = 0;
    this->SaveRuleCountToEeprom();
    new_rule_eeprom_address_ = RULE_ZERO_EEPROM_ADDRESS;
}

void RuleWriter::SaveRuleCountToEeprom() {
    EEPROM_WriteByte(0,0, this->rule_count_);
}

RuleWriter::RuleWriter(uint8_t start) {
    rule_count_ = this->GetRuleCountFromEeprom();
    if (rule_count_ == 0) {
        new_rule_eeprom_address_ = RULE_ZERO_EEPROM_ADDRESS;
    } else {
        new_rule_eeprom_address_ = this->GetNewRuleEepromAddress();
    }
}

RuleWriter &RuleWriter_GetRuleWriter() {
    return ruleWriter;
}

/*
 * C accessible functions
 */

uint8_t RuleWriter_GetRuleCount() {
    return ruleWriter.GetRuleCount();
}

uint8_t RuleWriter_AddNewRule(uint8_t *buf, uint8_t length) {
    return ruleWriter.AddNewRule(buf, length);
}

void RuleWriter_ClearRules() {
    ruleWriter.ClearRules();
}

void RuleWriter_GetRuleData(uint8_t rule_id, uint8_t* data, uint8_t* length) {
    RuleWriterRule rule = ruleWriter.GetRule(rule_id);

    *length = rule.GetLength();

    memcpy(data, rule.GetData(), *length);
}

void RuleWriter_SaveRuleCount() {
    ruleWriter.SaveRuleCountToEeprom();
}

void RuleWriter_Init() {
    ruleWriter = RuleWriter(1);
}

