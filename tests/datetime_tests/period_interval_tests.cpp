//
// Created by mcochrane on 17/05/17.
//

#include "datetime/PeriodInterval.h"

#include "gtest/gtest.h"

TEST(PeriodIntervalCompileTestFixture, create_empty) {
    PeriodInterval pi = PeriodInterval::Empty();
    EXPECT_TRUE(pi.isEmpty());
}

TEST(PeriodIntervalCompileTestFixture, default_constructor_creates_empty) {
    PeriodInterval pi = PeriodInterval();
    EXPECT_TRUE(pi.isEmpty());
}

TEST(PeriodIntervalCompileTestFixture, is_empty_returns_false_for_non_empty) {
    Period p1 = Period::MakeCustomPeriod(10, PERIODFIELD_DAYS);
    Period p2 = Period::MakeCustomPeriod(25, PERIODFIELD_DAYS);
    PeriodInterval pi = PeriodInterval(p1, p2);
    EXPECT_FALSE(pi.isEmpty());
}

TEST(PeriodIntervalCompileTestFixture, start_end_constructor) {
    Period p1 = Period::MakeCustomPeriod(10, PERIODFIELD_DAYS);
    Period p2 = Period::MakeCustomPeriod(25, PERIODFIELD_DAYS);
    PeriodInterval pi = PeriodInterval(p1, p2);
    EXPECT_EQ(pi.getStart(), p1);
    EXPECT_EQ(pi.getEnd(), p2);
}

TEST(PeriodIntervalCompileTestFixture, to_interval) {
    Period p1 = Period::MakeCustomPeriod(10, PERIODFIELD_DAYS);
    Period p2 = Period::MakeCustomPeriod(25, PERIODFIELD_DAYS);
    PeriodInterval pi = PeriodInterval(p1, p2);

    Interval interval = pi.toInterval(DateTime(2017, 1, 1, 0, 0, 0));

    EXPECT_EQ(interval.getStart(), DateTime(2017, 1, 11, 0, 0, 0));
    EXPECT_EQ(interval.getEnd(),   DateTime(2017, 1, 26, 0, 0, 0));
}