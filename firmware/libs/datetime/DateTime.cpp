//
// Created by mcochrane on 5/04/17.
//

#include "DateTime.h"

#ifdef TESTING
    #define mk_gmtime mk_gmtime_test
    #define gmtime gmtime_test
    #define week_of_month week_of_month_test
    #define gmtime_r gmtime_r_test
    #define isotime_r isotime_r_test
    #define isotime1 isotime_test
    #define difftime difftime_test
#else
#define isotime1 isotime
#endif

DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day,
                   uint8_t hour, uint8_t minute, uint8_t second) {
    dt_tm time_ptr;
    time_ptr.tm_sec = second;
    time_ptr.tm_min = minute;
    time_ptr.tm_hour = hour;
    time_ptr.tm_mday = day;
    time_ptr.tm_mon = month-1;
    time_ptr.tm_year = year-1900;

    constructFromTmPtr(&time_ptr);
}

DateTime::DateTime(dt_tm *time_ptr) {
    constructFromTmPtr(time_ptr);
}

//bool isTmValid(dt_tm* tmptr) {
//    if (tmptr->tm_year+1900 < 2136) return true;
//    else if (tmptr->tm_year+1900 > 2136) return false;
//    else {
//        if (tmptr->tm_mon+1 < 2) return true;
//        else if (tmptr->tm_mon+1 > 2) return false;
//        else {
//            if (tmptr->tm_mday < 7) return true;
//            else if (tmptr->tm_mday > 7) return false;
//            else {
//                if (tmptr->tm_hour < 6) return true;
//                else if (tmptr->tm_hour > 6) return false;
//                else {
//                    if (tmptr->tm_min < 28) return true;
//                    else if (tmptr->tm_min > 28) return false;
//                    else {
//                        if (tmptr->tm_sec < 16) return true;
//                        else if (tmptr->tm_sec > 15) return false;
//                        else {
//                            //Here is == EMPTY... so false
//                            return false;
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

void DateTime::constructFromTmPtr(dt_tm *time_ptr) {
//    if (isTmValid(time_ptr)) {
//        time_ = mk_gmtime(time_ptr);
//    } else {
//        time_ = EMPTY_TIME;
//    }
    time_ = mk_gmtime(time_ptr);
}

//dt_tm *DateTime::toLocaltime() {
//    return localtime(&time_);
//}

dt_tm *DateTime::toGmtime() const {
    return gmtime(&time_);
}

uint16_t DateTime::getYear() const {
    int16_t yrs_since_1900 = toGmtime()->tm_year;  //years since 1900
    return (uint16_t) yrs_since_1900 + 1900;
}

uint8_t DateTime::getWeekOfMonth() const {
    dt_tm* timeptr = toGmtime();
    return week_of_month(timeptr, START_DAY_OF_WEEK);
}

void DateTime::setSecondOfMinute(int8_t second_of_minute) {
    dt_tm* time_ptr = toGmtime();
    time_ptr->tm_sec = second_of_minute;
    constructFromTmPtr(time_ptr);
}

void DateTime::setMinuteOfHour(int8_t minute) {
    dt_tm* time_ptr = toGmtime();
    time_ptr->tm_min = minute;
    constructFromTmPtr(time_ptr);
}

void DateTime::setHourOfDay(int8_t hour) {
    dt_tm* time_ptr = toGmtime();
    time_ptr->tm_hour = hour;
    constructFromTmPtr(time_ptr);
}

void DateTime::setDayOfMonth(int8_t day) {
    dt_tm* time_ptr = toGmtime();
    time_ptr->tm_mday = day;
    constructFromTmPtr(time_ptr);
}

void DateTime::setMonthOfYear(int8_t month) {
    dt_tm* time_ptr = toGmtime();
    time_ptr->tm_mon = month-1;
    constructFromTmPtr(time_ptr);
}

void DateTime::setYear(int16_t year) {
    dt_tm* time_ptr = toGmtime();
    time_ptr->tm_year = year - 1900;
    constructFromTmPtr(time_ptr);
}

DateTime DateTime::plusSeconds(int32_t seconds) const {
    return DateTime(time_ + seconds);
}

DateTime DateTime::plusMinutes(int32_t minutes) const {
    return DateTime(time_ + (60 * minutes));
}

DateTime DateTime::plusHours(int32_t hours) const {
    return DateTime(time_ + (ONE_HOUR * hours));
}

DateTime DateTime::plusDays(int32_t days) const {
    return DateTime(time_ + (ONE_DAY * days));
}

/**
 * Helper function
 * Sets the tm_year value in a tm struct, caps the year to 'valid' values of
 * 0 and 236.
 * @param time_ptr              the struct dt_tm* to modify
 * @param new_years_after_1900  the value to set tm_year in the tm struct to
 *                              for example, pass 100 to set the year to 2000
 */
void setTimePtrYears(dt_tm* time_ptr, int16_t new_years_after_1900) {
    // Add years with cap (INT16_MAX).
    // We can't represent after Tue Feb 7 2136 with dt_time_t (uint32_t).
    // tm_year is years since 1900 so max tm_year is 236.
    //TODO: consider what we should do here more carefully
    if (new_years_after_1900 >= 236) {
        time_ptr->tm_year = 236;
    } else if (new_years_after_1900 <= 0) {
        time_ptr->tm_year = 0;
    } else {
        time_ptr->tm_year = new_years_after_1900;
    }
}

DateTime DateTime::plusWeeks(int16_t weeks) const {
    return DateTime(time_ + (ONE_DAY * 7 * weeks));
}

DateTime DateTime::plusMonths(int16_t months_to_add) const {
    dt_tm* time_ptr = toGmtime();

    //if months > 12 then add a year (for each 12 months)
    int32_t years_to_add = months_to_add / 12;
    //months left to add then becomes the remainder
    months_to_add = months_to_add % 12;

    setTimePtrYears(time_ptr, time_ptr->tm_year + years_to_add);

    time_ptr->tm_mon += months_to_add;
    return DateTime(time_ptr);
}

DateTime DateTime::plusYears(int16_t years) const {
    dt_tm* time_ptr = toGmtime();

    setTimePtrYears(time_ptr, time_ptr->tm_year + years);

    return DateTime(time_ptr);
}


int32_t operator-(const DateTime &lhs, const DateTime &rhs) {
    return difftime(lhs.time_, rhs.time_);
//      return lhs.time_ - rhs.time_;
}

uint32_t DateTime::getSecondOfDay() const {
    dt_tm* time_ptr = toGmtime();
    return (uint32_t)time_ptr->tm_hour * ONE_HOUR + (uint32_t)time_ptr->tm_min * 60 + (uint32_t)time_ptr->tm_sec;
}

void DateTime::toGmtimeR(dt_tm *time_ptr) const {
    gmtime_r(&time_, time_ptr);
}

//size_t DateTime::strftime(char *s, size_t maxsize, const char *format) {
//    return ::strftime(s, maxsize, format, toGmtime());
//}
//
void DateTime::isotime(char *s) {
    ::isotime_r(toGmtime(), s);
}
//
char* DateTime::isotime() {
    return ::isotime1(toGmtime());
}
