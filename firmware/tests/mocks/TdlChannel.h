//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_TDLCHANNEL_H
#define WS_OST_TDLCHANNEL_H

#ifdef TESTING
#include "PinMock.h"
#else
#include "hal/Pin.h"
#endif

#include <stddef.h>
#include <libs/TimerDescriptionLanguage/TdlChannelState.h>
#include "../libs/cmocka/cmocka.h"

#define TDLCHANNEL_ID_NULL  -1

class TdlChannel {
public:

    TdlChannel()
            : id_(TDLCHANNEL_ID_NULL),
              state_(TDLCHANNELSTATE_DISABLED),
              initial_state_(TDLCHANNELSTATE_DISABLED),
//              port_(NULL),
              isEmpty_(true)
    {};

    TdlChannel(int id, TdlChannelState_t initial_state, Pin* pin)
            : id_(id),
              state_(initial_state),
              initial_state_(initial_state),
              isEmpty_(false)
    {};

    /**
     * Enables the channel (ie on)
     */
    void enable();

    /**
     * Disables the channel (ie off)
     */
    void disable();

    /**
     * Sets the state of the channel
     * @param state     The state to set the channel to (enabled/disabled)
     */
    void setState(TdlChannelState_t state);

    void setStateChangeCallback(void(*callback_ptr)(int)) { state_change_callback_ = callback_ptr; };

    void toggle();

    /**
     * Puts the channel back into it's initial state.
     */
    void reset() { setState(initial_state_); }

    TdlChannelState_t getState() { return state_; }

    int getId() { return id_; }

    bool isEmpty() { return isEmpty_; };

private:
    int id_;

    void(*state_change_callback_)(int);

    TdlChannelState_t state_;
    TdlChannelState_t initial_state_;
//    PORT_t* port_;
//    uint8_t pin_;
    bool isEmpty_;
};


#endif //WS_OST_TDLCHANNEL_H
