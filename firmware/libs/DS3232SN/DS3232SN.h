//
// Created by mcochrane on 8/04/17.
//

#ifndef WS_OST_DS3232SN_H
#define WS_OST_DS3232SN_H

#include <hal/I2c.h>
#include <hal/Pin.h>
#include <datetime/DateTime.h>
#include "DS3232SN_defs.h"


//Alarm masks
enum ALARM_TYPES_t {
    ALM1_EVERY_SECOND = 0x0F,
    ALM1_MATCH_SECONDS = 0x0E,     //match seconds
    ALM1_MATCH_MINUTES = 0x0C,     //match minutes *and* seconds
    ALM1_MATCH_HOURS = 0x08,       //match hours *and* minutes, seconds
    ALM1_MATCH_DATE = 0x00,        //match date *and* hours, minutes, seconds
    ALM1_MATCH_DAY = 0x10,         //match day *and* hours, minutes, seconds
    ALM2_EVERY_MINUTE = 0x8E,
    ALM2_MATCH_MINUTES = 0x8C,     //match minutes
    ALM2_MATCH_HOURS = 0x88,       //match hours *and* minutes
    ALM2_MATCH_DATE = 0x80,        //match date *and* hours, minutes
    ALM2_MATCH_DAY = 0x90,         //match day *and* hours, minutes
};

//Square-wave output frequency (TS2, RS1 bits)
enum SQWAVE_FREQS_t {SQWAVE_1_HZ, SQWAVE_1024_HZ, SQWAVE_4096_HZ, SQWAVE_8192_HZ, SQWAVE_NONE};

class DS3232SN {
public:
//    DS3232SN(I2c& i2c,
//             uint8_t nreset_port, uint8_t nreset_pin,
//             uint8_t npen_port, uint8_t npen_pin);

    DS3232SN(I2c& i2c,
//             uint8_t nreset_port, uint8_t nreset_pin,
             uint8_t npen_port, uint8_t npen_pin,
             uint8_t ninterrupt_port, uint8_t ninterrupt_pin,
             extint_callback_t int_callback);

    void init();
//    void reset();
    DateTime get();
    void set(DateTime dt);
    bool oscStopped(bool clearOSF);
    void setAlarm(ALARM_TYPES_t alarm_type, DateTime date_time);
    void setAlarm(ALARM_TYPES_t alarmType, uint8_t seconds,
                  uint8_t minutes, uint8_t hours, uint8_t daydate);
    void setAlarm(ALARM_TYPES_t alarmType, uint8_t minutes,
                  uint8_t hours, uint8_t daydate);
    void alarmInterrupt(uint8_t alarm_number, bool interrupt_enabled);
    bool alarm(uint8_t alarm_number);
    void squareWave(SQWAVE_FREQS_t freq);
    float temperature();

    void dumpRTC(uint8_t startAddr, uint8_t len);
    DateTime readAlarm();

    DateTime getNextAlarm() { return next_alarm_; };

    bool busy();
    bool conv();

    void forceTemperatureConversion();

    void enablePower();
    void disablePower();

    bool powerEnabled() {
        return power_enabled_;
    }

    /**
     * Disables the MCU interrupt on the nint pin
     */
    void disablePinInterrupt() { nint_pin_.unregisterCallback(); }

    /**
     * Enables the MCU interrupt on the nint pin
     */
    void enablePinInterrupt() { nint_pin_.registerCallback(int_callback_); }

private:
    uint8_t dec2bcd(uint8_t n);
    static uint8_t bcd2dec(uint8_t n);
    uint8_t readTime(dt_tm* dt);
    uint8_t writeTime(dt_tm* tm);

#if defined(MCU_TESTING)
public:
#else
private:
#endif
    //uint8_t testi2c();
    void i2cWriteByte(uint8_t subAddress, uint8_t data);
    uint8_t i2cReadByte(uint8_t subAddress);
//    void i2cReadBytes(uint8_t subAddress, uint8_t *dest, uint8_t count);
//    void i2cWriteBytes(uint8_t subAddress, uint8_t *data, uint8_t count);

#if defined(MCU_TESTING)
public:
#else
private:
#endif
    I2c& i2c_;
//    Pin nreset_pin_;
    Pin npen_pin_; //active low power_enable_pin
    Pin nint_pin_;  //active low interrupt pin
    DateTime next_alarm_;

//    Pin twi_sda_pin_;
//    Pin twi_scl_pin_;
    bool power_enabled_;
    extint_callback_t int_callback_;
};

void initRtc(I2c& i2c, extint_callback_t interruptCallback);
DS3232SN& getRtc();
DateTime getNow();

#endif //WS_OST_DS3232SN_H
