//
// Created by mcochrane on 17/05/17.
//

#ifndef OSTBOARDFIRMWARE_MOCKIO_H
#define OSTBOARDFIRMWARE_MOCKIO_H

#include <stdint.h>

/* I/O Ports */
typedef struct PORT_struct
{
    uint8_t DIR;  /* I/O Port Data Direction */
    uint8_t DIRSET;  /* I/O Port Data Direction Set */
    uint8_t DIRCLR;  /* I/O Port Data Direction Clear */
    uint8_t DIRTGL;  /* I/O Port Data Direction Toggle */
    uint8_t OUT;  /* I/O Port Output */
    uint8_t OUTSET;  /* I/O Port Output Set */
    uint8_t OUTCLR;  /* I/O Port Output Clear */
    uint8_t OUTTGL;  /* I/O Port Output Toggle */
    uint8_t IN;  /* I/O port Input */
    uint8_t INTCTRL;  /* Interrupt Control Register */
    uint8_t INT0MASK;  /* Port Interrupt 0 Mask */
    uint8_t INT1MASK;  /* Port Interrupt 1 Mask */
    uint8_t INTFLAGS;  /* Interrupt Flag Register */
    uint8_t reserved_0x0D;
    uint8_t REMAP;  /* I/O Port Pin Remap Register */
    uint8_t reserved_0x0F;
    uint8_t PIN0CTRL;  /* Pin 0 Control Register */
    uint8_t PIN1CTRL;  /* Pin 1 Control Register */
    uint8_t PIN2CTRL;  /* Pin 2 Control Register */
    uint8_t PIN3CTRL;  /* Pin 3 Control Register */
    uint8_t PIN4CTRL;  /* Pin 4 Control Register */
    uint8_t PIN5CTRL;  /* Pin 5 Control Register */
    uint8_t PIN6CTRL;  /* Pin 6 Control Register */
    uint8_t PIN7CTRL;  /* Pin 7 Control Register */
} PORT_t;

/* Port Interrupt 0 Level */
typedef enum PORT_INT0LVL_enum
{
    PORT_INT0LVL_OFF_gc = (0x00<<0),  /* Interrupt Disabled */
    PORT_INT0LVL_LO_gc = (0x01<<0),  /* Low Level */
    PORT_INT0LVL_MED_gc = (0x02<<0),  /* Medium Level */
    PORT_INT0LVL_HI_gc = (0x03<<0),  /* High Level */
} PORT_INT0LVL_t;

/* Port Interrupt 1 Level */
typedef enum PORT_INT1LVL_enum
{
    PORT_INT1LVL_OFF_gc = (0x00<<2),  /* Interrupt Disabled */
    PORT_INT1LVL_LO_gc = (0x01<<2),  /* Low Level */
    PORT_INT1LVL_MED_gc = (0x02<<2),  /* Medium Level */
    PORT_INT1LVL_HI_gc = (0x03<<2),  /* High Level */
} PORT_INT1LVL_t;

/* Output/Pull Configuration */
typedef enum PORT_OPC_enum
{
    PORT_OPC_TOTEM_gc = (0x00<<3),  /* Totempole */
    PORT_OPC_BUSKEEPER_gc = (0x01<<3),  /* Totempole w/ Bus keeper on Input and Output */
    PORT_OPC_PULLDOWN_gc = (0x02<<3),  /* Totempole w/ Pull-down on Input */
    PORT_OPC_PULLUP_gc = (0x03<<3),  /* Totempole w/ Pull-up on Input */
    PORT_OPC_WIREDOR_gc = (0x04<<3),  /* Wired OR */
    PORT_OPC_WIREDAND_gc = (0x05<<3),  /* Wired AND */
    PORT_OPC_WIREDORPULL_gc = (0x06<<3),  /* Wired OR w/ Pull-down */
    PORT_OPC_WIREDANDPULL_gc = (0x07<<3),  /* Wired AND w/ Pull-up */
} PORT_OPC_t;

/* Input/Sense Configuration */
typedef enum PORT_ISC_enum
{
    PORT_ISC_BOTHEDGES_gc = (0x00<<0),  /* Sense Both Edges */
    PORT_ISC_RISING_gc = (0x01<<0),  /* Sense Rising Edge */
    PORT_ISC_FALLING_gc = (0x02<<0),  /* Sense Falling Edge */
    PORT_ISC_LEVEL_gc = (0x03<<0),  /* Sense Level (Transparent For Events) */
    PORT_ISC_INPUT_DISABLE_gc = (0x07<<0),  /* Disable Digital Input Buffer */
} PORT_ISC_t;

/* PORT.PIN0CTRL  bit masks and bit positions */
#define PORT_SRLEN_bm  0x80  /* Slew Rate Enable bit mask. */
#define PORT_SRLEN_bp  7  /* Slew Rate Enable bit position. */

#define PORT_INVEN_bm  0x40  /* Inverted I/O Enable bit mask. */
#define PORT_INVEN_bp  6  /* Inverted I/O Enable bit position. */

#define PORT_OPC_gm  0x38  /* Output/Pull Configuration group mask. */
#define PORT_OPC_gp  3  /* Output/Pull Configuration group position. */
#define PORT_OPC0_bm  (1<<3)  /* Output/Pull Configuration bit 0 mask. */
#define PORT_OPC0_bp  3  /* Output/Pull Configuration bit 0 position. */
#define PORT_OPC1_bm  (1<<4)  /* Output/Pull Configuration bit 1 mask. */
#define PORT_OPC1_bp  4  /* Output/Pull Configuration bit 1 position. */
#define PORT_OPC2_bm  (1<<5)  /* Output/Pull Configuration bit 2 mask. */
#define PORT_OPC2_bp  5  /* Output/Pull Configuration bit 2 position. */

#define PORT_ISC_gm  0x07  /* Input/Sense Configuration group mask. */
#define PORT_ISC_gp  0  /* Input/Sense Configuration group position. */
#define PORT_ISC0_bm  (1<<0)  /* Input/Sense Configuration bit 0 mask. */
#define PORT_ISC0_bp  0  /* Input/Sense Configuration bit 0 position. */
#define PORT_ISC1_bm  (1<<1)  /* Input/Sense Configuration bit 1 mask. */
#define PORT_ISC1_bp  1  /* Input/Sense Configuration bit 1 position. */
#define PORT_ISC2_bm  (1<<2)  /* Input/Sense Configuration bit 2 mask. */
#define PORT_ISC2_bp  2  /* Input/Sense Configuration bit 2 position. */

/* I/O port Configuration */
typedef struct PORTCFG_struct
{
    uint8_t MPCMASK;  /* Multi-pin Configuration Mask */
    uint8_t reserved_0x01;
    uint8_t VPCTRLA;  /* Virtual Port Control Register A */
    uint8_t VPCTRLB;  /* Virtual Port Control Register B */
    uint8_t CLKEVOUT;  /* Clock and Event Out Register */
    uint8_t EBIOUT;  /* EBI Output register */
    uint8_t EVOUTSEL;  /* Event Output Select */
} PORTCFG_t;

#endif //OSTBOARDFIRMWARE_MOCKIO_H
