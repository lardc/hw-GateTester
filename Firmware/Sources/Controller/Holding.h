// -----------------------------------------
// Controller of Holding current testing
// ----------------------------------------

#ifndef __HOLDING_H
#define __HOLDING_H


// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"


// Functions
//
// Prepare measurements
void HOLDING_Prepare();
// Process IH measurements
Boolean HOLDING_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning);

#endif // __HOLDING_H
