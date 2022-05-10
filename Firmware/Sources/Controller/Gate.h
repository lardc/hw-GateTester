// -----------------------------------------
// Logic of Gate testing
// ----------------------------------------

#ifndef __GATE_H
#define __GATE_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"

// Functions
//
// Prepare measurements
void GATE_Prepare();
// Process Gate measurements
Boolean GATE_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning);
// Notify that trip condition has been occurred in main circuit
void GATE_NotifyTripIM();
// Notify that trip condition has been occurred in gate circuit
void GATE_NotifyTripIGT();

#endif // __GATE_H
