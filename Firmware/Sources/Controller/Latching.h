// -----------------------------------------
// Logic of latching current testing process
// ----------------------------------------

#ifndef __LATCHING_H
#define __LATCHING_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"

// Functions
//
// Prepare measurements
void LATCHING_Prepare();
// Process LC measurements
Boolean LATCHING_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning);

#endif // __LATCHING_H
