// -----------------------------------------
// Driver for EEPROM & IOE via SPI
// ----------------------------------------

#ifndef __ZBSRAMIOE_H
#define __ZBSRAMIOE_H

// Include
#include "stdinc.h"
#include "ZwDSP.h"


// Functions
//
// Init SRAM interfaces
void ZbSRAMAndIOE_Init();
// Write values to EPROM
void ZbSRAM_WriteValuesEPROM(Int16U EPROMAddress, pInt16U Buffer, Int16U BufferSize);
// Read values from EPROM
void ZbSRAM_ReadValuesEPROM(Int16U EPROMAddress, pInt16U Buffer, Int16U BufferSize);
// Set state of IOE
void ZbIOE_WritePin(Int16U PinNumber, Boolean Value);


#endif // __ZBSRAMIOE_H
