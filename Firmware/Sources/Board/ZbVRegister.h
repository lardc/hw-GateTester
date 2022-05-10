// -----------------------------------------
// Driver for VRegister buffers via SPI
// ----------------------------------------

#ifndef __ZBVREGISTER_H
#define __ZBVREGISTER_H

// Include
#include "stdinc.h"


// Functions
//
// Initialize module
void ZbVRegister_Init(Int16U DefaultValue);
// Write whole output register
void ZbVRegister_WriteRegister(Int16U Value);


#endif // __ZBVREGISTER_H
