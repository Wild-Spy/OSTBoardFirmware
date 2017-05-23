////
//// Created by mcochrane on 21/05/17.
////
//
#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H
//

#include "gmock/gmock.h"
#include <stdint.h>
#include "exception/CException.h"
#include "nvm/NvmDriverInterface.h"

//void EEPROM_WriteByte( uint8_t pageAddr, uint8_t byteAddr, uint8_t value );
//uint8_t EEPROM_ReadByte( uint8_t pageAddr, uint8_t byteAddr );
//void EEPROM_WaitForNVM( void );
//void EEPROM_FlushBuffer( void );
//void EEPROM_LoadByte( uint8_t byteAddr, uint8_t value );
//void EEPROM_LoadPage( const uint8_t * values );
//void EEPROM_AtomicWritePage( uint8_t pageAddr );
//void EEPROM_ErasePage( uint8_t pageAddress );
//void EEPROM_SplitWritePage( uint8_t pageAddr );
//void EEPROM_EraseAll( void );

//class NvmDriverMockInterface {
//public:
//    virtual void ReadEEPROM(uint8_t* Data, uint8_t Length, uint16_t Address) {};
//    virtual void WriteEEPROM(uint8_t* Data, uint8_t Length, uint16_t Address) {};
//    virtual void EEPROM_FlushBuffer( void ) {};
//};

class NvmDriverMock : public NvmDriverInterface {
public:
    MOCK_METHOD3(read, void(nvm_data_t* data, nvm_size_t length, nvm_address_t address));
    MOCK_METHOD3(write, void(nvm_data_t* data, nvm_size_t length, nvm_address_t address));
    MOCK_METHOD0(flush, void(void));
};

//void ReadEEPROM(uint8_t* Data, uint8_t Length, uint16_t Address);
//void WriteEEPROM(uint8_t* Data, uint8_t Length, uint16_t Address);
//void EEPROM_FlushBuffer( void );
//
//void EEPROMDriverMock_Init();
//NvmDriverMock& EEPROMDriverMock_Get();
//void EEPROMDriverMock_Destroy();

#endif //EEPROM_DRIVER_H
