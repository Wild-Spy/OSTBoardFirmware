//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_PERIOD_H
#define WS_OST_PERIOD_H

#include <stddef.h>
#include <stdint.h>
#include "PeriodField.h"
#include "DateTime.h"

#define PERIOD_MAX_FIELDS   2
#define INFINITE_PERIOD_YEARS   1000000

/**
 * An immutable time period specifying a set of duration field values.
 * <p>
 * A time period is divided into a number of fields, such as hours and seconds.
 * The standard period type supports years, months, weeks, days, hours, minutes,
 * seconds and millis.
 */
class Period {
public:
    Period();

    /**
     * @see Decompile below.
     */
    Period(uint8_t* data, uint8_t* bytes_consumed);

    /**
     *
     * @param data              [in] the data to be decompiled
     * @param bytes_consumed    [out] the number of bytes from the buffer that
     *                          were consumed
     * @return                  a Period object from the buffer
     * @throws EX_BUFFER_OVERRUN if the number of fields is > PERIOD_MAX_FIELDS
     * @throws
     */
    static Period Decompile(uint8_t* data, uint8_t* bytes_consumed);

    /**
     * Create a period with a single field.
     * @param value     the value to assign to the field
     * @param type      the field type (eg days, months, etc.)
     */
    static Period MakeCustomPeriod(int32_t value, PeriodFieldType_t type);

    int32_t getYears() const;
    int32_t getMonths() const;
    int32_t getWeeks() const;
    int32_t getDays() const;
    int32_t getHours() const;
    int32_t getMinutes() const;
    int32_t getSeconds() const;
    int32_t getMillis() const;

    /**
     * An infinite period is one where the years field is 1 million.
     * @return
     */
    bool isInfinite() const { return getYears() == INFINITE_PERIOD_YEARS; };
    bool isEmpty() const { return field_count_ == 0; };

    uint8_t getFieldCount() { return field_count_; };

//    PeriodFieldType_t getLongestFieldType();

    friend DateTime operator+(const DateTime& lhs, const Period& rhs);

    static Period Empty() {
        return Period();
    }

    static Period Infinite() {
        return Period::MakeCustomPeriod(INFINITE_PERIOD_YEARS, PERIODFIELD_YEARS);
    }

    bool operator==(const Period& rhs) const {
        if (rhs.field_count_ != this->field_count_) return false;
        for (uint8_t i = 0; i < rhs.field_count_; i++) {
            if (this->fields_[i] != rhs.fields_[i]) return false;
        }
        return true;
    }

    bool operator!=(const Period& rhs) const {
        return !(*this == rhs);
    }

protected:
    PeriodField* findFieldWithType(PeriodFieldType_t p_type);
    int32_t getValueOfField(PeriodFieldType_t p_type) const;

private:
    Period(int32_t value, PeriodFieldType_t type);

private:
    PeriodField fields_[PERIOD_MAX_FIELDS];
    uint8_t field_count_;
};

#endif //WS_OST_PERIOD_H
