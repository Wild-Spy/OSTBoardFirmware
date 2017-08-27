//
// Created by mcochrane on 17/05/17.
//


#include "datetime/Interval.h"

#include "gtest/gtest.h"

TEST(IntervalTestFixture, create_empty) {
    Interval emptyInterval = Interval::Empty();
    EXPECT_TRUE(emptyInterval.getStart().isEmpty());
    EXPECT_TRUE(emptyInterval.getEnd().isEmpty());
    EXPECT_TRUE(emptyInterval.isEmpty());
}

TEST(IntervalTestFixture, default_constructor_creates_empty) {
    Interval emptyInterval = Interval();
    EXPECT_TRUE(emptyInterval.isEmpty());
}

TEST(IntervalTestFixture, check_start_and_end) {
    DateTime start = DateTime(2017, 1, 10, 12, 0, 0);
    DateTime end = DateTime(2017, 1, 10, 12, 30, 0);
    Interval interval = Interval(start, end);

    EXPECT_EQ(interval.getStart(), start);
    EXPECT_EQ(interval.getEnd(), end);
}

TEST(IntervalTestFixture, get_duration_seconds) {
    DateTime start = DateTime(2017, 1, 10, 12, 0, 0);
    DateTime end = DateTime(2017, 1, 10, 12, 30, 0);
    Interval interval = Interval(start, end);

    EXPECT_EQ(interval.getDurationSeconds(), 30*60);
}