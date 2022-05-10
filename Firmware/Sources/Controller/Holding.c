// -----------------------------------------
// Controller of Holding current testing
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
#define SAMPLE_FILTER_LENGTH		4
#define SAMPLE_FILTER_2ORDER		2
#define FREEZE_DELAY_US				(TIMER0_PERIOD * 2)


// Types
//
typedef enum __HoldingState
{
	HOLDING_STATE_START = 0,
	HOLDING_STATE_RISE,
	HOLDING_STATE_STRIKE,
	HOLDING_STATE_STRIKE_AP,
	HOLDING_STATE_WAIT_STAB,
	HOLDING_STATE_PROCESS,
	HOLDING_STATE_FINISH
} HoldingState;


// Variables
//
static HoldingState State;
static Boolean UseStrike;
static _iq CurrentRate, Ki, EndCurrent, CloseDelta, CloseRate, FineIH, OffsetIH;
static Int16U InternalFaultReason, InternalWarning, InternalProblem, StabCounter, StrikePulseLengthUS;
static Int16S SamplesI[SAMPLE_FILTER_LENGTH - 1];
//
static volatile Boolean Active, FreezeControl, CombinedWithSL;
static volatile _iq ControlSignal, TargetI, NowI, NowIM1, NowIM2;


// Forward functions
//
static void HOLDING_CacheVariables();
static void HOLDING_SetProbeResult(pInt16U Samples);
static void HOLDING_Cycle();


// Functions
//
void HOLDING_Prepare()
{
	const Int16U ADC_Channels[16] = { AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL,
									  AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL };
	
	// Save data from data table to local variables
	HOLDING_CacheVariables();

	// Configure ADC
	ZwADC_ConfigureSequentialCascaded(sizeof ADC_Channels / sizeof ADC_Channels[0], ADC_Channels);
	ZwADC_SubscribeToResults1(&HOLDING_SetProbeResult);
	
	// Adjust main controller
	CONTROL_SubcribeToCycle(&HOLDING_Cycle);

	DataTable[REG_RESULT_IH] = 0;
	MemZero16((pInt16U)SamplesI, SAMPLE_FILTER_LENGTH - 1);

	State = HOLDING_STATE_START;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(HOLDING_Process, "ramfuncs");
#endif
Boolean HOLDING_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning)
{
	static Int64U Timeout = 0;

	*FaultReason = FAULT_NONE;
	*Problem = PROBLEM_NONE;
	*Warning = WARNING_NONE;

	switch(State)
	{
		case HOLDING_STATE_START:
			{
				TargetI = _IQI(DataTable[REG_HOLD_START_CURRENT]);
				ControlSignal = _IQI(DataTable[REG_HOLD_CONTROL_OFFSET]);
				InternalFaultReason = FAULT_NONE;
				InternalProblem = PROBLEM_NONE;
				InternalWarning = WARNING_NONE;
				StabCounter = HOLD_STAB_COUNTER_START;
				NowIM1 = NowIM2 = 0;

				ZbGPIO_SwitchH(TRUE);
				ZbGPIO_SwitchExternalDriver(!CombinedWithSL);

				Active = TRUE;
				FreezeControl = FALSE;
				CONTROL_SwitchRTCycle(TRUE);

				Timeout = CONTROL_TimeCounter + DataTable[REG_HOLD_WAIT_TIMEOUT];
				State = HOLDING_STATE_RISE;
			}
			break;
		case HOLDING_STATE_RISE:
			{
				if(_IQabs(NowI - TargetI) < HOLD_START_CURRENT_TOLERANCE)
				{
					ZbGPIO_SwitchExternalDriver(FALSE);
					State = (UseStrike && !CombinedWithSL) ? HOLDING_STATE_STRIKE : HOLDING_STATE_WAIT_STAB;
				}
				else if(CONTROL_TimeCounter > Timeout)
				{
					InternalProblem = PROBLEM_HOLD_REACH_TIMEOUT;
					State = HOLDING_STATE_FINISH;
				}

				MU_LogScope(NowI, 0, TargetI, _IQint(ControlSignal));
			}
			break;
		case HOLDING_STATE_STRIKE:
			{
				FreezeControl = TRUE;
				DELAY_US(FREEZE_DELAY_US);

				ZbGPIO_SwitchStrike(TRUE);
				DELAY_US(StrikePulseLengthUS / 2);
				ZbGPIO_SwitchExternalDriver(TRUE);
				DELAY_US(StrikePulseLengthUS / 2);
				ZbGPIO_SwitchStrike(FALSE);

				State = HOLDING_STATE_STRIKE_AP;
			}
			break;
		case HOLDING_STATE_STRIKE_AP:
			{
				MU_LogScope(NowI, 0, TargetI, _IQint(ControlSignal));

				if(!(--StabCounter))
				{
					StabCounter = HOLD_STAB_COUNTER_START;
					State = HOLDING_STATE_WAIT_STAB;

					ZbGPIO_SwitchExternalDriver(FALSE);
					FreezeControl = FALSE;
				}
			}
			break;
		case HOLDING_STATE_WAIT_STAB:
			{
				MU_LogScope(NowI, 0, TargetI, _IQint(ControlSignal));

				if(!(--StabCounter))
					State = HOLDING_STATE_PROCESS;
			}
			break;
		case HOLDING_STATE_PROCESS:
			{
				_iq dI, errI;

				dI = NowI - NowIM1;
				errI = TargetI - NowI;

				MU_LogScope(NowI, 0, TargetI, _IQint(ControlSignal));

				if((-dI > CloseRate) || (errI > CloseDelta))
				{
					DataTable[REG_RESULT_IH] = _IQint(_IQmpy((NowIM1 + NowIM2) >> 1, FineIH));
					State = HOLDING_STATE_FINISH;
				}
				else if(NowI <= EndCurrent)
				{
					DataTable[REG_RESULT_IH] = _IQint(EndCurrent);
					InternalWarning = WARNING_HOLD_CURRENT_SMALL;
					State = HOLDING_STATE_FINISH;
				}

				TargetI -= CurrentRate;

				NowIM2 = NowIM1;
				NowIM1 = NowI;
			}
			break;
		case HOLDING_STATE_FINISH:
			{
				Active = FALSE;
				ZwADC_SubscribeToResults1(NULL);

				ZbGPIO_SwitchExternalDriver(FALSE);
				ZbGPIO_SwitchH(FALSE);

				*Warning = InternalWarning;
				*FaultReason = InternalFaultReason;
				*Problem = InternalProblem;
				return TRUE;
			}
	}	
	
	return FALSE;
}
// ----------------------------------------

static void HOLDING_CacheVariables()
{
	CurrentRate = _IQdiv(_IQI(DataTable[REG_HOLD_CURRENT_RATE_N]), _IQI(DataTable[REG_HOLD_CURRENT_RATE_D]));
	EndCurrent = _IQI(DataTable[REG_HOLD_END_CURRENT]);
	Ki = _IQdiv(_IQI(DataTable[REG_HOLD_Ki_N]), _IQI(DataTable[REG_HOLD_Ki_D]));
	CloseDelta = _IQI(DataTable[REG_HOLD_CLOSE_DELTA]);
	CloseRate = _IQmpy(CurrentRate, _IQdiv(_IQI(DataTable[REG_HOLD_CLOSE_RATE_COFF_N]), _IQI(DataTable[REG_HOLD_CLOSE_RATE_COFF_D])));
	UseStrike = DataTable[REG_HOLD_USE_STRIKE] ? TRUE : FALSE;
	StrikePulseLengthUS = DataTable[REG_HOLD_STRIKE_PULSE_LEN];
	FineIH = _IQdiv(_IQI(DataTable[REG_HOLD_FINE_IHL_N]), _IQI(DataTable[REG_HOLD_FINE_IHL_D]));
	OffsetIH = _IQI((Int16S)DataTable[REG_HOLD_IHL_OFFSET]);
	CombinedWithSL = DataTable[REG_HOLD_WITH_SL];
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(HOLDING_SetProbeResult, "ramfuncs");
#endif
static void HOLDING_SetProbeResult(pInt16U Samples)
{
	Int16U j, k;
	Int16U NewI = 0, FilteredI = 0;
	
	#pragma UNROLL(16)
	for(j = 0; j < 16; ++j)
		NewI += Samples[j];
	
	NewI >>= 4;

	for(k = SAMPLE_FILTER_LENGTH - 1; k > 0; --k)
	{
		FilteredI += SamplesI[k - 1];

		if(k > 1)
			SamplesI[k - 1] = SamplesI[k - 2];
		else
			SamplesI[0] = NewI;
	}
	
	NowI = _IQmpyI32(HOLD_CONVERT_V_C, (FilteredI + NewI) >> SAMPLE_FILTER_2ORDER);
	NowI = _IQmpy(NowI, FineIH) + OffsetIH;

	NowI = (NowI > 0) ? NowI : 0;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(HOLDING_Cycle, "ramfuncs");
#endif
static void HOLDING_Cycle()
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
