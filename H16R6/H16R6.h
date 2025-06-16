/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name  : H16R6.h
 Description: Header file for H16R6 module, supporting APA102 LED matrix control.
 Module: Declares functions, macros, and types for LED matrix control and communication.
 Peripherals: Defines UARTs (1-6), SPI1, TIM2, DMA1, and GPIO pins for LED matrix and module communication.
 Types: Includes module status enums and data types for LED patterns and configurations.
*/

/* Define to prevent recursive inclusion ***********************************/
#ifndef H16R6_H
#define H16R6_H

/* Includes ****************************************************************/
#include "BOS.h"
#include "H16R6_MemoryMap.h"
#include "H16R6_uart.h"
#include "H16R6_gpio.h"
#include "H16R6_dma.h"
#include "H16R6_inputs.h"
#include "H16R6_eeprom.h"
#include "H16R6_spi.h"
#include "APA102_LedMatrix.h"

/* Exported Macros *********************************************************/
#define	MODULE_PN		_H16R6

/* Port-related definitions */
#define	NUM_OF_PORTS	6
#define P_PROG 			P2		/* ST factory bootloader UART */

/* Define Available ports */
#define _P1
#define _P2
#define _P3
#define _P4
#define _P5
#define _P6

/* Define Available USARTs */
#define _USART1
#define _USART2
#define _USART3
#define _USART4
#define _USART5
#define _USART6

/* Port-UART mapping */
#define UART_P1 &huart4
#define UART_P2 &huart2
#define UART_P3 &huart3
#define UART_P4 &huart5
#define UART_P5 &huart1
#define UART_P6 &huart6

/* Module-specific Hardware Definitions ************************************/
/* Port Definitions */
#define	USART1_TX_PIN		GPIO_PIN_6
#define	USART1_RX_PIN		GPIO_PIN_7
#define	USART1_TX_PORT		GPIOB
#define	USART1_RX_PORT		GPIOB
#define	USART1_AF			GPIO_AF0_USART1

#define	USART2_TX_PIN		GPIO_PIN_2
#define	USART2_RX_PIN		GPIO_PIN_3
#define	USART2_TX_PORT		GPIOA
#define	USART2_RX_PORT		GPIOA
#define	USART2_AF			GPIO_AF1_USART2

#define	USART3_TX_PIN		GPIO_PIN_10
#define	USART3_RX_PIN		GPIO_PIN_11
#define	USART3_TX_PORT		GPIOB
#define	USART3_RX_PORT		GPIOB
#define	USART3_AF			GPIO_AF4_USART3
#define	USART4_TX_PIN		GPIO_PIN_0
#define	USART4_RX_PIN		GPIO_PIN_1
#define	USART4_TX_PORT		GPIOA
#define	USART4_RX_PORT		GPIOA
#define	USART4_AF			GPIO_AF4_USART4

#define	USART5_TX_PIN		GPIO_PIN_3
#define	USART5_RX_PIN		GPIO_PIN_2
#define	USART5_TX_PORT		GPIOD
#define	USART5_RX_PORT		GPIOD
#define	USART5_AF			GPIO_AF3_USART5

#define	USART6_TX_PIN		GPIO_PIN_8
#define	USART6_RX_PIN		GPIO_PIN_9
#define	USART6_TX_PORT		GPIOB
#define	USART6_RX_PORT		GPIOB
#define	USART6_AF			GPIO_AF8_USART6

/* SPI Pin Definition */
#define LED_MATRIX_SPI_SCK_PIN   GPIO_PIN_3
#define LED_MATRIX_SPI_MOSI_PIN  GPIO_PIN_5
#define LED_MATRIX_SPI_PORT      GPIOB

#define LED_MATRIX_SPI_HANDLER   &hspi1

/* Indicator LED */
#define _IND_LED_PORT		     GPIOC
#define _IND_LED_PIN		     GPIO_PIN_13

/* Module-specific Macro Definitions ***************************************/
#define	NUM_OF_MODULE_LEDMATRIX 	1
#define NUM_MODULE_PARAMS	        1
#define MAX_NUMBER_OF_LEDS          65

/* Module-specific Enumeration Definitions *********************************/
/* Basic colors */
enum BasicColors {
	BLACK = 1, WHITE, RED, BLUE, YELLOW, CYAN, MAGENTA, GREEN, AQUA, PURPLE, LIGHTBLUE, ORANGE, INDIGO,
};

 /* Module-specific Type Definition *****************************************/
 /* Module-status Type Definition */
typedef enum {
	H16R6_OK =0,
	H16R6_ERR_UnknownMessage,
	H16R6_ERR_WrongParams,
	H16R6_ERR_WrongColor,
	H16R6_ERR_WrongLedOutRange,
	H16R6_ERR_WrongIntensity,
	H16R6_ERROR =255
} Module_Status;

/* RGB LED operating modes */
enum LedMatrixMode {
	SCROLL_MODE = 1,
	FLASH_MODE,
	PICKER_MODE,
    FADE_MODE,
	FADE_ONE_RGB_LED_MODE,
	FADE_ALL_RGB_LED_MODE,
//	SPRINKLE_MODE,
};

/* Export UART variables */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;

/* Define UART Init prototypes */
extern void MX_USART1_UART_Init(void);
extern void MX_USART2_UART_Init(void);
extern void MX_USART3_UART_Init(void);
extern void MX_USART4_UART_Init(void);
extern void MX_USART5_UART_Init(void);
extern void MX_USART6_UART_Init(void);
extern void SystemClock_Config(void);

/***************************************************************************/
/***************************** General Functions ***************************/
/***************************************************************************/
Module_Status LEDMatrixSetRGB(uint8_t led, uint8_t red, uint8_t green, uint8_t blue,uint8_t intensity);
Module_Status LEDMatrixSetAllRGB(uint8_t red, uint8_t green, uint8_t blue,uint8_t intensity);
Module_Status LEDMatrixSetColor(uint8_t led,uint8_t color ,uint8_t intensity);
Module_Status LEDMatrixSetAllColor(uint8_t color,uint8_t intensity);
Module_Status LEDMatrixSetLedOff(uint8_t led);
Module_Status LEDMatrixSetAllLedOff();
Module_Status LEDMatrixSetLedOn(uint8_t led,uint8_t intensity);
Module_Status LEDMatrixSetAllLedOn(uint8_t intensity);
Module_Status LEDMatrixScrollMode(uint8_t baseColour,uint8_t scrollRow,uint8_t intensity,uint16_t scrollTime);
Module_Status LEDMatrixFlashMode(uint8_t baseColour,uint8_t flashColour,uint8_t intensity,uint16_t flashTime,uint16_t timeBetweenFlash);
Module_Status LEDMatrixRGBColorPickerMode(uint8_t color,uint16_t time,uint8_t intensity);
Module_Status LEDMatrixSetColorSomeLed(uint8_t StartLed,uint8_t EndLed,uint8_t color ,uint8_t intensity);
Module_Status LEDMatrixMotionMode(uint8_t baseColour,uint8_t SeconedColor,uint8_t intensity,float scaledqom);
Module_Status LEDMatrixCrossFadeMode(uint8_t baseColour,uint8_t seconedColor,uint8_t thirdColor,uint16_t time);
Module_Status LEDMatrixCrossFadeModeLEDRGB(uint8_t LED, uint8_t SecondRED,
		uint8_t SecondGREEN, uint8_t SecondBLUE, uint16_t interpolationtime, uint8_t intensity);
Module_Status LEDMatrixCrossFadeModeALLLEDRGB(uint8_t SecondRED,
		uint8_t SecondGREEN, uint8_t SecondBLUE, uint16_t interpolationtime, uint8_t intensity);
Module_Status LEDMatrixSprinkleMode(uint8_t TargetColorR, uint8_t TargetColorG,
		uint8_t TargetColorB, uint16_t TimeToFade, uint8_t ColorDeviation);
void LedMatrixTask(void *argument);

Module_Status LED_Matrix_Sprinkle_Mode();
Module_Status LED_Matrix_Cross_Fade_Mode_ALL_LED_RGB();
Module_Status LED_Matrix_Cross_Fade_Mode_LED_RGB();
Module_Status LED_Matrix_Cross_Fade_Mode();
Module_Status LED_Matrix_RGB_Color_Picker_Mode();
Module_Status LED_Matrix_Flash_Mode();
Module_Status LED_Matrix_Scroll_Mode();
#endif /* H16R6_H */

/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
