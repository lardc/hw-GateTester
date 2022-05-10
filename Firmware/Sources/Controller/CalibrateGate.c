// -----------------------------------------
// Logic of Gate calibration
// ----------------------------------------

// Header
#include "CalibrateGate.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Global.h"
#include "IQmathLib.h"
#include "Controller.h"
#include "MeasureUtils.h"
#include "IQmathUtils.h"


// Constants
//
#define SAMPLES_COUNT		64


// Types
//
typedef enum __CalGateState
{
	CALGATE_STATE_START = 0,
	CALGATE_STATE_RISE,
	CALGATE_STATE_PROCESS,
	CALGATE_STATE_FINISH
} CalGateState;


// Variables
//
static CalGateState State;
static Int16U InternalFaultReason, InternalProblem, TimeoutCounter;
static _iq Ki, FineVGT, FineIGT, OffsetVGT, OffsetIGT;
//
static volatile Int32U ChVSum, ChISum;
static volatile Boolean Active, FreezeControl;
static volatile Int16U SampleCounter;
static volatile _iq NowI, NowV;
static volatile _iq ControlSignal, TargetI;


// Forward functions
//
static void CALGATE_CacheVariables();
static void CALGATE_SetProbeResult(pInt16U Samples);
static void CALGATE_Cycle();


// Functions
//
void CALGATE_Prepare()
{
	const Int16U ADC_Channels[8] = {AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT};

	// Init state
	CALGATE_CacheVariables();

	// Configure ADC
	ZwADC_ConfigureSimultaneousCascaded(sizeof ADC_Channels / sizeof ADC_Channels[0], ADC_Channels);
	ZwADC_SubscribeToResults1(&CALGATE_SetProbeResult);

	// Adjust main controller
	CONTROL_SubcribeToCycle(&CALGATE_Cycle);

	DataTable[REG_RESULT_CAL_I] = 0;
	DataTable[REG_RESULT_CAL_V] = 0;

	ChISum = ChVSum = 0;
	SampleCounter = 0;
	TimeoutCounter = 0;

	State = CALGATE_STATE_START;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CALGATE_Process, "ramfuncs");
#endif
Boolean CALGATE_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning)
{
	*FaultReason = FAULT_NONE;
	*Problem = PROBLEM_NONE;
	*Warning = WARNING_NONE;

	switch(State)
	{
		case CALGATE_STATE_START:
			{
				TargetI = _IQI(DataTable[REG_CAL_CURRENT]);
				ControlSignal = _IQI(DataTable[REG_GATE_CONTROL_OFFSET]);
				InternalFaultReason = FAULT_NONE;
				InternalProblem = PROBLEM_NONE;

				// Clear Stop_G
				ZbGPIO_SwitchStop(FALSE);

				FreezeControl = FALSE;
				Active = TRUE;
				CONTROL_SwitchRTCycle(TRUE);
				State = CALGATE_STATE_RISE;
			}
			break;
		case CALGATE_STATE_RISE:
			{
				MU_LogScope(NowI, NowV, TargetI, _IQint(ControlSignal));

				if((_IQabs(NowI - TargetI) < CAL_CURRENT_TOLERANCE) || (NowI > TargetI))
				{
					FreezeControl = TRUE;
					ChISum = ChVSum = 0;
					SampleCounter = 0;

					State = CALGATE_STATE_PROCESS;
				}

				if(TimeoutCounter++ > CAL_TIMEOUT)
				{
					FreezeControl = TRUE;
					InternalProblem = PROBLEM_GATE_FLW_ERROR;
					State = CALGATE_STATE_FINISH;
				}
			}
			break;
		case CALGATE_STATE_PROCESS:
			{
				ZbGPIO_SwitchDebugPin(TRUE);

				MU_LogScope(NowI, NowV, TargetI, _IQint(ControlSignal));

				if(SampleCounter++ >= SAMPLES_COUNT)
				{
					DataTable[REG_RESULT_CAL_I] = ChISum / SampleCounter;
					DataTable[REG_RESULT_CAL_V] = ChVSum / SampleCounter;

					State = CALGATE_STATE_FINISH;
				}
				else
				{
					ChISum += _IQint(NowI);
					ChVSum += _IQint(NowV);
				}
			}
			break;
		case CALGATE_STATE_FINISH:
			{
				ZbGPIO_SwitchDebugPin(FALSE);

				Active = FALSE;
				ZwADC_SubscribeToResults1(NULL);

				*FaultReason = InternalFaultReason;
				*Problem = InternalProblem;

				return TRUE;
			}
	}

	return FALSE;
}
// ----------------------------------------

static void CALGATE_CacheVariables()
{
	Ki = _FPtoIQ2(DataTable[REG_GATE_Ki_N], DataTable[REG_GATE_Ki_D]);
	FineVGT = _FPtoIQ2(DataTable[REG_GATE_FINE_VGT_N], DataTable[REG_GATE_FINE_VGT_D]);
	FineIGT = _FPtoIQ2(DataTable[REG_GATE_FINE_IGT_N], DataTable[REG_GATE_FINE_IGT_D]);
	OffsetIGT = _IQI((Int16S)DataTable[REG_GATE_IGT_OFFSET]);
	OffsetVGT = _IQI((Int16S)DataTable[REG_GATE_VGT_OFFSET]);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CALGATE_SetProbeResult, "ramfuncs");
#endif
static void CALGATE_SetProbeResult(pInt16U Samples)
{
	Int16U j, RawI = 0, RawV = 0;

	// Patch ADC
	Samples[0] = Samples[2];
	Samples[1] = Samples[3];

	#pragma UNROLL(8)
	for(j = 0; j < 16; j += 2)
	{
		RawV += Samples[j];
		RawI += Samples[j + 1];
	}

	RawI >>= 3; RawV >>= 3;

	NowI = _IQmpy(_IQmpyI32(GATE_CONVERT_V_C, RawI), FineIGT) + OffsetIGT;
	NowV = _IQmpy(_IQmpyI32(GATE_CONVERT_V_V, RawV), FineVGT) + OffsetVGT;

	NowI = (NowI > 0) ? NowI : 0;
	NowV = (NowV > 0) ? NowV : 0;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CALGATE_Cycle, "ramfuncs");
#endif
static void CALGATE_Cycle()
{
	if(Active)
	{
		_iq err = TargetI - NowI;

		if(!FreezeControl)
		{
			// Discreet PI-controller
			ControlSignal = _IQsat(ControlSignal + _IQmpy(err, Ki), _IQ(4095), 0);
			// Write signal
			ZbGUDAC_WriteA(_IQint(ControlSignal));
		}

		ZwADC_StartSEQ1();
	}
	else
	{
		ZbGUDAC_WriteA(0);
		CONTROL_SwitchRTCycle(FALSE);
	}
}
// ----------------------------------------

// No more
