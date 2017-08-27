//
// Created by mcochrane on 21/05/17.
//


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SimplePinMock.h>
#include <TimerDescriptionLanguage/TdlAction.h>
#include <TimerDescriptionLanguage/TdlChannels.h>
#include <TimerDescriptionLanguage/TdlRules.h>

using ::testing::Return;
using ::testing::Sequence;

#define PIN_COUNT 4
static Pin* pins[PIN_COUNT];

class TdlChannelsTestFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        for (uint8_t i = 0; i < PIN_COUNT; i++) {
            pins[i] = new Pin(i);
        }
        TdlChannels_Init(4, TDLCHANNELSTATE_DISABLED, pins);
    }

    virtual void TearDown() {
        TdlChannels_Destroy();
        for (uint8_t i = 0; i < PIN_COUNT; i++) {
            delete pins[i];
        }
    }
};

TEST(TdlChannelsTestNoFixture, no_init_test) {
    CEXCEPTION_T e;
    Try {
        TdlChannels &chans = TdlChannels_GetInstance();
        FAIL() << "Should have thrown an exception";
    } Catch(e) {
        EXPECT_EQ(e, EX_NULL_POINTER);
    }
}

TEST(TdlChannelsTestNoFixture, init_default_state_enabled) {
    TdlChannels_Init(4, TDLCHANNELSTATE_ENABLED, pins);
    EXPECT_EQ(TdlChannels_GetInstance().get(0).getState(), TDLCHANNELSTATE_ENABLED);
    EXPECT_EQ(TdlChannels_GetInstance().get(1).getState(), TDLCHANNELSTATE_ENABLED);
    EXPECT_EQ(TdlChannels_GetInstance().get(2).getState(), TDLCHANNELSTATE_ENABLED);
    EXPECT_EQ(TdlChannels_GetInstance().get(3).getState(), TDLCHANNELSTATE_ENABLED);
    delete &TdlChannels_GetInstance();
}

TEST_F(TdlChannelsTestFixture, setup_teardown_test) {
}

TEST_F(TdlChannelsTestFixture, init_default_state_disabled) {
    EXPECT_EQ(TdlChannels_GetInstance().get(0).getState(), TDLCHANNELSTATE_DISABLED);
    EXPECT_EQ(TdlChannels_GetInstance().get(1).getState(), TDLCHANNELSTATE_DISABLED);
    EXPECT_EQ(TdlChannels_GetInstance().get(2).getState(), TDLCHANNELSTATE_DISABLED);
    EXPECT_EQ(TdlChannels_GetInstance().get(3).getState(), TDLCHANNELSTATE_DISABLED);
}

TEST_F(TdlChannelsTestFixture, get_instance) {
    TdlChannels &chans = TdlChannels_GetInstance();
}

TEST_F(TdlChannelsTestFixture, get) {
    TdlChannels &chans = TdlChannels_GetInstance();
    EXPECT_EQ(chans.get(0).getId(), 0);
    EXPECT_EQ(chans.get(0).getPin(), pins[0]);
}

TEST_F(TdlChannelsTestFixture, get_out_of_range_throws_exception) {
    TdlChannels &chans = TdlChannels_GetInstance();

    CEXCEPTION_T e;
    Try {
        chans.get(4);
        FAIL(); // Should have thrown an exception
    } Catch(e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }
}

TEST_F(TdlChannelsTestFixture, disable_all) {
    TdlChannels &chans = TdlChannels_GetInstance();

    // Don't really care which order they're called in
    EXPECT_CALL(chans.get(0), disableCalled()).Times(1);
    EXPECT_CALL(chans.get(1), disableCalled()).Times(1);
    EXPECT_CALL(chans.get(2), disableCalled()).Times(1);
    EXPECT_CALL(chans.get(3), disableCalled()).Times(1);

    chans.disableAll();
}

TEST_F(TdlChannelsTestFixture, reset_states) {
    TdlChannels &chans = TdlChannels_GetInstance();

    // Don't really care which order they're called in
    EXPECT_CALL(chans.get(0), resetCalled()).Times(1);
    EXPECT_CALL(chans.get(1), resetCalled()).Times(1);
    EXPECT_CALL(chans.get(2), resetCalled()).Times(1);
    EXPECT_CALL(chans.get(3), resetCalled()).Times(1);

    chans.resetStates();
}