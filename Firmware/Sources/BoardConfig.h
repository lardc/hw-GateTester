// -----------------------------------------
// Board parameters
// ----------------------------------------

#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

// Include
#include <ZwBase.h>

// Program build mode
//
#define BOOT_FROM_FLASH					// normal mode
#define RAM_CACHE_SPI_X					// cache SPI-A(BCD) functions

// Board options
#define DSP28_2809						// on-board CPU
#define OSC_FRQ				(20MHz)		// on-board oscillator
#define CPU_FRQ_MHZ			100			// CPU frequency = 100MHz
#define CPU_FRQ				(CPU_FRQ_MHZ * 1000000L)
#define SYS_HSP_FREQ		(CPU_FRQ / 2) // High-speed bus frequency
#define SYS_LSP_FREQ		(CPU_FRQ / 4) // Low-speed bus frequency
//
#define ZW_PWM_DUTY_BASE	1024

// Peripheral options
#define HWUSE_SPI_A
#define HWUSE_SPI_B
#define HWUSE_SPI_C
#define HWUSE_SPI_D
#define HWUSE_SCI_B

// IO placement
#define SPI_A_QSEL		    GPAQSEL2
#define SPI_A_MUX			GPAMUX2
#define SPI_A_SIMO			GPIO16	
#define SPI_A_SOMI			GPIO17		
#define SPI_A_CLK			GPIO18
#define SPI_A_CS			GPIO19
//
#define SPI_B_QSEL			GPAQSEL2
#define SPI_B_MUX			GPAMUX2
#define SPI_B_SIMO			GPIO24	
#define SPI_B_SOMI			GPIO25		
#define SPI_B_CLK			GPIO26
#define SPI_B_CS			GPIO27
//
#define SPI_C_QSEL			GPAQSEL2
#define SPI_C_MUX			GPAMUX2
#define SPI_C_SIMO			GPIO20	
#define SPI_C_SOMI			GPIO21		
#define SPI_C_CLK			GPIO22
#define SPI_C_CS			GPIO23
//
#define SPI_D_QSEL			GPAQSEL1
#define SPI_D_MUX			GPAMUX1
#define SPI_D_SIMO			GPIO1	
#define SPI_D_SOMI			GPIO3		
#define SPI_D_CLK			GPIO5
#define SPI_D_CS			GPIO7
//
#define SCI_B_QSEL			GPAQSEL1
#define SCI_B_MUX			GPAMUX1
#define SCI_B_TX			GPIO9
#define SCI_B_RX			GPIO11
#define SCI_B_MUX_SELECTOR	2
//
#define PIN_MC_H			4
#define PIN_ENABLE_STRIKE	6
#define PIN_DAC_W1			8
#define PIN_GATE_DRV		10
#define PIN_DEBUG			14
#define PIN_DAC_W2			15
#define PIN_VR_OE			17
#define PIN_PDAC_LDAC		21
#define PIN_UDAC_LDAC		25
#define PIN_SPI_W1			32
#define PIN_SPI_W2			33
#define PIN_WD_RST			34
//
#define IDX_LED_1			0
#define IDX_LED_2			1

// ADC placement
#define AIN_VRF				0x02	// 0010b = A + IN2
#define AIN_IH_IL			0x08	// 1000b = B + IN0
#define AIN_MC				0x0A	// 1010b = B + IN2
//
#define AIN_VGT_IGT			0x01	// 0001b = A IN1 and B IN1
//
#define AIN_LO				0x07	// 0111b = A + IN7

#endif // __BOARD_CONFIG_H
