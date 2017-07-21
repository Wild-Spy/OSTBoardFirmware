//
// Created by mcochrane on 17/04/17.
//

#include "TdlEvents.h"

TdlEvents events;

TdlEvents &TdlEvents_GetInstance() {
    return events;
}

void TdlEvents_ResetInstance() {
    events = TdlEvents();
}
