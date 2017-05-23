//
// Created by mcochrane on 17/05/17.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nvm/NvmRuleManager.h>
#include <NvmDriverMock.h>

using ::testing::_;
using ::testing::Return;
using ::testing::Sequence;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::Pointee;
using ::testing::SetArrayArgument;
using ::testing::ElementsAreArray;
using ::testing::Args;

NvmDriverMock* nvmDriverMock;

MATCHER_P2(PointerAndSizeEqArray, pointer, size, "") {
    const int actual_size = std::tr1::get<1>(arg);
    if (actual_size != size)
        return false;
    for (int i = 0; i < size; i++) {
        if (std::tr1::get<0>(arg)[i] != pointer[i])
            return false;
    }
    return true;
}

class NvmRuleManagerInitTestFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        nvmDriverMock = new NvmDriverMock();
    }

    virtual void TearDown() {
        CEXCEPTION_T e;
        Try {
            NvmRuleManager_Destroy();
            delete nvmDriverMock;
            nvmDriverMock = NULL;
        } Catch (e) {
        }
    }
};

TEST_F(NvmRuleManagerInitTestFixture, init_zero_rules) {
    // Number of rules - read 1 byte at location 0 -> return 0
    EXPECT_CALL(*nvmDriverMock, read(_, 1, 0))
            .WillOnce(SetArgPointee<0>(0));

    NvmRuleManager_Init(0, 1024, *nvmDriverMock);
    EXPECT_EQ(NvmRuleManager_Get().getNewRuleNvmAddressVar(), 1);
    EXPECT_EQ(NvmRuleManager_GetRuleCount(), 0);
}

TEST_F(NvmRuleManagerInitTestFixture, init_start_at_non_zero_nvm_address) {
    // Number of rules - read 1 byte at location 100 -> return 0
    EXPECT_CALL(*nvmDriverMock, read(_, 1, 100))
            .WillOnce(SetArgPointee<0>(0));

    // Start at 100 instead of 0.
    NvmRuleManager_Init(100, 1024, *nvmDriverMock);
    EXPECT_EQ(NvmRuleManager_Get().getNewRuleNvmAddressVar(), 101);
    EXPECT_EQ(NvmRuleManager_GetRuleCount(), 0);
}

TEST_F(NvmRuleManagerInitTestFixture, init_nvm_end_before_nvm_start_throws_exception) {
    CEXCEPTION_T e;

    Try {
        // End before Start..
        NvmRuleManager_Init(1025, 1024, *nvmDriverMock);
        FAIL() << "Should have thrown an exception.";
    } Catch(e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }
}

TEST_F(NvmRuleManagerInitTestFixture, init_nvm_end_equals_nvm_start_throws_exception) {
    CEXCEPTION_T e;

    // Number of rules
    EXPECT_CALL(*nvmDriverMock, read(_, 1, 100))
            .WillOnce(SetArgPointee<0>(0));

    Try {
        // End equals Start..
        NvmRuleManager_Init(1024, 1024, *nvmDriverMock);
        FAIL() << "Should have thrown an exception.";
    } Catch(e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }
}

TEST_F(NvmRuleManagerInitTestFixture, init_uninitialised_nvm_gives_zero_rules) {
    // Number of rules = 0xFF (255) which is what the nvm will probably be set
    // to if it's uninitialised.  In that case we just want to say that we have
    // zero rules.
    EXPECT_CALL(*nvmDriverMock, read(_, 1, 0))
            .WillOnce(SetArgPointee<0>(0xFF));

    NvmRuleManager_Init(0, 1024, *nvmDriverMock);
    EXPECT_EQ(NvmRuleManager_Get().getNewRuleNvmAddressVar(), 1);
    EXPECT_EQ(NvmRuleManager_GetRuleCount(), 0);
}

TEST_F(NvmRuleManagerInitTestFixture, init_twice_throws_exception) {
    CEXCEPTION_T e;

    nvmDriverMock = new NvmDriverMock();
    NvmRuleManager_Init(0, 1024, *nvmDriverMock);

    Try {
        NvmRuleManager_Init(0, 1024, *nvmDriverMock);
        FAIL() << "Should have thrown an exception.";
    } Catch(e) {
        EXPECT_EQ(e, EX_ALREADY_INITIALISED);
    }
}


class NvmRuleManagerTestFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        Sequence seq;
        nvmDriverMock = new NvmDriverMock();

        // Number of rules
        EXPECT_CALL(*nvmDriverMock, read(_, 1, 0))
                .InSequence(seq)
                .WillOnce(SetArgPointee<0>(3));


        // Read length of rule 0 (address = 1)
        EXPECT_CALL(*nvmDriverMock, read(_, 1, 1))
                .InSequence(seq)
                .WillOnce(SetArgPointee<0>(10)); // Length = 10

        // Read length of rule 1 (address = 1+10+1=12)
        EXPECT_CALL(*nvmDriverMock, read(_, 1, 12))
                .InSequence(seq)
                .WillOnce(SetArgPointee<0>(11)); // Length = 11

        // Read length of rule 2 (address = 1+10+1+11+1=24)
        EXPECT_CALL(*nvmDriverMock, read(_, 1, 24))
                .InSequence(seq)
                .WillOnce(SetArgPointee<0>(5)); // Length = 5

        //New Rule EEPROM address should now be: 1+10+1+11+1+5+1 = 30

        NvmRuleManager_Init(0, 100, *nvmDriverMock);
    }

    virtual void TearDown() {
        NvmRuleManager_Destroy();
        delete nvmDriverMock;
        nvmDriverMock = NULL;
    }
};

TEST_F(NvmRuleManagerTestFixture, get_rule_count) {
    EXPECT_EQ(NvmRuleManager_GetRuleCount(), 3);
}

TEST_F(NvmRuleManagerTestFixture, get_next_eeprom_address) {
    EXPECT_EQ(NvmRuleManager_Get().getNewRuleNvmAddressVar(), 30);
}

TEST_F(NvmRuleManagerTestFixture, add_new_rule) {
    Sequence seq;

    uint8_t ruleLength = 7;
    uint8_t ruleData[] = {0, 1, 2, 1, 5, 5, 0};
    nvm_address_t newRuleAddress = NvmRuleManager_Get().getNewRuleNvmAddressVar();

    EXPECT_CALL(*nvmDriverMock, write(Pointee(ruleLength), 1, newRuleAddress))
            .Times(1)
            .InSequence(seq);

    EXPECT_CALL(*nvmDriverMock, write(_, ruleLength, newRuleAddress+1))
            .With(PointerAndSizeEqArray(ruleData, ruleLength))
            .Times(1)
            .InSequence(seq);

    EXPECT_CALL(*nvmDriverMock, flush())
            .Times(1)
            .InSequence(seq);

    // Should return the number of the rule that was just added
    EXPECT_EQ(NvmRuleManager_Get().addNewRule(ruleData, ruleLength), 3);

    // Checks
    EXPECT_EQ(NvmRuleManager_Get().getNewRuleNvmAddressVar(), newRuleAddress + ruleLength + 1); // 30 + 7 + 1 = 38
    EXPECT_EQ(NvmRuleManager_Get().getRuleCount(), 4);

}

TEST_F(NvmRuleManagerTestFixture, add_new_rule_overflow_nvm_max_address) {
    CEXCEPTION_T e;
    Sequence seq;

    // The maximum NVM address (set in setup function) is 100, we're at 30 already so
    // when we write a rule with length 110 we should get an exception!

    uint8_t ruleLength = 110;
    uint8_t ruleData[110];
    nvm_address_t newRuleAddress = NvmRuleManager_Get().getNewRuleNvmAddressVar();

    Try {
        // Should return the number of the rule that was just added
        EXPECT_EQ(NvmRuleManager_Get().addNewRule(ruleData, ruleLength), 3);
        FAIL() << "Should have thrown an exception";
    } Catch(e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }

    // Check that nothing changed
    EXPECT_EQ(NvmRuleManager_GetRuleCount(), 3);
    EXPECT_EQ(NvmRuleManager_Get().getNewRuleNvmAddressVar(), 30);

}

TEST_F(NvmRuleManagerTestFixture, get_rule_data_for_first_rule) {
    Sequence seq;
    uint8_t nvmData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    uint8_t nvmDataLength = 10;

    uint8_t readData[100];
    uint8_t returnedLength;

    // Read length of rule 0 (address = 1)
    EXPECT_CALL(*nvmDriverMock, read(_, 1, 1))
            .InSequence(seq)
            .WillOnce(SetArgPointee<0>(10)); // Length = 10

    // Read data of rule 0 (data address = 1+1 = 2)
    EXPECT_CALL(*nvmDriverMock, read(_, 10, 2))
            .InSequence(seq)
            .WillOnce(SetArrayArgument<0>(nvmData, nvmData+nvmDataLength));

    NvmRuleManager_Get().getRuleData(0, readData, &returnedLength);

    EXPECT_EQ(returnedLength, nvmDataLength);
    EXPECT_THAT(nvmData, ElementsAreArray(readData, nvmDataLength));
}

TEST_F(NvmRuleManagerTestFixture, get_rule_data_for_second_rule) {
    Sequence seq;
    uint8_t nvmData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint8_t nvmDataLength = 11;

    uint8_t readData[100];
    uint8_t returnedLength;

    // Read length of rule 0 (address = 1)
    EXPECT_CALL(*nvmDriverMock, read(_, 1, 1))
            .InSequence(seq)
            .WillOnce(SetArgPointee<0>(10)); // Length = 10

    // Read length of rule 1 (address = 1+10+1=12)
    EXPECT_CALL(*nvmDriverMock, read(_, 1, 12))
            .InSequence(seq)
            .WillOnce(SetArgPointee<0>(11)); // Length = 11

    // Read data of rule 1 (data address = 12+1 = 13)
    EXPECT_CALL(*nvmDriverMock, read(_, 11, 13))
            .InSequence(seq)
            .WillOnce(SetArrayArgument<0>(nvmData, nvmData+nvmDataLength));

    NvmRuleManager_Get().getRuleData(1, readData, &returnedLength);

    EXPECT_EQ(returnedLength, nvmDataLength);
    EXPECT_THAT(nvmData, ElementsAreArray(readData, nvmDataLength));
}

TEST_F(NvmRuleManagerTestFixture, get_rule_data_out_of_range) {
    CEXCEPTION_T e;
    uint8_t readData[100];
    uint8_t returnedLength;

    Try {
        NvmRuleManager_Get().getRuleData(3, readData, &returnedLength);
        FAIL() << "Should have thrown an exception!";
    } Catch (e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }
}

TEST_F(NvmRuleManagerTestFixture, clear_rules) {
    Sequence seq;

    // Write number of rules
    EXPECT_CALL(*nvmDriverMock, write(Pointee(0), 1, 0))
            .Times(1)
            .InSequence(seq);

    EXPECT_CALL(*nvmDriverMock, flush())
            .Times(1)
            .InSequence(seq);

    NvmRuleManager_Get().clearRules();
    EXPECT_EQ(NvmRuleManager_GetRuleCount(), 0);
    EXPECT_EQ(NvmRuleManager_Get().getNewRuleNvmAddressVar(), 1);
}

TEST_F(NvmRuleManagerTestFixture, clear_rules_multiple_times_only_writes_to_nvm_once) {
    Sequence seq;

    // Write number of rules
    EXPECT_CALL(*nvmDriverMock, write(Pointee(0), 1, 0))
            .Times(1)
            .InSequence(seq);

    EXPECT_CALL(*nvmDriverMock, flush())
            .Times(1)
            .InSequence(seq);

    // Call multiple times
    NvmRuleManager_Get().clearRules();
    NvmRuleManager_Get().clearRules();
    NvmRuleManager_Get().clearRules();
    NvmRuleManager_Get().clearRules();
    NvmRuleManager_Get().clearRules();

    EXPECT_EQ(NvmRuleManager_GetRuleCount(), 0);
    EXPECT_EQ(NvmRuleManager_Get().getNewRuleNvmAddressVar(), 1);
}

TEST_F(NvmRuleManagerTestFixture, save_rule_count_to_nvm) {
    Sequence seq;

    // Write number of rules
    EXPECT_CALL(*nvmDriverMock, write(Pointee(3), 1, 0))
            .Times(1)
            .InSequence(seq);

    EXPECT_CALL(*nvmDriverMock, flush())
            .Times(1)
            .InSequence(seq);

    // The deed
    NvmRuleManager_Get().saveRuleCountToNvm();

    EXPECT_EQ(NvmRuleManager_GetRuleCount(), 3);
}

