//
// Created by mcochrane on 20/05/17.
//

#ifndef OSTBOARDFIRMWARE_DELAYMOCK_H
#define OSTBOARDFIRMWARE_DELAYMOCK_H

#include "gmock/gmock.h"

class DelayMockInterface {
public:
    DelayMockInterface() {};

    virtual void _delay_ms(double __ms) {};
    virtual void _delay_us(double __us) {};
};

class DelayMock : public DelayMockInterface {
public:
    DelayMock() {};
//    DelayMock( const DelayMock&) {};

    MOCK_METHOD1(_delay_ms, void(double __ms));
    MOCK_METHOD1(_delay_us, void(double __us));
};

void DelayMock_Reset();

void _delay_ms(double __ms);
void _delay_us(double __us);

#endif //OSTBOARDFIRMWARE_DELAYMOCK_H
