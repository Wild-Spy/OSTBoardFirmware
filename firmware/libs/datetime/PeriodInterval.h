//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_PERIODINTERVAL_H
#define WS_OST_PERIODINTERVAL_H

#include "Period.h"
#include "DateTime.h"
#include "Interval.h"

class PeriodInterval {
public:

    PeriodInterval() {};

    PeriodInterval(Period start, Period end)
            : start_(start),
              end_(end)
    {}

    static PeriodInterval Decompile(uint8_t* data, uint8_t* byte_count)
    {
        return PeriodInterval(data, byte_count);
    }

    Interval toInterval(DateTime start_time_instant);

private:
    PeriodInterval(uint8_t* data, uint8_t* byte_count);

private:
    Period start_;
    Period end_;
};


#endif //WS_OST_PERIODINTERVAL_H
