//
// Created by mcochrane on 20/05/17.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <TimerDescriptionLanguage/TdlAction.h>

using ::testing::Return;
using ::testing::Sequence;

TEST(TdlActionTestFixture, default_constructor_is_empty) {
    TdlAction action = TdlAction();
    ASSERT_TRUE(action.isEmpty());
}

TEST(TdlActionTestFixture, empty_activator_is_empty) {
    TdlActivator activator = TdlActivator();
    TdlAction action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);
    ASSERT_TRUE(action.isEmpty());
}

TEST(TdlActionTestFixture, create_action) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    TdlAction action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);
    ASSERT_EQ(action.getActivatorStateWhenRunning(), TDLACTIVATORSTATE_ENABLED);
    ASSERT_EQ(action.getActivator(), activator);
}

TEST(TdlActionTestFixture, initial_state_is_disabled) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    TdlAction action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);
    ASSERT_EQ(action.getState(), TDLACTIONSTATE_INACTIVE);
}

TEST(TdlActionTestFixture, valid_action_is_not_empty) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    TdlAction action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);
    ASSERT_FALSE(action.isEmpty());
}

TEST(TdlActionTestFixture, get_activator_type) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    TdlAction channel_action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);
    ASSERT_EQ(channel_action.getActivatorType(), ACTIVATOR_TYPE_CHANNEL);

    TdlRule rule(111, true);
    TdlActivator rule_activator = TdlActivator(&rule, TDLACTIVATORSTATE_DISABLED);
    TdlAction rule_action = TdlAction(TDLACTIVATORSTATE_ENABLED, rule_activator);
    ASSERT_EQ(rule_action.getActivatorType(), ACTIVATOR_TYPE_RULE);
}

TEST(TdlActionTestFixture, get_activator_state) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    TdlAction channel_action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);
    ASSERT_EQ(channel_action.getActivatorState(), TDLACTIVATORSTATE_DISABLED);
}

TEST(TdlActionTestFixture, start_stop_channel) {
    Sequence seq;
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    TdlAction action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);

    EXPECT_CALL(channel, enableCalled())
            .Times(1)
            .InSequence(seq);

    EXPECT_CALL(channel, disableCalled())
            .Times(1)
            .InSequence(seq);

    EXPECT_CALL(channel, enableCalled())
            .Times(1)
            .InSequence(seq);


    action.start(DateTime());
    action.stop(DateTime());
    action.start(DateTime());
}

TEST(TdlActionTestFixture, toggle_channel) {
    Sequence seq;
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    TdlAction action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);

    EXPECT_CALL(channel, enableCalled())
            .Times(1)
            .InSequence(seq);

    EXPECT_CALL(channel, disableCalled())
            .Times(1)
            .InSequence(seq);

    EXPECT_CALL(channel, enableCalled())
            .Times(1)
            .InSequence(seq);


    action.toggle(DateTime());
    action.toggle(DateTime());
    action.toggle(DateTime());
}

TEST(TdlActionTestFixture, channel_not_changed_if_action_state_not_changed_start) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    TdlAction action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);

    EXPECT_CALL(channel, enableCalled())
            .Times(1);

    /* Even though we call start many times we should only see channel.enable() called once.
     * When it's set to TDLACTIONSTATE_ACTIVE then it won't call activator.enable() on
     * subsequent calls.
     */
    action.start(DateTime());
    action.start(DateTime());
    action.start(DateTime());
    action.start(DateTime());
    action.start(DateTime());
}

TEST(TdlActionTestFixture, channel_not_changed_if_action_state_not_changed_stop) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    TdlAction action = TdlAction(TDLACTIVATORSTATE_ENABLED, activator);

    /* Even though we call stop many times we should never see channel.disable() called.
     * When it's set to TDLACTIONSTATE_INACTIVE then it won't call activator.disable().
     * Note that TDLACTIONSTATE_INACTIVE is the default, which is why we don't get
     * channel.disable() called at all.
     */
    action.stop(DateTime());
    action.stop(DateTime());
    action.stop(DateTime());
    action.stop(DateTime());
    action.stop(DateTime());
}