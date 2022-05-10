// -----------------------------------------
// Logic of Gate resistance testing
// ----------------------------------------

// Header
#include "RGate.h"
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
#define SAMPLE_FILTER_LENGTH		4
#define SAMPLE_FILTER_2ORDER		2
#define SHDN_DELAY_US				(TIMER0_PERIOD * 5)


// Types
//
typedef enum __RGateState
{
	RGATE_STATE_START = 0,
	RGATE_STATE_WAIT_STAB,
	RGATE_STATE_PROCESS,
	RGATE_STATE_SAMPLING,
	RGATE_STATE_FINISH
} RGateState;


// Variables
//
static RGateState State;
static Int16U InternalFaultReason, InternalProblem, SamplingWaitCounter, StabCounter, SampleCount;
static Int32U SumI, SumV;
static _iq CurrentRate, Ki, EndCurrent, StartCurrent, FollowErrThreshold, FineVGT, FineIGT, OffsetVGT, OffsetIGT;
static Int16S SamplesI[SAMPLE_FILTER_LENGTH - 1], SamplesV[SAMPLE_FILTER_LENGTH - 1];
//
static volatile Boolean Active, FreezeControl, TripIGTDetected, FirstRun = TRUE;
static volatile _iq NowI, NowV, ControlSignal, TargetI, ErrorI;


// Forward functions
//
static void RGATE_CacheVariables();
static void RGATE_SetProbeResult(pInt16U Samples);
static void RGATE_Cycle();


// Functions
//
void RGATE_Prepare()
{
	const Int16U ADC_Channels[8] = {AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT};

	// Init state
	RGATE_CacheVariables();

	// Configure ADC
	ZwADC_ConfigureSimultaneousCascaded(sizeof ADC_Channels / sizeof ADC_Channels[0], ADC_Channels);
	ZwADC_SubscribeToResults1(&RGATE_SetProbeResult);

	// Adjust main controller
	CONTROL_SubcribeToCycle(&RGATE_Cycle);

	DataTable[REG_RESULT_RG] = 0;
	MemZero16((pInt16U)SamplesI, SAMPLE_FILTER_LENGTH - 1);
	MemZero16((pInt16U)SamplesV, SAMPLE_FILTER_LENGTH - 1);

	State = RGATE_STATE_START;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(RGATE_Process, "ramfuncs");
#endif
Boolean RGATE_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning)
{
	*FaultReason = FAULT_NONE;
	*Problem = PROBLEM_NONE;
	*Warning = WARNING_NONE;

	switch(State)
	{
		case RGATE_STATE_START:
			{
				TargetI = StartCurrent;
				ControlSignal = _IQI(DataTable[REG_GATE_CONTROL_OFFSET]);
				InternalFaultReason = FAULT_NONE;
				InternalProblem = PROBLEM_NONE;
				TripIGTDetected = FALSE;
				StabCounter = GATE_STAB_COUNTER_START;
				SumI = SumV = 0;
				SamplingWaitCounter = 0;

				// Clear Stop_G
				ZbGPIO_SwitchStop(FALSE);

				FreezeControl = FALSE;
				Active = TRUE;
				CONTROL_SwitchRTCycle(TRUE);
				State = RGATE_STATE_WAIT_STAB;
			}
			break;
		case RGATE_STATE_WAIT_STAB:
			{
				if(!(--StabCounter))
				{
					// TZ_IGT
					ZbGPIO_EnableProtectionIGT(TRUE);
					State = RGATE_STATE_PROCESS;
				}
			}
			break;
		case RGATE_STATE_PROCESS:
			{
				MU_LogScope(NowI, NowV, TargetI, _IQint(ControlSignal));

				if(TripIGTDetected)
				{
					InternalProblem = PROBLEM_RG_OVERLOAD;
					State = RGATE_STATE_FINISH;
				}
				else if(ErrorI > FollowErrThreshold)
				{
					InternalProblem = PROBLEM_RG_FLW_ERROR;
					State = RGATE_STATE_FINISH;
				}
				else if(TargetI >= EndCurrent)
				{
					FreezeControl = TRUE;
					State = RGATE_STATE_SAMPLING;
				}
				else
				{
					TargetI += CurrentRate;
				}
			}
			break;
		case RGATE_STATE_SAMPLING:
			{
				MU_LogScope(NowI, NowV, TargetI, _IQint(ControlSignal));

				if((SamplingWaitCounter++) < SampleCount)
				{
					SumI += _IQint(NowI);
					SumV += _IQint(NowV);
				}
				else
				{
					// * 10 to receive result in [Ohm * 10]
					DataTable[REG_RESULT_RG] = (SumV * 10) / SumI;
					State = RGATE_STATE_FINISH;
				}
			}
			break;
		case RGATE_STATE_FINISH:
			{
				// TZ_IGT
				ZbGPIO_EnableProtectionIGT(FALSE);

				Active = FALSE;
				ZwADC_SubscribeToResults1(NULL);
				DELAY_US(SHDN_DELAY_US);

				// Clear STOP_G
				ZbGPIO_SwitchStop(FALSE);

				*FaultReason = InternalFaultReason;
				*Problem = InternalProblem;
				FirstRun = FALSE;

				return TRUE;
			}
	}

	return FALSE;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(RGATE_NotifyTripIGT, "ramfuncs");
#endif
void RGATE_NotifyTripIGT()
{
	if(!FirstRun)
	{
		// Clear STOP_G signal
		ZbGPIO_SwitchStop(TRUE);

		TripIGTDetected = TRUE;
		DELAY_US(TRIP_DELAY_US);

		ZbGPIO_EnableProtectionIGT(FALSE);
	}
}
// ----------------------------------------

static void RGATE_CacheVariables()
{
	CurrentRate = _FPtoIQ2(DataTable[REG_GATE_CURRENT_RATE_N], DataTable[REG_GATE_CURRENT_RATE_D]);
	FineVGT = _FPtoIQ2(DataTable[REG_GATE_FINE_VGT_N], DataTable[REG_GATE_FINE_VGT_D]);
	FineIGT = _FPtoIQ2(DataTable[REG_GATE_FINE_IGT_N], DataTable[REG_GATE_FINE_IGT_D]);
	Ki = _FPtoIQ2(DataTable[REG_GATE_Ki_N], DataTable[REG_GATE_Ki_D]);
	FollowErrThreshold = _IQI(DataTable[REG_GATE_FE_THRESHOLD]);
	StartCurrent = _IQI(DataTable[REG_GATE_START_CURRENT]);
	SampleCount = DataTable[REG_RG_SAMPLE_COUNT];
	EndCurrent = _IQI(DataTable[REG_RG_CURRENT]);
	OffsetIGT = _IQI((Int16S)DataTable[REG_GATE_IGT_OFFSET]);
	OffsetVGT = _IQI((Int16S)DataTable[REG_GATE_VGT_OFFSET]);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(RGATE_SetProbeResult, "ramfuncs");
#endif
static void RGATE_SetProbeResult(pInt16U Samples)
{
	Int16U i, k;
	Int16U NewI = 0, NewV = 0, FilteredI = 0, FilteredV = 0;

	// Patch ADC
	Samples[0] = Samples[2];
	Samples[1] = Samples[3];

	#pragma UNROLL(8)
	for(i = 0; i < 16; i += 2)
	{
		NewV += Samples[i];
		NewI += Samples[i + 1];
	}

	NewI >>= 3; NewV >>= 3;

	for(k = SAMPLE_FILTER_LENGTH - 1; k > 0; --k)
	{
		FilteredI += SamplesI[k - 1];
		FilteredV += SamplesV[k - 1];

		if(k > 1)
		{
			SamplesI[k - 1] = SamplesI[k - 2];
			SamplesV[k - 1] = SamplesV[k - 2];
		}
		else
		{
			SamplesI[0] = NewI;
			SamplesV[0] = NewV;
		}
	}

	FilteredI = (FilteredI + NewI) >> SAMPLE_FILTER_2ORDER;
	FilteredV = (FilteredV + NewV) >> SAMPLE_FILTER_2ORDER;

	NowI = _IQmpy(_IQmpyI32(GATE_CONVERT_V_C, FilteredI), FineIGT) + OffsetIGT;
	NowV = _IQmpy(_IQmpyI32(GATE_CONVERT_V_V, FilteredV), FineVGT) + OffsetVGT;

	NowI = (NowI > 0) ? NowI : 0;
	NowV = (NowV > 0) ? NowV : 0;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(RGATE_Cycle, "ramfuncs");
#endif
static void RGATE_Cycle()
{
	if(Active)
	{
		ErrorI = TargetI - NowI;

		if(!FreezeControl || TripIGTDetected)
		{
			// Discreet PI-controller
			ControlSignal = _IQsat(ControlSignal + _IQmpy(ErrorI, Ki), _IQ(4095), 0);

			// Write signal
			if(!TripIGTDetected)
				ZbGUDAC_WriteA(_IQint(ControlSignal));
			else
				ZbGUDAC_WriteA(0);
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
