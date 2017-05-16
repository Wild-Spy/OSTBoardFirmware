//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_TDLCHANNELS_H
#define WS_OST_TDLCHANNELS_H

#include "TdlChannel.h"
extern "C" {
    #include "exception/ExceptionValues.h"
};

#define CHANNEL_COUNT   4
#define CHANNEL_DEFAULT_STATE   TDLCHANNELSTATE_DISABLED

class TdlChannels {
public:

    TdlChannels(uint8_t channel_count, TdlChannelState_t default_state);

    void disableAll();

    /**
     * Gets a channel from it's index
     * @param index     the channel's index
     * @return  a reference to the channel
     * @throws  EX_OUT_OF_RANGE if the index is out of range
     */
    TdlChannel& get(int index);

    /**
     * Resets all channels back to their default states.
     */
    void resetStates();

private:
    uint8_t channel_count_;
    TdlChannel channels_[CHANNEL_COUNT];
};


#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC TdlChannels& TdlChannels_GetInstance();

#undef EXTERNC


#endif //WS_OST_TDLCHANNELS_H
