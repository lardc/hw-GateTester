// -----------------------------------------
// Global definitions
// ----------------------------------------

#ifndef __GLOBAL_H
#define __GLOBAL_H

// Include
#include "stdinc.h"

// 1 - CATHODE (POWER) 	- to bridge 2
// 2 - CATHODE CONTROL 	- to bridge 4
// 3 - CONTROL			- to bridge 3
// 4 - ANODE (POWER)	- to bridge 1

// Global parameters
//--------------------------------------------------------
// Password to unlock non-volatile area for write
#define ENABLE_LOCKING					FALSE
#define UNLOCK_PWD_1					1
#define UNLOCK_PWD_2					1
#define UNLOCK_PWD_3					1
#define UNLOCK_PWD_4					1
//
#define	SCCI_TIMEOUT_TICKS				1000
#define DT_EPROM_ADDRESS				0
#define EP_COUNT						2
#define VALUES_x_SIZE					4000
//--------------------------------------------------------

// Gate parameters
//--------------------------------------------------------
// I = D * 3000 / (4096 * 8.2 * 2.33 * 0.1)
#define GATE_CONVERT_V_C				_IQ(0.3833f)
// V = D * 3000 / (4096 * 0.423 * 1.33)
#define GATE_CONVERT_V_V				_IQ(1.3019f)
#define GATE_SAMPLING_WAIT				50				// 50 ms
#define GATE_STAB_COUNTER_START			10				// 10 * 1 ms
// D = (I * 0.1 * 1.66 * 1.47) / 3300 * 4096
#define GATE_CONVERT_C_D_IM				_IQ(0.3029f)
// D = (1550mA * 0.1 * 8.2 * 2.33) / 3000 * 4096
#define GATE_TH_C_D_IGT					4050
//
#define GATE_HISTORY_LENGTH				8
//--------------------------------------------------------

// Holding parameters
//--------------------------------------------------------
// I = D * 3000 / (4096 * 8.2 * 2.33 * 0.1)
#define HOLD_CONVERT_V_C				_IQ(0.2682f)
#define HOLD_START_CURRENT_TOLERANCE	_IQ(5.0f)
#define HOLD_STAB_COUNTER_START			2			// 2 * 1 ms
//--------------------------------------------------------

// Latching parameters
//--------------------------------------------------------
// I = D * 3000 / (4096 * 8.2 * 3.33 * 0.1)
#define LATCH_CONVERT_V_C				_IQ(0.2682f)
#define LATCH_FIRING_DELAY				200				// 50uS * 4
//--------------------------------------------------------

// Calibration parameters
//--------------------------------------------------------
#define CAL_CURRENT_TOLERANCE			_IQ(1.0f)
#define CAL_TIMEOUT						3000
//--------------------------------------------------------

#endif // __GLOBAL_H
