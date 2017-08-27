//
// Created by mcochrane on 17/05/17.
//

#include "datetime/Period.h"

#include "gtest/gtest.h"

/* Compiled Structure
 * byte (field count)
 * (field 1)
 * byte (field type)
 * 4 bytes (field value)
 * (field 2)
 * byte (field type)
 * 4 bytes (field value)
 * (...)
 * (field N)
 * byte (field type)
 * 4 bytes (field value)
 */

TEST(PeriodCompileTestFixture, decompile_empty) {
    uint8_t data[] = {0};
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);
    EXPECT_TRUE(period.isEmpty());
    EXPECT_EQ(bytes_consumed, 1);
}

TEST(PeriodCompileTestFixture, decompile_single_field_second) {
    uint8_t data[] = {1, 6, 0, 0, 0, 5}; // 5 seconds
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);

    EXPECT_EQ(period.getFieldCount(), 1);

    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 5);
    EXPECT_EQ(bytes_consumed, 6);
}

TEST(PeriodCompileTestFixture, decompile_single_field_minute) {
    uint8_t data[] = {1, 5, 0, 0, 0, 5}; // 5 minutes
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);

    EXPECT_EQ(period.getFieldCount(), 1);

    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 5);
    EXPECT_EQ(period.getSeconds(), 0);
    EXPECT_EQ(bytes_consumed, 6);
}

TEST(PeriodCompileTestFixture, decompile_single_field_hour) {
    uint8_t data[] = {1, 4, 0, 0, 0, 5}; // 5 hours
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);

    EXPECT_EQ(period.getFieldCount(), 1);

    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   5);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
    EXPECT_EQ(bytes_consumed, 6);
}

TEST(PeriodCompileTestFixture, decompile_single_field_day) {
    uint8_t data[] = {1, 3, 0, 0, 0, 5}; // 5 days
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);

    EXPECT_EQ(period.getFieldCount(), 1);

    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    5);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
    EXPECT_EQ(bytes_consumed, 6);
}

TEST(PeriodCompileTestFixture, decompile_single_field_week) {
    uint8_t data[] = {1, 2, 0, 0, 0, 5}; // 5 weeks
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);

    EXPECT_EQ(period.getFieldCount(), 1);

    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   5);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
    EXPECT_EQ(bytes_consumed, 6);
}

TEST(PeriodCompileTestFixture, decompile_single_field_month) {
    uint8_t data[] = {1, 1, 0, 0, 0, 5}; // 5 months
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);

    EXPECT_EQ(period.getFieldCount(), 1);

    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  5);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
    EXPECT_EQ(bytes_consumed, 6);
}

TEST(PeriodCompileTestFixture, decompile_single_field_year) {
    uint8_t data[] = {1, 0, 0, 0, 0, 5}; // 5 years
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);

    EXPECT_EQ(period.getFieldCount(), 1);

    EXPECT_EQ(period.getYears(),   5);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 0);
    EXPECT_EQ(bytes_consumed, 6);
}

TEST(PeriodCompileTestFixture, decompile_two_fields) {
    uint8_t data[] = {2, 0, 0, 0, 0, 5, 6, 0, 0, 0, 10}; // 5 years, 10 seconds
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);

    EXPECT_EQ(period.getFieldCount(), 2);

    EXPECT_EQ(period.getYears(),   5);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 10);
    EXPECT_EQ(bytes_consumed, 11);
}

TEST(PeriodCompileTestFixture, decompile_big_field) {
    uint8_t data[] = {1, 6, 0x07, 0x5B, 0xCD, 0x15}; // 123456789 seconds
    uint8_t bytes_consumed;
    Period period = Period::Decompile(data, &bytes_consumed);

    EXPECT_EQ(period.getFieldCount(), 1);

    EXPECT_EQ(period.getYears(),   0);
    EXPECT_EQ(period.getMonths(),  0);
    EXPECT_EQ(period.getWeeks(),   0);
    EXPECT_EQ(period.getDays(),    0);
    EXPECT_EQ(period.getHours(),   0);
    EXPECT_EQ(period.getMinutes(), 0);
    EXPECT_EQ(period.getSeconds(), 123456789);
    EXPECT_EQ(bytes_consumed, 6);
}