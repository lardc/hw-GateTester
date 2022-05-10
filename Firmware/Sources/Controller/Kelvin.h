// -----------------------------------------
// Controller of Kelvin block
// ----------------------------------------

#ifndef __KELVIN_H
#define __KELVIN_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"

// Functions
//
// Prepare measurements
void KELVIN_Prepare(Boolean FullTestMode);
// Process Kelvin measurements
Boolean KELVIN_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning);

#endif // __KELVIN_H
