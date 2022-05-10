// -----------------------------------------
// Driver for DAC via SPI
// ----------------------------------------

// Header
#include "ZbDAC.h"
//
#include "SysConfig.h"


// Functions
//
void ZbPDAC_Init()
{
	// Init SPI-B (DAC)
	ZwSPIc_Init(TRUE, PDAC_BAUDRATE, 16, DAC_PLR, DAC_PHASE, ZW_SPI_INIT_TX | ZW_SPI_INIT_CS, FALSE, FALSE);
	ZwSPIc_InitFIFO(0, 0);
	ZwSPIc_ConfigInterrupts(FALSE, FALSE);
	ZwSPIc_EnableInterrupts(FALSE, FALSE);
	
	// Init LDAC pins
	ZwGPIO_WritePin(PIN_PDAC_LDAC, TRUE);
	ZwGPIO_PinToOutput(PIN_PDAC_LDAC);
}
// ----------------------------------------

void ZbUDAC_Init()
{
	// Init SPI-B (DAC)
	ZwSPIb_Init(TRUE, UDAC_BAUDRATE, 16, DAC_PLR, DAC_PHASE, ZW_SPI_INIT_TX | ZW_SPI_INIT_CS, FALSE, FALSE);
	ZwSPIb_InitFIFO(0, 0);
	ZwSPIb_ConfigInterrupts(FALSE, FALSE);
	ZwSPIb_EnableInterrupts(FALSE, FALSE);

	// Init LDAC and multiplexer's pins
	ZwGPIO_WritePin(PIN_UDAC_LDAC, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W1, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W2, TRUE);
	ZwGPIO_PinToOutput(PIN_UDAC_LDAC);
	ZwGPIO_PinToOutput(PIN_DAC_W1);
	ZwGPIO_PinToOutput(PIN_DAC_W2);
}
// ----------------------------------------

void ZbPDAC_WriteA(Int16U A)
{
	// Clear control bits
	A &= 0x0FFF;

	// Send data
	ZwSPIc_Send(&A, 1, 16, STTNormal);
	DELAY_US(PDAC_WRITE_DELAY_US);
	
	// Strobe to latch
	ZwGPIO_WritePin(PIN_PDAC_LDAC, FALSE);
    asm(" RPT #2 || NOP");
	ZwGPIO_WritePin(PIN_PDAC_LDAC, TRUE);
}
// ----------------------------------------

void ZbPDAC_WriteB(Int16U B)
{
	// Clear control bits
	B &= 0x0FFF;

	// Set B value to B channel
	B |= BIT15;
	
	// Send data
	ZwSPIc_Send(&B, 1, 16, STTNormal);
	DELAY_US(PDAC_WRITE_DELAY_US);
	
	// Strobe to latch
	ZwGPIO_WritePin(PIN_PDAC_LDAC, FALSE);
    asm(" RPT #2 || NOP");
	ZwGPIO_WritePin(PIN_PDAC_LDAC, TRUE);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbGUDAC_WriteA, "ramfuncs");
#endif
void ZbGUDAC_WriteA(Int16U A)
{
	// Select GU-DAC
	ZwGPIO_WritePin(PIN_DAC_W2, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W1, FALSE);

	// Clear control bits
	A &= 0x0FFF;

	// Send data
	ZwSPIb_Send(&A, 1, 16, STTNormal);
	DELAY_US(UDAC_WRITE_DELAY_US);

	// Strobe to latch
	ZwGPIO_WritePin(PIN_UDAC_LDAC, FALSE);
	DELAY_US(2);
	ZwGPIO_WritePin(PIN_UDAC_LDAC, TRUE);
}
// ----------------------------------------

#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(ZbPUDAC_WriteA, "ramfuncs");
#endif
void ZbPUDAC_WriteA(Int16U A)
{
	// Select PU-DAC
	ZwGPIO_WritePin(PIN_DAC_W1, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W2, FALSE);

	// Clear control bits
	A &= 0x0FFF;

	// Send data
	ZwSPIb_Send(&A, 1, 16, STTNormal);
	DELAY_US(UDAC_WRITE_DELAY_US);

	// Strobe to latch
	ZwGPIO_WritePin(PIN_UDAC_LDAC, FALSE);
	DELAY_US(2);
	ZwGPIO_WritePin(PIN_UDAC_LDAC, TRUE);
}
// ----------------------------------------

// No more.
