//
// Created by mcochrane on 17/05/17.
//

#include <gtest/gtest.h>
#include <simplib/array.h>
#include <stdint.h>

typedef sl::Array<uint16_t, 10> U16Ary10;

TEST(ArrayTestFixture, max_size) {
    uint16_t array_max_size = sl::Array<uint8_t, 100>::MAX_SIZE;
    EXPECT_EQ(array_max_size, 100);
}

TEST(ArrayTestFixture, empty_on_create) {
    U16Ary10 ary = U16Ary10();
    EXPECT_EQ(ary.getCount(), 0);
}

TEST(ArrayTestFixture, add_items_check_count) {
    U16Ary10 ary = U16Ary10();

    ary.append(0);
    EXPECT_EQ(ary.getCount(), 1);

    ary.append(0);
    EXPECT_EQ(ary.getCount(), 2);

    ary.append(0);
    EXPECT_EQ(ary.getCount(), 3);
}

TEST(ArrayTestFixture, add_items_and_read_back) {
    U16Ary10 ary = U16Ary10();

    ary.append(5);
    EXPECT_EQ(ary.get(0), 5);

    ary.append(22);
    EXPECT_EQ(ary.get(1), 22);

    ary.append(1234);
    EXPECT_EQ(ary.get(2), 1234);
}

TEST(ArrayTestFixture, remove_last) {
    U16Ary10 ary = U16Ary10();

    ary.append(1);
    ary.append(2);
    ary.append(3);
    EXPECT_EQ(ary.getCount(), 3);

    ary.remove_last();
    EXPECT_EQ(ary.getCount(), 2);

    // Check the actual values
    EXPECT_EQ(ary.get(0), 1);
    EXPECT_EQ(ary.get(1), 2);
}

TEST(ArrayTestFixture, clear) {
    U16Ary10 ary = U16Ary10();

    ary.append(1);
    ary.append(2);
    ary.append(3);
    EXPECT_EQ(ary.getCount(), 3);

    ary.clear();
    EXPECT_EQ(ary.getCount(), 0);

    // Can still append and everything works..
    ary.append(55);
    EXPECT_EQ(ary.get(0), 55);
}

TEST(ArrayTestFixture, remove_at) {
    U16Ary10 ary = U16Ary10();

    // Add some values
    ary.append(0);
    ary.append(1);
    ary.append(2);
    EXPECT_EQ(ary.getCount(), 3);

    // Remove the second item
    ary.remove(1);

    // Check the values
    EXPECT_EQ(ary.get(0), 0);
    EXPECT_EQ(ary.get(1), 2);
}

TEST(ArrayTestFixture, remove_at_start) {
    U16Ary10 ary = U16Ary10();

    // Add some values
    ary.append(0);
    ary.append(1);
    ary.append(2);
    EXPECT_EQ(ary.getCount(), 3);

    // Remove the first item
    ary.remove(0);

    // Check the values
    EXPECT_EQ(ary.get(0), 1);
    EXPECT_EQ(ary.get(1), 2);
}

TEST(ArrayTestFixture, remove_at_end) {
    U16Ary10 ary = U16Ary10();

    // Add some values
    ary.append(0);
    ary.append(1);
    ary.append(2);
    EXPECT_EQ(ary.getCount(), 3);

    // Remove the third item
    ary.remove(2);

    // Check the values
    EXPECT_EQ(ary.get(0), 0);
    EXPECT_EQ(ary.get(1), 1);
}

TEST(ArrayTestFixture, get_out_of_range) {
    CEXCEPTION_T e;
    U16Ary10 ary = U16Ary10();

    // Add some values
    ary.append(0);
    ary.append(1);
    ary.append(2);
    EXPECT_EQ(ary.getCount(), 3);

    Try {
        // Try and get value which is out of range
        ary.get(3);
        FAIL(); // Should have thrown exception
    } Catch(e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }
}

TEST(ArrayTestFixture, remove_out_of_range) {
    CEXCEPTION_T e;
    U16Ary10 ary = U16Ary10();

    // Add some values
    ary.append(0);
    ary.append(1);
    ary.append(2);
    EXPECT_EQ(ary.getCount(), 3);

    Try {
        // Try and get value which is out of range
        ary.remove(3);
        FAIL(); // Should have thrown exception
    } Catch(e) {
        EXPECT_EQ(e, EX_OUT_OF_RANGE);
    }
}

TEST(ArrayTestFixture, append_overrun) {
    CEXCEPTION_T e;
    uint8_t i;
    U16Ary10 ary = U16Ary10();

    Try {
        for (i = 0; i < 11; i++) {
            // Add values
            ary.append(i);
        }
        FAIL(); // Should have thrown exception
    } Catch(e) {
        EXPECT_EQ(e, EX_BUFFER_OVERRUN);
        EXPECT_EQ(i, 10);
    }
}