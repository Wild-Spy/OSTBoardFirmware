 ## OST Bootloader V1
 
To build you'll need a copy of IAR Embedded Workbench for ARM.  The outputs included in the __outputs__ directory were compiled with version 8.11.2.13606
and shared components version 8.0.8.4892.
A free version of IAR Embedded Workbench for ARM can be downloaded from the IAR website, since the code compiled by this bootloader is under 4kB, the
free kickstarter version can be used (which allows up to 32kB programs to be compiled).  The reason for using IAR to compile the bootloader is that
Atmel (Microchip now) recommend compiling their SAM-BA bootloader (which this bootloader is based off) with IAR to reduce code size.  The IAR compiler
is supposedly more space efficient than gcc.

This bootloader is a slightly modified version of Atmel's SAM-BA bootloader.  The main midification is to change the BOOT_PIN from PA15 to PA14.  More info about the SAM-BA bootloader can be found [here](http://www.atmel.com/tools/ATMELSAM-BAIN-SYSTEMPROGRAMMER.aspx).