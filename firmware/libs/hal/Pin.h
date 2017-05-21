//
// Created by mcochrane on 8/04/17.
//

#ifndef WS_OST_PIN_H
#define WS_OST_PIN_H

#include <stddef.h>


//#ifdef TESTING
//#include "avr/mockio.h"
//#else
//extern "C" {
//#include <avr/io.h>
//};
//#endif

extern "C" {
#include <avr/io.h>
};

#define PIN2MASK(p)     (1<<p)

typedef enum pin_dir_enum {
    pin_dir_input,
    pin_dir_output
} pin_dir_t;

class Pin {
public:
    Pin()
        : port_(NULL),
          pin_(0)
    {

    }

    Pin(PORT_t* port, uint8_t pin)
            : port_ (port),
              pin_(pin),
              pin_mask_((uint8_t)PIN2MASK(pin)),
              pin_ctrl_reg_(0)
    {}

    /**
     * Returns true if this pin is not valid.
     */
    bool isEmpty() const { return port_ == NULL; };

    /**
     * Makes the pin an output
     */
    void setDirOutput();

    /**
     * Makes the pin an input
     */
    void setDirInput();

    void setDir(pin_dir_t dir);
    pin_dir_t getDir() const;


    bool isInverted() { return (bool) (pin_ctrl_reg_ & PORT_INVEN_bm); };

    /**
     * Setting inverted to true will enable inverted output and
     * input data on this pin.
     * @param inverted  true to invert the pin, false otherwise.
     */
    void setInverted(bool inverted);

    void setInputSenseConfiguration(PORT_ISC_t isc);
    PORT_ISC_t setInputSenseConfiguration() { return (PORT_ISC_t) (pin_ctrl_reg_ & PORT_ISC_gm); };

    void setOutputPullConfiguration(PORT_OPC_t opc);
    PORT_OPC_t getOutputPullConfiguration() { return (PORT_OPC_t) (pin_ctrl_reg_ & PORT_OPC_gm); };

    /**
     * @return true if pin level is high, false if low
     */
    bool getValue() const;

    /**
     * Checks if this pin is enabled for the specified interrupt
     * @param interrupt_number  can be either 0 or 1
     * @return true if the interrupt is enabled, false otherwise
     */
    bool interruptEnabled(uint8_t interrupt_number);

    /**
     * Enables this pin on the specifiec interrupt.
     * The interrupt level needs to be set before the interrupt
     * will actually be triggered!
     * @param interrupt_number  can be either 0 or 1
     */
    void enableInterrupt(uint8_t interrupt_number);

    /**
     * Disables this pin on the specifiec interrupt.
     * @param interrupt_number  can be either 0 or 1
     */
    void disableInterrupt(uint8_t interrupt_number);

    /**
     * Sets the level of the specified interrupt
     * The interrupt needs to be enabled as well.
     * Call enableInterrupt along with this to enable an interrupt.
     * @param interrupt_number  can be either 0 or 1
     * @param level             ALWAYS pass in a PORT_INT0_LVL_t regardless of
     *                          whether interrupt_number is 0 or 1 !
     */
    void setInterruptLevel(uint8_t interrupt_number, PORT_INT0LVL_t level);

    /**
     * Sets the pin level
     * @param value  true sets the pin level high
     *               false sets the pin level low
     */
    void setOutput(bool value);
    void setOutputHigh();
    void setOutputLow();
    void toggleOutput();

    uint8_t getPin() const { return pin_; };
    PORT_t* getPort() const { return port_; };

private:
    void setPinCtrlReg(uint8_t value);

private:
    PORT_t* port_;
    uint8_t pin_;
    uint8_t pin_mask_;
    pin_dir_t dir_;
    uint8_t pin_ctrl_reg_;
};


#endif //WS_OST_PIN_H
