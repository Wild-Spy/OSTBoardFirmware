//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_INTERVAL_H
#define WS_OST_INTERVAL_H

#include "DateTime.h"

class Interval {
public:
    Interval()
            : start_(DateTime::Empty()),
              end_(DateTime::Empty())
    { };

    Interval(DateTime start, DateTime end)
            : start_(start),
              end_(end)
    { };

    DateTime getStart() { return start_; };
    DateTime getEnd() { return end_; };

    bool isEmpty() {
        return start_.isEmpty() && end_.isEmpty();
    }

    int32_t getDurationSeconds() {
        return end_ - start_;
    }

    static Interval Empty() {
        return Interval();
    }

private:
    DateTime start_;
    DateTime end_;
};


#endif //WS_OST_INTERVAL_H
