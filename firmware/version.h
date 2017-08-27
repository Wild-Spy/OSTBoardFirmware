//
// Created by mcochrane on 22/07/17.
//

#ifndef WS_OST_VERSION_H
#define WS_OST_VERSION_H

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define OST_HARDWARE_NAME               "OSTRev1"
#define OST_FIRMWARE_VERSION_NAME       "OST_FW_V0.2"
#ifdef DEBUGPRINTS
#define OST_FIRMWARE_VERSION            OST_FIRMWARE_VERSION_NAME "(" STR(DEBUGPRINTS) ")"
#else
#define OST_FIRMWARE_VERSION            OST_FIRMWARE_VERSION_NAME
#endif

#endif //WS_OST_VERSION_H
