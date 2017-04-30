//
// Created by mcochrane on 8/04/17.
//

#ifndef WS_OST_DS3232SN_DEFS_H
#define WS_OST_DS3232SN_DEFS_H

//DS3232 I2C Address
#define DSRTC_ADDR 0x68

//DS3232 Register Addresses
#define DSRTC_SECONDS 0x00
#define DSRTC_MINUTES 0x01
#define DSRTC_HOURS 0x02
#define DSRTC_DAY 0x03
#define DSRTC_DATE 0x04
#define DSRTC_MONTH 0x05
#define DSRTC_YEAR 0x06
#define DSALM1_SECONDS 0x07
#define DSALM1_MINUTES 0x08
#define DSALM1_HOURS 0x09
#define DSALM1_DAYDATE 0x0A
#define DSALM2_MINUTES 0x0B
#define DSALM2_HOURS 0x0C
#define DSALM2_DAYDATE 0x0D
#define DSRTC_CONTROL 0x0E
#define DSRTC_STATUS 0x0F
#define DSRTC_AGING 0x10
#define DSTEMP_MSB 0x11
#define DSTEMP_LSB 0x12
#define DSSRAM_START_ADDR 0x14    //first SRAM address
#define DSSRAM_SIZE 236           //number of bytes of SRAM

//Alarm mask bits
#define A1M1 7
#define A1M2 7
#define A1M3 7
#define A1M4 7
#define A2M2 7
#define A2M3 7
#define A2M4 7

//Control register bits
#define EOSC 7
#define BBSQW 6
#define CONV 5
#define RS2 4
#define RS1 3
#define INTCN 2
#define A2IE 1
#define A1IE 0

//Status register bits
#define OSF 7
#define BB32KHZ 6
#define CRATE1 5
#define CRATE0 4
#define EN32KHZ 3
#define BSY 2
#define A2F 1
#define A1F 0

#define ALARM_1 1                  //constants for calling functions
#define ALARM_2 2

//Other
#define DS1307_CH 7                //for DS1307 compatibility, Clock Halt bit in Seconds register
#define HR1224 6                   //Hours register 12 or 24 hour mode (24 hour mode==0)
#define CENTURY 7                  //Century bit in Month register
#define DYDT 6                     //Day/Date flag bit in alarm Day/Date registers


#endif //WS_OST_DS3232SN_DEFS_H
