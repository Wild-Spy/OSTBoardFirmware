//
// Created by mcochrane on 17/04/17.
//

#ifndef WS_OST_TDLEVENTS_H
#define WS_OST_TDLEVENTS_H

#include <simplib/array.h>
#include "TdlEvent.h"
#include "TdlRuleRunner.h"

class TdlEvents {
public:

    TdlEvents() {};

    void setCurrentEventAsProcessed() {
        last_event_ = unprocessed_event;
        unprocessed_event = TdlEvent::Empty();
    }

    /**
     * Call this function to trigger an event.
     * This function can be called from an interrupt.
     * @param event     the even that just occurred
     */
    void triggerEvent(TdlEvent event) {
        unprocessed_event = event;
        // Don't force an update here (ie by calling TdlRuleRunner_GetInstance().mainLoopCallback()).
        // The rule runner will trigger the event next time mainLoopCallback is called.
    }

    TdlEvent& getLastEvent() { return last_event_; };
    TdlEvent& getCurrentEvent() { return unprocessed_event; };

    /**
     * Returns true if there is an unprocessed event.
     * @return
     */
    bool isUnprocessedEvent() { return !unprocessed_event.isEmpty(); };

private:
    TdlEvent last_event_;
    TdlEvent unprocessed_event;
};

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC TdlEvents& TdlEvents_GetInstance();
EXTERNC void TdlEvents_ResetInstance();

#undef EXTERNC

#endif //WS_OST_TDLEVENTS_H
