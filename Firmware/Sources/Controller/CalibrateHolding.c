// -----------------------------------------
// Logic of Holding calibration
// ----------------------------------------

// Header
#include "Holding.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Controller.h"
#include "IQmathLib.h"
#include "IQmathUtils.h"
#include "MeasureUtils.h"


// Constants
//
#define SAMPLES_COUNT		64


// Types
//
typedef enum __CalHoldState
{
	CALHOLD_STATE_START = 0,
	CALHOLD_STATE_RISE,
	CALHOLD_STATE_PROCESS,
	CALHOLD_STATE_FINISH
} CalHoldState;


// Variables
//
static CalHoldState State;
static Int16U InternalFaultReason, InternalProblem, TimeoutCounter;
static _iq Ki, FineIH, OffsetIH;
//
static volatile Int32U SampleSum;
static volatile Boolean Active, FreezeControl;
static volatile Int16U SampleCounter;
static volatile _iq ControlSignal, TargetI;
static volatile _iq NowI;


// Forward functions
//
static void CALHOLD_CacheVariables();
static void CALHOLD_SetProbeResult(pInt16U Samples);
static void CALHOLD_Cycle();


// Functions
//
void CALHOLD_Prepare()
{
	const Int16U ADC_Channels[16] = { AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL,
									  AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL };
	
	// Save data from data table to local variables
	CALHOLD_CacheVariables();

	// Configure ADC
	ZwADC_ConfigureSequentialCascaded(sizeof ADC_Channels / sizeof ADC_Channels[0], ADC_Channels);
	ZwADC_SubscribeToResults1(&CALHOLD_SetProbeResult);
	
	// Clear output
	DataTable[REG_RESULT_CAL_I] = 0;

	// Adjust main controller
	CONTROL_SubcribeToCycle(&CALHOLD_Cycle);

	SampleSum = 0;
	SampleCounter = 0;
	TimeoutCounter = 0;

	State = CALHOLD_STATE_START;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CALHOLD_Process, "ramfuncs");
#endif
Boolean CALHOLD_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning)
{
	*FaultReason = FAULT_NONE;
	*Problem = PROBLEM_NONE;
	*Warning = WARNING_NONE;

	switch(State)
	{
		case CALHOLD_STATE_START:
			{
				TargetI = _IQI(DataTable[REG_CAL_CURRENT]);
				ControlSignal = _IQI(DataTable[REG_HOLD_CONTROL_OFFSET]);
				InternalFaultReason = FAULT_NONE;
				InternalProblem = PROBLEM_NONE;

				ZbGPIO_SwitchH(TRUE);

				FreezeControl = FALSE;
				Active = TRUE;
				CONTROL_SwitchRTCycle(TRUE);
				State = CALHOLD_STATE_RISE;
			}
			break;
		case CALHOLD_STATE_RISE:
			{
				if(_IQabs(NowI - TargetI) < CAL_CURRENT_TOLERANCE || (NowI > TargetI))
				{
					FreezeControl = TRUE;
					SampleSum = 0;
					SampleCounter = 0;

					State = CALHOLD_STATE_PROCESS;
				}
				
				MU_LogScope(NowI, 0, TargetI, _IQint(ControlSignal));
				
				if(TimeoutCounter++ > CAL_TIMEOUT)
				{
					FreezeControl = TRUE;
					InternalProblem = PROBLEM_HOLD_REACH_TIMEOUT;
					State = CALHOLD_STATE_FINISH;
				}
			}
			break;
		case CALHOLD_STATE_PROCESS:
			{
				ZbGPIO_SwitchDebugPin(TRUE);

				MU_LogScope(NowI, 0, TargetI, _IQint(ControlSignal));

				if(SampleCounter++ >= SAMPLES_COUNT)
				{
					DataTable[REG_RESULT_CAL_I] = SampleSum / SampleCounter;

					State = CALHOLD_STATE_FINISH;
				}
				else
					SampleSum += _IQint(NowI);
			}
			break;
		case CALHOLD_STATE_FINISH:
			{
				ZbGPIO_SwitchDebugPin(FALSE);

				Active = FALSE;
				ZwADC_SubscribeToResults1(NULL);

				ZbGPIO_SwitchH(FALSE);

				*FaultReason = InternalFaultReason;
				*Problem = InternalProblem;
				return TRUE;
			}
	}	
	
	return FALSE;
}
// ----------------------------------------

static void CALHOLD_CacheVariables()
{
	Ki = _IQdiv(_IQI(DataTable[REG_HOLD_Ki_N]), _IQI(DataTable[REG_HOLD_Ki_D]));
	FineIH = _IQdiv(_IQI(DataTable[REG_HOLD_FINE_IHL_N]), _IQI(DataTable[REG_HOLD_FINE_IHL_D]));
	OffsetIH = _IQI((Int16S)DataTable[REG_HOLD_IHL_OFFSET]);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CALHOLD_SetProbeResult, "ramfuncs");
#endif
static void CALHOLD_SetProbeResult(pInt16U Samples)
{
	Int16U j, RawI = 0;
	
	#pragma UNROLL(16)
	for(j = 0; j < 16; ++j)
		RawI += Samples[j];
	
	RawI >>= 4;

	NowI = _IQmpyI32(HOLD_CONVERT_V_C, RawI);
	NowI = _IQmpy(NowI, FineIH) + OffsetIH;

	NowI = (NowI > 0) ? NowI : 0;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CALHOLD_Cycle, "ramfuncs");
#endif
static void CALHOLD_Cycle()
{
	if(Active)
	{
		_iq err = TargetI - NowI;

		if(!FreezeControl)
		{
			// Discreet PI-controller
			ControlSignal = _IQsat(ControlSignal + _IQmpy(err, Ki), _IQ(4095.0f), 0);
			// Write signal
			ZbPUDAC_WriteA(_IQint(ControlSignal));
		}

		ZwADC_StartSEQ1();
	}
	else
	{
		ZbPUDAC_WriteA(0);
		CONTROL_SwitchRTCycle(FALSE);
	}
}
// ----------------------------------------

// No more
