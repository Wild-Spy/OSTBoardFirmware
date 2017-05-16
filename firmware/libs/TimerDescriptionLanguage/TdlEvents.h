//
// Created by mcochrane on 17/04/17.
//

#ifndef WS_OST_TDLEVENTS_H
#define WS_OST_TDLEVENTS_H

#include <simplib/array.h>
#include "TdlEvent.h"

class TdlEvents {
public:

    TdlEvents() {};

    void setCurrentEventAsProcessed() {
        last_event_ = current_event_;
        current_event_ = TdlEvent::Empty();
    }

    void setCurrentEvent(TdlEvent event) {
        current_event_ = event;
    }

    TdlEvent& getLastEvent() { return last_event_; };
    TdlEvent& getCurrentEvent() { return current_event_; };

private:
    TdlEvent last_event_;
    TdlEvent current_event_;
};

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC TdlEvents& TdlEvents_GetInstance();

#undef EXTERNC

#endif //WS_OST_TDLEVENTS_H
