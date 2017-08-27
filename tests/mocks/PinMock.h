//
// Created by mcochrane on 8/04/17.
//

#ifndef WS_OST_PIN_H
#define WS_OST_PIN_H

#include <stddef.h>
#include "avr/mockio.h"
#include "gmock/gmock.h"

#define PIN2MASK(p)     (1<<p)

typedef enum pin_dir_enum {
    pin_dir_input,
    pin_dir_output
} pin_dir_t;

typedef enum pin_output_level_enum {
    pin_outlvl_low,
    pin_outlvl_high
} pin_outlvl_t;

class PinInterface {
public:
    PinInterface() {};
    PinInterface(PORT_t* port, uint8_t pin) {};

    /**
     * Returns true if this pin is not valid.
     */
    virtual bool isEmpty() const  = 0;

    virtual void setDirOutput() = 0;
    virtual void setDirInput() = 0;

    virtual void setDir(pin_dir_t dir) = 0;
    virtual pin_dir_t getDir() const = 0;


    virtual bool isInverted() = 0;

    /**
     * Setting inverted to true will enable inverted output and
     * input data on this pin.
     * @param inverted  true to invert the pin, false otherwise.
     */
    virtual void setInverted(bool inverted) = 0;

    virtual void setInputSenseConfiguration(PORT_ISC_t isc) = 0;
    virtual PORT_ISC_t setInputSenseConfiguration()  = 0;

    virtual void setOutputPullConfiguration(PORT_OPC_t opc) = 0;
    virtual PORT_OPC_t getOutputPullConfiguration() = 0;

    /**
     * @return true if pin level is high, false if low
     */
    virtual bool getValue() const = 0;

    /**
     * Checks if this pin is enabled for the specified interrupt
     * @param interrupt_number  can be either 0 or 1
     * @return true if the interrupt is enabled, false otherwise
     */
    virtual bool interruptEnabled(uint8_t interrupt_number) = 0;

    /**
     * Enables this pin on the specifiec interrupt.
     * The interrupt level needs to be set before the interrupt
     * will actually be triggered!
     * @param interrupt_number  can be either 0 or 1
     */
    virtual void enableInterrupt(uint8_t interrupt_number) = 0;

    /**
     * Disables this pin on the specifiec interrupt.
     * @param interrupt_number  can be either 0 or 1
     */
    virtual void disableInterrupt(uint8_t interrupt_number) = 0;

    /**
     * Sets the level of the specified interrupt
     * The interrupt needs to be enabled as well.
     * Call enableInterrupt along with this to enable an interrupt.
     * @param interrupt_number  can be either 0 or 1
     * @param level             ALWAYS pass in a PORT_INT0_LVL_t regardless of
     *                          whether interrupt_number is 0 or 1 !
     */
    virtual void setInterruptLevel(uint8_t interrupt_number, PORT_INT0LVL_t level) = 0;

    /**
     * Sets the pin level
     * @param value  true sets the pin level high
     *               false sets the pin level low
     */
    virtual void setOutput(bool value) = 0;
    virtual void setOutputHigh() = 0;
    virtual void setOutputLow() = 0;
    virtual void toggleOutput() = 0;

    virtual uint8_t getPin() const  = 0;
    virtual PORT_t* getPort() const  = 0;

protected:
//    virtual void setPinCtrlReg(uint8_t value) = 0;
};

class PinMockInterface : public PinInterface {
public:
    PinMockInterface()
            : port_(NULL),
              pin_(0)
    {}

    PinMockInterface(PORT_t* port, uint8_t pin)
            : port_ (port),
              pin_(pin),
              inverted_(false),
              dir_(pin_dir_input),
              value_(false),
              isc_(PORT_ISC_BOTHEDGES_gc),
              opc_(PORT_OPC_TOTEM_gc)
    {
        interrupt_enabled_[0] = false;
        interrupt_enabled_[1] = false;
        interrupt_level_[0] = PORT_INT0LVL_OFF_gc;
        interrupt_level_[1] = PORT_INT0LVL_OFF_gc;
    }

    bool isEmpty() const { return port_ == NULL; };

    void setDirOutput() { setDir(pin_dir_output); };
    void setDirInput() { setDir(pin_dir_input); };
    void setDir(pin_dir_t dir) { dir_ = dir; dirChanged(dir_); };
    pin_dir_t getDir() const { return dir_; };

    bool isInverted() { return inverted_; };
    void setInverted(bool inverted) { inverted_ = inverted; invertedChanged(inverted_); };

    void setInputSenseConfiguration(PORT_ISC_t isc) { this->isc_ = isc; iscChanged(isc_); };
    PORT_ISC_t setInputSenseConfiguration() { return isc_; };

    void setOutputPullConfiguration(PORT_OPC_t opc) { this->opc_ = opc; opcChanged(opc_); };
    PORT_OPC_t getOutputPullConfiguration() { return opc_; };

    bool getValue() const {
        if (getDir() == pin_dir_output)
            return value_;
        else
            return returnInputValue();
    };

    pin_outlvl_t getOutputLevel() const { return (pin_outlvl_t) (inverted_?!value_:value_); };

    bool interruptEnabled(uint8_t interrupt_number) { return interrupt_enabled_[interrupt_number]; };
    void enableInterrupt(uint8_t interrupt_number) { interrupt_enabled_[interrupt_number] = true; interruptStateChanged(interrupt_number, true); };
    void disableInterrupt(uint8_t interrupt_number) { interrupt_enabled_[interrupt_number] = false; interruptStateChanged(interrupt_number, false); };

    void setInterruptLevel(uint8_t interrupt_number, PORT_INT0LVL_t level) { interrupt_level_[interrupt_number] = level; interruptLevelChanged(interrupt_number, level); };

    void setOutput(bool value) { value_ = value; valueChanged(value_); outputLevelChanged(getOutputLevel());};
    void setOutputHigh() { setOutput(true); };
    void setOutputLow() { setOutput(false); };
    void toggleOutput() { setOutput(!value_); };

    uint8_t getPin() const { return pin_; };
    PORT_t* getPort() const { return port_; };

    // Mock functions - these will be mocked.  They are called when their corresponding parameter changes
    virtual void valueChanged(bool newValue) {};
    virtual void outputLevelChanged(pin_outlvl_t newOutputLevel) {};
    virtual void invertedChanged(bool newValue) {};
    virtual void iscChanged(PORT_ISC_t newValue) {};
    virtual void opcChanged(PORT_OPC_t newValue) {};
    virtual void dirChanged(pin_dir_t newValue) {};
    virtual void interruptStateChanged(uint8_t int_number, bool enabled) {};
    virtual void interruptLevelChanged(uint8_t int_number, PORT_INT0LVL_t level) {};

    virtual bool returnInputValue() const { return false; };

protected:
    PORT_t* port_;
    uint8_t pin_;
    pin_dir_t dir_;

    bool interrupt_enabled_[2];
    PORT_INT0LVL_t interrupt_level_[2];
    bool value_;
    PORT_OPC_t opc_;
    PORT_ISC_t isc_;
    bool inverted_;
};

class Pin : public PinMockInterface {
public:
    Pin(PORT_t* port, uint8_t pin)
        : PinMockInterface(port, pin)
    {}

    MOCK_METHOD1(valueChanged, void(bool value));
    MOCK_METHOD1(outputLevelChanged, void(pin_outlvl_t value));
    MOCK_METHOD1(invertedChanged, void(bool value));
    MOCK_METHOD1(iscChanged, void(PORT_ISC_t value));
    MOCK_METHOD1(opcChanged, void(PORT_OPC_t value));
    MOCK_METHOD1(dirChanged, void(pin_dir_t value));
    MOCK_METHOD2(interruptStateChanged, void(uint8_t value, bool enabled));
    MOCK_METHOD2(interruptLevelChanged, void(uint8_t value, PORT_INT0LVL_t level));
    MOCK_METHOD0(returnInputValue, bool(void));
};


#endif //WS_OST_PIN_H
