//
// Created by mcochrane on 16/04/17.
//
#include "encode_decode.h"

/* Worker functions to write words into a buffer in big endian format
 * using generic C
 *
 * A good C compiler will spot what is going on and use byte manipulation
 * instructions to get the right effect (functions declared static
 * to hint to compiler that code can be inlined)
 */
void encode_32(int32_t data, uint8_t buf[]) {
    buf[0] = (uint8_t)((data & 0xff000000UL) >> 24);
    buf[1] = (uint8_t)((data & 0x00ff0000UL) >> 16);
    buf[2] = (uint8_t)((data & 0x0000ff00UL) >> 8);
    buf[3] = (uint8_t)(data & 0x000000ffUL);
}
void encode_u32(uint32_t data, uint8_t buf[]) {
    buf[0] = (uint8_t)((data & 0xff000000UL) >> 24);
    buf[1] = (uint8_t)((data & 0x00ff0000UL) >> 16);
    buf[2] = (uint8_t)((data & 0x0000ff00UL) >> 8);
    buf[3] = (uint8_t)(data & 0x000000ffUL);
}

void encode_16(int16_t data, uint8_t buf[]) {
    buf[0] = (uint8_t)((data & 0x0000ff00UL) >> 8);
    buf[1] = (uint8_t)(data & 0x000000ffUL);
}

void encode_u16(uint16_t data, uint8_t buf[]) {
    buf[0] = (uint8_t)((data & 0x0000ff00UL) >> 8);
    buf[1] = (uint8_t)(data & 0x000000ffUL);
}

int32_t decode_32(uint8_t buf[]) {
    return (int32_t) decode_u32(buf);
}

uint32_t decode_u32(uint8_t buf[]) {
    uint32_t res;
    res = ((uint32_t)(buf[0]) << 24) | ((uint32_t)(buf[1]) << 16) | ((uint32_t)(buf[2]) << 8) | (uint32_t)(buf[3]);
    return res;
}

int16_t decode_16(uint8_t buf[]) {
    return (int16_t) decode_u16(buf);
}

uint16_t decode_u16(uint8_t buf[]) {
    uint16_t res;
    res = ((uint16_t)(buf[0]) << 8) | (uint16_t)(buf[1]);
    return res;
}