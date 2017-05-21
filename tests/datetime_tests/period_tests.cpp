//
// Created by mcochrane on 17/05/17.
//

#include "exception/ExceptionValues.h"

#include "datetime/Period.h"

#include "gtest/gtest.h"

TEST(PeriodTestFixture, create_empty_period) {
    Period period = Period::Empty();
    EXPECT_TRUE(period.isEmpty());
}

TEST(PeriodTestFixture, default_constructor_is_empty) {
    Period period = Period();
    EXPECT_TRUE(period.isEmpty());
}

TEST(PeriodTestFixture, create_infinite_period) {
    Period period = Period::Infinite();
    EXPECT_TRUE(period.isInfinite());
}

TEST(PeriodTestFixture, non_empty_period_returns_is_empty_false) {
    Period period = Period::MakeCustomPeriod(2, PERIODFIELD_DAYS);
    EXPECT_FALSE(period.isEmpty());
}

TEST(PeriodTestFixture, get_field_count) {
    Period period = Period::MakeCustomPeriod(2, PERIODFIELD_DAYS);
    EXPECT_EQ(period.getFieldCount(), 1);
}

TEST(PeriodTestFixture, get_fileds_year_period) {
    Period period = Period::MakeCustomPeriod(2, PERIODFIELD_YEARS);
    EXPECT_EQ(period.getYears(),   2);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
}

TEST(PeriodTestFixture, get_fileds_month_period) {
    Period period = Period::MakeCustomPeriod(2, PERIODFIELD_MONTHS);
    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  2);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
}

TEST(PeriodTestFixture, get_fileds_week_period) {
    Period period = Period::MakeCustomPeriod(2, PERIODFIELD_WEEKS);
    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   2);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
}

TEST(PeriodTestFixture, get_fileds_day_period) {
    Period period = Period::MakeCustomPeriod(2, PERIODFIELD_DAYS);
    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    2);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
}

TEST(PeriodTestFixture, get_fileds_hour_period) {
    Period period = Period::MakeCustomPeriod(5, PERIODFIELD_HOURS);
    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   5);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
}

TEST(PeriodTestFixture, get_fileds_minute_period) {
    Period period = Period::MakeCustomPeriod(20, PERIODFIELD_MINUTES);
    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 20);
    EXPECT_EQ(period.getSeconds(), 0);
}

TEST(PeriodTestFixture, get_fileds_second_period) {
    Period period = Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS);
    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 2);
}

TEST(PeriodTestFixture, equals_operator) {
    Period p1 = Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS);
    Period p2 = Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS);
    Period p3 = Period::MakeCustomPeriod(3, PERIODFIELD_SECONDS);
    Period p4 = Period::MakeCustomPeriod(2, PERIODFIELD_HOURS);

    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
    EXPECT_FALSE(p1 == p4);
    EXPECT_FALSE(p3 == p4);
}

TEST(PeriodTestFixture, not_equals_operator) {
    Period p1 = Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS);
    Period p2 = Period::MakeCustomPeriod(2, PERIODFIELD_SECONDS);

    EXPECT_FALSE(p1 != p2);
}

/**
 * This is interesting to note.  At present the order of the fields must be the same for them to be considered equal.
 */
TEST(PeriodTestFixture, two_fields_swapped_order_not_equal) {
    uint8_t data[] = {2, 0, 0, 0, 0, 5, 6, 0, 0, 0, 10}; // 5 years, 10 seconds
    uint8_t bytes_consumed;
    Period p1 = Period::Decompile(data, &bytes_consumed);

    uint8_t data1[] = {2, 6, 0, 0, 0, 10, 0, 0, 0, 0, 5}; // 10 seconds, 5 years
    Period p2 = Period::Decompile(data1, &bytes_consumed);

    EXPECT_FALSE(p1 == p2);
}

TEST(PeriodTestFixture, too_many_fields_throws_exception) {
    uint8_t data[] = {7,
                      0, 0, 0, 0, 5,
                      1, 0, 0, 0, 8,
                      2, 0, 0, 0, 8,
                      3, 0, 0, 0, 8,
                      4, 0, 0, 0, 8,
                      5, 0, 0, 0, 8,
                      6, 0, 0, 0, 8};
    uint8_t bytes_consumed;
    CEXCEPTION_T e;

    Try {
        Period p1 = Period::Decompile(data, &bytes_consumed);
        FAIL(); // Should have thrown exception.
    } Catch(e) {
        EXPECT_EQ(e, EX_BUFFER_OVERRUN);
    }
}

TEST(PeriodTestFixture, invalid_field_type_throws_exception) {
    uint8_t data[] = {1,
                      50, 0, 0, 0, 8};
    uint8_t bytes_consumed;
    CEXCEPTION_T e;

    Try {
        Period p1 = Period::Decompile(data, &bytes_consumed);
        FAIL(); // Should have thrown exception.
    } Catch(e) {
        EXPECT_EQ(e, EX_INVALID_INPUT_VALUE);
    }
}