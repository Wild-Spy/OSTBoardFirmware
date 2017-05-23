//
// Created by mcochrane on 21/05/17.
//


#include <gtest/gtest.h>
#include <gmock/gmock.h>
//#include <simplePinMock.h>
//#include <TimerDescriptionLanguage/TdlAction.h>
//#include <TimerDescriptionLanguage/TdlChannels.h>
#include <TimerDescriptionLanguage/TdlRules.h>

using ::testing::_;
using ::testing::Return;
using ::testing::Sequence;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;

static TdlRule* rules;
#define RULE_COUNT 4


TEST(TdlRulesTestNoFixture, no_init_test) {
    CEXCEPTION_T e;
//    TdlRules_Destroy();
    Try {
        TdlRules &rules = TdlRules_GetInstance();
        FAIL() << "Should have thrown an exception";
    } Catch(e) {
        EXPECT_EQ(e, EX_NULL_POINTER);
    }
}


class TdlRulesTestFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
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
    }
};

TEST_F(TdlRulesTestFixture, setup_teardown_test) {
}

TEST_F(TdlRulesTestFixture, correct_rules_referenced) {
    EXPECT_EQ(&TdlRules_GetInstance().get(0), rules+0);
    EXPECT_EQ(&TdlRules_GetInstance().get(1), rules+1);
    EXPECT_EQ(&TdlRules_GetInstance().get(2), rules+2);
    EXPECT_EQ(&TdlRules_GetInstance().get(3), rules+3);
}

TEST_F(TdlRulesTestFixture, get_max_rules) {
    EXPECT_EQ(TdlRules_GetInstance().getMaxRules(), RULE_COUNT);
}

TEST_F(TdlRulesTestFixture, get_count) {
    EXPECT_EQ(TdlRules_GetInstance().getCount(), RULE_COUNT);
}


class TdlRulesDefaultInitTestFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        Sequence seq;
        NvmRuleManager_Init();

        EXPECT_CALL(NvmRuleManager_Get(), getRuleCount())
                .InSequence(seq)
                .WillOnce(Return(3));

        uint8_t data0[2] = {0, (uint8_t)false};  // id = 0, enabled on startup = false
        EXPECT_CALL(NvmRuleManager_Get(), getRuleData(0, _, _))
                .InSequence(seq)
                .WillOnce(DoAll(SetArrayArgument<1>(data0, data0+2), SetArgPointee<2>(2)));

        uint8_t data1[2] = {1, (uint8_t)true};  // id = 1, enabled on startup = true
        EXPECT_CALL(NvmRuleManager_Get(), getRuleData(1, _, _))
                .InSequence(seq)
                .WillOnce(DoAll(SetArrayArgument<1>(data1, data1+2), SetArgPointee<2>(2)));

        uint8_t data2[2] = {2, (uint8_t)true};  // id = 2, enabled on startup = true
        EXPECT_CALL(NvmRuleManager_Get(), getRuleData(2, _, _))
                .InSequence(seq)
                .WillOnce(DoAll(SetArrayArgument<1>(data2, data2+2), SetArgPointee<2>(2)));

        TdlRules_Init(10, NvmRuleManager_Get());
    }

    virtual void TearDown() {
        TdlRules_Destroy();
    }
};

TEST_F(TdlRulesDefaultInitTestFixture, setup_teardown_test) {
}

TEST_F(TdlRulesDefaultInitTestFixture, expect_max_rule_count_ten) {
    EXPECT_EQ(TdlRules_GetInstance().getMaxRules(), 10);
}

TEST_F(TdlRulesDefaultInitTestFixture, expect_three_rules) {
    EXPECT_EQ(TdlRules_GetInstance().getCount(), 3);
}

TEST_F(TdlRulesDefaultInitTestFixture, check_rule_contents) {
    EXPECT_EQ(TdlRules_GetInstance().get(0).getId(), 0);
    EXPECT_EQ(TdlRules_GetInstance().get(0).isEnabledOnStart(), false);
    EXPECT_EQ(TdlRules_GetInstance().get(1).getId(), 1);
    EXPECT_EQ(TdlRules_GetInstance().get(1).isEnabledOnStart(), true);
    EXPECT_EQ(TdlRules_GetInstance().get(2).getId(), 2);
    EXPECT_EQ(TdlRules_GetInstance().get(2).isEnabledOnStart(), true);
}

TEST_F(TdlRulesDefaultInitTestFixture, get_out_of_range_throws_exception) {
    CEXCEPTION_T e;
    Try {
        TdlRules_GetInstance().get(10);
        FAIL() << "Should have thrown an exception.";
    } Catch(e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }
}
