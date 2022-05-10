// -----------------------------------------
// Constraints for tunable parameters
// ----------------------------------------

// Header
#include "Constraints.h"
#include "DeviceObjectDictionary.h"

#define NO		0	// equal to FALSE
#define YES		1	// equal to TRUE

// Constants
//
const TableItemConstraint NVConstraint[DATA_TABLE_NV_SIZE] =
                                       {
                                    		   {0, 0, 0},																// 0
                                    		   {0, 0, 0},																// 1
                                    		   {0, 0, 0},																// 2
                                    		   {0, 0, 0},																// 3
                                    		   {0, 0, 0},																// 4
                                    		   {0, 0, 0},																// 5
                                    		   {0, 0, 0},																// 6
                                    		   {0, 0, 0},																// 7
                                    		   {0, 0, 0},																// 8
                                    		   {0, 0, 0},																// 9
                                    		   {NO, YES, YES},															// 10

                                    		   {KELVIN_PROBE_TH_MIN, KELVIN_PROBE_TH_MAX, KELVIN_PROBE_TH_DEF},			// 11
                                    		   {KELVIN_PROBE_TIME_MIN, KELVIN_PROBE_TIME_MAX, KELVIN_PROBE_TIME_DEF},	// 12

                                    		   {0, 0, 0},																// 13
                                    		   {0, 0, 0},																// 14
                                    		   {0, 0, 0},																// 15
                                    		   {0, 0, 0},																// 16
                                    		   {0, 0, 0},																// 17
                                    		   {0, 0, 0},																// 18
                                    		   {0, 0, 0},																// 19
                                    		   {0, 0, 0},																// 20

                                    		   {HOLDING_START_CURRENT_MIN, HOLDING_START_CURRENT_MAX, HOLDING_START_CURRENT_DEF},	// 21
                                    		   {HOLDING_END_CURRENT_MIN, HOLDING_END_CURRENT_MAX, HOLDING_END_CURRENT_DEF},			// 22
                                    		   {HOLDING_CURRENT_RATE_N_MIN, HOLDING_CURRENT_RATE_N_MAX, HOLDING_CURRENT_RATE_N_DEF},// 23
                                    		   {X_D_MIN, X_D_MAX, X_D_DEF},												// 24
                                    		   {HOLDING_CONTROL_OFFSET_MIN, HOLDING_CONTROL_OFFSET_MAX, HOLDING_CONTROL_OFFSET_DEF},// 25
                                    		   {1, INT16U_MAX, HOLDING_KI_N_DEF},										// 26
                                    		   {X_D_MIN, X_D_MAX, X_D_DEF},												// 27
                                    		   {1, INT16U_MAX, HOLDING_CLOSE_RATE_N_DEF},								// 28
                                    		   {X_D_MIN, X_D_MAX, X_D_DEF},												// 29
                                    		   {HOLDING_CLOSE_DELTA_MIN, HOLDING_CLOSE_DELTA_MAX, HOLDING_CLOSE_DELTA_DEF},			// 30
                                    		   {HG_WAIT_TIMEOUT_MIN, HG_WAIT_TIMEOUT_MAX, HG_WAIT_TIMEOUT_DEF},		// 31
                                    		   {HOLDING_STRIKE_LEN_MIN, HOLDING_STRIKE_LEN_MAX, HOLDING_STRIKE_LEN_DEF},// 32
                                    		   {1, INT16U_MAX, X_D_DEF},												// 33
                                    		   {X_D_MIN, X_D_MAX, X_D_DEF},												// 34
                                    		   {0, INT16U_MAX, 0},														// 35

                                    		   {0, 0, 0},																// 36
                                    		   {0, 0, 0},																// 37
                                    		   {0, 0, 0},																// 38
                                    		   {0, 0, 0},																// 39
                                    		   {0, 0, 0},																// 40

                                    		   {GATE_START_CURRENT_MIN, GATE_START_CURRENT_MAX, GATE_START_CURRENT_DEF},// 41
                                    		   {GATE_END_CURRENT_MIN, GATE_END_CURRENT_MAX, GATE_END_CURRENT_DEF},		// 42
                                    		   {GATE_CURRENT_RATE_N_MIN, GATE_CURRENT_RATE_N_MAX, GATE_CURRENT_RATE_N_DEF},			// 43
                                    		   {X_D_MIN, X_D_MAX, X_D_DEF},												// 44
                                    		   {1, INT16U_MAX, GATE_KI_N_DEF},											// 45
                                    		   {X_D_MIN, X_D_MAX, X_D_DEF},												// 46
                                    		   {GATE_CONTROL_OFFSET_MIN, GATE_CONTROL_OFFSET_MAX, GATE_CONTROL_OFFSET_DEF},			// 47
                                    		   {GATE_FE_THRESHOLD_MIN, GATE_FE_THRESHOLD_MAX, GATE_FE_THRESHOLD_DEF},	// 48
                                    		   {GATE_CURR_MC_THRESHOLD_MIN, GATE_CURR_MC_THRESHOLD_MAX, GATE_CURR_MC_THRESHOLD_DEF},// 49
                                    		   {1, INT16U_MAX, X_D_DEF},												// 50
                                    		   {X_D_MIN, X_D_MAX, X_D_DEF},												// 51
                                    		   {1, INT16U_MAX, X_D_DEF},												// 52
                                    		   {X_D_MIN, X_D_MAX, X_D_DEF},												// 53
                                    		   {GATE_FE_TIME_MIN, GATE_FE_TIME_MAX, GATE_FE_TIME_DEF},					// 54
                                    		   {0, GATE_HISTORY_OFFSET_MAX, GATE_HISTORY_OFFSET_DEF},					// 55

                                    		   {0, INT16U_MAX, 0},														// 56
                                    		   {0, INT16U_MAX, 0},														// 57
                                    		   {HG_WAIT_TIMEOUT_MIN, HG_WAIT_TIMEOUT_MAX, HG_WAIT_TIMEOUT_DEF},			// 58
                                    		   {0, 0, 0},																// 59
                                    		   {0, 0, 0},																// 60
                                    		   {0, 0, 0},																// 61
                                    		   {0, 0, 0},																// 62
                                    		   {0, 0, 0},																// 63
                                    		   {0, 0, 0},																// 64
                                    		   {0, 0, 0},																// 65
                                    		   {0, 0, 0},																// 66
                                    		   {0, 0, 0},																// 67
                                    		   {0, 0, 0},																// 68
                                    		   {0, 0, 0},																// 69
                                    		   {0, 0, 0},																// 70

                                    		   {LATCH_SIGNAL_LARGE_STEP_MIN, LATCH_SIGNAL_LARGE_STEP_MAX, LATCH_SIGNAL_LARGE_STEP_DEF},	// 71
                                    		   {LATCH_SIGNAL_SMALL_STEP_MIN, LATCH_SIGNAL_SMALL_STEP_MAX, LATCH_SIGNAL_SMALL_STEP_DEF},	// 72
                                    		   {LATCH_SIGNAL_START_MIN, LATCH_SIGNAL_START_MAX, LATCH_SIGNAL_START_DEF},				// 73
                                    		   {LATCH_SIGNAL_END_MIN, LATCH_SIGNAL_END_MAX, LATCH_SIGNAL_END_DEF},		// 74
                                    		   {LATCH_CLOSE_TH_MIN, LATCH_CLOSE_TH_MAX, LATCH_CLOSE_TH_DEF},			// 75
                                    		   {LATCH_END_CURRENT_MIN, LATCH_END_CURRENT_MAX, LATCH_END_CURRENT_DEF},	// 76
                                    		   {0, 0, 0},																// 77
                                    		   {0, 0, 0},																// 78

                                    		   {0, 0, 0},																// 79
                                    		   {0, 0, 0},																// 80
                                    		   {0, 0, 0},																// 81
                                    		   {0, 0, 0},																// 82
                                    		   {0, 0, 0},																// 83
                                    		   {0, 0, 0},																// 84
                                    		   {0, 0, 0},																// 85
                                    		   {0, 0, 0},																// 86
                                    		   {0, 0, 0},																// 87
                                    		   {0, 0, 0},																// 88
                                    		   {0, 0, 0},																// 89
                                    		   {0, 0, 0},																// 90

                                    		   {0, 0, 0},																// 91
                                    		   {0, 0, 0},																// 92
                                    		   {GATE_START_CURRENT_MIN, GATE_END_CURRENT_MAX, RG_CURRENT_DEF},			// 93
                                    		   {1, INT16U_MAX, RG_SAMPLE_DEF},											// 94

                                    		   {0, 0, 0},																// 95
                                    		   {0, 0, 0},																// 96
                                    		   {0, 0, 0},																// 97
                                    		   {0, 0, 0},																// 98
                                    		   {0, 0, 0},																// 99
                                    		   {0, 0, 0},																// 100
                                    		   {0, 0, 0},																// 101
                                    		   {0, 0, 0},																// 102
                                    		   {0, 0, 0},																// 103
                                    		   {0, 0, 0},																// 104
                                    		   {0, 0, 0},																// 105
                                    		   {0, 0, 0},																// 106
                                    		   {0, 0, 0},																// 107
                                    		   {0, 0, 0},																// 108
                                    		   {0, 0, 0},																// 109
                                    		   {0, 0, 0},																// 110
                                    		   {0, 0, 0},																// 111
                                    		   {0, 0, 0},																// 112
                                    		   {0, 0, 0},																// 113
                                    		   {0, 0, 0},																// 114
                                    		   {0, 0, 0},																// 115
                                    		   {0, 0, 0},																// 116
                                    		   {0, 0, 0},																// 117
                                    		   {0, 0, 0},																// 118
                                    		   {0, 0, 0},																// 119
                                    		   {0, 0, 0},																// 120
                                    		   {0, 0, 0},																// 121
                                    		   {0, 0, 0},																// 122
                                    		   {0, 0, 0},																// 123
                                    		   {0, 0, 0},																// 124
                                    		   {0, 0, 0},																// 125
                                    		   {0, 0, 0},																// 126

                                    		   {INT16U_MAX, 0, 0}														// 127
                                       };

const TableItemConstraint VConstraint[DATA_TABLE_WP_START - DATA_TABLE_WR_START] =
                                      {
											   {NO, YES, NO},															// 128
											   {NO, YES, NO},															// 129
											   {NO, YES, NO},															// 130

											   {0, 0, 0},																// 131
											   {0, 0, 0},																// 132
											   {0, 0, 0},																// 133
											   {0, 0, 0},																// 134
											   {0, 0, 0},																// 135
											   {0, 0, 0},																// 136
											   {0, 0, 0},																// 137
											   {0, 0, 0},																// 138
											   {0, 0, 0},																// 139
											   {0, INT16U_MAX, 0},														// 140
											   {0, 0, 0},																// 141
											   {0, 0, 0},																// 142
											   {0, 0, 0},																// 143
											   {0, 0, 0},																// 144
											   {0, 0, 0},																// 145
											   {0, 0, 0},																// 146
											   {0, 0, 0},																// 147
											   {0, 0, 0},																// 148
											   {0, 0, 0},																// 149

											   {SCOPE_TYPE_NONE, SCOPE_TYPE_CONTROL, SCOPE_TYPE_I},						// 150
											   {SCOPE_TYPE_NONE, SCOPE_TYPE_CONTROL, SCOPE_TYPE_V},						// 151

											   {0, 0, 0},																// 152
											   {0, 0, 0},																// 153
											   {0, 0, 0},																// 154
											   {0, 0, 0},																// 155
											   {0, 0, 0},																// 156
											   {0, 0, 0},																// 157
											   {0, 0, 0},																// 158
											   {0, 0, 0},																// 159
											   {0, 0, 0},																// 160
											   {0, 0, 0},																// 161
											   {0, 0, 0},																// 162
											   {0, 0, 0},																// 163
											   {0, 0, 0},																// 164
											   {0, 0, 0},																// 165
											   {0, 0, 0},																// 166
											   {0, 0, 0},																// 167
											   {0, 0, 0},																// 168
											   {0, 0, 0},																// 169
											   {0, 0, 0},																// 170
											   {0, 0, 0},																// 171
											   {0, 0, 0},																// 172
											   {0, 0, 0},																// 173
											   {0, 0, 0},																// 174
											   {0, 0, 0},																// 175
											   {0, 0, 0},																// 176
											   {0, 0, 0},																// 177
											   {0, 0, 0},																// 178
											   {0, 0, 0},																// 179

											   {0, INT16U_MAX, 0},														// 180
											   {0, INT16U_MAX, 0},														// 181
											   {0, INT16U_MAX, 0},														// 182
											   {0, INT16U_MAX, 0},														// 183

											   {0, 0, 0},																// 184
											   {0, 0, 0},																// 185
											   {0, 0, 0},																// 186
											   {0, 0, 0},																// 187
											   {0, 0, 0},																// 188
											   {0, 0, 0},																// 189
											   {0, 0, 0},																// 190

											   {INT16U_MAX, 0, 0}														// 191
                                      };

// No more
