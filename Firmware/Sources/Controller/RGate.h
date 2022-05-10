// -----------------------------------------
// Logic of Gate resistance testing
// ----------------------------------------

#ifndef __RGATE_H
#define __RGATE_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"

// Functions
//
// Prepare measurements
void RGATE_Prepare();
// Process Gate measurements
Boolean RGATE_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning);
// Notify that trip condition has been occurred in gate circuit
void RGATE_NotifyTripIGT();

#endif // __RGATE_H
