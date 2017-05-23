//
// Created by mcochrane on 17/05/17.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nvm/NvmRuleLoader.h>
#include <NvmDriverMock.h>

using ::testing::_;
using ::testing::Return;
using ::testing::Sequence;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;

void cleanup() {
    CEXCEPTION_T e;
    Try {
        NvmRuleManager_Destroy();
    } Catch (e) {
    }
}

TEST(RuleWriterTestNoFixture, init) {
    NvmRuleManager_Init();
    cleanup();
}

TEST(RuleWriterTestNoFixture, init_twice_throws_exception) {
    CEXCEPTION_T e;

    RuleWriter_Init();

    Try {
        RuleWriter_Init();
        FAIL() << "Should have thrown an exception.";
    } Catch(e) {
        EXPECT_EQ(e, EX_ALREADY_INITIALISED);
    }
}

class RuleWriterTestFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        Sequence seq;
        EEPROMDriverMock_Init();

        // Number of rules
        EXPECT_CALL(EEPROMDriverMock_Get(), ReadEEPROM(_, 1, 0))
                .InSequence(seq)
                .WillOnce(SetArgPointee<0>(3));

        // Read length of rule 0 (address = 1)
        EXPECT_CALL(EEPROMDriverMock_Get(), ReadEEPROM(_, 1, 1))
                .InSequence(seq)
                .WillOnce(SetArgPointee<0>(10)); // Length = 10

        // Read length of rule 1 (address = 1+10+1=11)
        EXPECT_CALL(EEPROMDriverMock_Get(), ReadEEPROM(_, 1, 12))
                .InSequence(seq)
                .WillOnce(SetArgPointee<0>(11)); // Length = 11

        // Read length of rule 2 (address = 1+10+1+11+1=22)
        EXPECT_CALL(EEPROMDriverMock_Get(), ReadEEPROM(_, 1, 24))
                .InSequence(seq)
                .WillOnce(SetArgPointee<0>(5)); // Length = 5

        //New Rule EEPROM address should now be: 1+10+1+11+1+5+1 = 30

        RuleWriter_Init();
    }

    virtual void TearDown() {
        cleanup();
        EEPROMDriverMock_Destroy();
    }
};

TEST_F(RuleWriterTestFixture, get_rule_count) {
    EXPECT_EQ(RuleWriter_GetRuleCount(), 3);
}

TEST_F(RuleWriterTestFixture, get_next_eeprom_address) {
    EXPECT_EQ(RuleWriter_GetRuleWriter().getNewRuleEepromAddress(), 30);
}