//
// Created by mcochrane on 15/04/17.
//

#ifndef WS_OST_PERIODFIELD_H
#define WS_OST_PERIODFIELD_H

#include "min/encode_decode.h"

typedef enum PeriodFieldType_enum {
    PERIODFIELD_YEARS = 0,
    PERIODFIELD_MONTHS = 1,
    PERIODFIELD_WEEKS = 2,
    PERIODFIELD_DAYS = 3,
    PERIODFIELD_HOURS = 4,
    PERIODFIELD_MINUTES = 5,
    PERIODFIELD_SECONDS = 6,
    PERIODFIELD_MILLIS = 7,
    PERIODFIELD_NONE = 255
} PeriodFieldType_t;

#define PERIODFIELD_MILLIS_STD_SEC      0
#define PERIODFIELD_SECONDS_STD_SEC     1
#define PERIODFIELD_MINUTES_STD_SEC     60
#define PERIODFIELD_HOURS_STD_SEC       3600
#define PERIODFIELD_DAYS_STD_SEC        (24UL*PERIODFIELD_HOURS_STD_SEC)
#define PERIODFIELD_WEEKS_STD_SEC       (7UL*PERIODFIELD_DAYS_STD_SEC)
#define PERIODFIELD_MONTHS_STD_SEC      (31UL*PERIODFIELD_DAYS_STD_SEC)
#define PERIODFIELD_YEARS_STD_SEC       (365UL*PERIODFIELD_DAYS_STD_SEC)
#define PERIODFIELD_NONE_STD_SEC        0

inline uint32_t get_std_period_type_length_secs(PeriodFieldType_t type) {
    switch (type) {
        case PERIODFIELD_MILLIS: return PERIODFIELD_MILLIS_STD_SEC;
        case PERIODFIELD_SECONDS: return PERIODFIELD_SECONDS_STD_SEC;
        case PERIODFIELD_MINUTES: return PERIODFIELD_MINUTES_STD_SEC;
        case PERIODFIELD_HOURS: return PERIODFIELD_HOURS_STD_SEC;
        case PERIODFIELD_DAYS: return PERIODFIELD_DAYS_STD_SEC;
        case PERIODFIELD_WEEKS: return PERIODFIELD_WEEKS_STD_SEC;
        case PERIODFIELD_MONTHS: return PERIODFIELD_MONTHS_STD_SEC;
        case PERIODFIELD_YEARS: return PERIODFIELD_YEARS_STD_SEC;
        default: return PERIODFIELD_NONE_STD_SEC;
    }
}

class PeriodField {
public:
    PeriodField()
            : type_(PERIODFIELD_NONE),
              value_(0)
    {};

    PeriodField(PeriodFieldType_t type, int32_t value)
            : type_(type),
              value_(value)
    {};

    //data points to the start of the PeriodField
    PeriodField(uint8_t* data, uint8_t* bytes_consumed) {
        type_ = (PeriodFieldType_t)data[0];
        value_ = decode_32(data+1);
        *bytes_consumed = 5;
    };

    static PeriodField Decompile(uint8_t* data, uint8_t* bytes_consumed) {
        return PeriodField(data, bytes_consumed);
    };

    uint32_t get_std_length_sec() {
        return value_*get_std_period_type_length_secs(type_);
    }

    PeriodFieldType_t getType() const { return type_; };
    int32_t getValue() const { return value_; };

private:
    PeriodFieldType_t type_;
    int32_t value_;
};

#endif //WS_OST_PERIODFIELD_H
