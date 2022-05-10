// -----------------------------------------
// Logic of latching current testing process
// ----------------------------------------

// Header
#include "Latching.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Controller.h"
#include "IQmathLib.h"
#include "IQmathUtils.h"


// Constants
//
#define SAMPLE_FILTER_LENGTH		4
#define SAMPLE_FILTER_2ORDER		2


// Types
//
typedef enum __LatchingState
{
	LATCHING_STATE_START = 0,
	LATCHING_STATE_PROBE,
	LATCHING_STATE_FINISH
} LatchingState;
//
typedef enum __LatchProbingState
{
	LP_NONE			 = 0,
	LP_FIRE			 = 1,
	LP_FETCH_CURRENT = 2,
	LP_CLOSE_DELAY	 = 3,
	LP_PROBE_START	 = 4,
	LP_PROBE		 = 5,
	LP_BACKWARD		 = 6
} LatchProbingState;


// Variables
//
static LatchingState State;
static LatchProbingState StepStage;
static Boolean Forward;
static Int16U InternalFaultReason, InternalProblem;
static Int16S SignalLargeStep, SignalSmallStep, SignalStart, SignalEnd;
static Int16S SamplesI[SAMPLE_FILTER_LENGTH - 1];
static _iq LatchingI, CloseThreshold, EndCurrent, FineIL, OffsetIL;
//
static volatile Int16S ControlSignal;
static volatile _iq NowI;


// Forward functions
//
static void LATCHING_CacheVariables();
static void LATCHING_SetProbeResult(pInt16U Samples);
static void LATCHING_HandleStateProbe();


// Functions
//
void LATCHING_Prepare()
{
	const Int16U ADC_Channels[16] = { AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL,
									  AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL, AIN_IH_IL };

	// Save data from data table to local variable
	LATCHING_CacheVariables();

	// Configure ADC
	ZwADC_ConfigureSequentialCascaded(sizeof ADC_Channels / sizeof ADC_Channels[0], ADC_Channels);
	ZwADC_SubscribeToResults1(&LATCHING_SetProbeResult);

	DataTable[REG_RESULT_IL] = 0;
	MemZero16((pInt16U)SamplesI, SAMPLE_FILTER_LENGTH - 1);

	State = LATCHING_STATE_START;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(LATCHING_Process, "ramfuncs");
#endif
Boolean LATCHING_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning)
{
	*FaultReason = FAULT_NONE;
	*Problem = PROBLEM_NONE;
	*Warning = WARNING_NONE;

	switch(State)
	{
		case LATCHING_STATE_START:
			{
				ControlSignal = SignalStart;
				InternalFaultReason = FAULT_NONE;
				InternalProblem = PROBLEM_NONE;
				StepStage = LP_FIRE;
				Forward = TRUE;

				ZbGPIO_SwitchH(TRUE);

				State = LATCHING_STATE_PROBE;
			}
			break;
		case LATCHING_STATE_PROBE:
			LATCHING_HandleStateProbe();
			break;
		case LATCHING_STATE_FINISH:
			{
				ZwADC_SubscribeToResults1(NULL);
				ZbPUDAC_WriteA(0);
				ZbGPIO_SwitchH(FALSE);
				ZbGPIO_SwitchExternalDriver(FALSE);

				*FaultReason = InternalFaultReason;
				*Problem = InternalProblem;

				return TRUE;
			}
	}

	return FALSE;
}
// ----------------------------------------

static void LATCHING_CacheVariables()
{
	SignalLargeStep = DataTable[REG_LATCH_SIGNAL_LARGE_STEP];
	SignalSmallStep = DataTable[REG_LATCH_SIGNAL_SMALL_STEP];
	SignalStart = DataTable[REG_LATCH_SIGNAL_START];
	SignalEnd = DataTable[REG_LATCH_SIGNAL_END];
	CloseThreshold = _IQI(DataTable[REG_LATCH_CLOSE_THRESHOLD]);
	EndCurrent = _IQI(DataTable[REG_LATCH_END_CURRENT]);
	FineIL = _IQdiv(_IQI(DataTable[REG_HOLD_FINE_IHL_N]), _IQI(DataTable[REG_HOLD_FINE_IHL_D]));
	OffsetIL = _IQint((Int16S)DataTable[REG_HOLD_IHL_OFFSET]);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(LATCHING_HandleStateProbe, "ramfuncs");
#endif
static void LATCHING_HandleStateProbe()
{
	switch(StepStage)
	{
		case LP_FIRE:
			{
				ZbPUDAC_WriteA(ControlSignal);
				ZbGPIO_SwitchExternalDriver(TRUE);
				DELAY_US(LATCH_FIRING_DELAY);
				ZwADC_StartSEQ1();
			}
			break;
		case LP_FETCH_CURRENT:
			{
				ZbGPIO_SwitchExternalDriver(FALSE);
				LatchingI = NowI;

				if(LatchingI >= EndCurrent)
				{
					InternalProblem = PROBLEM_LATCH_CURRENT_HIGH;
					State = LATCHING_STATE_FINISH;
				}
			}
			break;
		case LP_CLOSE_DELAY:
			break;
		case LP_PROBE_START:
			{
				ZwADC_StartSEQ1();
			}
			break;
		case LP_PROBE:
			{
				if(Forward)
				{
					if(NowI > CloseThreshold)
					{
						ZbPUDAC_WriteA(0);
						Forward = FALSE;
					}
					else
					{
						ControlSignal += SignalLargeStep;

						if(ControlSignal >= SignalEnd)
						{
							InternalProblem = PROBLEM_LATCH_FLW_ERROR;
							State = LATCHING_STATE_FINISH;
						}
						else
							StepStage = LP_NONE;
					}
				}
				else
				{
					if(NowI <= CloseThreshold)
					{
						DataTable[REG_RESULT_IL] = _IQint(LatchingI);
						State = LATCHING_STATE_FINISH;
					}

					ZbPUDAC_WriteA(0);
				}
			}
			break;
		case LP_BACKWARD:
			{
				ControlSignal -= SignalSmallStep;

				if(ControlSignal <= SignalStart)
				{
					InternalProblem = PROBLEM_LATCH_PROCESS_ERROR;
					State = LATCHING_STATE_FINISH;
				}
				else
					StepStage = LP_NONE;
			}
			break;
	}

	++StepStage;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(LATCHING_SetProbeResult, "ramfuncs");
#endif
static void LATCHING_SetProbeResult(pInt16U Samples)
{
	Int16U j;
	Int32S NewI = 0;

	#pragma UNROLL(16)
	for(j = 0; j < 16; ++j)
		NewI += Samples[j];

	NowI = _IQmpyI32(LATCH_CONVERT_V_C, NewI >> 4);
	NowI = _IQmpy(NowI, FineIL) + OffsetIL;

	NowI = (NowI > 0) ? NowI : 0;
}
// ----------------------------------------

// No more
