//
// Created by mcochrane on 26/05/17.
//

#include "SamdEmulatedEepromDriver.h"
#include <memory/MemoryWrapper.h>
#include <nvm.h>
#ifdef MCU_TESTING
#include <cmocka.h>
#else
#include <libs/min/min_transmit_cmds.h>
#endif
#include <cstring>

SamdEmulatedEepromDriver::SamdEmulatedEepromDriver(enum nvm_eeprom_emulator_size eepromSize)
    : NvmDriverInterface(nvmEmulatorSizeToNvmSize(eepromSize)),
      row_data_valid_(false),
      row_needs_flush_(false),
      row_buffer_address_(255)
{
    struct nvm_fusebits nvmFusebits;
    struct nvm_config nvmConfig;

    nvm_get_config_defaults(&nvmConfig);
    // initialise the nvm
    //TODO: maybe we don't want to do this here.  What if we had multiple drivers accessing the nvm?
    nvm_set_config(&nvmConfig);

    nvm_get_fuses(&nvmFusebits);

    // Check if eeprom size is correct,
    // If it's not, update it
    if (nvmFusebits.eeprom_size != eepromSize) {
        nvmFusebits.eeprom_size = eepromSize;
        nvm_set_fuses(&nvmFusebits);
    }

    // Get NVM parameters
    struct nvm_parameters nvmParameters;
    nvm_get_parameters(&nvmParameters);

    // Set member variables
    page_size_ = nvmParameters.page_size;
    global_end_address_ = nvmParameters.nvm_number_of_pages*page_size_;
    global_start_address_ = (nvmParameters.nvm_number_of_pages - nvmParameters.eeprom_number_of_pages)*page_size_;

    // Allocate memory for buffers - now this driver cannot be destroyed as we have no delete/free operators!!
    page_buffer_ = (uint8_t*)MemoryWrapper_AllocateMemory(page_size_, 1);
    row_buffer_ = (uint8_t*)MemoryWrapper_AllocateMemory((size_t) (page_size_ * NVMCTRL_ROW_PAGES), 1);
}

void SamdEmulatedEepromDriver::read(nvm_data_t *data, nvm_size_t length, nvm_address_t address) {
    // Flush the row buffer before we try and write from the NVM
    flush();

    nvm_address_t end_address = address + length; // Won't actually read the end address.
    // For example if address = 0 and length = 2, then end address = 0 + 2 = 2 but we will read:
    // Byte 0 into data[0] and byte 1 into data[1].
    check_local_address(address+length);
    page_address_t start_page = getPageOfLocalByteAddress(address);
    page_address_t end_page = getPageOfLocalByteAddress(address+length);
    // if start is 0 and end is 2, then start_page = 0 and end_page = 0
    // if start = 63 and length is 2 then, end is 65, start_page = 63/64 = 0 end_page = 65/64 = 1
    // so we want to read from all pages, including the end page...

    nvm_address_t read_start;
    nvm_size_t read_length ;
    for (page_address_t page = start_page; page <= end_page; page++) {
        if (start_page == end_page) {
            read_start = localToGlobalAddress(address);
            read_length = length;
        } else if (page == start_page) {
            read_start = localToGlobalAddress(address);
            // eg. nvm_start_ = 6400, second page starts at 6464
            // say we want to read from 63 (6463) to 65 (6465)
            // we read 1 byte (6464-6463) from 6463 in page 0
            read_length = (nvm_size_t) (localPageToGlobalByteAddress(start_page + 1) - read_start);
        } else if (page == end_page) {
            read_start = localPageToGlobalByteAddress(end_page);
            read_length = (nvm_size_t) (localToGlobalAddress(end_address) - read_start);
        } else {
            read_start = localPageToGlobalByteAddress(page);
            read_length = page_size_;
        }

        if (read_length == 0) break;


        if (page == start_page) {
            // If it's the first page then we will be reading from somewhere other than the start of the page.  But
            // nvm_read_buffer requires the address to be aligned to the start of a page!
            readPageIntoPageBuffer(page);
            uint8_t add_in_page = getAddressInPage(address, NULL);
            memcpy_ram2ram(data, page_buffer_+add_in_page, read_length);
        } else {
            // Do the actual read
            enum status_code result = nvm_read_buffer(read_start, data, read_length);
//        print_message("rb(%lu, d, %u) = %d\n", read_start, read_length, result);
            #ifdef MCU_TESTING
            print_message("rb(%lu, d, %u) = %d\n", read_start, read_length, result);
            #else
            report_printf("rb(%lu, d, %u) = %d\n", read_start, read_length, result);
            #endif

            if (result != STATUS_OK) Throw(EX_NVM_READ_ERROR);
        }




        // Move local buffer pointer
        data += read_length;
    }
}

void SamdEmulatedEepromDriver::write(nvm_data_t *data, nvm_size_t length, nvm_address_t address) {
    //Assume single row first
    row_address_t row;
    uint8_t page_in_row;
    uint8_t address_in_page;
    getAddressInRow(address, &row, &page_in_row, &address_in_page);

//    print_message("rba: %u, r: %u, pir: %u, aip: %u, a: %lu\n", row_buffer_address_, row, page_in_row, address_in_page, address);

    if (row_buffer_address_ != row) {
//        print_message("a");
        flush();
        readRowIntoRowBuffer(row);
    }

    uint16_t index_in_row = (uint16_t) (page_in_row * page_size_ + address_in_page);

    for (nvm_size_t i = 0; i < length; i++) {
        row_buffer_[index_in_row++] = *data++;
        if (index_in_row > page_size_*NVMCTRL_ROW_PAGES) {
            row_needs_flush_ = true;
            flush();
            index_in_row = 0;
        }
    }

    row_needs_flush_ = true;
}

void SamdEmulatedEepromDriver::flush() {
    // Check if we have a row loaded
    if (!row_needs_flush_ || !row_data_valid_) return;

    // Erase current row
    eraseRow(row_buffer_address_);

    // Write all row data to row
    writeRowFromBuffer(row_buffer_address_);

    // Update flags
    row_needs_flush_ = false;
}

SamdEmulatedEepromDriver::page_address_t SamdEmulatedEepromDriver::getPageOfLocalByteAddress(nvm_address_t local_address) {
    return (page_address_t) (local_address / page_size_);
}

SamdEmulatedEepromDriver::row_address_t SamdEmulatedEepromDriver::getRowOfLocalByteAddress(nvm_address_t local_address) {
    return getRowOfLocalPageAddress(getPageOfLocalByteAddress(local_address));
}

SamdEmulatedEepromDriver::row_address_t SamdEmulatedEepromDriver::getRowOfLocalPageAddress(page_address_t page) {
    // Division performs 'truncation towards zero'
    return (row_address_t) (page / NVMCTRL_ROW_PAGES);
}

void SamdEmulatedEepromDriver::writePage(nvm_data_t *data, page_address_t local_page_address) {
    nvm_address_t globalByteAddress = localPageToGlobalByteAddress(local_page_address);

    for (uint8_t i = 0; i < page_size_; i++) {
        if (data[i] != 0xCC) {
//            print_message("!");
        }
    }

    enum status_code result = nvm_write_buffer(globalByteAddress, data, page_size_);
    if (result != STATUS_OK) Throw(EX_NVM_WRITE_ERROR);
    result = nvm_execute_command(NVM_COMMAND_WRITE_PAGE, globalByteAddress, 0);
    if (result != STATUS_OK) Throw(EX_NVM_WRITE_ERROR);
//    print_message("wb(%lu, d, %u)", globalByteAddress, page_size_);
//    print_message("x");
}

void SamdEmulatedEepromDriver::writePageFromBuffer(page_address_t local_page_address) {
    writePage(page_buffer_, local_page_address);
}

void SamdEmulatedEepromDriver::writePageFromRowBuffer(uint8_t page_in_row_buffer_to_write, row_address_t local_row_address) {
    if (page_in_row_buffer_to_write >= NVMCTRL_ROW_PAGES) Throw(EX_OUT_OF_RANGE);
//    nvm_address_t global_byte_Address = localPageToGlobalByteAddress(localRowToLocalPageAddress(local_row_address) + page_in_row_buffer_to_write);
    uint8_t* buffer_address = row_buffer_ + (page_size_*page_in_row_buffer_to_write);

//    print_message("wpfrb(%lu, d, %u)\n", page_in_row_buffer_to_write, local_row_address);
//    print_message("rba: %lu rba: %lu rba: %u\n", (uint32_t)row_buffer_, (uint32_t)buffer_address, page_size_*page_in_row_buffer_to_write);
    writePage(buffer_address, localRowToLocalPageAddress(local_row_address) + page_in_row_buffer_to_write);

//    enum status_code result = nvm_write_buffer(global_byte_Address, buffer_address, page_size_);
//    if (result != STATUS_OK) Throw(EX_NVM_WRITE_ERROR);
}

void SamdEmulatedEepromDriver::writeRowFromBuffer(row_address_t local_row_address) {
    for (uint8_t i = 0; i < NVMCTRL_ROW_PAGES; i++)
        writePageFromRowBuffer(i, local_row_address);
}

void SamdEmulatedEepromDriver::check_local_address(nvm_address_t address) {
    if (address > nvm_region_size_) Throw(EX_NVM_READ_ERROR);
}

void SamdEmulatedEepromDriver::readPage(nvm_data_t *data, page_address_t local_page_address) {
    check_local_address( (nvm_address_t) ((local_page_address + 1) * page_size_) );
    nvm_address_t address = local_page_address*page_size_+global_start_address_;

    enum status_code result = nvm_read_buffer(address, data, page_size_);
    if (result != STATUS_OK) Throw(EX_NVM_READ_ERROR);
}

void SamdEmulatedEepromDriver::readPageIntoPageBuffer(page_address_t local_page_address) {
    readPage(page_buffer_, local_page_address);
}

void SamdEmulatedEepromDriver::readRow(nvm_data_t *data, row_address_t local_row_address) {
    page_address_t local_page_address = (page_address_t) (local_row_address * 4);
    for (uint8_t i = 0; i < NVMCTRL_ROW_PAGES; i++) {
        readPage(data, local_page_address);
        local_page_address++;
        data += page_size_;
    }
}

/**
 * Reads a row into the buffer.
 * Note: This will overwrite the data in the buffer.  Make sure flush() is called becore
 *       calling this function of the data should be saved.
 * @param local_row_address
 */
void SamdEmulatedEepromDriver::readRowIntoRowBuffer(row_address_t local_row_address) {
    readRow(row_buffer_, local_row_address);
    row_buffer_address_ = local_row_address;
    row_needs_flush_ = false;
    row_data_valid_ = true;
}

void SamdEmulatedEepromDriver::eraseRow(row_address_t local_row_address) {
    enum status_code result = nvm_erase_row(localRowToGlobalByteAddress(local_row_address));
    if (result != STATUS_OK) Throw(EX_NVM_WRITE_ERROR);
}

nvm_address_t SamdEmulatedEepromDriver::localRowToGlobalByteAddress(row_address_t local_row_address) {
    return localPageToGlobalByteAddress(localRowToLocalPageAddress(local_row_address));
}

SamdEmulatedEepromDriver::page_address_t SamdEmulatedEepromDriver::localRowToLocalPageAddress(row_address_t local_row_address) {
    return (row_address_t) (local_row_address * NVMCTRL_ROW_PAGES);
}

nvm_address_t SamdEmulatedEepromDriver::localPageToGlobalByteAddress(page_address_t local_page_address) {
    return localToGlobalAddress(local_page_address*page_size_);
}

nvm_address_t SamdEmulatedEepromDriver::localToGlobalAddress(nvm_address_t local_byte_address) {
    return local_byte_address + global_start_address_;
}

nvm_size_t SamdEmulatedEepromDriver::nvmEmulatorSizeToNvmSize(enum nvm_eeprom_emulator_size eepromSize) {
    switch (eepromSize) {
        case NVM_EEPROM_EMULATOR_SIZE_16384:
            return 16384;
        case NVM_EEPROM_EMULATOR_SIZE_8192:
            return 8192;
        case NVM_EEPROM_EMULATOR_SIZE_4096:
            return 4096;
        case NVM_EEPROM_EMULATOR_SIZE_2048:
            return 2048;
        case NVM_EEPROM_EMULATOR_SIZE_1024:
            return 1024;
        case NVM_EEPROM_EMULATOR_SIZE_512:
            return 512;
        case NVM_EEPROM_EMULATOR_SIZE_256:
            return 256;
        case NVM_EEPROM_EMULATOR_SIZE_0:
            return 0;
    }
}

/**
 * Gets the address of a local space byte within a page.
 * @param local_byte_address    the byte address in question (in local space)
 * @param page                  (out) the local page that the byte resides in
 * @return                      the index of local_byte_address in the page
 */
uint8_t SamdEmulatedEepromDriver::getAddressInPage(nvm_address_t local_byte_address, page_address_t *page) {
    page_address_t pg = (page_address_t) (local_byte_address / page_size_);
    nvm_address_t local_page_start_address = pg * page_size_;
    if (page != NULL) *page = pg;
    return (uint8_t) (local_byte_address - local_page_start_address);
}

/**
 * Gets the address of a local space byte within a row.
 * @param local_byte_address    the byte address in question (in local space)
 * @param row                   (out) the local row that the byte resides in
 * @param page_in_row           (out) the page within that row that the byte resides in (0-3)
 * @param address_in_page       (out) the index of local_byte_address in the page
 */
void SamdEmulatedEepromDriver::getAddressInRow(nvm_address_t local_byte_address, row_address_t *row,
                                               uint8_t *page_in_row, uint8_t* address_in_page) {
    page_address_t pg;
    uint8_t pg_addr = getAddressInPage(local_byte_address, &pg);
    row_address_t row_addr = getRowOfLocalPageAddress(pg);
    page_address_t row_first_page = (page_address_t) (row_addr * NVMCTRL_ROW_PAGES);

//    print_message("rowa: %u rfp: %u pg: %u, pgad: %u\n", row_addr, row_first_page, pg, pg_addr);
    // Set outputs
    if (row != NULL) *row = row_addr;
    if (page_in_row != NULL) *page_in_row = (uint8_t) (pg - row_first_page);
    if (address_in_page != NULL) *address_in_page = pg_addr;
}

void SamdEmulatedEepromDriver::erase() {
    row_address_t row_count = getRowOfLocalByteAddress(nvm_region_size_);

    // Want to decrement before we start..
    while(row_count--)
        eraseRow(row_count);
}
