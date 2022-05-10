// -----------------------------------------
// Controller of Kelvin block
// ----------------------------------------

// Header
#include "Kelvin.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "IQmathLib.h"


// Types
//
typedef struct __BridgeState
{
	Int16U High, Low;
} BridgeState;
//
typedef enum __KelvinState
{
	KELVIN_STATE_PREPARE = 0,
	KELVIN_STATE_PROCESS = 1
} KelvinState;


// Constants
//
#define KELVIN_PROBE_COUNT					4
#define KELVIN_BRIDGE_ARMS					4
//
#define KELVIN_PROBE_OVERSAMPLE				16
#define KELVIN_PROBE_OVERSAMPLE_2OREDER		4
//
#ifdef BOOT_FROM_FLASH
	#pragma DATA_SECTION(BridgeContacts, "ramconsts");
	#pragma DATA_SECTION(BridgeHigh, "ramconsts");
	#pragma DATA_SECTION(BridgeLow, "ramconsts");
	#pragma DATA_SECTION(ResultsPattern, "ramconsts");
#endif
// Tests: CC<->CP = 1, AP -> CP = 0, CP -> AP = 0, AC -> CC = 1
const BridgeState BridgeContacts[KELVIN_PROBE_COUNT + 1] = {{1, 2}, {4, 1}, {1, 4}, {3, 2}, {0, 0}};
// CP = K3, CC = K1, AC = K2, AP = K4
const Int16U BridgeHigh[KELVIN_BRIDGE_ARMS] = {0x08u, 0x80u, 0x20u, 0x02u};
const Int16U BridgeLow[KELVIN_BRIDGE_ARMS] = {0x10u, 0x01u, 0x40u, 0x04u};
// Pattern of correct results
const Boolean ResultsPattern[KELVIN_PROBE_COUNT] = {TRUE, FALSE, FALSE, TRUE};


// Variables
//
static Int16U InternalProbeCounter, ProbeIndex;
static KelvinState State;
static volatile Int16U LastProbeResult;
static Boolean UseFullTest, Results[KELVIN_PROBE_COUNT];


// Forward functions
//
static void KELVIN_SwitchBridge(BridgeState State);
static void KELVIN_SetProbeResult(pInt16U Result);


// Functions
//
void KELVIN_Prepare(Boolean FullTestMode)
{
	const Int16U KelvinADC[16] = { AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF,
								   AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF, AIN_VRF };
	UseFullTest = FullTestMode;
	ZwADC_ConfigureSequentialCascaded(sizeof KelvinADC / sizeof KelvinADC[0], KelvinADC);
	ZwADC_SubscribeToResults1(&KELVIN_SetProbeResult);
	
	State = KELVIN_STATE_PREPARE;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(KELVIN_Process, "ramfuncs");
#endif
Boolean KELVIN_Process(pInt16U FaultReason, pInt16U Problem, pInt16U Warning)
{
	*FaultReason = FAULT_NONE;
	*Problem = PROBLEM_NONE;
	*Warning = WARNING_NONE;

	switch(State)
	{
		case KELVIN_STATE_PREPARE:
			InternalProbeCounter = 0;
			ProbeIndex = 0;
			KELVIN_SwitchBridge(BridgeContacts[ProbeIndex]);
			State = KELVIN_STATE_PROCESS;
			break;
		case KELVIN_STATE_PROCESS:
			{
				++InternalProbeCounter;

				if(InternalProbeCounter == DataTable[REG_KELVIN_PROBE_TIME])
				{
					InternalProbeCounter = 0;
					Results[ProbeIndex] = (LastProbeResult > DataTable[REG_KELVIN_PROBE_TH]);
					DataTable[REG_KELVIN_D1 + ProbeIndex] = LastProbeResult;

					KELVIN_SwitchBridge(BridgeContacts[++ProbeIndex]);
					LastProbeResult = 0;
				}
				else if(InternalProbeCounter == (DataTable[REG_KELVIN_PROBE_TIME] - 1))
					ZwADC_StartSEQ1();
			}
			break;
	}

	if(ProbeIndex == KELVIN_PROBE_COUNT)
	{
		if (UseFullTest)
		{
			Int16U i;
			Boolean total = TRUE;

			for(i = 0; i < KELVIN_PROBE_COUNT; ++i)
			{
				total = total && (Results[i] == ResultsPattern[i]);
				DataTable[REG_KELVIN_1_2 + i] = Results[i] ? 1 : 0;
			}

			DataTable[REG_RESULT_KELVIN] = total ? 1 : 0;
		}
		else
		{
			Int16U i;

			for(i = 0; i < KELVIN_PROBE_COUNT; ++i)
				DataTable[REG_KELVIN_1_2 + i] = Results[i] ? 1 : 0;

			if (Results[1] || Results[2])
			{
				DataTable[REG_RESULT_KELVIN] = 0;
				*Problem = PROBLEM_KELVIN_CC_SHORT;
			}
			else
				DataTable[REG_RESULT_KELVIN] = 1;
		}

		return TRUE;
	}
	
	return FALSE;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(KELVIN_SetProbeResult, "ramfuncs");
#endif
static void KELVIN_SetProbeResult(pInt16U Result)
{
	Int16U i;
	Int32U sum = 0;
	
	for(i = 0; i < KELVIN_PROBE_OVERSAMPLE; ++i)
		sum += *(Result++);
		
	LastProbeResult = sum >> KELVIN_PROBE_OVERSAMPLE_2OREDER;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(KELVIN_SwitchBridge, "ramfuncs");
#endif
static void KELVIN_SwitchBridge(BridgeState State)
{
	Int16U Bridge = VR_DEFAULT_VALUE;

	if((State.Low > 0) && (State.Low <= KELVIN_BRIDGE_ARMS))
		Bridge |= BridgeLow[State.Low - 1];	
	if((State.High > 0) && (State.High <= KELVIN_BRIDGE_ARMS))
		Bridge |= BridgeHigh[State.High - 1];
	
	ZbVRegister_WriteRegister(Bridge);
}

// No more
