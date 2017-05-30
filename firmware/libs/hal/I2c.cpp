//
// Created by mcochrane on 29/05/17.
//

#include <libs/exception/CException.h>
#include "I2c.h"

#define MAX_RETRIES 10

/**
 *
 * Use i2c_master_get_config_defaults(...) to get config then modify
 * to your liking.
 * @param hw
 * @param config
 */
I2c::I2c(Sercom *const hw,
         struct i2c_master_config *const config)
{
//    config->buffer_timeout = 10000;
    enum status_code result = i2c_master_init(&module_, hw, config);
    if (result != STATUS_OK) Throw(EX_INVALID_INPUT_VALUE);
}

I2c::~I2c() {

}

void I2c::enable() {
    i2c_master_enable(&module_);
}

void I2c::disable() {
    i2c_master_disable(&module_);
}

uint8_t I2c::readByte(uint8_t slave_i2c_address, uint8_t slave_reg_address) {
    uint8_t data = slave_reg_address;
    uint16_t timeout = 0;

    struct i2c_master_packet packet = {
            .address = slave_i2c_address,
            .data_length = 1,
            .data = &data,
            .ten_bit_address = false,
            .high_speed = false,
            .hs_master_code = 0x0,
    };

    // Write slave_reg_address
    while (i2c_master_write_packet_wait_no_stop(&module_, &packet) != STATUS_OK) {
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == MAX_RETRIES) Throw(EX_FLASH_READ_ERROR);
    }

    // Read response
    while (i2c_master_read_packet_wait(&module_, &packet) != STATUS_OK) {
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == MAX_RETRIES) Throw(EX_FLASH_READ_ERROR);
    }

    return data;
}

void I2c::readBytes(uint8_t slave_i2c_address, uint8_t slave_reg_address, uint8_t length, uint8_t *data) {
    uint16_t timeout = 0;

    struct i2c_master_packet packet = {
            .address = slave_i2c_address,
            .data_length = 1,
            .data = &slave_reg_address,
            .ten_bit_address = false,
            .high_speed = false,
            .hs_master_code = 0x0,
    };

    // Write slave_reg_address
    while (i2c_master_write_packet_wait_no_stop(&module_, &packet) != STATUS_OK) {
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == MAX_RETRIES) Throw(EX_FLASH_READ_ERROR);
    }

    // Response should go into buffer
    packet.data = data;
    packet.data_length = length;

    // Get data
    while (i2c_master_read_packet_wait(&module_, &packet) != STATUS_OK) {
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == MAX_RETRIES) Throw(EX_FLASH_READ_ERROR);
    }

}

void I2c::writeByte(uint8_t slave_i2c_address, uint8_t slave_reg_address, uint8_t data) {
    uint16_t timeout = 0;

    struct i2c_master_packet packet = {
            .address = slave_i2c_address,
            .data_length = 1,
            .data = &slave_reg_address,
            .ten_bit_address = false,
            .high_speed = false,
            .hs_master_code = 0x0,
    };

    // Write slave_reg_address
    while (i2c_master_write_packet_wait_no_stop(&module_, &packet) != STATUS_OK) {
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == MAX_RETRIES) Throw(EX_FLASH_WRITE_ERROR);
    }

    i2c_master_write_byte(&module_, data);

    i2c_master_send_stop(&module_);

//    // Write value of data parameter
//    packet.data = &data;
//
//    // Write data
//    while (i2c_master_write_packet_wait(&module_, &packet) != STATUS_OK) {
//        /* Increment timeout counter and check if timed out. */
//        if (timeout++ == MAX_RETRIES) Throw(EX_FLASH_WRITE_ERROR);
//    }

}

void I2c::writeBytes(uint8_t slave_i2c_address, uint8_t slave_reg_address, uint8_t length, uint8_t *data) {
    uint16_t timeout = 0;

    struct i2c_master_packet packet = {
            .address = slave_i2c_address,
            .data_length = 1,
            .data = &slave_reg_address,
            .ten_bit_address = false,
            .high_speed = false,
            .hs_master_code = 0x0,
    };

    // Write slave_reg_address
    while (i2c_master_write_packet_wait_no_stop(&module_, &packet) != STATUS_OK) {
        /* Increment timeout counter and check if timed out. */
        if (timeout++ == MAX_RETRIES) Throw(EX_FLASH_WRITE_ERROR);
    }

    while (length--) {
        i2c_master_write_byte(&module_, *data++);
    }

    i2c_master_send_stop(&module_);

//    // Writing from buffer now
//    packet.data = data;
//    packet.data_length = length;
//
//    // Write data
//    while (i2c_master_write_packet_wait(&module_, &packet) != STATUS_OK) {
//        /* Increment timeout counter and check if timed out. */
//        if (timeout++ == MAX_RETRIES) Throw(EX_FLASH_WRITE_ERROR);
//    }
}

