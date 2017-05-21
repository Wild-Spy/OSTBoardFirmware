/*
 * ErrorValues.h
 *
 *  Created on: 06/02/2012
 *      Author: MC
 */

#ifndef EXCEPTIONVALUES_H_
#define EXCEPTIONVALUES_H_

#include "CException.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum EXCEPTION_VALUES_enum {
	EX_NULL,                    //0
	EX_MEM_OUT_OF_MEMORY,        //1
	EX_MEM_INVALID_POINTER,        //2
	EX_NULL_POINTER,            //3
	EX_BUFFER_OVERRUN,            //4
	EX_NOT_INITIALISED,            //5
	EX_ALREADY_INITIALISED,        //6
	EX_RET_STR_TOO_SMALL,        //7
	EX_DATA_NOT_AVAILABLE,        //8
	EX_DATA_OVERFLOW,            //9
	EX_INVALID_DATETIME,        //10
	EX_INVALID_FORMATTING,        //11
	EX_VALUE_OUT_OF_BOUNDS,        //12
	EX_TIMEOUT,                    //13
	EX_INVALID_INPUT_VALUE,        //14
	EX_INVALID_CHECKSUM,        //15
	EX_WRITE_ERROR,                //16
	EX_FLASH_READ_ERROR,        //17
	EX_FLASH_WRITE_ERROR,        //18
	EX_FLASH_OPERATION_FAIL,    //19
	EX_GPS_ABORT_FIX,            //20
	EX_INVALID_BAUDRATE,        //21
	EX_OUT_OF_RANGE,            //22

	EX_EXCEPTIONS_COUNT
} EXCEPTION_VALUE_t;
#ifdef __cplusplus
}
#endif

#if defined(TESTING)
void ExceptionValues_InitMsgStrings(void);
char* GetExceptionMessage(CEXCEPTION_T exception_id);

#define TEST_FAIL_EXCEPTION_WITH_MESSAGE(e)					TEST_FAIL_MESSAGE(GetExceptionMessage(e))
#define TEST_ASSERT_EQUAL_EXCEPTION(expected, actual)		TEST_ASSERT_EQUAL_STRING_MESSAGE(GetExceptionMessage(expected), GetExceptionMessage(actual), "Unexpected exception.")
#endif

#endif /* EXCEPTIONVALUES_H_ */
