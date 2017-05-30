//
// Created by mcochrane on 5/04/17.
//

#include "TdlChannels.h"
#include <new>

static TdlChannels* tdl_channels = NULL;

/**
 * Setup TdlChannels
 * Note the first Pin in pinList will correspond to the channel with id 0,
 * the next wil correspond to the channel with id 1, and so on.
 * @param channel_count     the number of channels that exist
 * @param default_state     the default state of all channels
 * @param pinList           an array of Pins specifying the pin for each channel
 */
TdlChannels::TdlChannels(uint8_t channel_count, TdlChannelState_t default_state, Pin* pinList[])
        : channel_count_(channel_count)
{
    channels_ = (TdlChannel*)malloc(sizeof(TdlChannel)*channel_count);

    for (uint8_t i = 0; i < channel_count_; i++) {
        // Creating these in-place with the placement new operator
        new (channels_+i) TdlChannel(i, default_state, pinList[i]);
    }
}

void TdlChannels::disableAll() {
    for (uint8_t i = 0; i < channel_count_; i++) {
        get(i).disable();
    }
}

TdlChannel& TdlChannels::get(int index) {
    if (index < 0 || index >= channel_count_) Throw(EX_OUT_OF_RANGE);
    return channels_[index];
}

void TdlChannels::resetStates() {
    for (uint8_t i = 0; i < channel_count_; i++) {
        get(i).reset();
    }
}

#if defined(TESTING)
TdlChannels::~TdlChannels() {
    for (uint8_t i = 0; i < channel_count_; i++) {
        // These were created in-place with the placement new operator so
        // we need to explicitly call their destructor when we destroy then.
        // Note: This is very necessary as TdlChannel is mocked at this point.
        channels_[i].~TdlChannel();
    }
    free(channels_);
}
#endif

TdlChannels& TdlChannels_GetInstance() {
    if (tdl_channels == NULL) Throw(EX_NULL_POINTER);
    return *tdl_channels;
}

void TdlChannels_Init(uint8_t channels, TdlChannelState_t default_state, Pin* pins[]) {
    if (channels > CHANNEL_COUNT_MAX) Throw(EX_DATA_OVERFLOW);
    tdl_channels = new TdlChannels(channels, default_state, pins);
}

#ifdef TESTING
void TdlChannels_Destroy() {
    delete tdl_channels;
    tdl_channels = NULL;
}
#endif