// -----------------------------------------
// Logic of Holding calibration
// ----------------------------------------

#ifndef __CAL_HOLD_H
#define __CAL_HOLD_H


// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"


// Functions
//
// Prepare measurements
void CALHOLD_Prepare();
// Process IH measurements
Boolean CALHOLD_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning);

#endif // __CAL_HOLD_H
