// -----------------------------------------
// Board-specific GPIO functions
// ----------------------------------------

// Header
#include "ZbGPIO.h"
//
#include "SysConfig.h"


// Forward functions
//
void ZbGPIO_InitPWMOutputs();


// Functions
//
void ZbGPIO_Init()
{
   	ZwGPIO_WritePin(PIN_MC_H, FALSE);
   	ZwGPIO_WritePin(PIN_ENABLE_STRIKE, FALSE);
   	ZwGPIO_WritePin(PIN_GATE_DRV, FALSE);
   	ZwGPIO_WritePin(PIN_DEBUG, FALSE);
   	ZwGPIO_PinToOutput(PIN_MC_H);
   	ZwGPIO_PinToOutput(PIN_ENABLE_STRIKE);
   	ZwGPIO_PinToOutput(PIN_GATE_DRV);
   	ZwGPIO_PinToOutput(PIN_DEBUG);

   	ZbGPIO_InitPWMOutputs();
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGPIO_SwitchExternalDriver, "ramfuncs");
#endif
void ZbGPIO_SwitchExternalDriver(Boolean Enable)
{
	ZwGPIO_WritePin(PIN_GATE_DRV, Enable);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGPIO_SwitchH, "ramfuncs");
#endif
void ZbGPIO_SwitchH(Boolean Enable)
{
	ZwGPIO_WritePin(PIN_MC_H, Enable);
}
// ----------------------------------------

// Enable L-switch
#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGPIO_SwitchLbyPWM, "ramfuncs");
#endif
void ZbGPIO_SwitchLbyPWM(Boolean Enable)
{
	// Enable MC_L
	ZwPWM1_SetValueA(Enable ? ZW_PWM_DUTY_BASE : 0);
}
// ----------------------------------------

// Enable Stop-switch
#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGPIO_SwitchStop, "ramfuncs");
#endif
void ZbGPIO_SwitchStop(Boolean Enable)
{
	// Disable Stop_G
	ZwPWM2_SetValueA(Enable ? 0 : ZW_PWM_DUTY_BASE);
}
// ----------------------------------------

// Enable TZ on IM
#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGPIO_EnableProtectionIM, "ramfuncs");
#endif
void ZbGPIO_EnableProtectionIM(Boolean Enable)
{
	// TZ_IM
	ZwPWM_ConfigTZ1(Enable, PWM_TZ_QUAL);
}
// ----------------------------------------

// Enable TZ on IGT
#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGPIO_EnableProtectionIGT, "ramfuncs");
#endif
void ZbGPIO_EnableProtectionIGT(Boolean Enable)
{
	// TZ_IGT
	ZwPWM_ConfigTZ2(Enable, PWM_TZ_QUAL);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGPIO_SwitchStrike, "ramfuncs");
#endif
void ZbGPIO_SwitchStrike(Boolean Enable)
{
	ZwGPIO_WritePin(PIN_ENABLE_STRIKE, Enable);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGPIO_SwitchDebugPin, "ramfuncs");
#endif
void ZbGPIO_SwitchDebugPin(Boolean Set)
{
	ZwGPIO_WritePin(PIN_DEBUG, Set);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGPIO_ToggleDebugPin, "ramfuncs");
#endif
void ZbGPIO_ToggleDebugPin()
{
	ZwGPIO_TogglePin(PIN_DEBUG);
}
// ----------------------------------------

void ZbGPIO_InitPWMOutputs()
{
	// Initialize TZ functionality
	ZwPWM_SetTZPullup(PFDisable, PFDisable, PFDontcare, PFDontcare, PFDontcare, PFDontcare);

	// Initialize PWM1 & PWM2 (enable TZ_IM & TZ_IGT, MC_L & STOP_IGT)
	ZwPWM1_Init(PWMUp, CPU_FRQ, PWM_RUN_FREQ, TRUE, FALSE, 0, BIT0, FALSE, FALSE, TRUE, FALSE, FALSE);
	ZwPWM2_Init(PWMUp, CPU_FRQ, PWM_RUN_FREQ, TRUE, FALSE, 0, BIT1, FALSE, FALSE, TRUE, FALSE, FALSE);

	// Reset PWM signals (EN_MAIN_L and STOP_G)
	ZwPWM1_SetValueA(0);
	ZwPWM2_SetValueA(0);

	// Start PWM clocking
	ZwPWM_Enable(TRUE);

	// Enable TZ_IM and TZ_IGT
	ZwPWM_ConfigTZIntOST(TRUE, TRUE, FALSE, FALSE, FALSE, FALSE);

	// Enable interrupt on TZ events
	ZwPWM_EnableTZInterrupts(TRUE, TRUE, FALSE, FALSE, FALSE, FALSE);
	ZwPWM_EnableTZInterruptsGlobal(TRUE);
}

// No more.
