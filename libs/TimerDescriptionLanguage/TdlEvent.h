//
// Created by mcochrane on 6/04/17.
//

#ifndef WS_OST_TDLEVENT_H
#define WS_OST_TDLEVENT_H

#include <stdint-gcc.h>
#include <datetime/DateTime.h>

typedef uint8_t event_id_t;

#define EVENT_ID_NULL   (0x00)

class TdlEvent {
public:
    TdlEvent()
            : id_(EVENT_ID_NULL),
              time_()
    {};

    TdlEvent(event_id_t id, DateTime time)
            : id_(id),
              time_(time)
    {}

    event_id_t getId() { return id_; };
    DateTime getTime() { return time_; };

    bool isEmpty() { return id_ == EVENT_ID_NULL; };

    static TdlEvent Empty() { return TdlEvent(); };

private:
    event_id_t id_;
    DateTime time_;
};

#endif //WS_OST_TDLEVENT_H
