//
// Created by mcochrane on 5/06/17.
//

#include "TdlRulesMock.h"
#include <exception/CException.h>

static TdlRules* rules;

static bool ptr_is_null() {
    return (rules == NULL);
}

TdlRules& TdlRules_GetInstance() {
    if (ptr_is_null()) Throw(EX_NULL_POINTER);
    return *rules;
}

void TdlRules_Init() {
    if (!ptr_is_null()) Throw(EX_ALREADY_INITIALISED);
    rules = new TdlRules();
}

void TdlRules_Destroy() {
    if (ptr_is_null()) Throw(EX_NULL_POINTER);
    delete rules;
    rules = NULL;
}
