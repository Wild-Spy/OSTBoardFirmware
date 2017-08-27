//
// Created by mcochrane on 5/04/17.
//

#include "PeriodInterval.h"

PeriodInterval::PeriodInterval(uint8_t *data, uint8_t *byte_count) {
    uint8_t start_byte_count;
    uint8_t end_byte_count;
    start_ = Period::Decompile(data, &start_byte_count);
    data += start_byte_count;
    end_ = Period::Decompile(data, &end_byte_count);
    *byte_count = (start_byte_count + end_byte_count);
}

Interval PeriodInterval::toInterval(DateTime start_time_instant) const {
    return Interval(start_time_instant + start_, start_time_instant + end_);
}