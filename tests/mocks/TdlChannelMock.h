
#ifndef WS_OST_TDLCHANNEL_H
#define WS_OST_TDLCHANNEL_H

#include <stdint.h>
#include "gmock/gmock.h"

#include "SimplePinMock.h"

#include "TimerDescriptionLanguage/TdlChannelState.h"

#define TDLCHANNEL_ID_NULL  -1

class TdlChannelInterface {
public:
    TdlChannelInterface()
            : id_(TDLCHANNEL_ID_NULL),
              state_(TDLCHANNELSTATE_DISABLED),
              initial_state_(TDLCHANNELSTATE_DISABLED),
              pin_(0)
    {};

    TdlChannelInterface(int id, TdlChannelState_t initial_state, Pin* pin)
            : id_(id),
              initial_state_(initial_state),
              state_(initial_state),
              pin_(pin)
    {
        //Set up the pin as an output and put it in the correct state
//        pin_->setDirOutput();
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
    void reset() { setState(initial_state_); resetCalled(); }

    TdlChannelState_t getState() { return state_; }

    int getId() { return id_; }
    Pin* getPin() { return pin_; }

    bool isEmpty() { return pin_ == 0; };

    virtual void enableCalled() {};
    virtual void disableCalled() {};
    virtual void resetCalled() {};

private:
    int id_;
    TdlChannelState_t state_;
    TdlChannelState_t initial_state_;
    Pin* pin_;
};

class TdlChannel : public TdlChannelInterface {
public:
    TdlChannel()
        : TdlChannelInterface()
    {};

    TdlChannel(int id, TdlChannelState_t initial_state, Pin* pin)
        : TdlChannelInterface(id, initial_state, pin)
    {}

    TdlChannel(int id, TdlChannelState_t initial_state)
        : default_pin(1),
          TdlChannelInterface(id, initial_state, &default_pin)
    {}

    MOCK_METHOD0(enableCalled, void(void));
    MOCK_METHOD0(disableCalled, void(void));
    MOCK_METHOD0(resetCalled, void(void));

private:
    Pin default_pin;

};

#endif //WS_OST_TDLCHANNEL_H
