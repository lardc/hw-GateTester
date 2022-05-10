// -----------------------------------------
// Controller logic
// ----------------------------------------

// Header
#include "Controller.h"
//
// Includes
#include "SysConfig.h"
//
#include "ZbBoard.h"
#include "DeviceObjectDictionary.h"
#include "DataTable.h"
#include "SCCISlave.h"
#include "DeviceProfile.h"
#include "Kelvin.h"
#include "Gate.h"
#include "Holding.h"
#include "Latching.h"
#include "RGate.h"
#include "CalibrateGate.h"
#include "CalibrateHolding.h"
#include "IQmathLib.h"

	
// Variables
//
static CONTROL_FUNC_RealTimeRoutine RealTimeRoutine = NULL;
//
volatile Int64U CONTROL_TimeCounter = 0;
volatile DeviceState CONTROL_State = DS_None;
static volatile Boolean CycleActive = FALSE;
//
#pragma DATA_SECTION(CONTROL_Values_1, "data_mem");
Int16U CONTROL_Values_1[VALUES_x_SIZE];
#pragma DATA_SECTION(CONTROL_Values_2, "data_mem");
Int16U CONTROL_Values_2[VALUES_x_SIZE];
volatile Int16U CONTROL_Values_1_Counter = 0, CONTROL_Values_2_Counter = 0;
//
// Boot-loader flag
#pragma DATA_SECTION(CONTROL_BootLoaderRequest, "bl_flag");
volatile Int16U CONTROL_BootLoaderRequest = 0;


// Forward functions
//
static void CONTROL_SetDeviceState(DeviceState NewState);
static void CONTROL_FillWPPartDefault();
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U UserError);
static void CONTROL_InitProtection();
static void CONTROL_HaltExecution();


// Functions
//
void CONTROL_Init(Boolean BadClockDetected)
{
	// Variables for endpoint configuration
	Int16U EPIndexes[EP_COUNT] = {EP16_Data_I, EP16_Data_V};
	Int16U EPSized[EP_COUNT] = {VALUES_x_SIZE, VALUES_x_SIZE};
	pInt16U EPCounters[EP_COUNT] = {(pInt16U)&CONTROL_Values_1_Counter, (pInt16U)&CONTROL_Values_2_Counter};
	pInt16U EPDatas[EP_COUNT] = {CONTROL_Values_1, CONTROL_Values_2};
	// Data-table EPROM service configuration
	EPROMServiceConfig EPROMService = {&ZbSRAM_WriteValuesEPROM, &ZbSRAM_ReadValuesEPROM};

	// Init data table
	DT_Init(EPROMService, BadClockDetected);
	// Fill state variables with default values
	CONTROL_FillWPPartDefault();

	// Device profile initialization
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitEPService(EPIndexes, EPSized, EPCounters, EPDatas);
	// Reset control values
	DEVPROFILE_ResetControlSection();

	if(!BadClockDetected)
	{
		if(DataTable[REG_GLOBAL_CAL_ADC])
		{
			// Calibrate ADC
			ZwADC_CalibrateLO(AIN_LO);
		}

		if(ZwSystem_GetDogAlarmFlag())
		{
			DataTable[REG_WARNING] = WARNING_WATCHDOG_RESET;
			ZwSystem_ClearDogAlarmFlag();
		}

		CONTROL_InitProtection();
	}
	else
	{
		CycleActive = TRUE;
		DataTable[REG_DISABLE_REASON] = DISABLE_BAD_CLOCK;
		CONTROL_SetDeviceState(DS_Disabled);
	}
}
// ----------------------------------------

void CONTROL_SelfExecutedTest()
{
	// First fake-self-drucken test
	// eliminate zeroes at first test execution
	CONTROL_SetDeviceState(DS_Gate);
	CycleActive = TRUE;
	ZbIOE_WritePin(IDX_LED_2, TRUE);
	GATE_Prepare();
}
// ----------------------------------------

void CONTROL_Idle()
{
	DEVPROFILE_ProcessRequests();

	DEVPROFILE_UpdateCANDiagStatus();
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CONTROL_UpdateHigh, "ramfuncs");
#endif
void CONTROL_UpdateHigh()
{
	if(CycleActive)
	{
		Boolean completed = FALSE;
		Int16U faultReason = FAULT_NONE;
		Int16U problem = PROBLEM_NONE;
		Int16U warning = WARNING_NONE;

		switch(CONTROL_State)
		{
			case DS_Kelvin:
				completed = KELVIN_Process(&faultReason, &problem, &warning);
				break;		
			case DS_Gate:
				completed = GATE_Process(&faultReason, &problem, &warning);
				break;
			case DS_IH:
				completed = HOLDING_Process(&faultReason, &problem, &warning);
				break;
			case DS_IL:
				completed = LATCHING_Process(&faultReason, &problem, &warning);
				break;
			case DS_RG:
				completed = RGATE_Process(&faultReason, &problem, &warning);
				break;
			case DS_CalGate:
				completed = CALGATE_Process(&faultReason, &problem, &warning);
				break;
			case DS_CalHolding:
				completed = CALHOLD_Process(&faultReason, &problem, &warning);
				break;
		}

		if(completed)
		{
			DEVPROFILE_ResetEPReadState();

			if(faultReason != FAULT_NONE)
			{
				DataTable[REG_FAULT_REASON] = faultReason;
				CONTROL_SetDeviceState(DS_Fault);
			}
			else
				CONTROL_SetDeviceState(DS_None);

			CycleActive = FALSE;
			DataTable[REG_PROBLEM] = problem;
			DataTable[REG_WARNING] = warning;
			DataTable[REG_TEST_FINISHED] = (problem == PROBLEM_NONE) ? OPRESULT_OK : OPRESULT_FAIL;

			ZbIOE_WritePin(IDX_LED_2, FALSE);
		}	
	}				
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CONTROL_RealTimeCycle, "ramfuncs");
#endif
void CONTROL_RealTimeCycle()
{
	if(RealTimeRoutine)
		RealTimeRoutine();
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CONTROL_SwitchRTCycle, "ramfuncs");
#endif
void CONTROL_SwitchRTCycle(Boolean Enable)
{
	if(Enable)
		ZwTimer_StartT0();
	else
		ZwTimer_StopT0();
}
// ----------------------------------------

void CONTROL_SubcribeToCycle(CONTROL_FUNC_RealTimeRoutine Routine)
{
	RealTimeRoutine = Routine;
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CONTROL_NotifyTripIGT, "ramfuncs");
#endif
void CONTROL_NotifyTripIGT()
{
	switch(CONTROL_State)
	{
		case DS_Gate:
			GATE_NotifyTripIGT();
			break;
		case DS_RG:
			RGATE_NotifyTripIGT();
			break;
		default:
			ZbGPIO_EnableProtectionIGT(FALSE);
			break;
	}
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(CONTROL_NotifyCANFault, "ramfuncs");
#endif
void CONTROL_NotifyCANFault(ZwCAN_SysFlags Flag)
{
	DEVPROFILE_NotifyCANFault(Flag);
}
// ----------------------------------------

static void CONTROL_SetDeviceState(DeviceState NewState)
{
	// Set new state
	CONTROL_State = NewState;
	DataTable[REG_DEV_STATE] = NewState;
}
// ----------------------------------------

static void CONTROL_FillWPPartDefault()
{
	Int16U i;

	// Set states
	DataTable[REG_TEST_FINISHED] = OPRESULT_OK;
	DataTable[REG_DEV_STATE] = DS_None;
	DataTable[REG_FAULT_REASON] = FAULT_NONE;
	DataTable[REG_DISABLE_REASON] = DISABLE_NONE;
	DataTable[REG_WARNING] = WARNING_NONE;

	// Set results to zero
	for(i = REG_RESULT_KELVIN; i <= REG_KELVIN_4_1; ++i)
		DataTable[i] = 0;
}
// ----------------------------------------

static void CONTROL_HaltExecution()
{
	CONTROL_SwitchRTCycle(FALSE);
	ZwADC_SubscribeToResults1(NULL);
	//
	ZbGPIO_EnableProtectionIM(FALSE);
	ZbGPIO_EnableProtectionIGT(FALSE);
	ZbGPIO_SwitchH(FALSE);
	ZbGPIO_SwitchLbyPWM(FALSE);
	ZbGPIO_SwitchStop(FALSE);
	//
	ZbGUDAC_WriteA(0);
	ZbPUDAC_WriteA(0);
}
// ----------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U UserError)
{
	switch(ActionID)
	{
		case ACT_START_KELVIN:
			if(CONTROL_State == DS_None)
			{
				DataTable[REG_TEST_FINISHED] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_Kelvin);
				DEVPROFILE_ResetScopes(0);
				KELVIN_Prepare(TRUE);
				CycleActive = TRUE;
				ZbIOE_WritePin(IDX_LED_2, TRUE);
			}
			else
				*UserError = ERR_OPERATION_BLOCKED;
			break;
		case ACT_START_GATE:
			if(CONTROL_State == DS_None)
			{
				DataTable[REG_TEST_FINISHED] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_Gate);
				DEVPROFILE_ResetScopes(0);
				GATE_Prepare();
				CycleActive = TRUE;
				ZbIOE_WritePin(IDX_LED_2, TRUE);
			}
			else
				*UserError = ERR_OPERATION_BLOCKED;
			break;
		case ACT_START_IH:
			if(CONTROL_State == DS_None)
			{
				DataTable[REG_TEST_FINISHED] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_IH);
				DEVPROFILE_ResetScopes(0);
				HOLDING_Prepare();
				CycleActive = TRUE;
				ZbIOE_WritePin(IDX_LED_2, TRUE);
			}
			else
				*UserError = ERR_OPERATION_BLOCKED;
			break;
		case ACT_START_IL:
			if(CONTROL_State == DS_None)
			{
				DataTable[REG_TEST_FINISHED] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_IL);
				DEVPROFILE_ResetScopes(0);
				LATCHING_Prepare();
				CycleActive = TRUE;
				ZbIOE_WritePin(IDX_LED_2, TRUE);
			}
			else
				*UserError = ERR_OPERATION_BLOCKED;
			break;
		case ACT_START_RG:
			if(CONTROL_State == DS_None)
			{
				DataTable[REG_TEST_FINISHED] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_RG);
				DEVPROFILE_ResetScopes(0);
				RGATE_Prepare();
				CycleActive = TRUE;
				ZbIOE_WritePin(IDX_LED_2, TRUE);
			}
			else
				*UserError = ERR_OPERATION_BLOCKED;
			break;
		case ACT_START_KELVIN_POWER:
			if(CONTROL_State == DS_None)
			{
				DataTable[REG_TEST_FINISHED] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_Kelvin);
				DEVPROFILE_ResetScopes(0);
				KELVIN_Prepare(FALSE);
				CycleActive = TRUE;
				ZbIOE_WritePin(IDX_LED_2, TRUE);
			}
			else
				*UserError = ERR_OPERATION_BLOCKED;
			break;
		case ACT_STOP_TEST:
			if (CONTROL_State != DS_None && CONTROL_State != DS_Fault)
			{
				CONTROL_HaltExecution();
				CONTROL_FillWPPartDefault();
				DataTable[OPRESULT_FAIL] = OPRESULT_FAIL;
				DataTable[REG_PROBLEM] = PROBLEM_OPERATION_STOPPED;
			}
			break;
		case ACT_START_CAL_GATE:
			if(CONTROL_State == DS_None)
			{
				DataTable[REG_TEST_FINISHED] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_CalGate);
				DEVPROFILE_ResetScopes(0);
				CALGATE_Prepare();
				CycleActive = TRUE;
				ZbIOE_WritePin(IDX_LED_2, TRUE);
			}
			else
				*UserError = ERR_OPERATION_BLOCKED;
			break;
		case ACT_START_CAL_HOLDING:
			if(CONTROL_State == DS_None)
			{
				DataTable[REG_TEST_FINISHED] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_CalHolding);
				DEVPROFILE_ResetScopes(0);
				CALHOLD_Prepare();
				CycleActive = TRUE;
				ZbIOE_WritePin(IDX_LED_2, TRUE);
			}
			else
				*UserError = ERR_OPERATION_BLOCKED;
			break;
		case ACT_CLR_FAULT:
			if(CONTROL_State == DS_Fault)
				CONTROL_SetDeviceState(DS_None);

			DataTable[REG_FAULT_REASON] = FAULT_NONE;
			break;
		case ACT_CLR_WARNING:
			DataTable[REG_WARNING] = WARNING_NONE;
			break;
		case ACT_DBG_EXT_DRIVER:
			if(CONTROL_State == DS_None)
			{
				ZbGPIO_SwitchExternalDriver(TRUE);
				DELAY_US(5000);
				ZbGPIO_SwitchExternalDriver(FALSE);
			}
			break;
		default:
			return FALSE;
	}

	return TRUE;
}
// ----------------------------------------

static void CONTROL_InitProtection()
{
	// Set protection level for MC
	ZbPDAC_WriteA(_IQmpyI32int(GATE_CONVERT_C_D_IM, DataTable[REG_GATE_CURR_MC_THRESHOLD]));
	// Set protection level for IGT
	ZbPDAC_WriteB(GATE_TH_C_D_IGT);

	// Drucken TZ system
	ZbGPIO_EnableProtectionIM(TRUE);
	ZbGPIO_EnableProtectionIGT(TRUE);

	DELAY_US(5000);

	ZbGPIO_EnableProtectionIGT(FALSE);
	ZbGPIO_EnableProtectionIM(FALSE);
}

// No more.
