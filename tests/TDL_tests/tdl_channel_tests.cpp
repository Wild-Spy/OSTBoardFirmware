//
// Created by mcochrane on 17/05/17.
//

#include <gtest/gtest.h>
#include <TimerDescriptionLanguage/TdlChannel.h>

using ::testing::Return;
using ::testing::Sequence;

TEST(TdlChannelTestFixture, default_constructor_is_empty) {
    Sequence setupSequence;

    Pin* pin = new Pin((PORT_t*)1, (uint8_t)0);

    EXPECT_CALL(*pin, dirChanged(pin_dir_output))
            .Times(0);
    EXPECT_CALL(*pin, valueChanged(false))
            .Times(0);

    TdlChannel ch = TdlChannel();

    EXPECT_EQ(ch.isEmpty(), true);
}

TEST(TdlChannelTestFixture, create_with_pin) {
    Sequence setupSequence;

    Pin* pin = new Pin((PORT_t*)1, (uint8_t)0);

    ON_CALL(*pin, returnInputValue())
            .WillByDefault(Return(false));

    EXPECT_CALL(*pin, dirChanged(pin_dir_output))
            .Times(1)
            .InSequence(setupSequence);
    EXPECT_CALL(*pin, valueChanged(false))
            .Times(1)
            .InSequence(setupSequence);

    TdlChannel ch = TdlChannel(0, TDLCHANNELSTATE_DISABLED, pin);

    EXPECT_EQ(ch.isEmpty(), false);
}

TEST(TdlChannelTestFixture, enable_disable) {
    Sequence seq;

    Pin* pin = new Pin((PORT_t*)1, (uint8_t)0);
    TdlChannel ch = TdlChannel(0, TDLCHANNELSTATE_DISABLED, pin);

    ON_CALL(*pin, returnInputValue())
            .WillByDefault(Return(false));

    EXPECT_CALL(*pin, valueChanged(true))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(*pin, valueChanged(false))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(*pin, valueChanged(true))
            .Times(1)
            .InSequence(seq);

    ch.enable();
    ch.disable();
    ch.enable();
}

TEST(TdlChannelTestFixture, toggle) {
    Sequence seq;

    Pin* pin = new Pin((PORT_t*)1, (uint8_t)0);
    TdlChannel ch = TdlChannel(0, TDLCHANNELSTATE_DISABLED, pin);

    ON_CALL(*pin, returnInputValue())
            .WillByDefault(Return(false));

    EXPECT_CALL(*pin, valueChanged(true))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(*pin, valueChanged(false))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(*pin, valueChanged(true))
            .Times(1)
            .InSequence(seq);

    ch.enable();
    ch.toggle();
    ch.toggle();
}

TEST(TdlChannelTestFixture, reset) {
    Sequence seq;

    Pin* pin = new Pin((PORT_t*)1, (uint8_t)0);
    TdlChannel ch = TdlChannel(0, TDLCHANNELSTATE_DISABLED, pin);

    ON_CALL(*pin, returnInputValue())
            .WillByDefault(Return(false));

    EXPECT_CALL(*pin, valueChanged(true))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(*pin, valueChanged(false))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(*pin, valueChanged(false))
            .Times(1)
            .InSequence(seq);

    ch.enable();
    ch.reset();
    ch.reset();
}

TEST(TdlChannelTestFixture, get_id) {
    Sequence seq;

    Pin* pin = new Pin((PORT_t*)1, (uint8_t)0);
    TdlChannel ch = TdlChannel(123, TDLCHANNELSTATE_DISABLED, pin);

    ON_CALL(*pin, returnInputValue())
            .WillByDefault(Return(false));

    EXPECT_CALL(*pin, valueChanged(true))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(*pin, valueChanged(false))
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(*pin, valueChanged(false))
            .Times(1)
            .InSequence(seq);

     EXPECT_EQ(ch.getId(), 123);
}

TEST(TdlChannelTestFixture, get_state) {
    Pin* pin = new Pin((PORT_t*)1, (uint8_t)0);
    TdlChannel ch = TdlChannel(0, TDLCHANNELSTATE_DISABLED, pin);

    EXPECT_EQ(ch.getState(), TDLCHANNELSTATE_DISABLED);
    ch.enable();
    EXPECT_EQ(ch.getState(), TDLCHANNELSTATE_ENABLED);
    ch.disable();
    EXPECT_EQ(ch.getState(), TDLCHANNELSTATE_DISABLED);

}