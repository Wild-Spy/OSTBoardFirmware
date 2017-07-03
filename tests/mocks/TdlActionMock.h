
#ifndef WS_OST_TDLACTION_H
#define WS_OST_TDLACTION_H

#include <stdint.h>
#include "gmock/gmock.h"

#include <TimerDescriptionLanguage/TdlActivator.h>
//#include "TimerDescriptionLanguage/TdlActivatorState.h"

#define TDLCHANNEL_ID_NULL  -1
typedef enum ActionState_enum {
    TDLACTIONSTATE_ACTIVE,
    TDLACTIONSTATE_INACTIVE
} TdlActionState_t;

class TdlAction { //: public TdlActionInterface {
public:
    TdlAction()
    {};

    TdlAction(TdlActivatorState_t activator_state_when_running, TdlActivator activator)
    {}

//    TdlChannel(int id, TdlChannelState_t initial_state, Pin* pin)
//        : TdlChannelInterface(id, initial_state, pin)
//    {}

    MOCK_METHOD1(start, void(DateTime));
    MOCK_METHOD1(stop, void(DateTime));
    MOCK_METHOD1(toggle, void(DateTime));

    MOCK_METHOD0(getActivatorStateWhenRunning, TdlActivatorState_t(void));
    MOCK_METHOD0(getState, TdlActionState_t(void));
    MOCK_METHOD0(getActivatorState, TdlActivatorState_t(void));
    MOCK_METHOD0(getActivatorType, TdlActivatorType_t(void));
    MOCK_METHOD0(getActivator, TdlActivator&(void));
    bool isEmpty() { return false; };

//    static TdlAction Decompile(uint8_t* data) { return TdlAction(); }
    static TdlAction Decompile(uint8_t* data, TdlAction* address_to_store) { new (address_to_store) TdlAction(); }

    static const uint8_t ACTION_TARGET_CHANNEL = 0;
    static const uint8_t ACTION_TARGET_RULE = 1;
    static const uint8_t ACTION_COMPILED_LENGTH = 5;
};

#endif //WS_OST_TDLACTION_H
