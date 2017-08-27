//
// Created by mcochrane on 20/05/17.
//


#include <gtest/gtest.h>
#include <TimerDescriptionLanguage/TdlActivator.h>

using ::testing::Return;
using ::testing::Sequence;
using ::testing::_;

TEST(TdlActivatorTestFixture, default_constructor_is_empty) {
    TdlActivator activator = TdlActivator();
    EXPECT_TRUE(activator.isEmpty());
}

TEST(TdlActivatorTestFixture, channel_activator) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    EXPECT_FALSE(activator.isEmpty());
    EXPECT_EQ(activator.getChannel(), &channel);
    EXPECT_EQ(activator.getRule(), (void*)NULL);
    EXPECT_EQ(activator.getActivatorType(), ACTIVATOR_TYPE_CHANNEL);
}

TEST(TdlActivatorTestFixture, rule_activator) {
    TdlRule rule(111, true);
    TdlActivator activator = TdlActivator(&rule, TDLACTIVATORSTATE_DISABLED);
    EXPECT_FALSE(activator.isEmpty());
    EXPECT_EQ(activator.getChannel(), (void*)NULL);
    EXPECT_EQ(activator.getRule(), &rule);
    EXPECT_EQ(activator.getActivatorType(), ACTIVATOR_TYPE_RULE);
}

TEST(TdlActivatorTestFixture, get_target_id) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator ch_act = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    EXPECT_EQ(ch_act.getTargetId(), 10);

    TdlRule rule(111, true);
    TdlActivator rule_act = TdlActivator(&rule, TDLACTIVATORSTATE_DISABLED);
    EXPECT_EQ(rule_act.getTargetId(), 111);
}

TEST(TdlActivatorTestFixture, channel_activator_change_state) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);

    EXPECT_CALL(channel, enableCalled())
            .Times(1);
    EXPECT_CALL(channel, disableCalled())
            .Times(1);

    activator.enable(DateTime());
    activator.disable();
}

TEST(TdlActivatorTestFixture, rule_activator_change_state) {
    Sequence seq;
    TdlRule rule(111, true);
    TdlActivator activator(&rule, TDLACTIVATORSTATE_DISABLED);

    EXPECT_CALL(rule, enableCalled())
            .Times(1)
            .InSequence(seq);
    EXPECT_CALL(rule, updateThisRuleAndParentsIfNecessary(DateTime(222)))
            .Times(1);
    EXPECT_CALL(rule, disableCalled())
            .Times(1)
            .InSequence(seq);

    activator.enable(DateTime(222));
    activator.disable();
}

TEST(TdlActivatorTestFixture, channel_activator_get_state) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);

    EXPECT_EQ(activator.getState(), TDLACTIVATORSTATE_DISABLED);
    activator.enable(DateTime());
    EXPECT_EQ(activator.getState(), TDLACTIVATORSTATE_ENABLED);
    activator.disable();
    EXPECT_EQ(activator.getState(), TDLACTIVATORSTATE_DISABLED);
}

TEST(TdlActivatorTestFixture, activator_sets_correct_default_state) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator_dis = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    EXPECT_EQ(activator_dis.getState(), TDLACTIVATORSTATE_DISABLED);
    EXPECT_EQ(activator_dis.getDefaultState(), TDLACTIVATORSTATE_DISABLED);

    TdlActivator activator_en = TdlActivator(&channel, TDLACTIVATORSTATE_ENABLED);
    EXPECT_EQ(activator_en.getState(), TDLACTIVATORSTATE_ENABLED);
    EXPECT_EQ(activator_en.getDefaultState(), TDLACTIVATORSTATE_ENABLED);
}

TEST(TdlActivatorTestFixture, is_in_default_state) {
    TdlChannel channel(10, TDLCHANNELSTATE_DISABLED);
    TdlActivator activator = TdlActivator(&channel, TDLACTIVATORSTATE_DISABLED);
    activator.enable(DateTime());
    EXPECT_FALSE(activator.isInDefaultState());
    activator.disable();
    EXPECT_TRUE(activator.isInDefaultState());
}