//
// Created by mcochrane on 27/12/16.
//

extern "C" {
#include "MemoryWrapper.h"
};
#include "CppNewDeleteOps.h"

void* operator new(size_t objsize) {
    return MemoryWrapper_AllocateMemory(objsize, 1);
    //return malloc(objsize);
}

//void operator delete(void* obj) {
//    free(obj);
//}

void* operator new[](unsigned int x) {
    return MemoryWrapper_AllocateMemory(x, 1);
}

//void operator delete[](void *) {
//
//}