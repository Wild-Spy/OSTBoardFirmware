//
// Created by mcochrane on 16/05/17.
//

#include "datetime/DateTime.h"

#include "gtest/gtest.h"

class DateTimeBasicTestFixture : public ::testing::Test {
    
};


TEST_F(DateTimeBasicTestFixture, test_eq) {
    DateTime dateTime = DateTime(100);
    EXPECT_EQ(dateTime.toTimet(), 100);
//    EXPECT_EQ(1, 0);

}