//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_TDLCHANNELS_H
#define WS_OST_TDLCHANNELS_H

#ifdef TESTING
#include "TdlChannelMock.h"
#elif MCU_TESTING
#include <tests/mocks/TdlChannel.h>
#include <cmocka.h>
#else
#include "TdlChannel.h"
#endif

#include "exception/CException.h"

#define CHANNEL_COUNT_MAX   4
#define CHANNEL_DEFAULT_STATE   TDLCHANNELSTATE_DISABLED

class TdlChannels {
public:

    TdlChannels(uint8_t channel_count, TdlChannelState_t default_state, Pin* pinList[]);

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

    #if defined(TESTING) || defined(MCU_TESTING)
    ~TdlChannels();
    #endif

private:
    uint8_t channel_count_;
    TdlChannel* channels_;//[CHANNEL_COUNT_MAX];
};


#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC TdlChannels& TdlChannels_GetInstance();
EXTERNC void TdlChannels_Init(uint8_t channels, TdlChannelState_t default_state, Pin* pins[]);

#if defined(TESTING) || defined(MCU_TESTING)
EXTERNC void TdlChannels_Destroy();
#endif

#undef EXTERNC


#endif //WS_OST_TDLCHANNELS_H
