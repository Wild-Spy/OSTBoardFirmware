#include <main.h>

#include <system.h>
#include <udc/udc.h>
//#include <udi_cdc.h>
#include <sleepmgr.h>
#include <min/min.h>
#include <nvm/NvmRuleManager.h>
//#include <nvm.h>
#include <samd21.h>
#include <libs/DS3232SN/DS3232SN.h>
#include <delay.h>
#include "hw/hal_gpio.h"
#include "nvm/SamdEmulatedEepromDriver.h"
//#include <helpers/compile_time_sizeof.h>
//#include <time.h>

//COMPILE_TIME_SIZEOF(char)
//COMPILE_TIME_SIZEOF(int)
//COMPILE_TIME_SIZEOF(long)
//COMPILE_TIME_SIZEOF(time_t)

//HAL_GPIO_PIN(LED,      A, 27)
//HAL_GPIO_PIN(BUTTON,   A, 14)

NvmDriverInterface* eepromDriver = NULL;
I2c* i2c = NULL;
Pin led(HAL_GPIO_PORTA, 27);

//struct nvm_config nvmConfig;
//struct nvm_fusebits nvmFusebits;

void DS3232SNInterruptCallback(void) {

}

void i2cInit() {
    struct i2c_master_config i2c_cfg;
    i2c_master_get_config_defaults(&i2c_cfg);
    i2c_cfg.baud_rate = I2C_MASTER_BAUD_RATE_100KHZ;
    i2c_cfg.buffer_timeout = 1000;
    i2c_cfg.pinmux_pad0 = PINMUX_PA16C_SERCOM1_PAD0;
    i2c_cfg.pinmux_pad1 = PINMUX_PA17C_SERCOM1_PAD1;
    i2c_cfg.generator_source = GCLK_GENERATOR_3;
    i2c = new I2c(SERCOM1, &i2c_cfg);
}

void init() {
    sleepmgr_init();
    system_init();
    delay_init();
    udc_start();
    udc_attach();

    init_min();

    led.setDirOutput();
    led.setOutputLow();

    eepromDriver = new SamdEmulatedEepromDriver(NVM_EEPROM_EMULATOR_SIZE_16384);
    NvmRuleManager_Init(0, eepromDriver->getRegionSize(), *eepromDriver);

    i2cInit();
    initRtc(*i2c, DS3232SNInterruptCallback);
    getRtc().enablePower();
}

void loop() {
    CEXCEPTION_T e = CEXCEPTION_NONE;

//    while(1) {
//
//        DateTime t(2017, 3, 1, 10, 10, 10);
//        char ss[100];
//        t.isotime(ss);
//        report_printf(ss);
//    }

//    TdlRuleRunner_GetInstance().mainLoopCallback();

//    while(1) {
//        report_printf("abc %luHz, %luHz, %luHz, %luHz", system_gclk_gen_get_hz(0), system_gclk_gen_get_hz(1), system_gclk_gen_get_hz(2), system_gclk_gen_get_hz(3));
//        getRtc().dumpRTC(0, 16);
//    }

    if (get_cdc_enumerated()) {
        poll_rx_bytes();
    }

//    sleepmgr_enter_sleep();
}

int main(void) {
    CEXCEPTION_T e;
    CEXCEPTION_FILELINEDEFS;

    irq_initialize_vectors();
    cpu_irq_enable();

    Try {
        init();
    } CatchFL(e) {
        while(1) {
            report_printf("Error in setup! Exception: %u (%s:%u)", e, ex_file, ex_line);
            for (uint8_t i = 0; i < 10; i++) {
                poll_rx_bytes();
            }
        }
    }

    Try {
        while (true) loop();
    } CatchFL(e) {
        while(1) {
            report_printf("Error in loop! Exception: %u (%s:%u)", e, ex_file, ex_line);
        }
        //while(1); // TODO: what to do????? - restart???
    }

//    nvm_get_config_defaults(&nvmConfig);
//
//    nvm_get_fuses(&nvmFusebits);
//
//    nvmFusebits.eeprom_size = NVM_EEPROM_EMULATOR_SIZE_16384;
//
//    Pin* led = new Pin(HAL_GPIO_PORTA, 27);
//    led->setDirOutput();
//    led->setOutputLow();
//    led->setOutputHigh();

//    while(1);

//    while(1) {
//        led.setOutput(port_pin_get_input_level(USB_VBUS_PIN));
////        led.setOutput(get_vbus_state());
////        led.setOutput(get_usb_enabled());
////        led.setOutput(get_sof_event());
////        led.setOutput(get_cdc_configured());
////        led.setOutput(get_cdc_enumerated());
//        sleepmgr_enter_sleep();
//    }

//    while(1) {
//        udi_cdc_putc('9');
//        for (uint32_t i = 0; i < 400000; i++) asm("nop");
//    }

//    Pin led(HAL_GPIO_PORTA, 27);
//    Pin btn(HAL_GPIO_PORTA, 14);
//
//    led.setDirOutput();
//    btn.setDirInput();
//
//    while(1) {
//        led.setOutputLow();
//        for (uint32_t i = 0; i < 80000; i++) asm("nop");
//        led.setOutputHigh();
//        for (uint32_t i = 0; i < 80000; i++) asm("nop");
//    }
}

