//
// Created by mcochrane on 26/05/17.
//

#include <stdio.h>
#include <cmocka.h>
#include <stdint.h>
#include "nvm/SamdEmulatedEepromDriver.h"

SamdEmulatedEepromDriver* eepromDriver;

static int setup(void** state) {
    eepromDriver = new SamdEmulatedEepromDriver(NVM_EEPROM_EMULATOR_SIZE_16384);
}

static int teardown(void** state) {
//    delete eepromDriver;
//    eepromDriver = NULL;
}

static void correct_nvm_size(void** state) {
    (void) state; // unused
    assert_int_equal(eepromDriver->getRegionSize(), 16384);

    struct nvm_parameters nvmParameters;
    nvm_get_parameters(&nvmParameters);

    print_message("nvm_np: %u\n", nvmParameters.nvm_number_of_pages);
    print_message("eep_np: %lu\n", nvmParameters.eeprom_number_of_pages);
    print_message("bl_np: %lu\n", nvmParameters.bootloader_number_of_pages);
    print_message("pgsize: %u\n", nvmParameters.page_size);
}

static void format_all_data_is_ff(void** state) {
    (void) state; // unused
    eepromDriver->erase();
//    print_message("erased\n");

    const uint8_t buf_size = 64;
    uint8_t buffer[buf_size];
    uint8_t expected_buffer[buf_size];

    //Expect all 0xFF's
    for (uint8_t i = 0; i < buf_size; i++) {
        expected_buffer[i] = 0xFF;
    }

    for (nvm_address_t add = 0; add < eepromDriver->getRegionSize(); add += buf_size) {
        eepromDriver->read(buffer, buf_size, add);
        assert_memory_equal(buffer, expected_buffer, buf_size);
        print_message(".");
    }

    print_message("\n");
}

static void write_all_data_test(void** state) {
    (void) state; // unused
//    eepromDriver->erase();
//    print_message("erased\n");

    const uint8_t buf_size = 64;
    uint8_t buffer[buf_size];
    uint8_t expected_buffer[buf_size];

    //Write, and then expect all 0xCC's
    for (uint8_t i = 0; i < buf_size; i++) {
        buffer[i] = 0x00;
        expected_buffer[i] = 0xCC;
    }

    // Write
    for (nvm_address_t add = 0; add < eepromDriver->getRegionSize(); add += buf_size) {
        eepromDriver->write(expected_buffer, buf_size, add);
        print_message("+");
    }
    print_message("\n");

    // Compare
    for (nvm_address_t add = 0; add < eepromDriver->getRegionSize(); add += buf_size) {
        eepromDriver->read(buffer, buf_size, add);
        assert_memory_equal(buffer, expected_buffer, buf_size);
        print_message(".");
    }

    print_message("\n");
}

const struct CMUnitTest nvm_tests[] = {
    cmocka_unit_test(correct_nvm_size),
    cmocka_unit_test(write_all_data_test),
    cmocka_unit_test(format_all_data_is_ff),
//    cmocka_unit_test(heap_overflow),
};

void run_nvm_tests() {
    CEXCEPTION_T e;
    char c;

    print_message("Press 'y' to run nvm tests or 'n' to skip... \r\n");


    while(true) {
        scanf("%c", &c);
        if (c == 'y') break;
        else return;
    }

    Try {
        setup(NULL);
        cmocka_run_group_tests(nvm_tests, NULL, NULL);
    } Catch (e) {
        print_error("Threw Exception %u!\n", e);
    }
}