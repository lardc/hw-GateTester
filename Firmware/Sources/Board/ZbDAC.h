// -----------------------------------------
// Driver for DAC via SPI
// ----------------------------------------

#ifndef __ZBDAC_H
#define __ZBDAC_H

// Include
#include "stdinc.h"
#include "ZwDSP.h"

// Functions
//
// Init Protection DAC interface
void ZbPDAC_Init();
// Init GU-DAC and PU-DAC interface
void ZbUDAC_Init();
// Write to Protection DAC (A-channel)
void ZbPDAC_WriteA(Int16U A);
// Write to Protection DAC (B-channel)
void ZbPDAC_WriteB(Int16U B);
// Write to GU-DAC (A-channel)
void ZbGUDAC_WriteA(Int16U A);
// Write to PU-DAC (A-channel)
void ZbPUDAC_WriteA(Int16U A);

#endif // __ZBDAC_H
