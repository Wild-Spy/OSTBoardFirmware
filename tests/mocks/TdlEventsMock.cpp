//
// Created by mcochrane on 5/06/17.
//

#include "TdlEventsMock.h"


#include <exception/CException.h>

static TdlEvents* events;

static bool ptr_is_null() {
    return (events == NULL);
}

TdlEvents& TdlEvents_GetInstance() {
    if (ptr_is_null()) Throw(EX_NULL_POINTER);
    return *events;
}

void TdlEvents_Init() {
    if (!ptr_is_null()) Throw(EX_ALREADY_INITIALISED);
    events = new TdlEvents();
}

void TdlEvents_Destroy() {
    if (ptr_is_null()) Throw(EX_NULL_POINTER);
    delete events;
    events = NULL;
}