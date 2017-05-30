//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_DATETIME_H
#define WS_OST_DATETIME_H

extern "C" {
#ifdef TESTING
    #include <time_test.h>
#else
    #include <time_avr.h>
#endif
//daylight savings.....
//#include <util/usa_dst.h>
};

// Names below renamed so they can be mocked in testing
#ifdef TESTING
    typedef time_t_test dt_time_t;
    typedef struct tm_test dt_tm;
#else
    typedef time_t_avr dt_time_t;
    typedef struct tm_avr dt_tm;
#endif

#include <stdint.h>

#define EMPTY_TIME          0xFFFFFFFF
#define START_DAY_OF_WEEK   MONDAY

/**
 * DateTime class which represents a date and time
 * NOTE: This class is not checked for overflow or underflow!
 *       If DateTime is initialised with a date/time before
 *       MINIMUM TIME: Jan 1 2000 00:00:00
 *       or after
 *       MAXIMUM TIME: Feb 7 2136 06:28:14
 *       then it's behaviour is undefined.
 *       This also applies to any addition or subtraction which leads to
 *       a datetime outside of this range.
 */
class DateTime {
public:
    DateTime()
            : time_(EMPTY_TIME)
    {};

    DateTime(dt_time_t time)
            : time_(time)
    {};

    DateTime(uint16_t year, uint8_t month, uint8_t day,
             uint8_t hour, uint8_t minute, uint8_t second);

    DateTime(dt_tm* time_ptr);

    static DateTime Empty() { return DateTime(); };

    bool isEmpty() { return (time_ == EMPTY_TIME); };

    uint8_t getSecondOfMinute() const { return (uint8_t) toGmtime()->tm_sec; };
    uint32_t getSecondOfDay() const;
    uint8_t getMinuteOfHour() const { return (uint8_t) toGmtime()->tm_min; };
    uint8_t getHourOfDay() const { return (uint8_t) toGmtime()->tm_hour; };
    uint8_t getDayOfWeek() const { return (uint8_t) toGmtime()->tm_wday; };
    uint8_t getDayOfMonth() const { return (uint8_t) toGmtime()->tm_mday; };
    uint16_t getDayOfYear() const { return (uint16_t) toGmtime()->tm_yday; };
    uint8_t getWeekOfMonth() const;
    uint8_t getMonthOfYear() const { return (uint8_t) toGmtime()->tm_mon+1; };
    uint16_t getYear() const;

    friend bool operator<(const DateTime& lhs, const DateTime& rhs) {
        return lhs.time_ < rhs.time_;
    };

    friend bool operator<=(const DateTime& lhs, const DateTime& rhs) {
        return lhs.time_ <= rhs.time_;
    };

    friend bool operator>(const DateTime& lhs, const DateTime& rhs) {
        return lhs.time_ > rhs.time_;
    };

    friend bool operator>=(const DateTime& lhs, const DateTime& rhs) {
        return lhs.time_ >= rhs.time_;
    };

    friend bool operator==(const DateTime& lhs, const DateTime& rhs) {
        return lhs.time_ == rhs.time_;
    };

    friend bool operator!=(const DateTime& lhs, const DateTime& rhs) {
        return lhs.time_ != rhs.time_;
    };

    /**
     * @see Period operator + between DateTime and Period.
     */
    //friend DateTime operator+(const DateTime& lhs, const Period& rhs);

    /**
     * Gives the duration in seconds between two DateTimes
     * @param lhs
     * @param rhs
     * @return int32_t duration in seconds
     */
    friend int32_t operator-(const DateTime& lhs, const DateTime& rhs);

    void setSecondOfMinute(int8_t second);
    void setMinuteOfHour(int8_t minute);
    void setHourOfDay(int8_t hour);
    void setDayOfMonth(int8_t day);
    /**
     * @param month where 1 = JAN, 2 = FEB, etc.
     */
    void setMonthOfYear(int8_t month);
    void setYear(int16_t year);

    DateTime plusSeconds(int32_t seconds) const;
    DateTime plusMinutes(int32_t minutes) const;
    DateTime plusHours(int32_t hours) const;
    DateTime plusDays(int32_t days) const;
    DateTime plusWeeks(int16_t weeks) const;
    DateTime plusMonths(int16_t months_to_add) const;
    DateTime plusYears(int16_t years) const;

//    size_t strftime(char* s, size_t maxsize, const char* format);

    /**
     * Constructs an ascii string of the form:
     *      2013-03-23 01:03:52

     * @return the string (length is always 19)
     */
    void isotime(char* s);
    char* isotime();

    dt_tm* toGmtime() const;
    void toGmtimeR(dt_tm* time_ptr) const;
//    dt_tm* toLocaltime();
    dt_time_t toTimet() { return time_; };
private:
    void constructFromTmPtr(dt_tm *time_ptr);

private:
    dt_time_t time_;
};


#endif //WS_OST_DATETIME_H
