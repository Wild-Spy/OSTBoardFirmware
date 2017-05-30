//
// Created by mcochrane on 8/04/17.
//

#include "DS3232SN.h"
#include <delay.h>
#include <libs/exception/ExceptionValues.h>
#include <libs/exception/CException.h>
#include <libs/hw/hal_gpio.h>

//extern "C" {
//#include <parts.h>
//#include <time.h>
////#include <util/delay.h>
////#include <avr/io.h>
//}
//#include <TimerDescriptionLanguage/TdlChannels.h>
//#include <libs/hal/i2c.h>

#include "min/min_transmit_cmds.h"


DS3232SN* rtc = NULL;

//DS3232SN::DS3232SN(I2c& i2c, uint8_t nreset_port, uint8_t nreset_pin, uint8_t npen_port, uint8_t npen_pin)
//        : nreset_pin_(nreset_port, nreset_pin),
//          npen_pin_(npen_port, npen_pin),
//          power_enabled_(true),
//          i2c_(i2c)
//{
//
////    i2cCreate(&i2c_, twi);
//}


DS3232SN::DS3232SN(I2c& i2c,
                   uint8_t npen_port, uint8_t npen_pin,
                   uint8_t ninterrupt_port, uint8_t ninterrupt_pin,
                   extint_callback_t int_callback)
    : i2c_(i2c),
      npen_pin_(npen_port, npen_pin),
      nint_pin_(ninterrupt_port, ninterrupt_pin),
      power_enabled_(true),
      int_callback_(int_callback)
{
//    i2cCreate(&i2c_, twi);

//#if XMEGA_C4
//    if (twi == &TWIE) {
//        twi_sda_pin_ = Pin(&PORTE, 0);
//        twi_scl_pin_ = Pin(&PORTE, 1);
//    } else if (twi == &TWIC) {
//        twi_sda_pin_ = Pin(&PORTC, 0);
//        twi_scl_pin_ = Pin(&PORTC, 1);
//    } else {
//        Throw(EX_OUT_OF_RANGE);
//    }
//#else
//    #error Please define TWI Pins for your MCU
//#endif

}

void DS3232SN::init() {
//    nreset_pin_.setDirInput();
    npen_pin_.setDirOutput();

    npen_pin_.setOutputLow(); // power is enabled
//    nreset_pin_.setOutputHigh(); //not being reset

    nint_pin_.setDirInput();
    nint_pin_.setupInterrupt(EXTINT_DETECT_LOW, EXTINT_PULL_UP, true, false);

//    twi_sda_pin_.setDirInput();
//    twi_scl_pin_.setDirInput();

    delay_ms(200);

    reset();
    i2c_.enable();

    enablePinInterrupt();
}

void DS3232SN::reset() {
//    nreset_pin_.setOutputLow();
//    _delay_ms(300);
//    nreset_pin_.setOutputHigh();
}

DateTime DS3232SN::get(void) {
    dt_tm tm;

    readTime(&tm);
    return DateTime(&tm);
}

uint8_t DS3232SN::readTime(dt_tm *dt) {
    uint8_t buf[7];
    i2c_.readBytes(DSRTC_ADDR, DSRTC_SECONDS, 7, buf);

    dt->tm_sec = bcd2dec((uint8_t) (buf[0] & ~(1 << DS1307_CH)));
    dt->tm_min = bcd2dec(buf[1]);
    dt->tm_hour = bcd2dec((uint8_t) (buf[2] & ~(1 << HR1224))); //assumes 24h clock
    dt->tm_wday = buf[3];
    dt->tm_mday = bcd2dec(buf[4]);
    dt->tm_mon = bcd2dec((uint8_t) (buf[5] & ~(1<<CENTURY))) - 1; //don't use the century bit
    dt->tm_year = bcd2dec(buf[6]) + 2000 - 1900;

    report_printf("time");
    report_printf("ymdhms: %d-%d-%d %d:%d:%d", dt->tm_year, dt->tm_mon, dt->tm_mday, dt->tm_hour, dt->tm_min, dt->tm_sec);

    return 0;
}


DateTime DS3232SN::readAlarm() {
    dt_tm dt;
    uint8_t buf[4];
    i2c_.readBytes(DSRTC_ADDR, DSALM1_SECONDS, 4, buf);

    dt.tm_sec = bcd2dec((uint8_t)buf[0]);
    dt.tm_min = bcd2dec((uint8_t)buf[1]);
    dt.tm_hour = bcd2dec((uint8_t)buf[2]);
    dt.tm_mday = bcd2dec((uint8_t)buf[3]);

    dt.tm_mon = 0;
    dt.tm_year = 2000 - 1900;

    return DateTime(&dt);
}

void DS3232SN::dumpRTC(uint8_t startAddr, uint8_t len) {
    uint8_t alen = len;
    uint8_t sa = startAddr;
    while (alen--){
        uint8_t b = i2cReadByte(sa++);
        report_printf("read[%02X]: %02X", sa-1, b);
    }
    uint8_t data[len];
    i2c_.readBytes(DSRTC_ADDR, startAddr, len, data);
    for (uint8_t i = 0; i < len; i++) {
        report_printf("read[%02X]: %02X", startAddr+i, data[i]);
    }

}

uint8_t __attribute__((noinline)) DS3232SN::bcd2dec(uint8_t n) {
    return n - 6 * (n >> 4);
}

/**
 * Write a single byte to RTC RAM.
 * @param subAddress    address to write to
 * @param data          data to write to address
 */
void DS3232SN::i2cWriteByte(uint8_t subAddress, uint8_t data) {
    cpu_irq_disable();
    i2c_.writeByte(DSRTC_ADDR, subAddress, data);
    cpu_irq_enable();
}

/**
 * Write bytes to RTC RAM.
 * @param subAddress    address to write to
 * @param data          data to be written
 * @param count         number of bytes to be written
 */
//void DS3232SN::i2cWriteBytes(uint8_t subAddress, uint8_t* data, uint8_t count)
//{
//    cpu_irq_disable();
//    i2c_.writeBytes(DSRTC_ADDR, subAddress, data, count);
//    cpu_irq_enable();
//}

/**
 * Read a single byte from RTC RAM.
 * @param subAddress    RTC address to read from
 * @return              the byte that was read
 */
uint8_t DS3232SN::i2cReadByte(uint8_t subAddress)
{
    uint8_t data; // `data` will store the register data
    cpu_irq_enter_critical();
    data = i2c_.readByte(DSRTC_ADDR, subAddress);
    cpu_irq_leave_critical();
    return data;                             // Return data read from slave register
}

/**
 * Read multiple bytes from RTC RAM.
 * @param subAddress    address in RAM to read from
 * @param data          data that was read
 * @param count         number of bytes to read
 */
//void DS3232SN::i2cReadBytes(uint8_t subAddress, uint8_t * dest, uint8_t count)
//{
//    cli();
//    i2cStartWrite(&i2c_, DSRTC_ADDR);   // Initialize the Tx buffer
//    // Next send the register to be read. OR with 0x80 to indicate multi-read.
//    i2cPut(&i2c_, subAddress);     // Put slave register address in Tx buffer
//    i2cStartRead(&i2c_, DSRTC_ADDR);
//    for (uint8_t i = 0; i < count; ++i)
//    {
//        dest[i] = (uint8_t) i2cGet(&i2c_); // Put read results in the Rx buffer
//    }
//    i2cStop(&i2c_);
//    sei();
//}

//uint8_t DS3232SN::testi2c() {
//    return i2cReadByte(DSRTC_STATUS);
//}

uint8_t DS3232SN::writeTime(dt_tm *tm) {
    cpu_irq_enter_critical();
    uint8_t buf[7];

//    for (uint8_t i = 0; i < 7; i++) buf[i] = 0;

    buf[0] = dec2bcd(tm->tm_sec);
    buf[1] = dec2bcd(tm->tm_min);
    buf[2] = dec2bcd(tm->tm_hour);        //sets 24 hour format (Bit 6 == 0)
    buf[3] = tm->tm_wday;
    buf[4] = dec2bcd(tm->tm_mday);
    buf[5] = dec2bcd(tm->tm_mon+1);
    buf[6] = dec2bcd((int16_t) tm->tm_year + 1900 - 2000);

//    report_printf("wt buf %02X, %02X, %02X, %02X, %02X, %02X, %02X", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    i2c_.writeBytes(DSRTC_ADDR, DSRTC_SECONDS, 7, buf);

    uint8_t s = i2cReadByte(DSRTC_STATUS);          //read the status register
    i2cWriteByte( DSRTC_STATUS, s & ~((1<<OSF)));   //clear the Oscillator Stop Flag

    cpu_irq_leave_critical();
    return 0;
}

void DS3232SN::set(DateTime dt) {
    dt_tm* tm = dt.toGmtime();
    writeTime(tm);
}

/**
 * Returns the value of the oscillator stop flag (OSF) bit in the
 * control/status register which indicates that the oscillator is or
 * was stopped, and that the timekeeping data may be invalid.
 * @param clearOSF  if true, clears the OSF bit
 * @return
 */
bool DS3232SN::oscStopped(bool clearOSF) {
    uint8_t s = i2cReadByte(DSRTC_STATUS);
    bool ret = (bool) (s & (1 << OSF));
    if (ret && clearOSF) {
        i2cWriteByte(DSRTC_STATUS, s & ~(1<<OSF));
    }
    return ret;
}

uint8_t __attribute__((noinline)) DS3232SN::dec2bcd(uint8_t n) {
    return n + 6 * (n / 10);
}

/**
 * Set an alarm time.  Sets the alarm registors only.  To cause the
 * INT pin to be asserted on alarm match, use alarmInterrupt().
 * This method can set either Alarm 1 or Alarm 2, depending on the
 * value of alarmType.
 * When setting Alarm2, the seconds value is ignored (Alarm 2 has
 * no seconds register).
 * @param alarmType     the type of alarm to set
 * @param seconds       seconds register
 * @param minutes       minutes register
 * @param hours         hours register
 * @param daydate       day of week or date register
 */
void DS3232SN::setAlarm(ALARM_TYPES_t alarmType, uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t daydate) {
    uint8_t addr;

    seconds = dec2bcd(seconds);
    minutes = dec2bcd(minutes);
    hours   = dec2bcd(hours);
    daydate = dec2bcd(daydate);

    if (alarmType & 0x01) seconds |= (1<<A1M1);
    if (alarmType & 0x02) minutes |= (1<<A1M2);
    if (alarmType & 0x04) hours |= (1<<A1M3);
    if (alarmType & 0x10) daydate |= (1<<DYDT);
    if (alarmType & 0x08) daydate |= (1<<A1M4);

    if (!(alarmType & 0x08)) { //alarm 1
        addr = DSALM1_SECONDS;
        i2cWriteByte(addr++, seconds);
    } else {
        addr = DSALM2_MINUTES;
    }
    i2cWriteByte(addr++, minutes);
    i2cWriteByte(addr++, hours);
    i2cWriteByte(addr++, daydate);
}

void DS3232SN::setAlarm(ALARM_TYPES_t alarmType, uint8_t minutes, uint8_t hours, uint8_t daydate) {
    setAlarm(alarmType, 0, minutes, hours, daydate);
}

/**
 * Enable or disable tn alarm interrupt
 * @param alarm_number          the alarm number (1 or 2)
 * @param interrupt_enabled     the new interrupt state (true for enabled)
 */
void DS3232SN::alarmInterrupt(uint8_t alarm_number, bool interrupt_enabled) {
    uint8_t control_reg, mask;
    control_reg = i2cReadByte(DSRTC_CONTROL);
    mask = (uint8_t) ((1 << A1IE) << (alarm_number - 1));
    if (interrupt_enabled)
        control_reg |= mask;
    else
        control_reg &= ~ mask;
    i2cWriteByte(DSRTC_CONTROL, control_reg);
}


/**
 * Checks if an alarm has been triggered and resets the alarm flag bit
 * if is has been.
 * @param alarm_number  the alarm number to check (1 or 2)
 * @return true if the given alarm has been triggered, false otherwise
 */
bool DS3232SN::alarm(uint8_t alarm_number) {
    uint8_t status_reg, mask;

    status_reg = i2cReadByte(DSRTC_STATUS);
    mask = (uint8_t) ((1 << A1F) << (alarm_number - 1));
    if (status_reg & mask) {
        status_reg &= ~mask;
        i2cWriteByte(DSRTC_STATUS, status_reg);
        return true;
    } else {
        return false;
    }
}

/**
 * Checks if the device is busy executing TCXO functions.
 * @return true if the device is busy, false if not
 */
bool DS3232SN::busy() {
    uint8_t status_reg;

    status_reg = i2cReadByte(DSRTC_STATUS);
    if (status_reg & (1<<BSY)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Enable or disable the square wave output
 * @param freq  the square wave frequency
 */
void DS3232SN::squareWave(SQWAVE_FREQS_t freq) {
    uint8_t control_reg;

    control_reg = i2cReadByte(DSRTC_CONTROL);
    if (freq >= SQWAVE_NONE) {
        control_reg |= (1<<INTCN); //off
    } else {
        control_reg = (control_reg & 0xE3) | (freq << RS1); //on
    }
    i2cWriteByte(DSRTC_CONTROL, control_reg);
}

/**
 * Returns the temperature in celcius.
 */
float DS3232SN::temperature() {
    union int16_byte {
        int i;
        uint8_t b[2];
    } rtcTemp;
    rtcTemp.b[0] = i2cReadByte(DSTEMP_LSB);
    rtcTemp.b[1] = i2cReadByte(DSTEMP_MSB);
    return ((float)(rtcTemp.i/64))/4;
}

void DS3232SN::setAlarm(ALARM_TYPES_t alarm_type, DateTime date_time) {
    dt_tm time_ptr;

    next_alarm_ = date_time;

    date_time.toGmtimeR(&time_ptr);

    if (alarm_type == ALM1_MATCH_DAY || alarm_type == ALM2_MATCH_DAY) {
        setAlarm(alarm_type, (uint8_t) time_ptr.tm_sec, (uint8_t) time_ptr.tm_min,
                 (uint8_t) time_ptr.tm_hour, (uint8_t) time_ptr.tm_wday);
    } else {
        setAlarm(alarm_type, (uint8_t) time_ptr.tm_sec, (uint8_t) time_ptr.tm_min,
                 (uint8_t) time_ptr.tm_hour, (uint8_t) time_ptr.tm_mday);
    }
}

void DS3232SN::enablePower() {
    npen_pin_.setOutputLow(); // power is enabled

    //TODO: Should set pins to regular mode?
//    twi_sda_pin_.setDirInput();
//    twi_scl_pin_.setDirInput();

    i2c_.enable();
    power_enabled_ = true;
}

void DS3232SN::disablePower() {
    i2c_.disable();

    //TODO: Should set pins to low power mode?
//    twi_sda_pin_.setDirOutput();
//    twi_sda_pin_.setOutputLow();
//    twi_scl_pin_.setDirOutput();
//    twi_scl_pin_.setOutputLow();

    npen_pin_.setOutputHigh(); // power is disabled
    power_enabled_ = false;
}

void initRtc(I2c& i2c, extint_callback_t interruptCallback) {
    rtc = new DS3232SN(i2c, HAL_GPIO_PORTA, 19, HAL_GPIO_PORTA, 6, interruptCallback);

    getRtc().init();
    // disable interrupts
    getRtc().alarmInterrupt(1, false);
    getRtc().alarmInterrupt(2, false);
    // reset alarm flags
    getRtc().alarm(1);
    getRtc().alarm(2);
}

DS3232SN &getRtc() {
    if (rtc == NULL) Throw(EX_NULL_POINTER);
    return *rtc;
}

DateTime getNow() {
    if (getRtc().powerEnabled())
        return getRtc().get();
    else {
        getRtc().enablePower();
        DateTime now = getRtc().get();
        getRtc().disablePower();
        return now;
    }
}

