//
// Created by mcochrane on 16/04/17.
//

#ifndef WS_OST_ENCODE_DECODE_H
#define WS_OST_ENCODE_DECODE_H

#include <stdint.h>

//TODO: make these work in c or in c++

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

/* Worker functions to write words into a buffer in big endian format
 * using generic C
 *
 * A good C compiler will spot what is going on and use byte manipulation
 * instructions to get the right effect (functions declared static
 * to hint to compiler that code can be inlined)
 */
EXTERNC void encode_32(int32_t data, uint8_t buf[]);
EXTERNC void encode_u32(uint32_t data, uint8_t buf[]);
EXTERNC void encode_16(int16_t data, uint8_t buf[]);
EXTERNC void encode_u16(uint16_t data, uint8_t buf[]);

EXTERNC int32_t decode_32(uint8_t buf[]);
EXTERNC uint32_t decode_u32(uint8_t buf[]);
EXTERNC int16_t decode_16(uint8_t buf[]);
EXTERNC uint16_t decode_u16(uint8_t buf[]);

#undef EXTERNC

#endif //WS_OST_ENCODE_DECODE_H
