//
// Created by mcochrane on 22/05/17.
//

#ifndef OSTBOARDFIRMWARE_EEPROMDRIVER_H
#define OSTBOARDFIRMWARE_EEPROMDRIVER_H

#include "nvm/NvmDriverInterface.h"

class XmegaEepromDriver : public NvmDriverInterface {
public:

    XmegaEepromDriver(nvm_address_t eeprom_size_bytes)
        : eeprom_max_addr_(eeprom_size_bytes)
    {};

    /**
     * Read a number of bytes from EEPROM.
     * @param data      pointer to write the data to
     * @param length    number of bytes to read
     * @param address   NVM address to read from
     */
    virtual void read(nvm_data_t* data, nvm_size_t length, nvm_address_t address);

    /**
     * Write a number of bytes from EEPROM.
     * @param data      pointer to the data to be written
     * @param length    number of bytes to write
     * @param address   EEPROM address at which to write data
     */
    virtual void write(nvm_data_t* data, nvm_size_t length, nvm_address_t address);

    /**
     * Flush the write buffer to EEPROM.
     * The implementation uses a page buffer to avoid
     * rewriting entire pages on small writes to the same
     * page, this function flushes that buffer and writes
     * all changes to the EEPROM.
     */
    virtual void flush();

private:
    nvm_address_t eeprom_max_addr_;
};


#endif //OSTBOARDFIRMWARE_EEPROMDRIVER_H
