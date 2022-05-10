// -----------------------------------------
// Logic controller
// ----------------------------------------

#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// ACTIONS
//
#define ACT_CLR_FAULT				3	// Clear fault (try switch state from FAULT to NONE)
#define ACT_CLR_WARNING				4	// Clear warning
//
#define ACT_DBG_EXT_DRIVER			10	// Start gate driver pulse
//
#define ACT_START_KELVIN			100	// Start Kelvin measurement
#define ACT_START_GATE				101	// Start gate measurement
#define ACT_START_IH				102	// Start holding current measurement
#define ACT_START_IL				103	// Start latching current measurement
#define ACT_START_RG				104 // Start gate resistance test
#define ACT_STOP_TEST				105 // Stop current test
#define ACT_START_KELVIN_POWER		106 // Start Kelvin anode to cathode test
//
#define ACT_START_CAL_GATE			110 // Start gate circuit calibration
#define ACT_START_CAL_HOLDING		111	// Start main circuit calibration
//
#define ACT_SAVE_TO_ROM				200	// Save parameters to EEPROM module
#define ACT_RESTORE_FROM_ROM		201	// Restore parameters from EEPROM module
#define ACT_RESET_TO_DEFAULT		202	// Reset parameters to default values (only in controller memory)
#define ACT_LOCK_NV_AREA			203	// Lock modifications of parameters area
#define ACT_UNLOCK_NV_AREA			204	// Unlock modifications of parameters area (password-protected)
//
#define ACT_BOOT_LOADER_REQUEST		320	// Request reboot to bootloader

// REGISTERS
//
#define REG_GLOBAL_CAL_ADC			10	// Use ADC start-up calibration
//
#define REG_KELVIN_PROBE_TH			11	// Threshold for probing in ADC ticks
#define REG_KELVIN_PROBE_TIME		12	// Time for probing in ms
//
#define REG_HOLD_START_CURRENT		21	// Start current for holding measurement in mA
#define REG_HOLD_END_CURRENT		22	// Minimum available current for holding in mA
#define REG_HOLD_CURRENT_RATE_N		23	// Current rate mA/ms (N)
#define REG_HOLD_CURRENT_RATE_D		24	// Current rate mA/ms (D)
#define REG_HOLD_CONTROL_OFFSET		25	// Offset for control signal (V-dropout compensation)
#define REG_HOLD_Ki_N				26	// PID Ki (N)
#define REG_HOLD_Ki_D				27	// PID Ki (D)
#define REG_HOLD_CLOSE_RATE_COFF_N	28	// IH_Rate / CurrentRate = RateCoff (N)
#define REG_HOLD_CLOSE_RATE_COFF_D	29	// IH_Rate / CurrentRate = RateCoff (D)
#define REG_HOLD_CLOSE_DELTA		30	// Critical level of current error (Target - Actual) for close detection
#define REG_HOLD_WAIT_TIMEOUT		31	// Timeout for reaching desired start current in ms
#define REG_HOLD_STRIKE_PULSE_LEN	32	// Pulse length in us
#define REG_HOLD_FINE_IHL_N			33	// Fine coefficient for IH (N)
#define REG_HOLD_FINE_IHL_D			34  // Fine coefficient for IH (D)
#define REG_HOLD_IHL_OFFSET			35	// Offset correction for IH
//
#define REG_GATE_START_CURRENT		41	// Start current for test in mA
#define REG_GATE_END_CURRENT		42	// Maximum available current for ITG in mA
#define REG_GATE_CURRENT_RATE_N		43	// Current rate mA/ms (N)
#define REG_GATE_CURRENT_RATE_D		44	// Current rate mA/ms (D)
#define REG_GATE_Ki_N				45	// PID Ki (N)
#define REG_GATE_Ki_D				46	// PID Ki (D)
#define REG_GATE_CONTROL_OFFSET		47	// Offset for control signal (V-dropout compensation)
#define REG_GATE_FE_THRESHOLD		48	// Maximum following error in mA
#define REG_GATE_CURR_MC_THRESHOLD	49	// Trip current in main circuit in mA
#define REG_GATE_FINE_IGT_N			50	// Fine coefficient for IGT (N)
#define REG_GATE_FINE_IGT_D			51	// Fine coefficient for IGT (D)
#define REG_GATE_FINE_VGT_N			52	// Fine coefficient for VGT (N)
#define REG_GATE_FINE_VGT_D			53	// Fine coefficient for VGT (D)
#define REG_GATE_FE_TIME			54	// Time for detection following error in ms
#define REG_GATE_HISTORY_OFFSET		55	// Time offset in results history
#define REG_GATE_VGT_OFFSET			56	// Offset correction for VGT
#define REG_GATE_IGT_OFFSET			57	// Offset correction for IGT
#define REG_GATE_WAIT_TIMEOUT		58	// Timeout for reaching desired start current in ms
//
#define REG_LATCH_SIGNAL_LARGE_STEP	71	// Large step of increasing control signal in forward direction
#define REG_LATCH_SIGNAL_SMALL_STEP 72	// Small step of decreasing control signal in backward direction
#define REG_LATCH_SIGNAL_START		73	// Start value of control signal
#define REG_LATCH_SIGNAL_END		74	// Maximum value of control signal
#define REG_LATCH_CLOSE_THRESHOLD	75	// Critical current level to detect closing of thyristor in mA
#define REG_LATCH_END_CURRENT		76	// Maximum available current for IL in mA
//
#define REG_RG_CURRENT				93	// Test current
#define REG_RG_SAMPLE_COUNT			94	// Sample count
//
#define REG_SP__1					127
//
// ----------------------------------------
#define REG_GATE_VGT_PURE			128	// Sample VGT after terminating main current
#define REG_HOLD_USE_STRIKE			129	// Strike pulse before holding current
//
#define REG_CAL_CURRENT				140	// Calibration current
//
#define REG_SCOPE_TYPE_1			150	// Scope type 1
#define REG_SCOPE_TYPE_2			151	// Scope type 2
//
#define REG_PWD_1					180	// Unlock password location 1
#define REG_PWD_2					181	// Unlock password location 2
#define REG_PWD_3					182	// Unlock password location 3
#define REG_PWD_4					183	// Unlock password location 4
#define REG_SP__2					191
//
// ----------------------------------------
//
#define REG_DEV_STATE				192	// Device state
#define REG_FAULT_REASON			193	// Fault reason in the case DeviceState -> FAULT
#define REG_DISABLE_REASON			194	// Fault reason in the case DeviceState -> DISABLED
#define REG_WARNING					195	// Warning if present
#define REG_PROBLEM					196	// Problem reason
#define REG_TEST_FINISHED			197	// Indicates that test is complete
#define REG_RESULT_KELVIN			198	// AGGREGATED RESULT
#define REG_RESULT_IGT				199	// IGT [mA]
#define REG_RESULT_VGT				200	// VGT [mV]
#define REG_RESULT_IH				201	// IH  [mA]
#define REG_RESULT_IL				202	// IL  [mA]
#define REG_RESULT_RG				203	// R   [Ohm * 10]
#define REG_RESULT_CAL_I			204	// calibration I(GT, H) [mA]
#define REG_RESULT_CAL_V			205	// calibration V(GT) [mV]
//
#define REG_KELVIN_1_2				211	// MUST BE 1
#define REG_KELVIN_4_1				212	// MUST BE 0
#define REG_KELVIN_1_4				213	// MUST BE 0
#define REG_KELVIN_3_2				214	// MUST BE 1
//
#define REG_CAN_BUSOFF_COUNTER		220 // Counter of bus-off states
#define REG_CAN_STATUS_REG			221	// CAN status register (32 bit)
#define REG_CAN_STATUS_REG_32		222
#define REG_CAN_DIAG_TEC			223	// CAN TEC
#define REG_CAN_DIAG_REC			224	// CAN REC
//
#define REG_KELVIN_D1				240
#define REG_KELVIN_D2				241
#define REG_KELVIN_D3				242
#define REG_KELVIN_D4				243
//
#define REG_SP__3					255

// ENDPOINTS
//
#define EP16_Data_I					1	// Data of I
#define EP16_Data_V					2	// Data of V

// SCOPE TYPE (80 & 81)
//
#define SCOPE_TYPE_NONE				0	// Deactivate this scope
#define SCOPE_TYPE_I				1	// Log I values
#define SCOPE_TYPE_V				2	// Log V values
#define SCOPE_TYPE_TARGET_I			3	// Log target I values
#define SCOPE_TYPE_CONTROL			4	// Log control signal's values

// OPERATION RESULTS
//
#define OPRESULT_NONE				0	// No information or not finished
#define OPRESULT_OK					1	// Operation was successful
#define OPRESULT_FAIL				2	// Operation failed

// PROBLEM CODES
//
#define PROBLEM_NONE				0
#define PROBLEM_OPERATION_STOPPED	100 // Process was terminated by user
#define PROBLEM_HOLD_REACH_TIMEOUT	101	// Current hasn't reached desired start value
//
#define PROBLEM_GATE_CURRENT_HIGH	111	// Gate current is higher than testing end current
#define PROBLEM_GATE_FLW_ERROR		112	// Current hasn't reached desired value
#define PROBLEM_GATE_IGT_OVERLOAD	113	// Short-circuit on gate
//
#define PROBLEM_LATCH_CURRENT_HIGH	121	// Latching current is higher than testing end current
#define PROBLEM_LATCH_FLW_ERROR		122	// Current hasn't reached desired value
#define PROBLEM_LATCH_PROCESS_ERROR 123 // Can't close thyristor
//
#define PROBLEM_KELVIN_CC_SHORT		131	// Power cathod to power cathod short
//
#define PROBLEM_RG_FLW_ERROR		141 // Current hasn't reached desired value
#define PROBLEM_RG_OVERLOAD			142 // Short-circuit on gate

// FAULT CODES
//
#define FAULT_NONE					0	// No fault

// WARNING CODES
//
#define WARNING_NONE				0
//
#define WARNING_HOLD_CURRENT_SMALL	101	// Holding current is smaller than testing end current
//
#define WARNING_WATCHDOG_RESET		1001	// System has been reseted by WD

// DISABLE CODES
//
#define DISABLE_NONE				0
//
#define DISABLE_BAD_CLOCK			1001	// Problem with main oscillator

// DIAG CODES
//
#define DIAG_NO_ERROR				0
#define DIAG_NO_SYMBOL				1
#define DIAG_WRONG_SYMBOL			2

// USER ERROR CODES
//
#define ERR_NONE					0	// No error
#define ERR_CONFIGURATION_LOCKED	1	// Device is locked for writing
#define ERR_OPERATION_BLOCKED		2	// Operation can't be done due to current device state
#define ERR_DEVICE_NOT_READY		3	// Device isn't ready to switch state
#define ERR_WRONG_PWD				4	// Wrong password - unlock failed


#endif // __DEV_OBJ_DIC_H
