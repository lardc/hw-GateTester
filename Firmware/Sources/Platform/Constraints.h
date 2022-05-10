// -----------------------------------------
// Constraints for tunable parameters
// ----------------------------------------

#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
//
#include "DataTable.h"
#include "Global.h"

// Types
//
typedef struct __TableItemConstraint
{
	Int16U Min;
	Int16U Max;
	Int16U Default;
} TableItemConstraint;


// Restrictions
//
#define KELVIN_PROBE_TH_MIN			4		// 4
#define KELVIN_PROBE_TH_MAX			4090	// 4095 - 5
#define KELVIN_PROBE_TH_DEF			1000	// experimental
//
#define KELVIN_PROBE_TIME_MIN		1		// 1 ms
#define KELVIN_PROBE_TIME_MAX		100		// 100 ms
#define KELVIN_PROBE_TIME_DEF		5		// 5 ms
//
#define HOLDING_START_CURRENT_MIN	100		// 100 mA
#define HOLDING_START_CURRENT_MAX	1500	// 1500 mA
#define HOLDING_START_CURRENT_DEF	750		// 750 mA
//
#define HOLDING_END_CURRENT_MIN		5		// 5 mA
#define HOLDING_END_CURRENT_MAX		100		// 100 mA
#define HOLDING_END_CURRENT_DEF		10		// 20 mA
//
#define HOLDING_CURRENT_RATE_N_MIN	1		// unlimited
#define HOLDING_CURRENT_RATE_N_MAX	10000	// unlimited
#define HOLDING_CURRENT_RATE_N_DEF	300		// 2 mA/ms = 200(HOLDING_CURRENT_RATE_N_DEF) / 100(X_D_DEF)
//
#define HOLDING_CONTROL_OFFSET_MIN	0		// 0
#define HOLDING_CONTROL_OFFSET_MAX	1000	// 1000
#define HOLDING_CONTROL_OFFSET_DEF	300		// 300 = 4096 / 7.5V * 0.55V
//
#define HOLDING_KI_N_DEF			6		// 0.06 = 6(HOLDING_KI_N_DEF) / 100(X_D_DEF)
#define HOLDING_CLOSE_RATE_N_DEF	300		// 3 = 300(HOLDING_CLOSE_RATE_N_DEF) / 100(X_D_DEF)
//
#define HOLDING_CLOSE_DELTA_MIN		1		// 1 mA
#define HOLDING_CLOSE_DELTA_MAX		50		// 50 mA
#define HOLDING_CLOSE_DELTA_DEF		10		// 10 mA
//
#define HOLDING_STRIKE_LEN_MIN		10		// 1 us
#define HOLDING_STRIKE_LEN_MAX		50000	// 50 ms
#define HOLDING_STRIKE_LEN_DEF		1000	// 1000 us
//
#define HG_WAIT_TIMEOUT_MIN		10		// 10 ms
#define HG_WAIT_TIMEOUT_MAX		1000	// 1000 ms
#define HG_WAIT_TIMEOUT_DEF		500		// 500 ms
//
#define GATE_START_CURRENT_MIN		1		// 1 mA
#define GATE_START_CURRENT_MAX		100		// 100 mA
#define GATE_START_CURRENT_DEF		15		// 15 mA
//
#define GATE_END_CURRENT_MIN		500		// 500 mA
#define GATE_END_CURRENT_MAX		2000	// 2000 mA
#define GATE_END_CURRENT_DEF		1500	// 1500 mA
//
#define GATE_CURRENT_RATE_N_MIN		1		// unlimited
#define GATE_CURRENT_RATE_N_MAX		10000	// unlimited
#define GATE_CURRENT_RATE_N_DEF		100		// 1 mA/ms = 100(GATE_CURRENT_RATE_N_DEF) / 100(X_D_DEF)
//
#define GATE_FE_THRESHOLD_MIN		1		// 1 mA
#define GATE_FE_THRESHOLD_MAX		500		// 500 mA
#define GATE_FE_THRESHOLD_DEF		50		// 50 mA
//
#define GATE_FE_TIME_MIN			1		// 1 ms
#define GATE_FE_TIME_MAX			500		// 500 ms
#define GATE_FE_TIME_DEF			10		// 10 ms
//
#define GATE_CONTROL_OFFSET_MIN		0		// 0
#define GATE_CONTROL_OFFSET_MAX		2000	// 2000
#define GATE_CONTROL_OFFSET_DEF		700		// 650
//
#define GATE_CURR_MC_THRESHOLD_MIN	2000	// 2 A
#define GATE_CURR_MC_THRESHOLD_MAX	11000	// 11 A
#define GATE_CURR_MC_THRESHOLD_DEF	5000	// 5 A
//
#define GATE_KI_N_DEF				60		// 0.6 = 60(HOLDING_KI_N_DEF) / 100(X_D_DEF)
//
#define GATE_HISTORY_OFFSET_MAX		(GATE_HISTORY_LENGTH - 1)		// History MAX
#define GATE_HISTORY_OFFSET_DEF		1		// History DEF
//
#define X_D_MIN						1
#define X_D_MAX						10000
#define X_D_DEF						100
//
#define LATCH_SIGNAL_LARGE_STEP_MIN	1		// 1
#define LATCH_SIGNAL_LARGE_STEP_MAX	500		// 500
#define LATCH_SIGNAL_LARGE_STEP_DEF	50		// 50
//
#define LATCH_SIGNAL_SMALL_STEP_MIN	1		// 1
#define LATCH_SIGNAL_SMALL_STEP_MAX	100		// 100
#define LATCH_SIGNAL_SMALL_STEP_DEF	5		// 5
//
#define LATCH_SIGNAL_START_MIN		0		// 1
#define LATCH_SIGNAL_START_MAX		500		// 500
#define LATCH_SIGNAL_START_DEF		100		// 100
//
#define LATCH_SIGNAL_END_MIN		500		// 500
#define LATCH_SIGNAL_END_MAX		4095	// 4095
#define LATCH_SIGNAL_END_DEF		4000	// 4000
//
#define LATCH_CLOSE_TH_MIN			1		// 1 mA
#define LATCH_CLOSE_TH_MAX			100		// 100 mA
#define LATCH_CLOSE_TH_DEF			15		// 15 mA
//
#define LATCH_END_CURRENT_MIN		100		// 100 mA
#define LATCH_END_CURRENT_MAX		1500	// 1500 mA
#define LATCH_END_CURRENT_DEF		1000	// 1000 mA
//
#define RG_CURRENT_DEF				50		// 50 mA
#define RG_SAMPLE_DEF				10		// 10


// Variables
//
extern const TableItemConstraint NVConstraint[DATA_TABLE_NV_SIZE];
extern const TableItemConstraint VConstraint[DATA_TABLE_WP_START - DATA_TABLE_WR_START];


#endif // __CONSTRAINTS_H
