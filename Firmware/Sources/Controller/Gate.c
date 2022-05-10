// -----------------------------------------
// Logic of Gate testing
// ----------------------------------------

// Header
#include "Gate.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Global.h"
#include "IQmathLib.h"
#include "IQmathUtils.h"
#include "Controller.h"
#include "MeasureUtils.h"


// Constants
//
#define SAMPLE_FILTER_LENGTH		8
#define SAMPLE_FILTER_2ORDER		3
#define SHDN_DELAY_US				(TIMER0_PERIOD * 5)
#define GATE_HISTORY_LENGTH			8


// Types
//
typedef enum __GateState
{
	GATE_STATE_START = 0,
	GATE_STATE_WAIT_STAB,
	GATE_STATE_PROCESS,
	GATE_STATE_SAMPLING,
	GATE_STATE_FINISH
} GateState;


// Variables
//
static Boolean PureVGT;
static Int16U InternalFaultReason, InternalProblem, SamplingWaitCounter, StabCounter, FECounterMax, FECounter;
static _iq CurrentRate, Ki, EndCurrent, FollowErrThreshold, StartCurrent, FineVGT, FineIGT, OffsetIGT, OffsetVGT;
static Int16S SamplesI[SAMPLE_FILTER_LENGTH - 1], SamplesV[SAMPLE_FILTER_LENGTH - 1];
static _iq HistoryI[GATE_HISTORY_LENGTH], HistoryV[GATE_HISTORY_LENGTH];
//
static volatile GateState State;
static volatile Boolean Active, TripIMDetected, TripIGTDetected;
static volatile _iq NowI, NowV;
static volatile _iq ControlSignal, TargetI, ErrorI;


// Forward functions
//
static void GATE_CacheVariables();
static void GATE_SetProbeResult(pInt16U Samples);
static void GATE_Cycle();


// Functions
//
void GATE_Prepare()
{
	const Int16U ADC_Channels[8] = {AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT, AIN_VGT_IGT};

	// Save data from data table
	GATE_CacheVariables();

	// Configure ADC
	ZwADC_ConfigureSimultaneousCascaded(sizeof ADC_Channels / sizeof ADC_Channels[0], ADC_Channels);
	ZwADC_SubscribeToResults1(&GATE_SetProbeResult);

	CONTROL_SubcribeToCycle(&GATE_Cycle);

	DataTable[REG_RESULT_VGT] = DataTable[REG_RESULT_IGT] = 0;

	MemZero16((pInt16U)SamplesI, SAMPLE_FILTER_LENGTH - 1);
	MemZero16((pInt16U)SamplesV, SAMPLE_FILTER_LENGTH - 1);
	MemZero32((pInt32U)HistoryI, GATE_HISTORY_LENGTH - 1);
	MemZero32((pInt32U)HistoryV, GATE_HISTORY_LENGTH - 1);

	State = GATE_STATE_START;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(GATE_Process, "ramfuncs");
#endif
Boolean GATE_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning)
{
	*FaultReason = FAULT_NONE;
	*Problem = PROBLEM_NONE;
	*Warning = WARNING_NONE;

	switch(State)
	{
		case GATE_STATE_START:
			{
				TargetI = StartCurrent;
				ControlSignal = _IQI(DataTable[REG_GATE_CONTROL_OFFSET]);
				InternalFaultReason = FAULT_NONE;
				InternalProblem = PROBLEM_NONE;
				TripIMDetected = TripIGTDetected = FALSE;
				StabCounter = GATE_STAB_COUNTER_START;
				FECounter = 0;

				// Enable MC_H
				ZbGPIO_SwitchH(TRUE);
				// Enable MC_L
				ZbGPIO_SwitchLbyPWM(TRUE);
				// Disable STOP_G
				ZbGPIO_SwitchStop(FALSE);

				Active = TRUE;
				CONTROL_SwitchRTCycle(TRUE);
				State = GATE_STATE_WAIT_STAB;
			}
			break;
		case GATE_STATE_WAIT_STAB:
			{
				if(!(--StabCounter))
				{
					// Enable TZ_IM and TZ_IGT
					ZbGPIO_EnableProtectionIM(TRUE);
					ZbGPIO_EnableProtectionIGT(TRUE);

					State = GATE_STATE_PROCESS;
				}
			}
			break;
		case GATE_STATE_PROCESS:
			{
				Int16U i;

				for(i = GATE_HISTORY_LENGTH; i > 0; --i)
				{
					HistoryI[i - 1] = (i > 1) ? HistoryI[i - 2] : NowI;
					HistoryV[i - 1] = (i > 1) ? HistoryV[i - 2] : NowV;
				}

				MU_LogScope(NowI, NowV, TargetI, _IQint(ControlSignal));

				if(TripIMDetected)
				{
					MU_SeekScope(1, -(DataTable[REG_GATE_HISTORY_OFFSET]));
					MU_SeekScope(2, -(DataTable[REG_GATE_HISTORY_OFFSET]));

					if(PureVGT)
					{
						TargetI = HistoryI[DataTable[REG_GATE_HISTORY_OFFSET]];

						SamplingWaitCounter = 0;
						State = GATE_STATE_SAMPLING;
					}
					else
					{
						DataTable[REG_RESULT_IGT] = _IQint(HistoryI[DataTable[REG_GATE_HISTORY_OFFSET]]);
						DataTable[REG_RESULT_VGT] = _IQint(HistoryV[DataTable[REG_GATE_HISTORY_OFFSET]]);

						State = GATE_STATE_FINISH;
					}
				}
				else if(TripIGTDetected)
				{
					InternalProblem = PROBLEM_GATE_IGT_OVERLOAD;
					State = GATE_STATE_FINISH;
				}
				else if(TargetI >= EndCurrent)
				{
					InternalProblem = PROBLEM_GATE_CURRENT_HIGH;
					State = GATE_STATE_FINISH;
				}
				else
				{
					if(ErrorI > FollowErrThreshold)
					{
						FECounter++;

						if(FECounter >= FECounterMax)
						{
							InternalProblem = PROBLEM_GATE_FLW_ERROR;
							State = GATE_STATE_FINISH;
						}
					}
					else
						FECounter = 0;

					TargetI += CurrentRate;
				}
			}
			break;
		case GATE_STATE_SAMPLING:
			{
				Int16U i;

				for(i = GATE_HISTORY_LENGTH; i > 0; --i)
				{
					HistoryI[i - 1] = (i > 1) ? HistoryI[i - 2] : NowI;
					HistoryV[i - 1] = (i > 1) ? HistoryV[i - 2] : NowV;
				}

				MU_LogScope(NowI, NowV, TargetI, _IQint(ControlSignal));

				if(GATE_SAMPLING_WAIT == SamplingWaitCounter++)
				{
					Int32S sumI = 0, sumV = 0;

					for(i = 0; i < GATE_HISTORY_LENGTH; ++i)
					{
						sumI += _IQint(HistoryI[i]);
						sumV += _IQint(HistoryV[i]);
					}

					DataTable[REG_RESULT_IGT] = (sumI / GATE_HISTORY_LENGTH);
					DataTable[REG_RESULT_VGT] = (sumV / GATE_HISTORY_LENGTH);
					State = GATE_STATE_FINISH;
				}
			}
			break;
		case GATE_STATE_FINISH:
			{
				// Disable TZ_IM and TZ_IGT
				ZbGPIO_EnableProtectionIM(FALSE);
				ZbGPIO_EnableProtectionIGT(FALSE);

				Active = FALSE;
				ZwADC_SubscribeToResults1(NULL);
				DELAY_US(SHDN_DELAY_US);

				ZbGPIO_SwitchH(FALSE);
				ZbGPIO_SwitchLbyPWM(FALSE);
				ZbGPIO_SwitchStop(FALSE);

				*FaultReason = InternalFaultReason;
				*Problem = InternalProblem;
				return TRUE;
			}
	}

	return FALSE;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(GATE_NotifyTripIM, "ramfuncs");
#endif
void GATE_NotifyTripIM()
{
	// Close low key
	ZbGPIO_SwitchLbyPWM(FALSE);

	TripIMDetected = TRUE;
	DELAY_US(TRIP_DELAY_US);

	ZbGPIO_EnableProtectionIM(FALSE);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(GATE_NotifyTripIGT, "ramfuncs");
#endif
void GATE_NotifyTripIGT()
{
	ZbGPIO_SwitchStop(TRUE);

	TripIGTDetected = TRUE;
	DELAY_US(TRIP_DELAY_US);

	ZbGPIO_EnableProtectionIGT(FALSE);
}
// ----------------------------------------

static void GATE_CacheVariables()
{
	// Init state
	CurrentRate = _FPtoIQ2(DataTable[REG_GATE_CURRENT_RATE_N], DataTable[REG_GATE_CURRENT_RATE_D]);
	FineVGT = _FPtoIQ2(DataTable[REG_GATE_FINE_VGT_N], DataTable[REG_GATE_FINE_VGT_D]);
	FineIGT = _FPtoIQ2(DataTable[REG_GATE_FINE_IGT_N], DataTable[REG_GATE_FINE_IGT_D]);
	Ki = _FPtoIQ2(DataTable[REG_GATE_Ki_N], DataTable[REG_GATE_Ki_D]);
	EndCurrent = _IQI(DataTable[REG_GATE_END_CURRENT]);
	FollowErrThreshold = _IQI(DataTable[REG_GATE_FE_THRESHOLD]);
	PureVGT = DataTable[REG_GATE_VGT_PURE] ? TRUE : FALSE;
	StartCurrent = _IQI(DataTable[REG_GATE_START_CURRENT]);
	FECounterMax = DataTable[REG_GATE_FE_TIME];
	OffsetIGT = _IQI((Int16S)DataTable[REG_GATE_IGT_OFFSET]);
	OffsetVGT = _IQI((Int16S)DataTable[REG_GATE_VGT_OFFSET]);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(GATE_SetProbeResult, "ramfuncs");
#endif
static void GATE_SetProbeResult(pInt16U Samples)
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
	#pragma CODE_SECTION(GATE_Cycle, "ramfuncs");
#endif
static void GATE_Cycle()
{
	if(Active)
	{
		ErrorI = TargetI - NowI;

		// Discreet PI-controller
		ControlSignal = _IQsat(ControlSignal + _IQmpy(ErrorI, Ki), _IQ(4095.0f), 0);

		// Write signal
		if(!TripIGTDetected)
			ZbGUDAC_WriteA(_IQint(ControlSignal));
		else
			ZbGUDAC_WriteA(0);

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
