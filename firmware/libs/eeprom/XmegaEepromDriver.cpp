//
// Created by mcochrane on 22/05/17.
//

#include "XmegaEepromDriver.h"
#include "eeprom_driver.h"

/**
 * Read a number of bytes from EEPROM.
 * @param data      pointer to write the data to
 * @param length    number of bytes to read
 * @param address   NVM address to read from
 */
void XmegaEepromDriver::read(nvm_data_t* data, nvm_size_t length, nvm_address_t address) {
    for (uint8_t i = 0; i < Length; i++) {
        *Data++ = EEPROM_ReadByte((uint8_t)(Address>>5), (uint8_t)(Address&0x1F));
        //		  EEPROM_ReadByte(         Page        ,         Byte           );
        Address++;
    }
}

/**
 * Write a number of bytes from EEPROM.
 * @param data      pointer to the data to be written
 * @param length    number of bytes to write
 * @param address   EEPROM address at which to write data
 */
virtual void XmegaEepromDriver::write(nvm_data_t* data, nvm_size_t length, nvm_address_t address) {
    uint8_t byte_addr, page_addr;

    byte_addr = (uint8_t)(address & 0x1F);
    page_addr = (uint8_t)(address >> 5);

    EEPROM_FlushBuffer();

    for (uint8_t i = 0; i < length; i++) {
        EEPROM_LoadByte(byte_addr, *data++);
        byte_addr++;
        // If we go beyond the end of the page, set
        // the byte back to 0 and increment page number.
        // Also Write the page to the EEPROM.
        if (byte_addr >= EEPROM_PAGESIZE) {
            byte_addr = 0;
            EEPROM_AtomicWritePage(page_addr);
            page_addr++;
            EEPROM_FlushBuffer();
        }
    }
    //Write the last page
    EEPROM_AtomicWritePage(page_addr);
}

/**
 * Flush the write buffer to EEPROM.
 * The implementation uses a page buffer to avoid
 * rewriting entire pages on small writes to the same
 * page, this function flushes that buffer and writes
 * all changes to the EEPROM.
 */
virtual void XmegaEepromDriver::flush() {
    EEPROM_FlushBuffer();
}