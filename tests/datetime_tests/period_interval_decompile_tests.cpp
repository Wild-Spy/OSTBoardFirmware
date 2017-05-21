//
// Created by mcochrane on 17/05/17.
//

#include "datetime/PeriodInterval.h"

#include "gtest/gtest.h"

/* Compiled Structure
 * (variable length) start period
 * (variable length) end period
 */

TEST(PeriodIntervalCompileTestFixture, decompile_empty) {
    uint8_t data[] = {0, 0};
    uint8_t bytes_consumed;
    PeriodInterval pi = PeriodInterval::Decompile(data, &bytes_consumed);
    EXPECT_TRUE(pi.isEmpty());
    EXPECT_EQ(bytes_consumed, 2);
}

TEST(PeriodIntervalCompileTestFixture, decompile) {
    uint8_t data[] = {1, 6, 0, 0, 0, 5, 1, 6, 0, 0, 0, 10}; // start = 5 seconds, end = 10 seconds
    uint8_t bytes_consumed;
    PeriodInterval pi = PeriodInterval::Decompile(data, &bytes_consumed);

    EXPECT_EQ(pi.getStart(), Period::MakeCustomPeriod(5, PERIODFIELD_SECONDS));
    EXPECT_EQ(pi.getEnd(), Period::MakeCustomPeriod(10, PERIODFIELD_SECONDS));

    EXPECT_EQ(bytes_consumed, 12);
}

TEST(PeriodIntervalCompileTestFixture, decompile_different_field_lengths) {
    uint8_t data[] = {2, 5, 0, 0, 0, 5, 6, 0, 0, 0, 1,  //start = 5 minutes, 1 seconds
                      1, 5, 0, 0, 0, 10};               // end = 10 minutes
    uint8_t bytes_consumed;
    PeriodInterval pi = PeriodInterval::Decompile(data, &bytes_consumed);

    EXPECT_EQ(pi.getStart().getFieldCount(), 2);
    EXPECT_EQ(pi.getStart().getSeconds(), 1);
    EXPECT_EQ(pi.getStart().getMinutes(), 5);
    EXPECT_EQ(pi.getEnd(), Period::MakeCustomPeriod(10, PERIODFIELD_MINUTES));

    EXPECT_EQ(bytes_consumed, 17);
}