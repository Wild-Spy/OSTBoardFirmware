//
// Created by mcochrane on 20/05/17.
//

#include "delayMock.h"
#include <stddef.h>

DelayMock* delayMock;

void DelayMock_Reset() {
    if (delayMock != NULL) {
//        delayMock->~DelayMock();
        delayMock = NULL;
    }
    delayMock = new DelayMock();
}

void _delay_ms(double __ms) {
    delayMock->_delay_ms(__ms);
}

void _delay_us(double __us) {
    delayMock->_delay_us(__us);
}

DelayMock* DelayMock_GetInstance() {
    return delayMock;
}