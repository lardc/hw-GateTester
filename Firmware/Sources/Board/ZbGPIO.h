// -----------------------------------------
// Board-specific GPIO functions
// ----------------------------------------

#ifndef __ZBGPIO_H
#define __ZBGPIO_H

// Include
#include "stdinc.h"
#include "ZwDSP.h"

// Functions
//
// Init GPIO and GPIO-PWM
void ZbGPIO_Init();
// Enable external driver
void ZbGPIO_SwitchExternalDriver(Boolean Enable);
// Enable H-switch
void ZbGPIO_SwitchH(Boolean Enable);
// Enable L-switch
void ZbGPIO_SwitchLbyPWM(Boolean Enable);
// Switch off Stop-switch
void ZbGPIO_SwitchStop(Boolean Enable);
// Enable TZ protection pins
void ZbGPIO_EnableProtectionIM(Boolean Enable);
void ZbGPIO_EnableProtectionIGT(Boolean Enable);
// Enable Strike-switch
void ZbGPIO_SwitchStrike(Boolean Enable);
// Switch state of debug pin
void ZbGPIO_SwitchDebugPin(Boolean Set);
// Toggle state of debug pin
void ZbGPIO_ToggleDebugPin();

#endif // __ZBGPIO_H
