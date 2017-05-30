
#include <udc/udc.h>
#include <udi_cdc.h>
#include <sleepmgr.h>

#include <cmocka.h>
#include <stdio.h>
#include "libs/stdio/stdio_usb/stdio_usb.h"
#include "tests.h"

int main(void) {

    irq_initialize_vectors();
    cpu_irq_enable();
    sleepmgr_init();
    system_init();
//    udc_start();

    stdio_usb_init();

    stdio_usb_enable();

    uint8_t ch;

//    const struct CMUnitTest tests[] = {
//        cmocka_unit_test(null_test_success),
//    };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(1) {
        char c;
        print_message("Press any key to run tests... \r\n");


        while(true) {
            scanf("%c", &c);
            break;
        }

        print_message("Running all tests...\r\n");
        RUN_TESTS();

        for (uint32_t i = 0; i < 4000000; i++) asm("nop");
    }
#pragma clang diagnostic pop

}