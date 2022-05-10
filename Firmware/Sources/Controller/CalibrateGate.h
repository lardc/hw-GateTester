// -----------------------------------------
// Logic of Gate calibration
// ----------------------------------------

#ifndef __CAL_GATE_H
#define __CAL_GATE_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"

// Functions
//
// Prepare measurements
void CALGATE_Prepare();
// Process Gate measurements
Boolean CALGATE_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning);

#endif // __CAL_GATE_H
