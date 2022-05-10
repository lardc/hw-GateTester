// -----------------------------------------
// Logic controller
// ----------------------------------------

#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"


// Constants
typedef enum __DeviceState
{
	DS_None			= 0,
	DS_Fault		= 1,
	DS_Disabled		= 2,
	DS_Kelvin		= 3,
	DS_Gate			= 4,
	DS_IH			= 5,
	DS_IL			= 6,
	DS_RG			= 7,
	DS_CalGate		= 8,
	DS_CalHolding   = 9
} DeviceState;


// Types
//
typedef void (*CONTROL_FUNC_RealTimeRoutine)();


// Variables
//
extern volatile Int64U CONTROL_TimeCounter;
extern volatile DeviceState CONTROL_State;
//
extern Int16U CONTROL_Values_1[VALUES_x_SIZE];
extern Int16U CONTROL_Values_2[VALUES_x_SIZE];
extern volatile Int16U CONTROL_Values_1_Counter, CONTROL_Values_2_Counter;
extern volatile Int16U CONTROL_BootLoaderRequest;


// Functions
//
// Initialize controller
void CONTROL_Init(Boolean BadClockDetected);
// Update low-priority states
void CONTROL_Idle();
// Main high-priority control cycle
void CONTROL_UpdateHigh();
// Real-time control routine
void CONTROL_RealTimeCycle();
// Switch-on/off real-time cycle
void CONTROL_SwitchRTCycle(Boolean Enable);
// Subscribe to real-time cycle
void CONTROL_SubcribeToCycle(CONTROL_FUNC_RealTimeRoutine Routine);
// Notify that trip condition has been occurred in gate circuit
void CONTROL_NotifyTripIGT();
// Notify that CAN system fault occurs
void CONTROL_NotifyCANFault(ZwCAN_SysFlags Flag);
// Dirty hack
void CONTROL_SelfExecutedTest();


#endif // __CONTROLLER_H
