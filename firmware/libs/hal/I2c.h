//
// Created by mcochrane on 29/05/17.
//

#ifndef WS_OST_I2CCPP_H
#define WS_OST_I2CCPP_H

#include <i2c_master.h>

class I2c {
public:
    I2c(Sercom *const hw,
        struct i2c_master_config *const config);

    ~I2c();

    void enable();
    void disable();

    uint8_t readByte(uint8_t slave_i2c_address, uint8_t slave_reg_address);
    void readBytes(uint8_t slave_i2c_address, uint8_t slave_reg_address, uint8_t length, uint8_t* data);

    void writeByte(uint8_t slave_i2c_address, uint8_t slave_reg_address, uint8_t data);
    void writeBytes(uint8_t slave_i2c_address, uint8_t slave_reg_address, uint8_t length, uint8_t* data);

private:
    struct i2c_master_module module_;
//    struct i2c_master_config config_;
//    Sercom *const hw;
};


#endif //WS_OST_I2CCPP_H
