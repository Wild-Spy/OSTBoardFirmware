//
// Created by mcochrane on 22/05/17.
//

#ifndef OSTBOARDFIRMWARE_NVMDRIVERINTERFACE_H
#define OSTBOARDFIRMWARE_NVMDRIVERINTERFACE_H

#include <stdint.h>
#include "exception/CException.h"

typedef uint8_t nvm_data_t;
typedef uint16_t nvm_size_t;
typedef uint32_t nvm_address_t;

/**
 * This is the interface for a Non Volatile Memory driver.
 *
 * The implementation defines it's own linear address space.
 * The addresses passed to read and write should refer to this
 * local address mapping.
 * The address space starts at 0 and ends at nvm_region_size_
 * in bytes.
 *
 * It should be assumed that the implementation uses
 * a buffer which must be flushed when writing is complete.
 *
 * @throws EX_OUT_OF_RANGE
 * @throws EX_NVM_READ_ERROR
 * @throws EX_NVM_WRITE_ERROR
 * @throws EX_NVM_HARDWARE_ERROR
 */
class NvmDriverInterface {
protected:
    NvmDriverInterface(nvm_size_t nvm_region_size)
            : nvm_region_size_(nvm_region_size)
    {}

public:
    /**
     * Read a number of bytes from NVM.
     * @param data      pointer to write the data to
     * @param length    number of bytes to read
     * @param address   NVM address to read from
     * @throws EX_
     */
    virtual void read(nvm_data_t* data, nvm_size_t length, nvm_address_t address) = 0;

    /**
     * Write a number of bytes from NVM.
     * @param data      pointer to the data to be written
     * @param length    number of bytes to write
     * @param address   NVM address at which to write data
     */
    virtual void write(nvm_data_t* data, nvm_size_t length, nvm_address_t address) = 0;

    /**
     * Flush the implementation's write buffer to NVM.
     * If the implementation uses a page buffer to avoid
     * rewriting entire pages on small writes to the same
     * page, this function flushes that buffer and writes
     * all changes to the NVM.
     */
    virtual void flush() = 0;

    nvm_size_t getRegionSize() { return nvm_region_size_; };

protected:
    nvm_size_t nvm_region_size_;
};

#endif //OSTBOARDFIRMWARE_NVMDRIVERINTERFACE_H
