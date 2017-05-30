//
// Created by mcochrane on 26/05/17.
//

#ifndef WS_OST_SAMDEEPROMDRIVER_H
#define WS_OST_SAMDEEPROMDRIVER_H

#include "NvmDriverInterface.h"
#include <nvm.h>


class SamdEmulatedEepromDriver : public NvmDriverInterface {
private:
    typedef uint16_t page_address_t;
    typedef uint16_t row_address_t;

public:
    SamdEmulatedEepromDriver(enum nvm_eeprom_emulator_size);

    /**
     * Read a number of bytes from NVM.
     * @param data      pointer to write the data to
     * @param length    number of bytes to read
     * @param address   NVM address to read from
     * @throws EX_
     */
    virtual void read(nvm_data_t* data, nvm_size_t length, nvm_address_t address);

    /**
     * Write a number of bytes from NVM.
     * @param data      pointer to the data to be written
     * @param length    number of bytes to write
     * @param address   NVM address at which to write data
     */
    virtual void write(nvm_data_t* data, nvm_size_t length, nvm_address_t address);

    /**
     * Flush the implementation's write buffer to NVM.
     * If the implementation uses a page buffer to avoid
     * rewriting entire pages on small writes to the same
     * page, this function flushes that buffer and writes
     * all changes to the NVM.
     */
    virtual void flush();


    /**
     * Erase the entire region
     */
    void erase();

private:
    nvm_size_t nvmEmulatorSizeToNvmSize(nvm_eeprom_emulator_size eepromSize);
    void check_local_address(nvm_address_t address);

    nvm_address_t global_start_address_;
    nvm_address_t global_end_address_;
    uint8_t page_size_;

    row_address_t row_buffer_address_;
    uint8_t* row_buffer_;
    uint8_t* page_buffer_;
    bool row_data_valid_;   // is the data in row_buffer valid or outdated?
    bool row_needs_flush_;  // has the data in row_buffer been modified compared to the data on the hardware?

    void readPage(nvm_data_t *data, page_address_t page_address);
    void readPageIntoPageBuffer(page_address_t local_page_address);
    void readRow(nvm_data_t *data, row_address_t local_row_address);
    void readRowIntoRowBuffer(row_address_t local_row_address);
    void eraseRow(row_address_t local_row_address);
    nvm_address_t localToGlobalAddress(nvm_address_t local_byte_address);
    nvm_address_t localPageToGlobalByteAddress(page_address_t local_page_address);
    nvm_address_t localRowToGlobalByteAddress(row_address_t local_row_address);
    page_address_t localRowToLocalPageAddress(row_address_t local_row_address);
    uint8_t getAddressInPage(nvm_address_t local_byte_address, page_address_t* page);
    void getAddressInRow(nvm_address_t local_byte_address, row_address_t* row, uint8_t* page_in_row, uint8_t* address_in_page);

    page_address_t getPageOfLocalByteAddress(nvm_address_t local_address);
    row_address_t getRowOfLocalByteAddress(nvm_address_t local_address);
    row_address_t getRowOfLocalPageAddress(page_address_t page);

    void writePage(nvm_data_t *data, page_address_t local_page_address);
    void writePageFromBuffer(page_address_t local_page_address);
    void writePageFromRowBuffer(uint8_t row_in_buffer_to_write, row_address_t local_row_address);
    void writeRowFromBuffer(row_address_t local_row_address);
};


#endif //WS_OST_SAMDEEPROMDRIVER_H
