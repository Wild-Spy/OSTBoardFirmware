//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_DATETIME_H
#define WS_OST_DATETIME_H

extern "C" {
#include <time.h>
//daylight savings.....
//#include <util/usa_dst.h>
};

#include <stdint.h>
#include <stdint-gcc.h>

#define EMPTY_TIME          0xFFFFFFFF
#define START_DAY_OF_WEEK   MONDAY

class DateTime {
public:
    DateTime()
            : time_(EMPTY_TIME)
    {};

    DateTime(time_t time)
            : time_(time)
    {};

    DateTime(uint16_t year, uint8_t month, uint8_t day,
             uint8_t hour, uint8_t minute, uint8_t second);

    DateTime(struct tm* time_ptr);

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
    void setMonthOfYear(int8_t month);
    void setYear(int16_t year);

    void addSeconds(int32_t seconds);
    void addMinutes(int32_t minutes);
    void addHours(int32_t hours);
    void addDays(int32_t days);
    void addWeeks(int16_t weeks);
    void addMonths(int16_t months_to_add);
    void addYears(int16_t years);

//    size_t strftime(char* s, size_t maxsize, const char* format);

    /**
     * Constructs an ascii string of the form:
     *      2013-03-23 01:03:52

     * @return the string (length is always 19)
     */
    void isotime(char* s);
    char* isotime();

    struct tm* toGmtime() const;
    void toGmtimeR(struct tm* time_ptr) const;
//    struct tm* toLocaltime();
    time_t toTimet() { return time_; };
private:
    void constructFromTmPtr(struct tm *time_ptr);

private:
    time_t time_;


};


#endif //WS_OST_DATETIME_H
