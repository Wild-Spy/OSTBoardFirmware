//
// Created by mcochrane on 20/05/17.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SimplePinMock.h>
#include <TimerDescriptionLanguage/TdlAction.h>
#include <TimerDescriptionLanguage/TdlChannels.h>
#include <TimerDescriptionLanguage/TdlRules.h>

using ::testing::Return;
using ::testing::Sequence;

#define RULE_COUNT 4
#define PIN_COUNT 4
static Pin* pins[PIN_COUNT];
static TdlRule* rules;

/**
 * Format:
 * target type (byte)
 * target id (uint16_t)
 * default state (byte)
 * state when running (byte)
 * total of 5 bytes (constant length)
 */

class TdlActionDecompileTestFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        for (uint8_t i = 0; i < PIN_COUNT; i++) {
            pins[i] = new Pin(i);
        }

        TdlChannels_Init(4, TDLCHANNELSTATE_DISABLED, pins);

        rules = (TdlRule*)malloc(sizeof(TdlRule)*RULE_COUNT);

        // Creating these in-place with the placement new operator
        new (rules+0) TdlRule(0, false);
        new (rules+1) TdlRule(1, false);
        new (rules+2) TdlRule(2, true);
        new (rules+3) TdlRule(3, true);

        TdlRules_Init_Test(rules, RULE_COUNT);
    }

    virtual void TearDown() {
        TdlRules_Destroy();
        TdlChannels_Destroy();
        for (uint8_t i = 0; i < PIN_COUNT; i++) {
            delete pins[i];
        }
    }
};

TEST_F(TdlActionDecompileTestFixture, setup_teardown_test) {
}

TEST_F(TdlActionDecompileTestFixture, correct_length) {
    uint8_t len = TdlAction::ACTION_COMPILED_LENGTH;
    EXPECT_EQ(len, 5);
}

TEST_F(TdlActionDecompileTestFixture, rules_test) {
    EXPECT_EQ(&TdlRules_GetInstance().get(0), rules);
    EXPECT_EQ(&TdlRules_GetInstance().get(1), rules+1);
}

TEST_F(TdlActionDecompileTestFixture, compile_channel) {
    uint8_t data[] = {TdlAction::ACTION_TARGET_CHANNEL,
                      0, 0,
                      TDLACTIVATORSTATE_DISABLED,
                      TDLACTIVATORSTATE_ENABLED};
    TdlAction action = TdlAction::Decompile(data);
    EXPECT_EQ(action.getActivatorType(), ACTIVATOR_TYPE_CHANNEL);
    EXPECT_EQ(action.getActivator().getTargetId(), 0);
    EXPECT_EQ(action.getActivatorState(), TDLACTIVATORSTATE_DISABLED);
    EXPECT_EQ(action.getActivatorStateWhenRunning(), TDLACTIVATORSTATE_ENABLED);
}

TEST_F(TdlActionDecompileTestFixture, compile_channel_alt) {
    uint8_t data[] = {TdlAction::ACTION_TARGET_CHANNEL,
                      0, 3,
                      TDLACTIVATORSTATE_ENABLED,
                      TDLACTIVATORSTATE_DISABLED};
    TdlAction action = TdlAction::Decompile(data);
    EXPECT_EQ(action.getActivatorType(), ACTIVATOR_TYPE_CHANNEL);
    EXPECT_EQ(action.getActivator().getTargetId(), 3);
    EXPECT_EQ(action.getActivatorState(), TDLACTIVATORSTATE_ENABLED);
    EXPECT_EQ(action.getActivatorStateWhenRunning(), TDLACTIVATORSTATE_DISABLED);
}

TEST_F(TdlActionDecompileTestFixture, compile_rule) {
    uint8_t data[] = {TdlAction::ACTION_TARGET_RULE,
                      0, 0,
                      TDLACTIVATORSTATE_DISABLED,
                      TDLACTIVATORSTATE_ENABLED};
    TdlAction action = TdlAction::Decompile(data);
    EXPECT_EQ(action.getActivatorType(), ACTIVATOR_TYPE_RULE);
    EXPECT_EQ(action.getActivator().getTargetId(), 0);
    EXPECT_EQ(action.getActivator().getRule(), rules+0); // Actually points to our rule?
    EXPECT_EQ(action.getActivatorState(), TDLACTIVATORSTATE_DISABLED);
    EXPECT_EQ(action.getActivatorStateWhenRunning(), TDLACTIVATORSTATE_ENABLED);
}

TEST_F(TdlActionDecompileTestFixture, compile_rule_alt) {
    uint8_t data[] = {TdlAction::ACTION_TARGET_RULE,
                      0, 2,
                      TDLACTIVATORSTATE_ENABLED,
                      TDLACTIVATORSTATE_DISABLED};
    TdlAction action = TdlAction::Decompile(data);
    EXPECT_EQ(action.getActivatorType(), ACTIVATOR_TYPE_RULE);
    EXPECT_EQ(action.getActivator().getTargetId(), 2);
    EXPECT_EQ(action.getActivator().getRule(), rules+2); // Actually points to our rule?
    EXPECT_EQ(action.getActivatorState(), TDLACTIVATORSTATE_ENABLED);
    EXPECT_EQ(action.getActivatorStateWhenRunning(), TDLACTIVATORSTATE_DISABLED);
}

TEST_F(TdlActionDecompileTestFixture, compile_rule_out_of_range) {
    CEXCEPTION_T e;
    uint8_t data[] = {TdlAction::ACTION_TARGET_RULE,
                      0, 4,
                      TDLACTIVATORSTATE_ENABLED,
                      TDLACTIVATORSTATE_DISABLED};
    Try {
        TdlAction action = TdlAction::Decompile(data);
        FAIL(); // Should have thrown an exception
    } Catch(e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }
}

TEST_F(TdlActionDecompileTestFixture, compile_channel_out_of_range) {
    CEXCEPTION_T e;
    uint8_t data[] = {TdlAction::ACTION_TARGET_CHANNEL,
                      0, 4,
                      TDLACTIVATORSTATE_ENABLED,
                      TDLACTIVATORSTATE_DISABLED};
    Try {
        TdlAction action = TdlAction::Decompile(data);
        FAIL(); // Should have thrown an exception
    } Catch(e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }
}

TEST_F(TdlActionDecompileTestFixture, invalid_activator_running_state) {
    CEXCEPTION_T e;
    uint8_t data[] = {TdlAction::ACTION_TARGET_CHANNEL,
                      0, 0,
                      TDLACTIVATORSTATE_ENABLED,
                      7}; // This is invalid
    Try {
        TdlAction action = TdlAction::Decompile(data);
        FAIL(); // Should have thrown an exception
    } Catch(e) {
        EXPECT_EQ(e, EX_INVALID_INPUT_VALUE);
    }
}

TEST_F(TdlActionDecompileTestFixture, invalid_activator_default_state) {
    CEXCEPTION_T e;
    uint8_t data[] = {TdlAction::ACTION_TARGET_CHANNEL,
                      0, 0,
                      2,    // This is invalid
                      TDLACTIVATORSTATE_ENABLED};
    Try {
        TdlAction action = TdlAction::Decompile(data);
        FAIL(); // Should have thrown an exception
    } Catch(e) {
        EXPECT_EQ(e, EX_INVALID_INPUT_VALUE);
    }
}

TEST_F(TdlActionDecompileTestFixture, invalid_target_type) {
    CEXCEPTION_T e;
    uint8_t data[] = {9,        // This is invalid
                      0, 0,
                      TDLACTIVATORSTATE_ENABLED,
                      TDLACTIVATORSTATE_ENABLED};
    Try {
        TdlAction action = TdlAction::Decompile(data);
        FAIL(); // Should have thrown an exception
    } Catch(e) {
        EXPECT_EQ(e, EX_INVALID_INPUT_VALUE);
    }
}