// -----------------------------------------
// Utility functions for measurements
// ----------------------------------------

#ifndef __MEASURE_UTIL_H
#define __MEASURE_UTIL_H

// Include
#include "stdinc.h"
//
#include "IQmathLib.h"


// Functions
//
// Log data to scope sequences
void MU_LogScope(_iq I, _iq V, _iq TargetI, Int16S ControlSignal);
// Seek scope head position
Int16U MU_SeekScope(Int16U Channel, Int16S Offset);

#endif // __MEASURE_UTIL_H
