//
// Created by mcochrane on 27/12/16.
//

#ifndef WS_OST_CPPNEWDELETEOPS_H
#define WS_OST_CPPNEWDELETEOPS_H
#include <stddef.h>

void* operator new(size_t objsize);
//void operator delete(void* obj);

//void* operator new[](unsigned int x);
void* operator new[](size_t x);
//void operator delete[](void *);

#ifndef TESTING
inline void* operator new(size_t, void* __p)
{ return __p; }

inline void* operator new[](size_t, void* __p)
{ return __p; }
#endif

#endif //WS_OST_CPPNEWDELETEOPS_H
