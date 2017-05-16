/************************************************************************/
/* I2C Driver                                                           */
/*                                                                      */
/* hal.cpp                                                              */
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

#include "i2c.h"
#include <avr/io.h>

// Statics
I2c* i2c_list[MAX_TWI_IND-1];

char i2cWhichTwi(TWI_t *_twi)
{
	#if MAX_TWI_IND >= TWIC_IND
	if ((uintptr_t)_twi == (uintptr_t)&TWIC)
	return TWIC_IND;
	#endif
	#if MAX_TWI_IND >= TWIE_IND
	if ((uintptr_t)_twi == (uintptr_t)&TWIE)
	return TWIE_IND;
	#endif
	#if MAX_TWI_IND >= TWID_IND
	if ((uintptr_t)_twi == (uintptr_t)&TWID)
	return TWID_IND;
	#endif
	#if MAX_TWI_IND >= TWIF_IND
	if ((uintptr_t)_twi == (uintptr_t)&TWIF)
	return TWIF_IND;
	#endif
	return -1;
}


TWI_t* i2cGetTwi(char _twi)
{
	switch (_twi)
	{
		#if MAX_TWI_IND >= TWIC_IND
		case TWIC_IND:
		return &TWIC;
		#endif
		#if MAX_TWI_IND >= TWIE_IND
		case TWIE_IND:
		return &TWIE;
		#endif
		#if MAX_TWI_IND >= TWID_IND
		case TWID_IND:
		return &TWID;
		#endif
		#if MAX_TWI_IND >= TWIF_IND
		case TWIF_IND:
		return &TWIF;
		#endif
		default:
		return 0;
	}
}


PORT_t* i2cGetPort(char _twi)
{
	switch (_twi)
	{
		#if MAX_TWI_IND >= TWIC_IND
		case TWIC_IND:
		return &PORTC;
		#endif
		#if MAX_TWI_IND >= TWIE_IND
		case TWIE_IND:
		return &PORTE;
		#endif
		#if MAX_TWI_IND >= TWID_IND
		case TWID_IND:
		return &PORTD;
		#endif
		#if MAX_TWI_IND >= TWIF_IND
		case TWIF_IND:
		return &PORTF;
		#endif
		default:
		return 0;
	}
}

void i2cCreate(I2c* i2c, TWI_t *_twi)
{
	i2c->twi = _twi;
	i2c->flags = 0;
	i2c->twi_ind = i2cWhichTwi(_twi);
	i2c_list[i2c->twi_ind-1] = i2c;
    i2cEnd(i2c);
}


void i2cDestroy(I2c* i2c)
{
	i2cEnd(i2c);
	i2c_list[i2c->twi_ind-1] = 0;
}


void i2cBegin(I2c* i2c, uint32_t baud)
{
    i2c->flags = I2C_MODE_MASTER;
	
	i2c->twi->MASTER.CTRLA = TWI_MASTER_INTLVL_OFF_gc | TWI_MASTER_ENABLE_bm;
	i2c->twi->MASTER.CTRLB = TWI_MASTER_TIMEOUT_DISABLED_gc;
	i2c->twi->MASTER.BAUD = (F_CPU / (2 * baud)) - 5;
	i2c->twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}


void i2cEnd(I2c* i2c)
{
	i2c->twi->MASTER.CTRLA = 0;
	
	i2c->flags = 0;
}


void i2cStartWrite(I2c* i2c, uint8_t addr)
{
	i2cStartRaw(i2c, (addr << 1) | 0x00);
}


void i2cStartRead(I2c* i2c, uint8_t addr)
{
	i2cStartRaw(i2c, (addr << 1) | 0x01);
}


void i2cStartRaw(I2c* i2c, uint8_t addr)
{
	if (i2c->flags & I2C_FLAG_ACTIVE)
	{
		i2c->twi->MASTER.CTRLC = TWI_MASTER_CMD_REPSTART_gc;
	}

	i2c->twi->MASTER.ADDR = addr;
	
	i2c->flags |= I2C_FLAG_ACTIVE;
	
	if (addr&0x01) {
		I2C_WAIT_READ_MASTER(i2c);	
	} else {
		I2C_WAIT_WRITE_MASTER(i2c);	
	}
	
	i2c->request = 0;
}


void i2cStop(I2c* i2c)
{
	if (i2c->flags & I2C_FLAG_ACTIVE)
	{
		i2c->twi->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc | TWI_MASTER_ACKACT_bm;
		i2c->flags &= ~I2C_FLAG_ACTIVE;
	}
}


void i2cSetRequest(I2c* i2c, size_t _request)
{
	i2c->request = _request;
}


size_t i2cAvailable(I2c* i2c)
{
	return i2c->request;
}


void i2cPut(I2c* i2c, uint8_t c)
{
	if (i2c->flags & I2C_FLAG_ACTIVE)
	{
		i2c->twi->MASTER.DATA = c;
		i2c->twi->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
		I2C_WAIT_WRITE_MASTER(i2c);
	}
}


uint8_t i2cGet(I2c* i2c)
{
	uint8_t c = 0;
	if (i2c->flags & I2C_FLAG_ACTIVE)
	{
		if (!(i2c->twi->MASTER.STATUS & TWI_MASTER_RIF_bm))
		{
			i2c->twi->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
			I2C_WAIT_READ_MASTER(i2c);
		}
		c = i2c->twi->MASTER.DATA;
	}
	return c;
}


// void i2cSetupStream(I2c* hal, FILE *stream)
// {
// 	fdev_setup_stream(stream, put, get, _FDEV_SETUP_RW);
// 	fdev_set_udata(stream, this);
// }
// 
// 
// // static
// int I2c::put(char c, FILE *stream)
// {
// 	I2c *u;
// 	u = (I2c *)fdev_get_udata(stream);
// 	if (u != 0)
// 	{
// 		u->put(c);
// 		return 0;
// 	}
// 	return _FDEV_ERR;
// }
// 
// 
// // static
// int I2c::get(FILE *stream)
// {
// 	I2c *u;
// 	u = (I2c *)fdev_get_udata(stream);
// 	if (u != 0)
// 	{
// 		return u->get();
// 	}
// 	return _FDEV_ERR;
// }
