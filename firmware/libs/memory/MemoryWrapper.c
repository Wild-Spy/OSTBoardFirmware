/*
 *   File Name: MemoryWrapper.c
 *      Module: MemoryWrapper
 *  Created On: February 06, 2012
 *      Author: Matthew Cochrane
 *     Company: Wild Spy
*/

#include "MemoryWrapper.h"
#include <stdlib.h>

/*///////////////////
//TEST ONLY DEFINES//
///////////////////*/
#if defined(TESTING) | defined(MCU_TESTING)
#define MAX_ALLOCS	10

typedef struct memInfo {
	void* ptr;
	uint16_t size;
} memInfo_t;

static uint8_t alloc_ptr_cnt = 0;
static memInfo_t alloc_pointers[MAX_ALLOCS];

#endif
/*///////////////////////
//END TEST ONLY DEFINES//
///////////////////////*/

static uint16_t heap_size = 0;

void* MemoryWrapper_AllocateMemory(size_t _NumOfElements, size_t _SizeOfElements)
{
    uint16_t memSize = _NumOfElements*_SizeOfElements;
    uint16_t newHeapSize = heap_size + memSize;

    if (newHeapSize > MEMORYWRAPPER_MAX_HEAPSIZEBYTES) {
        Throw(EX_MEM_OUT_OF_MEMORY);
    }

    void* retVal = calloc(_NumOfElements, _SizeOfElements);
    if (retVal == NULL) {
        Throw(EX_MEM_OUT_OF_MEMORY);
    }
#if defined(TEST)
    else {
		alloc_pointers[alloc_ptr_cnt].ptr = retVal;
		alloc_pointers[alloc_ptr_cnt].size = memSize;
		alloc_ptr_cnt++;
	}
#endif

    heap_size = newHeapSize;
    return retVal;
}

uint16_t MemoryWrapper_GetHeapSize() {
    return heap_size;
}

/*/////////////////////
//TEST ONLY FUNCTIONS//
/////////////////////*/
#if defined(TESTING) | defined(MCU_TESTING)
void MemoryWrapper_TEST_SetHeapSize(uint16_t val)
{
	heap_size = val;
}

void* MemoryWrapper_TEST_GetLastAllocPtr (void)
{
	return alloc_pointers[alloc_ptr_cnt-1].ptr;
}

uint16_t MemoryWrapper_TEST_GetLastAllocSize (void)
{
	return alloc_pointers[alloc_ptr_cnt-1].size;
}

uint8_t MemoryWrapper_TEST_AllocCount (void)
{
	return alloc_ptr_cnt;
}

void MemoryWrapper_TEST_FreeMemory(void *_Memory)
{
	uint8_t i;

	for (i = 0; i < alloc_ptr_cnt; i++)
		if (alloc_pointers[i].ptr == _Memory)
			break;

	if (i < alloc_ptr_cnt) {
		//Successfully found the pointer!
		free(_Memory);
		heap_size -= alloc_pointers[i].size;

		//Remove that Allocation from the Array
		while (i < alloc_ptr_cnt-1) {
			alloc_pointers[i].ptr = alloc_pointers[i+1].ptr;
			alloc_pointers[i].size = alloc_pointers[i+1].size;
			i++;
		};

		alloc_ptr_cnt--;

	} else {
		Throw(EX_MEM_INVALID_POINTER);
	}
}
#endif
/*/////////////////////////
//END TEST ONLY FUNCTIONS//
/////////////////////////*/
