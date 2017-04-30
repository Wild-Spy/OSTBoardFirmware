//
// Created by mcochrane on 16/04/17.
//

#ifndef WS_OST_COMPILE_TIME_SIZEOF_H
#define WS_OST_COMPILE_TIME_SIZEOF_H

//#define XSTR(x) STR(x)
//#define STR(x) #x

#define COMPILE_TIME_SIZEOF(t)      template<int s> struct SIZEOF_ ## t ## _IS; \
                                    struct foo { \
                                        int a,b; \
                                    }; \
                                    SIZEOF_ ## t ## _IS<sizeof(t)> SIZEOF_ ## t ## _IS;

#endif //WS_OST_COMPILE_TIME_SIZEOF_H
