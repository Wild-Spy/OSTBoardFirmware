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
#include <libs/TimerDescriptionLanguage/TdlRuleRunner.h>
#include <libs/TimerDescriptionLanguage/TdlChannels.h>
#include <libs/TimerDescriptionLanguage/TdlRules.h>
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

#define CHANNEL_COUNT   4

NvmDriverInterface* eepromDriver = NULL;
I2c* i2c = NULL;
Pin led(HAL_GPIO_PORTA, 27);

Pin* channel_pins[CHANNEL_COUNT];

static void i2cInit() {
    struct i2c_master_config i2c_cfg;
    i2c_master_get_config_defaults(&i2c_cfg);
    i2c_cfg.baud_rate = I2C_MASTER_BAUD_RATE_100KHZ;
    i2c_cfg.buffer_timeout = 1000;
    i2c_cfg.pinmux_pad0 = PINMUX_PA16C_SERCOM1_PAD0;
    i2c_cfg.pinmux_pad1 = PINMUX_PA17C_SERCOM1_PAD1;
    i2c_cfg.generator_source = GCLK_GENERATOR_3;
    i2c = new I2c(SERCOM1, &i2c_cfg);
}

void channelInit() {
    channel_pins[0] = new Pin(HAL_GPIO_PORTA, 27);

//    channel_pins[0] = new Pin(HAL_GPIO_PORTA, 1);
    channel_pins[1] = new Pin(HAL_GPIO_PORTA, 2);
    channel_pins[2] = new Pin(HAL_GPIO_PORTA, 3);
    channel_pins[3] = new Pin(HAL_GPIO_PORTA, 4);
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
    channelInit();

    TdlChannels_Init(CHANNEL_COUNT, TDLCHANNELSTATE_DISABLED, channel_pins);
    TdlRuleRunner_Init(*i2c, NvmRuleManager_Get());
    if (TdlRules_GetInstance().getCount() > 0) TdlRuleRunner_GetInstance().start();
}

void loop() {
    CEXCEPTION_T e = CEXCEPTION_NONE;

    TdlRuleRunner_GetInstance().mainLoopCallback();

    if (get_cdc_enumerated()) {
        poll_rx_bytes();
    }

    sleepmgr_enter_sleep();
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
}

