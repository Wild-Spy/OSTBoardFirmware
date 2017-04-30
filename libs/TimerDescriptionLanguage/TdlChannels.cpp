//
// Created by mcochrane on 5/04/17.
//

#include <min/min_transmit_cmds.h>
#include "TdlChannels.h"

static TdlChannels tdl_channels = TdlChannels(CHANNEL_COUNT, CHANNEL_DEFAULT_STATE);

TdlChannels::TdlChannels(uint8_t channel_count, TdlChannelState_t default_state)
        : channel_count_(channel_count)
{
    for (uint8_t i = 0; i < channel_count_; i++) {
        channels_[i] = TdlChannel(i, default_state, &PORTB, i);
    }
}

void TdlChannels::disableAll() {
    for (uint8_t i = 0; i < channel_count_; i++) {
        channels_[i].disable();
    }
}

TdlChannel& TdlChannels::get(int index) {
    if (index < 0 || index >= channel_count_) {
//        while (1) report_printf("choorind=%d", index);
        Throw(EX_OUT_OF_RANGE);
    }

    return channels_[index];
}

void TdlChannels::resetStates() {
    for (uint8_t i = 0; i < channel_count_; i++) {
        channels_[i].reset();
    }
}

TdlChannels& TdlChannels_GetInstance() {
    return tdl_channels;
}