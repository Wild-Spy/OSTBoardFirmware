//
// Created by mcochrane on 17/05/17.
//

#include <gtest/gtest.h>
#include "min/encode_decode.h"

TEST(EncodeDecodeTestFixture, encode_decode_u32) {
    uint8_t buf[4];

    uint32_t number = 123456789ul;

    encode_u32(number, buf);

    EXPECT_EQ(decode_u32(buf), number);
}

TEST(EncodeDecodeTestFixture, encode_decode_s32) {
    uint8_t buf[4];

    int32_t number = -666666;

    encode_32(number, buf);

    EXPECT_EQ(decode_32(buf), number);
}

TEST(EncodeDecodeTestFixture, encode_decode_u16) {
    uint8_t buf[2];

    uint16_t number = 1112u;

    encode_u16(number, buf);

    EXPECT_EQ(decode_u16(buf), number);
}

TEST(EncodeDecodeTestFixture, encode_decode_s16) {
    uint8_t buf[2];

    int16_t number = -555;

    encode_16(number, buf);

    EXPECT_EQ(decode_16(buf), number);
}