/*
 *   File Name: MemoryWrapper.h
 *      Module: MemoryWrapper
 *  Created On: February 06, 2012
 *      Author: Matthew Cochrane
 *     Company: Wild Spy
*/

#ifndef MemoryWrapper_H
#define MemoryWrapper_H

#include "ExceptionValues.h"
#include <stddef.h>
#include <stdint.h>

#define MEMORYWRAPPER_MAX_HEAPSIZEBYTES				2048

//Function Prototypes:
void* MemoryWrapper_AllocateMemory(size_t _NumOfElements, size_t _SizeOfElements);
uint16_t MemoryWrapper_GetHeapSize();

//Test Only Prototypes:
#if defined(TEST)
void MemoryWrapper_TEST_SetHeapSize(uint16_t val);
void* MemoryWrapper_TEST_GetLastAllocPtr();
uint16_t MemoryWrapper_TEST_GetLastAllocSize();
uint8_t MemoryWrapper_TEST_AllocCount();

void MemoryWrapper_TEST_FreeMemory(void *_Memory);

#define TEST_CHECKFORMEMORYLEAKS()		TEST_ASSERT_EQUAL(0, MemoryWrapper_GetHeapSize())

#endif

#endif // MemoryWrapper_H
