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
#include "TdlChannelState.h"

#define TDLCHANNEL_ID_NULL  -1

class TdlChannel {
public:

    TdlChannel()
            : id_(TDLCHANNEL_ID_NULL),
              state_(TDLCHANNELSTATE_DISABLED),
              initial_state_(TDLCHANNELSTATE_DISABLED),
//              port_(NULL),
              pin_(NULL)
    {};

//    TdlChannel(int id, TdlChannelState_t initial_state, PORT_t* port, uint8_t pin)
//            : rule_id(id),
//              initial_state_(initial_state),
//              state_(initial_state),
//              port_(port),
//              pin_(pin)
//    {
//        //Set up the pin as an output and put it in the correct state
//        port_->DIRSET = (uint8_t)(1<<pin_);
//        setState(initial_state);
//    };

    TdlChannel(int id, TdlChannelState_t initial_state, Pin* pin)
            : id_(id),
              state_(initial_state),
              initial_state_(initial_state),
              pin_(pin)
    {
        //Set up the pin as an output and put it in the correct state
//        port_->DIRSET = (uint8_t)(1<<pin_);
        pin_->setDirOutput();
        setState(initial_state);
    };

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

    void toggle();

    /**
     * Puts the channel back into it's initial state.
     */
    void reset() { setState(initial_state_); }

    TdlChannelState_t getState() { return state_; }

    int getId() { return id_; }

    bool isEmpty() { return pin_ == NULL; };

private:
    int id_;
    TdlChannelState_t state_;
    TdlChannelState_t initial_state_;
//    PORT_t* port_;
//    uint8_t pin_;
    Pin* pin_;
};


#endif //WS_OST_TDLCHANNEL_H
