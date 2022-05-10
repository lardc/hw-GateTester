// -----------------------------------------
// Utility functions for measurements
// ----------------------------------------

// Header
#include "MeasureUtils.h"
//
// Includes
#include "DeviceObjectDictionary.h"
#include "DataTable.h"
#include "Controller.h"
#include "stdlib.h"


// Functions
//
#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(MU_LogScope, "ramfuncs");
#endif
void MU_LogScope(_iq I, _iq V, _iq TargetI, Int16S ControlSignal)
{
	if(CONTROL_Values_1_Counter == VALUES_x_SIZE)
		CONTROL_Values_1_Counter = 0;

	switch(DataTable[REG_SCOPE_TYPE_1])
	{
		case SCOPE_TYPE_I:
			CONTROL_Values_1[CONTROL_Values_1_Counter++] = _IQint(I);
			break;
		case SCOPE_TYPE_V:
			CONTROL_Values_1[CONTROL_Values_1_Counter++] = _IQint(V);
			break;
		case SCOPE_TYPE_TARGET_I:
			CONTROL_Values_1[CONTROL_Values_1_Counter++] = _IQint(TargetI);
			break;
		case SCOPE_TYPE_CONTROL:
			CONTROL_Values_1[CONTROL_Values_1_Counter++] = ControlSignal;
			break;
	}

	if(CONTROL_Values_2_Counter == VALUES_x_SIZE)
		CONTROL_Values_2_Counter = 0;

	switch(DataTable[REG_SCOPE_TYPE_2])
	{
		case SCOPE_TYPE_I:
			CONTROL_Values_2[CONTROL_Values_2_Counter++] = _IQint(I);
			break;
		case SCOPE_TYPE_V:
			CONTROL_Values_2[CONTROL_Values_2_Counter++] = _IQint(V);
			break;
		case SCOPE_TYPE_TARGET_I:
			CONTROL_Values_2[CONTROL_Values_2_Counter++] = _IQint(TargetI);
			break;
		case SCOPE_TYPE_CONTROL:
			CONTROL_Values_2[CONTROL_Values_2_Counter++] = ControlSignal;
			break;
	}
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(MU_SeekScope, "ramfuncs");
#endif
Int16U MU_SeekScope(Int16U Channel, Int16S Offset)
{
	Int16U oldCounter = 0;

	switch(Channel)
	{
		case 1:
			{
				oldCounter = CONTROL_Values_1_Counter;
				CONTROL_Values_1_Counter = ((Int16S)CONTROL_Values_1_Counter) + Offset;

				if(CONTROL_Values_1_Counter >= VALUES_x_SIZE)
					CONTROL_Values_1_Counter = 0;
			}
			break;
		case 2:
			{
				oldCounter = CONTROL_Values_2_Counter;
				CONTROL_Values_2_Counter = ((Int16S)CONTROL_Values_2_Counter) + Offset;

				if(CONTROL_Values_2_Counter >= VALUES_x_SIZE)
					CONTROL_Values_2_Counter = 0;
			}
			break;
	}

	return oldCounter;
}

// No more
