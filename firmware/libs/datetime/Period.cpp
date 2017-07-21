//
// Created by mcochrane on 5/04/17.
//

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#if !defined(TESTING)
#include <min/min_transmit_cmds.h>
#endif

#include <exception/ExceptionValues.h>

#include "Period.h"

Period::Period()
        : field_count_(0)
{}

Period Period::Decompile(uint8_t *data, uint8_t* bytes_consumed) {
    return Period(data, bytes_consumed);
}

Period::Period(uint8_t *data, uint8_t* bytes_consumed) {
    *bytes_consumed = 0;
    field_count_ = *(data++);
//    while(1) {
//        report_printf("field_count_ = %u", field_count_);
//    }
    (*bytes_consumed) += 1;
    if (field_count_ > PERIOD_MAX_FIELDS) Throw(EX_BUFFER_OVERRUN);
    for (uint8_t i = 0; i < field_count_; i++) {
        uint8_t period_field_bytes;
        //TODO: check for invalid field types in PeriodField::Decompile
        fields_[i] = PeriodField::Decompile(data, &period_field_bytes);
        data += period_field_bytes;
        (*bytes_consumed) += period_field_bytes;
    }

}

PeriodField* Period::findFieldWithType(PeriodFieldType_t p_type) {
    for (uint8_t i = 0; i < PERIOD_MAX_FIELDS; i++) {
        if (fields_[i].getType() == p_type)
            return &fields_[i];
    }
    return NULL;
}

int32_t Period::getValueOfField(PeriodFieldType_t p_type) const {
    const PeriodField* field = const_cast<Period*>(this)->findFieldWithType(p_type);
    if (field == NULL) return 0;
    return field->getValue();
}

int32_t Period::getYears() const {
    return getValueOfField(PERIODFIELD_YEARS);
}

int32_t Period::getMonths()  const{
    return getValueOfField(PERIODFIELD_MONTHS);
}

int32_t Period::getWeeks() const {
    return getValueOfField(PERIODFIELD_WEEKS);
}

int32_t Period::getDays() const {
    return getValueOfField(PERIODFIELD_DAYS);
}

int32_t Period::getHours() const {
    return getValueOfField(PERIODFIELD_HOURS);
}

int32_t Period::getMinutes() const {
    return getValueOfField(PERIODFIELD_MINUTES);
}

int32_t Period::getSeconds() const {
    return getValueOfField(PERIODFIELD_SECONDS);
}

int32_t Period::getMillis() const {
    return getValueOfField(PERIODFIELD_MILLIS);
}

//void add_to_tm_field()

DateTime operator+(const DateTime &lhs, const Period &rhs) {
    DateTime ret_time = lhs;

    for (uint8_t i = 0; i < rhs.field_count_; i++) {
        const PeriodField& field = rhs.fields_[i];
        if (field.getType() == PERIODFIELD_YEARS) {
            ret_time = ret_time.plusYears((int16_t) field.getValue());
        } else if (field.getType() == PERIODFIELD_MONTHS) {
            ret_time = ret_time.plusMonths((int16_t) field.getValue());
        } else if (field.getType() == PERIODFIELD_WEEKS) {
            ret_time = ret_time.plusWeeks((int16_t) field.getValue());
        } else if (field.getType() == PERIODFIELD_DAYS) {
            ret_time = ret_time.plusDays((int16_t) field.getValue());
        } else if (field.getType() == PERIODFIELD_HOURS) {
            ret_time = ret_time.plusHours((int16_t) field.getValue());
        } else if (field.getType() == PERIODFIELD_MINUTES) {
            ret_time = ret_time.plusMinutes((int16_t) field.getValue());
        } else if (field.getType() == PERIODFIELD_SECONDS) {
            ret_time = ret_time.plusSeconds((int16_t) field.getValue());
        }
    }

    return ret_time;
}

Period Period::MakeCustomPeriod(int32_t value, PeriodFieldType_t type) {
    return Period(value, type);
}

Period::Period(int32_t value, PeriodFieldType_t type)
        : field_count_(1)
{
    fields_[0] = PeriodField(type, value);
}

//The approach below seems more intuitive so going with that for now...
///**
// * Returns the longest field type that exists in the Period.
// * For example if the field contains days weeks and years then
// * this will likely return PERIODFIELD_YEARS.
// * The value of the fields is taken into account. So if the
// * Period has 10 weeks and 1 month, PERIODFIELD_WEEKS will be
// * returned.
// * @return
// */
//PeriodFieldType_t Period::getLongestFieldType() {
//    PeriodFieldType_t longest_field = PERIODFIELD_NONE;
//    uint32_t longest_field_length_sec = 0;
//    // The longest field is years with a value of 0,
//    // the shortest field has the maximum value (ie value
//    // of the uint8_t in the PeriodFieldType_t.
//    for (uint8_t i = 0; i < field_count_; i++) {
//        uint32_t field_length_sec = fields_[i].get_std_length_sec();
//        if (field_length_sec > longest_field_length_sec) {
//            longest_field_length_sec = field_length_sec;
//            longest_field = fields_[i].getType();
//        }
//    }
//
//    return longest_field;
//}

/**
 * Returns the longest field type that exists in the Period.
 * For example if the field contains days weeks and years then
 * this will return PERIODFIELD_YEARS.
 * The value of the fields is not taken into account.  So if the
 * Period has 10 weeks and 1 month, PERIODFIELD_MONTHS will be
 * returned.
 * @return
 */
PeriodFieldType_t Period::getLongestFieldType() {
    PeriodFieldType_t longest_field = PERIODFIELD_NONE;
    // The longest field is years with a value of 0,
    // the shortest field has the maximum value (ie value
    // of the uint8_t in the PeriodFieldType_t.
    for (uint8_t i = 0; i < field_count_; i++) {
        if (fields_[i].getType() < longest_field)
            longest_field = fields_[i].getType();
    }

    return longest_field;
}
