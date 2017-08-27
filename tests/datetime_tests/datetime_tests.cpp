//
// Created by mcochrane on 16/05/17.
//

#include "datetime/DateTime.h"
#include "datetime/Period.h"

#include "gtest/gtest.h"

class DateTimeTestFixture : public ::testing::Test {
    
};


TEST(DateTimeTestFixture, init_from_timet_works) {
    DateTime dateTime = DateTime(100);
    EXPECT_EQ(dateTime.toTimet(), 100);
}

/**
 * EMPTY_TIME is 0xFFFFFFFF - the max value of a uint32_t.
 * The type of a dt_time_t is uint32_t.
 */
TEST(DateTimeTestFixture, empty_time_is_EMPTY_TIME) {
    DateTime dateTime = DateTime::Empty();
    EXPECT_EQ(dateTime.toTimet(), EMPTY_TIME);
}
TEST(DateTimeTestFixture, default_initialiser_is_empty_time) {
    DateTime dateTime = DateTime();
    EXPECT_EQ(dateTime, DateTime::Empty());
}

/**
 * Initialisation
 * dt_time_t represents seconds elapsed from Midnight, Jan 1 2000 UTC (the Y2K 'epoch').
 * Its range allows this implementation to represent time up to Tue Feb 7 06:28:15 2136 UTC.
*/
TEST(DateTimeTestFixture, y2k_epoch_is_zero) {
    DateTime dateTime = DateTime(2000, 1, 1, 0, 0, 0);
    EXPECT_EQ(dateTime.toTimet(), 0);
}
TEST(DateTimeTestFixture, max_range) {
    DateTime dateTime = DateTime(2136, 2, 7, 6, 28, 15);
    EXPECT_EQ(dateTime.toTimet(), 0xFFFFFFFF);
}
//Not necessarily rollover.  It's undefined...
//TEST(DateTimeTestFixture, Rollover) {
//    DateTime dateTime = DateTime(2136, 2, 7, 6, 28, 16);
//    EXPECT_EQ(dateTime.toTimet(), 0);
//}
TEST(DateTimeTestFixture, init_from_ymdhms) {
    DateTime dateTime;
    // Seconds
    dateTime = DateTime(2000, 1, 1, 0, 0, 10);
    EXPECT_EQ(dateTime.toTimet(), 10);
    // Minutes + Seconds
    dateTime = DateTime(2000, 1, 1, 0, 10, 10);
    EXPECT_EQ(dateTime.toTimet(), 60*10+10);
    // Hours + Minutes + Seconds
    dateTime = DateTime(2000, 1, 1, 1, 1, 10);
    EXPECT_EQ(dateTime.toTimet(), 3600+60+10);
    // Days
    dateTime = DateTime(2000, 1, 3, 0, 0, 0);
    EXPECT_EQ(dateTime.toTimet(), 3600*24*2);
    // Months
    dateTime = DateTime(2000, 2, 1, 0, 0, 0);
    EXPECT_EQ(dateTime.toTimet(), 3600*24*31);
    // Year
    dateTime = DateTime(2001, 1, 1, 0, 0, 0);
    EXPECT_EQ(dateTime.toTimet(), 3600*24*366); // 2000 is a leap year
    // Complex
    dateTime = DateTime(2001, 2, 4, 10, 48, 15);
    EXPECT_EQ(dateTime.toTimet(), 3600*24*366 + 3600*24*31 + 3600*24*3 + 3600*10 + 60*48 + 15); // 2000 is a leap year
}

/**
 * Invalid Initialisation
 * If the date is after Tue Feb 7 06:28:15 2136 UTC then it should return DateTime::Empty().
 * Nope, have decided that the class will be unchecked for over/underflow!
 * This will increase speed at the cost of reliability/security
 */
//TEST(DateTimeTestFixture, DateAfterRolloverReturnsEmpty) {
//    DateTime dateTime = DateTime(2136, 2, 7, 6, 28, 16);
//    EXPECT_EQ(dateTime.toTimet(), DateTime::Empty());
//
//    //Another
//    dateTime = DateTime(2300, 1, 1, 0, 0, 0);
//    EXPECT_EQ(dateTime.toTimet(), DateTime::Empty());
//}
//TEST(DateTimeTestFixture, InvalidDayReturnsEmpty) {
//    DateTime dateTime = DateTime(2136, 2, 7, 6, 28, 16);
//    EXPECT_EQ(dateTime.toTimet(), DateTime::Empty());
//
//    //Another
//    dateTime = DateTime(2300, 1, 1, 0, 0, 0);
//    EXPECT_EQ(dateTime.toTimet(), DateTime::Empty());
//}
//TEST(DateTimeTestFixture, AddReturnsEmpty) {
//    //2 minutes before rollover
//    DateTime dateTime = DateTime(2136, 2, 7, 6, 26, 15);
//    DateTime datetime2 = dateTime.plusSeconds(125);
//    EXPECT_EQ(datetime2.toTimet(), DateTime::Empty());
//}

/**
 * IsEmpty
 */
TEST(DateTimeTestFixture, is_empty) {
    DateTime dateTime;
    dateTime = DateTime::Empty();
    EXPECT_TRUE(dateTime.isEmpty());
    dateTime = DateTime(2000, 1, 3, 1, 5, 12);
    EXPECT_FALSE(dateTime.isEmpty());
}

/**
 * Get Parts
 */
TEST(DateTimeTestFixture, get_common_parts) {
    DateTime dateTime = DateTime(2017, 4, 10, 11, 45, 0);

    EXPECT_EQ(dateTime.getYear(), 2017);
    EXPECT_EQ(dateTime.getMonthOfYear(), 4);
    EXPECT_EQ(dateTime.getDayOfMonth(), 10);
    EXPECT_EQ(dateTime.getHourOfDay(), 11);
    EXPECT_EQ(dateTime.getMinuteOfHour(), 45);
    EXPECT_EQ(dateTime.getSecondOfMinute(), 0);
}
TEST(DateTimeTestFixture, get_uncommon_parts) {
    DateTime dateTime = DateTime(2017, 4, 10, 11, 45, 0);

    EXPECT_EQ(dateTime.getDayOfYear(), 31 + 28 + 31 + 9);
    EXPECT_EQ(dateTime.getWeekOfMonth(), 2);
    EXPECT_EQ(dateTime.getDayOfWeek(), MONDAY);
    EXPECT_EQ(dateTime.getSecondOfDay(), 11*3600 + 45*60);
}

/**
 * Comparison Operators
 */
TEST(DateTimeTestFixture, comparison_operators) {
    DateTime dateTime1 = DateTime(2000, 1, 1, 0, 0, 10);
    DateTime dateTime2 = DateTime(2000, 1, 1, 0, 0, 12);
    DateTime dateTime3 = DateTime(2000, 1, 1, 0, 0, 12);

    // LT
    EXPECT_TRUE(dateTime1 < dateTime2);
    EXPECT_FALSE(dateTime2 < dateTime1);

    // LE
    EXPECT_TRUE(dateTime1 <= dateTime2);
    EXPECT_FALSE(dateTime2 <= dateTime1);
    EXPECT_TRUE(dateTime2 <= dateTime3);

    // GT
    EXPECT_FALSE(dateTime1 > dateTime2);
    EXPECT_TRUE(dateTime2 > dateTime1);

    // GE
    EXPECT_FALSE(dateTime1 >= dateTime2);
    EXPECT_TRUE(dateTime2 >= dateTime1);
    EXPECT_TRUE(dateTime2 >= dateTime3);

    // EQ
    EXPECT_FALSE(dateTime1 == dateTime2);
    EXPECT_TRUE(dateTime2 == dateTime3);

    // NE
    EXPECT_TRUE(dateTime1 != dateTime2);
    EXPECT_FALSE(dateTime2 != dateTime3);
}

/**
 * Gives the duration in seconds between two DateTimes as a int32_t
 */
TEST(DateTimeTestFixture, diff_operator) {
    DateTime dateTime1 = DateTime(2000, 1, 1, 0, 0, 10);
    DateTime dateTime2 = DateTime(2000, 1, 1, 0, 0, 12);

    // 2 seconds difference
    EXPECT_EQ(dateTime2 - dateTime1, 2);
    EXPECT_EQ(dateTime1 - dateTime2, -2);
}


/**
 * Testing Plus[Part] functions
 */
TEST(DateTimeTestFixture, plus_seconds) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    EXPECT_EQ(dateTime.plusSeconds(10),   DateTime(2017, 1, 1, 0, 0, 10));
    EXPECT_EQ(dateTime.plusSeconds(70),   DateTime(2017, 1, 1, 0, 1, 10));
    EXPECT_EQ(dateTime.plusSeconds(3670), DateTime(2017, 1, 1, 1, 1, 10));
}
TEST(DateTimeTestFixture, plus_minutes) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    EXPECT_EQ(dateTime.plusMinutes(10),             DateTime(2017, 1, 1, 0, 10, 0));
    EXPECT_EQ(dateTime.plusMinutes(60 + 10),        DateTime(2017, 1, 1, 1, 10, 0));
    EXPECT_EQ(dateTime.plusMinutes(1440 + 60 + 10), DateTime(2017, 1, 2, 1, 10, 0));
}
TEST(DateTimeTestFixture, plus_hours) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    EXPECT_EQ(dateTime.plusHours(2),            DateTime(2017, 1, 1, 2, 0, 0));
    EXPECT_EQ(dateTime.plusHours(24 + 2),       DateTime(2017, 1, 2, 2, 0, 0));
    EXPECT_EQ(dateTime.plusHours(744 + 24 + 2), DateTime(2017, 2, 2, 2, 0, 0));
}
TEST(DateTimeTestFixture, plus_days) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    EXPECT_EQ(dateTime.plusDays(2),            DateTime(2017, 1, 3, 0, 0, 0));
    EXPECT_EQ(dateTime.plusDays(31 + 2),       DateTime(2017, 2, 3, 0, 0, 0));
    EXPECT_EQ(dateTime.plusDays(365 + 31 + 2), DateTime(2018, 2, 3, 0, 0, 0));
}
TEST(DateTimeTestFixture, plus_weeks) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    EXPECT_EQ(dateTime.plusWeeks(1),  DateTime(2017, 1, 8, 0, 0, 0));
    EXPECT_EQ(dateTime.plusWeeks(5),  DateTime(2017, 2, 5, 0, 0, 0));
    EXPECT_EQ(dateTime.plusWeeks(53), DateTime(2018, 1, 7, 0, 0, 0));
}
TEST(DateTimeTestFixture, plus_months) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    EXPECT_EQ(dateTime.plusMonths(2),      DateTime(2017, 3, 1, 0, 0, 0));
    EXPECT_EQ(dateTime.plusMonths(12 + 2), DateTime(2018, 3, 1, 0, 0, 0));
}
TEST(DateTimeTestFixture, plus_years) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    EXPECT_EQ(dateTime.plusYears(2), DateTime(2019, 1, 1, 0, 0, 0));
}


/**
 * Testing Plus[Part] functions with negative numbers
 */
TEST(DateTimeTestFixture, minus_seconds) {
    DateTime dateTime = DateTime(2017, 1, 1, 1, 1, 10);

    EXPECT_EQ(dateTime.plusSeconds(-10),   DateTime(2017, 1, 1, 1, 1, 0));
    EXPECT_EQ(dateTime.plusSeconds(-70),   DateTime(2017, 1, 1, 1, 0, 0));
    EXPECT_EQ(dateTime.plusSeconds(-3670), DateTime(2017, 1, 1, 0, 0, 0));
}
TEST(DateTimeTestFixture, minus_minutes) {
    DateTime dateTime = DateTime(2017, 1, 2, 1, 10, 0);

    EXPECT_EQ(dateTime.plusMinutes(-10),             DateTime(2017, 1, 2, 1, 0, 0));
    EXPECT_EQ(dateTime.plusMinutes(-60 - 10),        DateTime(2017, 1, 2, 0, 0, 0));
    EXPECT_EQ(dateTime.plusMinutes(-1440 - 60 - 10), DateTime(2017, 1, 1, 0, 0, 0));
}
TEST(DateTimeTestFixture, minus_hours) {
    DateTime dateTime = DateTime(2017, 2, 2, 2, 0, 0);

    EXPECT_EQ(dateTime.plusHours(-2),            DateTime(2017, 2, 2, 0, 0, 0));
    EXPECT_EQ(dateTime.plusHours(-24 - 2),       DateTime(2017, 2, 1, 0, 0, 0));
    EXPECT_EQ(dateTime.plusHours(-744 - 24 - 2), DateTime(2017, 1, 1, 0, 0, 0));
}
TEST(DateTimeTestFixture, minus_days) {
    DateTime dateTime = DateTime(2018, 2, 3, 0, 0, 0);

    EXPECT_EQ(dateTime.plusDays(-2),            DateTime(2018, 2, 1, 0, 0, 0));
    EXPECT_EQ(dateTime.plusDays(-31 - 2),       DateTime(2018, 1, 1, 0, 0, 0));
    EXPECT_EQ(dateTime.plusDays(-365 - 31 - 2), DateTime(2017, 1, 1, 0, 0, 0));
}
TEST(DateTimeTestFixture, minus_weeks) {
    DateTime dateTime = DateTime(2017, 1, 8, 0, 0, 0);
    EXPECT_EQ(dateTime.plusWeeks(-1),  DateTime(2017, 1, 1, 0, 0, 0));

    dateTime = DateTime(2017, 2, 5, 0, 0, 0);
    EXPECT_EQ(dateTime.plusWeeks(-5),  DateTime(2017, 1, 1, 0, 0, 0));

    dateTime = DateTime(2018, 1, 7, 0, 0, 0);
    EXPECT_EQ(dateTime.plusWeeks(-53), DateTime(2017, 1, 1, 0, 0, 0));
}
TEST(DateTimeTestFixture, minus_months) {
    DateTime dateTime = DateTime(2018, 3, 1, 0, 0, 0);

    EXPECT_EQ(dateTime.plusMonths(-2),      DateTime(2018, 1, 1, 0, 0, 0));
    EXPECT_EQ(dateTime.plusMonths(-12 - 2), DateTime(2017, 1, 1, 0, 0, 0));
}
TEST(DateTimeTestFixture, minus_years) {
    DateTime dateTime = DateTime(2019, 1, 1, 0, 0, 0);

    EXPECT_EQ(dateTime.plusYears(-2), DateTime(2017, 1, 1, 0, 0, 0));
}


/**
 * Set [part] functions
 */
TEST(DateTimeTestFixture, set_second_of_minute) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    dateTime.setSecondOfMinute(10);

    EXPECT_EQ(dateTime, DateTime(2017, 1, 1, 0, 0, 10));
}
TEST(DateTimeTestFixture, set_minute_of_hour) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    dateTime.setMinuteOfHour(10);

    EXPECT_EQ(dateTime, DateTime(2017, 1, 1, 0, 10, 0));
}
TEST(DateTimeTestFixture, set_hour_of_day) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    dateTime.setHourOfDay(10);

    EXPECT_EQ(dateTime, DateTime(2017, 1, 1, 10, 0, 0));
}
TEST(DateTimeTestFixture, set_day_of_month) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    dateTime.setDayOfMonth(10);

    EXPECT_EQ(dateTime, DateTime(2017, 1, 10, 0, 0, 0));
}
TEST(DateTimeTestFixture, set_month_of_year) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    dateTime.setMonthOfYear(10);

    EXPECT_EQ(dateTime, DateTime(2017, 10, 1, 0, 0, 0));
}
TEST(DateTimeTestFixture, set_year) {
    DateTime dateTime = DateTime(2017, 1, 1, 0, 0, 0);

    dateTime.setYear(2020);

    EXPECT_EQ(dateTime, DateTime(2020, 1, 1, 0, 0, 0));
}

/**
 * Time printing functions
 */
TEST(DateTimeTestFixture, iso_time) {
    DateTime dateTime = DateTime(2020, 1, 1, 3, 4, 5);

    EXPECT_STREQ(dateTime.isotime(), "2020-01-01 03:04:05");
}
TEST(DateTimeTestFixture, iso_time_reentrant) {
    DateTime dateTime = DateTime(2019, 1, 2, 3, 4, 5);

    char isostr[20];
    dateTime.isotime(isostr);

    EXPECT_STREQ(isostr, "2019-01-02 03:04:05");
}

TEST(DateTimeTestFixture, add_period_seconds) {
    DateTime dateTime = DateTime(2020, 1, 1, 0, 0, 0);
    EXPECT_EQ(dateTime + Period::MakeCustomPeriod(10, PERIODFIELD_SECONDS), DateTime(2020, 1, 1, 0, 0, 10));
}

TEST(DateTimeTestFixture, add_period_minutes) {
    DateTime dateTime = DateTime(2020, 1, 1, 0, 0, 0);
    EXPECT_EQ(dateTime + Period::MakeCustomPeriod(10, PERIODFIELD_MINUTES), DateTime(2020, 1, 1, 0, 10, 0));
}

TEST(DateTimeTestFixture, add_period_hours) {
    DateTime dateTime = DateTime(2020, 1, 1, 0, 0, 0);
    EXPECT_EQ(dateTime + Period::MakeCustomPeriod(10, PERIODFIELD_HOURS), DateTime(2020, 1, 1, 10, 0, 0));
}

TEST(DateTimeTestFixture, add_period_days) {
    DateTime dateTime = DateTime(2020, 1, 1, 0, 0, 0);
    EXPECT_EQ(dateTime + Period::MakeCustomPeriod(10, PERIODFIELD_DAYS), DateTime(2020, 1, 11, 0, 0, 0));
}

TEST(DateTimeTestFixture, add_period_months) {
    DateTime dateTime = DateTime(2020, 1, 1, 0, 0, 0);
    EXPECT_EQ(dateTime + Period::MakeCustomPeriod(10, PERIODFIELD_MONTHS), DateTime(2020, 11, 1, 0, 0, 0));
}

TEST(DateTimeTestFixture, add_period_weeks) {
    DateTime dateTime = DateTime(2020, 1, 1, 0, 0, 0);
    EXPECT_EQ(dateTime + Period::MakeCustomPeriod(1, PERIODFIELD_WEEKS), DateTime(2020, 1, 8, 0, 0, 0));
}