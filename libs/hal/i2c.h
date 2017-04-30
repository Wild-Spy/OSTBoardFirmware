/************************************************************************/
/* I2C Driver                                                           */
/*                                                                      */
/* hal.h                                                                */
/*                                                                      */
/* Alex Forencich <alex@alexforencich.com>                              */
/*                                                                      */
/* Copyright (c) 2011 Alex Forencich                                    */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files(the "Software"), to deal in the Software without restriction,  */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#ifndef __I2C_H
#define __I2C_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>

//#include "iostream.h"

// Defines
#define TWIC_IND 0
#define TWIE_IND 1
#define TWID_IND 2
#define TWIF_IND 3

#if defined(TWIF)
#define MAX_TWI_IND 3
#else
#define MAX_TWI_IND 1
#endif

#define I2C_MODE_MASTER         0x00
#define I2C_MODE_SLAVE          0x80

#define I2C_FLAG_ACTIVE         0x01


#define I2C_WAIT_WRITE_MASTER(i2c) while (!(i2c->twi->MASTER.STATUS & TWI_MASTER_WIF_bm)) { }
#define I2C_WAIT_READ_MASTER(i2c) while (!(i2c->twi->MASTER.STATUS & TWI_MASTER_RIF_bm)) { }

// Per object data
typedef struct {
	TWI_t *twi;
	int twi_ind;
	size_t request;
	char flags;
} I2c;

	
// Static data
//static I2c *i2c_list[MAX_TWI_IND-1];

	
// Private methods
	
// Private static methods
// char i2cWhichTwi(TWI_t *_twi);
// TWI_t* i2cGetTwi(char _twi);
// PORT_t* i2cGetPort(char _twi);

	
// Public methods
void i2cCreate(I2c* i2c, TWI_t *_twi);
void i2cDestroy(I2c* i2c);
	
void i2cBegin(I2c* i2c, uint32_t baud);
void i2cEnd(I2c* i2c);
	
void i2cStartWrite(I2c* i2c, uint8_t addr);
void i2cStartRead(I2c* i2c, uint8_t addr);
void i2cStartRaw(I2c* i2c, uint8_t addr);
void i2cStop(I2c* i2c);
	
void i2cSetRequest(I2c* i2c, size_t count);
size_t i2cAvailable(I2c* i2c);
	
void i2cPut(I2c* i2c, uint8_t c);

uint8_t i2cGet(I2c* i2c);
	
// void setup_stream(FILE *stream);
// 	
// // Static methods
// static int put(char c, FILE *stream);
// static int get(FILE *stream);
	
//static inline void handle_interrupts(char _i2c);
//static void handle_interrupts(Usart *_i2c);

// Prototypes


#endif // __I2C_H