////
//// Created by mcochrane on 21/05/17.
////
//
#include "NvmDriverMock.h"
#include <stddef.h>

//static NvmDriverMock* eepromDriverMock = NULL;
//
//void ReadEEPROM(uint8_t* Data, uint8_t Length, uint16_t Address) {
//    EEPROMDriverMock_Get().ReadEEPROM(Data, Length, Address);
//}
//
//void WriteEEPROM(uint8_t* Data, uint8_t Length, uint16_t Address) {
//    EEPROMDriverMock_Get().WriteEEPROM(Data, Length, Address);
//}
//
//void EEPROM_FlushBuffer( void ) {
//    EEPROMDriverMock_Get().EEPROM_FlushBuffer();
//}
//
//void check_pointer_valid() {
//    if (eepromDriverMock == NULL) Throw(EX_NOT_INITIALISED);
//}
//
//void EEPROMDriverMock_Init() {
//    eepromDriverMock = new NvmDriverMock();
//}
//
//NvmDriverMock& EEPROMDriverMock_Get() {
//    check_pointer_valid();
//    return *eepromDriverMock;
//}
//
//void EEPROMDriverMock_Destroy() {
//    check_pointer_valid();
//    delete eepromDriverMock;
//    eepromDriverMock = NULL;
//}
