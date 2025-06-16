/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name  : H16R6.c
 Description: Source code for H16R6 module, implementing APA102 LED matrix control.
 Module: Manages APA102 LED matrix for patterns, colors, and animations with SPI communication.
 Peripherals: Configures UARTs (1-6), SPI1, TIM2, DMA1, and GPIO for LED control and module communication.
 Tasks: Handles LED matrix task for pattern updates, CLI commands for LED control, and remote bootloader updates.
 Functions: Initializes LED matrix, controls patterns (scroll, flash, fade), adjusts intensity, and manages power modes (stop/standby).
*/

/* Includes ****************************************************************/
#include "BOS.h"
#include "H16R6_inputs.h"

/* Exported Typedef ********************************************************/
/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

TaskHandle_t LedMatrixTaskHandle = NULL;

/* Private Variables *******************************************************/
uint8_t OldColorR[LED_FRAME_SIZE] = { 0 };
uint8_t OldColorG[LED_FRAME_SIZE] = { 0 };
uint8_t OldColorB[LED_FRAME_SIZE] = { 0 };
uint8_t LedMatrixMode;
uint8_t BasicColor,SecondColor,ThirdColor,Intensity,Led,AmountOfLeds,Colordeviation,Color;
uint16_t FlashTime,TimeBetweenFlash,Time,InterpolationTime,TimetoFade,ScrollTime;
int RandomIndex[LED_FRAME_SIZE];

/* Module Parameters */
ModuleParam_t ModuleParam[NUM_MODULE_PARAMS] = { 0 };

/* Private Function Prototypes *********************************************/
uint8_t ClearROtopology(void);
void Module_Peripheral_Init(void);
void SetupPortForRemoteBootloaderUpdate(uint8_t port);
void RemoteBootloaderUpdate(uint8_t src,uint8_t dst,uint8_t inport,uint8_t outport);
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift);

/* Local Function Prototypes ***********************************************/
void RandomArray(int RandomIndex[65], int NbOfLeds);

/* Create CLI commands *****************************************************/
portBASE_TYPE CLI_SetRGBCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_SetAllRGBCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_SetColorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_SetAllColorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_SetLedOffCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_SetAllLedOffCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_SetLedOnCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_SetAllLedOnCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_ScrollModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_FlashModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_ColorPickerModeCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_SetColorSomeLedCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_MotionModeCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_CrossFadeModeCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_CrossFadeModeLEDRGBCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_CrossFadeModeALLLEDRGBCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen, const int8_t *pcCommandString);
portBASE_TYPE CLI_SprinkleModeCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen, const int8_t *pcCommandString);


/* CLI command structure ***************************************************/
/* CLI command structure : LEDMatrixSetRGB */
const CLI_Command_Definition_t CLI_SetRGBCommandDefinition = {
		(const int8_t*) "setrgb", /* The command string to type. */
		(const int8_t*) "setrgb:\r\n Set RGB LED (1st par.), red (2st par.), green (3nd par.), and blue (4rd par.) values (0-255) at a specific intensity (0-31%) (5th par.)\r\n\r\n",
		CLI_SetRGBCommand, /* The function to run. */
		5 /* five parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetAllRGB */
const CLI_Command_Definition_t CLI_SetAllRGBCommandDefinition = {
		(const int8_t*) "setallrgb", /* The command string to type. */
		(const int8_t*) "setallrgb:\r\n Set RGB  red (1st par.), green (2nd par.), and blue (3rd par.) values (0-255) at a specific intensity (0-31%) (4th par.)\r\n\r\n",
		CLI_SetAllRGBCommand, /* The function to run. */
		4 /* four parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetColor */
const CLI_Command_Definition_t CLI_SetColorCommandDefinition = {
		(const int8_t*) "setcolor", /* The command string to type. */
		(const int8_t*) "setcolor:\r\n Set RGB LED (1st par.),LED color (2st par.) at a specific intensity (0-31%) (3nd par.)\n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_SetColorCommand, /* The function to run. */
		3 /* Three parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetAllColor */
const CLI_Command_Definition_t CLI_SetAllColorCommandDefinition = {
		(const int8_t*) "setallcolor", /* The command string to type. */
		(const int8_t*) "setallcolor:\r\n Set LED color (1st par.) at a specific intensity (0-31%) (2nd par.)\n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_SetAllColorCommand, /* The function to run. */
		2 /* Two parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetLedOff */
const CLI_Command_Definition_t CLI_SetLedOffCommandDefinition = {
		(const int8_t*) "setledoff", /* The command string to type. */
		(const int8_t*) "setledoff:\r\n Set RGB LED off(1st par.)\r\n\r\n",
		CLI_SetLedOffCommand, /* The function to run. */
		1 /* One parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetAllLedOff */
const CLI_Command_Definition_t CLI_SetAllLedOffCommandDefinition = {
		(const int8_t*) "setallledoff", /* The command string to type. */
		(const int8_t*) "setallledoff:\r\n All LEDs off\r\n\r\n",
		CLI_SetAllLedOffCommand, /* The function to Off. */
		0 /* No parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetLedOn */
const CLI_Command_Definition_t CLI_SetLedOnCommandDefinition = {
		(const int8_t*) "setledon", /* The command string to type. */
		(const int8_t*) "setledon:\r\n Set RGB LED ON(1st par.),at a specific intensity (0-31%) (2th par.)\r\n\r\n",
		CLI_SetLedOnCommand, /* The function to Off. */
		2 /* Two parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetAllLedOn */
const CLI_Command_Definition_t CLI_SetAllLedOnCommandDefinition = {
		(const int8_t*) "setallledon", /* The command string to type. */
		(const int8_t*) "setallledon:\r\n Set All RGB LEDs ON at a specific intensity (0-31%) (1th par.)\r\n\r\n",
		CLI_SetAllLedOnCommand, /* The function to run. */
		1 /* One parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetLedOn */
const CLI_Command_Definition_t CLI_ScrollModeCommandDefinition = {
		(const int8_t*) "scrollmode", /* The command string to type. */
		(const int8_t*) "scrollmode:\r\n Set scrollmode  Basic color (1st par.) Secondary color(2st par.) at a specific intensity (0-31%) (3nd par.) scrollTime(4nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_ScrollModeCommand, /* The function to ScrollMode. */
		4 /* four parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetAllLedOn */
const CLI_Command_Definition_t CLI_FlashModeCommandDefinition = {
		(const int8_t*) "flashmode", /* The command string to type. */
		(const int8_t*) "flashmode:\r\n Set flashmode  Basic color (1st par.) Secondary color(2st par.) at a specific intensity (0-31%) (3nd par.) flashTime(4nd par.)  timeBetweenFlash(5nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_FlashModeCommand, /* The function to FlashMode. */
		5 /* five parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixRGBColorPickerMode */
const CLI_Command_Definition_t CLI_ColorPickerModeCommandDefinition = {
		(const int8_t*) "colorpickermode", /* The command string to type. */
		(const int8_t*) "colorpickermode:\r\n Set colorpickermode  color (1st par.)  Time(2nd par.) at a specific intensity (0-31%) (3nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_ColorPickerModeCommand, /* The function to SetColorSomeLed. */
		3 /* four parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetColorSomeLed */
const CLI_Command_Definition_t CLI_SetColorSomeLedCommandDefinition = {

		(const int8_t*) "setcolorsomeled", /* The command string to type. */
		(const int8_t*) "setcolorsomeled:\r\n Set setcolorsomeled  StartLed (1st par.)  EndLed (2nd par.) color (3nd par.) at a specific intensity (0-31%) (4nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_SetColorSomeLedCommand, /* The function to FlashMode. */
		4 /* four parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetColorSomeLed */
const CLI_Command_Definition_t CLI_MotionModeCommandDefinition = {

		(const int8_t*) "motionmode", /* The command string to type. */
		(const int8_t*) "motionmode:\r\n Set motionmode  basecolor (1st par.)  secondcolor (2nd par.)  intensity (0-31%) (3nd par.) scaledQOM (4nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_MotionModeCommand, /* The function to FlashMode. */
		4 /* four parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetColorSomeLed */
const CLI_Command_Definition_t CLI_CrossFadeModeCommandDefinition = {
		(const int8_t*) "crossfade", /* The command string to type. */
		(const int8_t*) "crossfade:\r\n Set crossfade  basecolor (1st par.)  secondcolor (2nd par.) thirdcolor (3nd par.) time (4nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_CrossFadeModeCommand, /* The function to FlashMode. */
		4 /* four parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetColorSomeLed */
const CLI_Command_Definition_t CLI_CrossFadeModeLEDRGBCommandDefinition = {
		(const int8_t*) "fademodeled", /* The command string to type. */
		(const int8_t*) "fademodeled:\r\n Set fademodeled  led (1st par.)  red (2nd par.) green (3nd par.) blue (4nd par.) intensity (0-31%) (5nd par.) inerpolationtime (6nd par.)\n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_CrossFadeModeLEDRGBCommand, /* The function to FlashMode. */
		6 /* four parameters are expected. */
};

/***************************************************************************/
/* CLI command structure : LEDMatrixSetColorSomeLed */
const CLI_Command_Definition_t CLI_CrossFadeModeALLLEDRGBCommandDefinition = {
		(const int8_t*) "fademodeallled", /* The command string to type. */
		(const int8_t*) "fademodeallled:\r\n Set fademodeallled  red (1nd par.) green (2nd par.) blue (3nd par.) intensity (0-31%) (4nd par.) inerpolationtime (5nd par.)\n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
		CLI_CrossFadeModeALLLEDRGBCommand, /* The function to FlashMode. */
		5 /* four parameters are expected. */
};

///***************************************************************************/
///* CLI command structure : LEDMatrixSetColorSomeLed */
//const CLI_Command_Definition_t CLI_SprinkleModeCommandDefinition = {
//		(const int8_t*) "sprinklemode", /* The command string to type. */
//		(const int8_t*) "sprinklemode:\r\n Set sprinklemode  red (1st par.)  green (2nd par.)  blue (3nd par.) colordeviation(4nd par.) timetofade(5nd par.)  \n\rRegistered colors are:\
//					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
//		CLI_SprinkleModeCommand, /* The function to FlashMode. */
//		5 /* four parameters are expected. */
//};


/***************************************************************************/
/************************ Private function Definitions *********************/
/***************************************************************************/
/* @brief  System Clock Configuration
 *         This function configures the system clock as follows:
 *            - System Clock source            = PLL (HSE)
 *            - SYSCLK(Hz)                     = 64000000
 *            - HCLK(Hz)                       = 64000000
 *            - AHB Prescaler                  = 1
 *            - APB1 Prescaler                 = 1
 *            - HSE Frequency(Hz)              = 8000000
 *            - PLLM                           = 1
 *            - PLLN                           = 16
 *            - PLLP                           = 2
 *            - Flash Latency(WS)              = 2
 *            - Clock Source for UART1,UART2,UART3 = 16MHz (HSI)
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI
			| RCC_OSCILLATORTYPE_HSE; // Enable both HSI and HSE oscillators
	RCC_OscInitStruct.HSEState = RCC_HSE_ON; // Enable HSE (External High-Speed Oscillator)
	RCC_OscInitStruct.HSIState = RCC_HSI_ON; // Enable HSI (Internal High-Speed Oscillator)
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1; // No division on HSI
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; // Default calibration value for HSI
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON; // Enable PLL
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE; // Set PLL source to HSE
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1; // Prescaler for PLL input
	RCC_OscInitStruct.PLL.PLLN = 16; // Multiplication factor for PLL
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // PLLP division factor
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2; // PLLQ division factor
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2; // PLLR division factor
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/** Initializes the CPU, AHB and APB buses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // Select PLL as the system clock source
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // AHB Prescaler set to 1
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; // APB1 Prescaler set to 1

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2); // Configure system clocks with flash latency of 2 WS
}

/***************************************************************************/
/* enable stop mode regarding only UART1 , UART2 , and UART3 */
BOS_Status EnableStopModebyUARTx(uint8_t port) {

	UART_WakeUpTypeDef WakeUpSelection;
	UART_HandleTypeDef *huart = GetUart(port);

	if ((huart->Instance == USART1) || (huart->Instance == USART2)
			|| (huart->Instance == USART3)) {

		/* make sure that no UART transfer is on-going */
		while (__HAL_UART_GET_FLAG(huart, USART_ISR_BUSY) == SET);

		/* make sure that UART is ready to receive */
		while (__HAL_UART_GET_FLAG(huart, USART_ISR_REACK) == RESET);

		/* set the wake-up event:
		 * specify wake-up on start-bit detection */
		WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_STARTBIT;
		HAL_UARTEx_StopModeWakeUpSourceConfig(huart, WakeUpSelection);

		/* Enable the UART Wake UP from stop mode Interrupt */
		__HAL_UART_ENABLE_IT(huart, UART_IT_WUF);

		/* enable MCU wake-up by LPUART */
		HAL_UARTEx_EnableStopMode(huart);

		/* enter STOP mode */
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	} else
		return BOS_ERROR;

}

/***************************************************************************/
/* Enable standby mode regarding wake-up pins:
 * WKUP1: PA0  pin
 * WKUP4: PA2  pin
 * WKUP6: PB5  pin
 * WKUP2: PC13 pin
 * NRST pin
 *  */
BOS_Status EnableStandbyModebyWakeupPinx(WakeupPins_t wakeupPins) {

	/* Clear the WUF FLAG */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF);

	/* Enable the WAKEUP PIN */
	switch (wakeupPins) {

	case PA0_PIN:
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); /* PA0 */
		break;

	case PA2_PIN:
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4); /* PA2 */
		break;

	case PB5_PIN:
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN6); /* PB5 */
		break;

	case PC13_PIN:
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2); /* PC13 */
		break;

	case NRST_PIN:
		/* do no thing*/
		break;
	}

	/* Enable SRAM content retention in Standby mode */
	HAL_PWREx_EnableSRAMRetention();

	/* Finally enter the standby mode */
	HAL_PWR_EnterSTANDBYMode();

	return BOS_OK;
}

/***************************************************************************/
/* Disable standby mode regarding wake-up pins:
 * WKUP1: PA0  pin
 * WKUP4: PA2  pin
 * WKUP6: PB5  pin
 * WKUP2: PC13 pin
 * NRST pin
 *  */
BOS_Status DisableStandbyModeWakeupPinx(WakeupPins_t wakeupPins) {

	/* The standby wake-up is same as a system RESET:
	 * The entire code runs from the beginning just as if it was a RESET.
	 * The only difference between a reset and a STANDBY wake-up is that, when the MCU wakes-up,
	 * The SBF status flag in the PWR power control/status register (PWR_CSR) is set */
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
		/* clear the flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

		/* Disable  Wake-up Pinx */
		switch (wakeupPins) {

		case PA0_PIN:
			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1); /* PA0 */
			break;

		case PA2_PIN:
			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN4); /* PA2 */
			break;

		case PB5_PIN:
			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN6); /* PB5 */
			break;

		case PC13_PIN:
			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2); /* PC13 */
			break;

		case NRST_PIN:
			/* do no thing*/
			break;
		}

		IND_blink(1000);

	} else
		return BOS_OK;

}

/***************************************************************************/
/* Save Command Topology in Flash RO */
uint8_t SaveTopologyToRO(void) {

	HAL_StatusTypeDef flashStatus = HAL_OK;

	/* flashAdd is initialized with 8 because the first memory room in topology page
	 * is reserved for module's ID */
	uint16_t flashAdd = 8;
	uint16_t temp = 0;

	/* Unlock the FLASH control register access */
	HAL_FLASH_Unlock();

	/* Erase Topology page */
	FLASH_PageErase(FLASH_BANK_2, TOPOLOGY_PAGE_NUM);

	/* Wait for an Erase operation to complete */
	flashStatus = FLASH_WaitForLastOperation(
			(uint32_t) HAL_FLASH_TIMEOUT_VALUE);

	if (flashStatus != HAL_OK) {
		/* return FLASH error code */
		return pFlash.ErrorCode;
	}

	else {
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	}

	/* Save module's ID and topology */
	if (myID) {

		/* Save module's ID */
		temp = (uint16_t) (N << 8) + myID;

		/* Save module's ID in Flash memory */
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, TOPOLOGY_START_ADDRESS,
				temp);

		/* Wait for a Write operation to complete */
		flashStatus = FLASH_WaitForLastOperation(
				(uint32_t) HAL_FLASH_TIMEOUT_VALUE);

		if (flashStatus != HAL_OK) {
			/* return FLASH error code */
			return pFlash.ErrorCode;
		}

		else {
			/* If the program operation is completed, disable the PG Bit */
			CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
		}

		/* Save topology */
		for (uint8_t row = 1; row <= N; row++) {
			for (uint8_t column = 0; column <= MAX_NUM_OF_PORTS; column++) {
				/* Check the module serial number
				 * Note: there isn't a module has serial number 0
				 */
				if (Array[row - 1][0]) {
					/* Save each element in topology Array in Flash memory */
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
							TOPOLOGY_START_ADDRESS + flashAdd,
							Array[row - 1][column]);
					/* Wait for a Write operation to complete */
					flashStatus = FLASH_WaitForLastOperation(
							(uint32_t) HAL_FLASH_TIMEOUT_VALUE);
					if (flashStatus != HAL_OK) {
						/* return FLASH error code */
						return pFlash.ErrorCode;
					} else {
						/* If the program operation is completed, disable the PG Bit */
						CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
						/* update new flash memory address */
						flashAdd += 8;
					}
				}
			}
		}
	}
	/* Lock the FLASH control register access */
	HAL_FLASH_Lock();
}

/***************************************************************************/
/* Save Command Snippets in Flash RO */
uint8_t SaveSnippetsToRO(void) {
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	uint8_t snipBuffer[sizeof(Snippet_t) + 1] = { 0 };

	/* Unlock the FLASH control register access */
	HAL_FLASH_Unlock();
	/* Erase Snippets page */
	FLASH_PageErase(FLASH_BANK_2, SNIPPETS_PAGE_NUM);
	/* Wait for an Erase operation to complete */
	FlashStatus = FLASH_WaitForLastOperation(
			(uint32_t) HAL_FLASH_TIMEOUT_VALUE);

	if (FlashStatus != HAL_OK) {
		/* return FLASH error code */
		return pFlash.ErrorCode;
	} else {
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	}

	/* Save Command Snippets */
	int currentAdd = SNIPPETS_START_ADDRESS;
	for (uint8_t index = 0; index < NumOfRecordedSnippets; index++) {
		/* Check if Snippet condition is true or false */
		if (Snippets[index].Condition.ConditionType) {
			/* A marker to separate Snippets */
			snipBuffer[0] = 0xFE;
			memcpy((uint32_t*) &snipBuffer[1], (uint8_t*) &Snippets[index],
					sizeof(Snippet_t));
			/* Copy the snippet struct buffer (20 x NumOfRecordedSnippets). Note this is assuming sizeof(Snippet_t) is even */
			for (uint8_t j = 0; j < (sizeof(Snippet_t) / 4); j++) {
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, currentAdd,
						*(uint64_t*) &snipBuffer[j * 8]);
				FlashStatus = FLASH_WaitForLastOperation(
						(uint32_t) HAL_FLASH_TIMEOUT_VALUE);
				if (FlashStatus != HAL_OK) {
					return pFlash.ErrorCode;
				} else {
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					currentAdd += 8;
				}
			}
			/* Copy the snippet commands buffer. Always an even number. Note the string termination char might be skipped */
			for (uint8_t j = 0; j < ((strlen(Snippets[index].CMD) + 1) / 4);
					j++) {
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, currentAdd,
						*(uint64_t*) (Snippets[index].CMD + j * 4));
				FlashStatus = FLASH_WaitForLastOperation(
						(uint32_t) HAL_FLASH_TIMEOUT_VALUE);
				if (FlashStatus != HAL_OK) {
					return pFlash.ErrorCode;
				} else {
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					currentAdd += 8;
				}
			}
		}
	}
	/* Lock the FLASH control register access */
	HAL_FLASH_Lock();
}

/***************************************************************************/
/* Clear Array topology in SRAM and Flash RO */
uint8_t ClearROtopology(void) {
	/* Clear the Array */
	memset(Array, 0, sizeof(Array));
	N = 1;
	myID = 0;

	return SaveTopologyToRO();
}

/***************************************************************************/
/* Trigger ST factory bootloader update for a remote module */
void RemoteBootloaderUpdate(uint8_t src, uint8_t dst, uint8_t inport,
		uint8_t outport) {

	uint8_t myOutport = 0, lastModule = 0;
	int8_t *pcOutputString;

	/* 1. Get Route to destination module */
	myOutport = FindRoute(myID, dst);
	if (outport && dst == myID) { /* This is a 'via port' update and I'm the last module */
		myOutport = outport;
		lastModule = myID;
	} else if (outport == 0) { /* This is a remote update */
		if (NumberOfHops(dst)== 1)
		lastModule = myID;
		else
		lastModule = Route[NumberOfHops(dst)-1]; /* previous module = Route[Number of hops - 1] */
	}

	/* 2. If this is the source of the message, show status on the CLI */
	if (src == myID) {
		/* Obtain the address of the output buffer.  Note there is no mutual
		 * exclusion on this buffer as it is assumed only one command console
		 * interface will be used at any one time. */
		pcOutputString = FreeRTOS_CLIGetOutputBuffer();

		if (outport == 0)		// This is a remote module update
			sprintf((char*) pcOutputString, pcRemoteBootloaderUpdateMessage,
					dst);
		else
			// This is a 'via port' remote update
			sprintf((char*) pcOutputString,
					pcRemoteBootloaderUpdateViaPortMessage, dst, outport);

		strcat((char*) pcOutputString, pcRemoteBootloaderUpdateWarningMessage);
		writePxITMutex(inport, (char*) pcOutputString,
				strlen((char*) pcOutputString), cmd50ms);
		Delay_ms(100);
	}

	/* 3. Setup my inport and outport for bootloader update */
	SetupPortForRemoteBootloaderUpdate(inport);
	SetupPortForRemoteBootloaderUpdate(myOutport);

	/* 5. Build a DMA stream between my inport and outport */
	StartScastDMAStream(inport, myID, myOutport, myID, BIDIRECTIONAL,
			0xFFFFFFFF, 0xFFFFFFFF, false);
}

/***************************************************************************/
/* Setup a port for remote ST factory bootloader update:
 * Set baudrate to 57600
 * Enable even parity
 * Set datasize to 9 bits
 */
void SetupPortForRemoteBootloaderUpdate(uint8_t port){

	UART_HandleTypeDef *huart =GetUart(port);
	HAL_UART_DeInit(huart);
	huart->Init.Parity = UART_PARITY_EVEN;
	huart->Init.WordLength = UART_WORDLENGTH_9B;
	HAL_UART_Init(huart);

	/* The CLI port RXNE interrupt might be disabled so enable here again to be sure */
	__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);

}

/***************************************************************************/
/* H16R6 module initialization */
void Module_Peripheral_Init(void) {

	/* Array ports */
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_USART4_UART_Init();
	MX_USART5_UART_Init();
	MX_USART6_UART_Init();

	MX_SPI1_Init();
	DigiLedInit();

	/* Circulating DMA Channels ON All Module */
	for (int i = 1; i <= NUM_OF_PORTS; i++) {
		if (GetUart(i) == &huart1) {
			dmaIndex[i - 1] = &(DMA1_Channel1->CNDTR);
		} else if (GetUart(i) == &huart2) {
			dmaIndex[i - 1] = &(DMA1_Channel2->CNDTR);
		} else if (GetUart(i) == &huart3) {
			dmaIndex[i - 1] = &(DMA1_Channel3->CNDTR);
		} else if (GetUart(i) == &huart4) {
			dmaIndex[i - 1] = &(DMA1_Channel4->CNDTR);
		} else if (GetUart(i) == &huart5) {
			dmaIndex[i - 1] = &(DMA1_Channel5->CNDTR);
		} else if (GetUart(i) == &huart6) {
			dmaIndex[i - 1] = &(DMA1_Channel6->CNDTR);
		}
	}

	if(LedMatrixTaskHandle == NULL)
			xTaskCreate(LedMatrixTask,(const char* ) "LedMatrixTask",configMINIMAL_STACK_SIZE,NULL,osPriorityNormal - osPriorityIdle,&LedMatrixTaskHandle);
}

/***************************************************************************/
/* H16R6 message processing task */
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift) {
	Module_Status result = H16R6_OK;

	uint16_t time = 0;
	uint16_t scrollTime = 0;
	uint16_t flashTime = 0;
	uint16_t TimeToFade = 0;
	uint16_t timeBetweenFlash = 0;
	uint16_t interpolationtime = 0;
	uint32_t Number_int;
	float scaledqom;

	switch (code) {
	case CODE_H16R6_SET_RGB:
		LEDMatrixSetRGB(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2],
				cMessage[port - 1][shift + 3], cMessage[port - 1][shift + 4]);
		break;

	case CODE_H16R6_SET_ALL_RGB:
		LEDMatrixSetAllRGB(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2],
				cMessage[port - 1][shift + 3]);
		break;

	case CODE_H16R6_SET_COLOR:
		LEDMatrixSetColor(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2]);
		break;

	case CODE_H16R6_SET_ALL_COLOR:
		LEDMatrixSetAllColor(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1]);
		break;

	case CODE_H16R6_SET_LED_ON:
		LEDMatrixSetLedOn(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1]);
		break;

	case CODE_H16R6_SET_ALL_LED_ON:
		LEDMatrixSetAllLedOn(cMessage[port - 1][shift]);
		break;

	case CODE_H16R6_SET_LED_OFF:
		LEDMatrixSetLedOff(cMessage[port - 1][shift]);
		break;

	case CODE_H16R6_SET_ALL_LED_OFF:
		LEDMatrixSetAllLedOff();
		break;

	case CODE_H16R6_SCROLL_MODE:
		scrollTime = (((uint16_t) cMessage[port - 1][shift + 3])
				+ ((uint16_t) cMessage[port - 1][shift + 4] << 8));
		LEDMatrixScrollMode(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2],
				scrollTime);
		break;

	case CODE_H16R6_FLASH_MODE:
		flashTime = (((uint16_t) cMessage[port - 1][shift + 3])
				+ ((uint16_t) cMessage[port - 1][shift + 4] << 8));
		timeBetweenFlash = (((uint16_t) cMessage[port - 1][shift + 5])
				+ ((uint16_t) cMessage[port - 1][shift + 6] << 8));
		LEDMatrixFlashMode(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2],
				flashTime, timeBetweenFlash);
		break;

	case CODE_H16R6_COLOR_PICKER_MODE:
		time = (((uint16_t) cMessage[port - 1][shift + 1])
				+ ((uint16_t) cMessage[port - 1][shift + 2] << 8));
		LEDMatrixRGBColorPickerMode(cMessage[port - 1][shift], time,
				cMessage[port - 1][shift + 3]);
		break;

	case CODE_H16R6_SET_COLOR_SOME_LED:
		LEDMatrixSetColorSomeLed(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2],
				cMessage[port - 1][shift + 3]);
		break;

	case CODE_H16R6_MOTION_MODE:
		Number_int = (((uint32_t) cMessage[port - 1][shift + 3])
				+ ((uint32_t) cMessage[port - 1][shift + 4] << 8)
				+ ((uint32_t) cMessage[port - 1][shift + 5] << 16)
				+ ((uint32_t) cMessage[port - 1][shift + 6] << 24));
		scaledqom = *((float*) &Number_int);
		LEDMatrixMotionMode(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2], scaledqom);
		break;

	case CODE_H16R6_CROSS_FADE_MODE:
		time = (((uint16_t) cMessage[port - 1][shift + 3])
				+ ((uint16_t) cMessage[port - 1][shift + 4] << 8));
		LEDMatrixCrossFadeMode(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2], time);

		break;

	case CODE_H16R6_CROSS_FADE_MODE_LED_RGB:
		interpolationtime = (((uint16_t) cMessage[port - 1][shift + 4])
				+ ((uint16_t) cMessage[port - 1][shift + 5] << 8));
		LEDMatrixCrossFadeModeLEDRGB(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2],
				cMessage[port - 1][shift + 3], interpolationtime,
				cMessage[port - 1][shift + 6]);
		break;

	case CODE_H16R6_CROSS_FADE_MODE_ALL_LED_RGB:
		interpolationtime = (((uint16_t) cMessage[port - 1][shift + 3])
				+ ((uint16_t) cMessage[port - 1][shift + 4] << 8));
		LEDMatrixCrossFadeModeALLLEDRGB(cMessage[port - 1][shift],
				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2],
				interpolationtime, cMessage[port - 1][shift + 5]);
		break;

//	case CODE_H16R6_SPRINKLEMODE:
//		TimeToFade = (((uint16_t) cMessage[port - 1][shift + 4])
//				+ ((uint16_t) cMessage[port - 1][shift + 5] << 8));
//		LEDMatrixSprinkleMode(cMessage[port - 1][shift],
//				cMessage[port - 1][shift + 1], cMessage[port - 1][shift + 2],
//				TimeToFade,
//				cMessage[port - 1][shift + 6]);
//		break;

	default:
		result = H16R6_ERR_UnknownMessage;
		break;
	}

	return result;
}

/***************************************************************************/
/* Get the port for a given UART */
uint8_t GetPort(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART4)
		return P1;
	else if (huart->Instance == USART2)
		return P2;
	else if (huart->Instance == USART3)
		return P3;
	else if (huart->Instance == USART5)
		return P4;
	else if (huart->Instance == USART1)
		return P5;
	else if (huart->Instance == USART6)
		return P6;

	return 0;
}

/***************************************************************************/
/* Register this module CLI Commands */
void RegisterModuleCLICommands(void) {
	FreeRTOS_CLIRegisterCommand(&CLI_SetRGBCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_SetAllRGBCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_SetColorCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_SetAllColorCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_SetLedOffCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_SetAllLedOffCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_SetLedOnCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_SetAllLedOnCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_ScrollModeCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_FlashModeCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_ColorPickerModeCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_SetColorSomeLedCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_MotionModeCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_CrossFadeModeCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_CrossFadeModeLEDRGBCommandDefinition);
	FreeRTOS_CLIRegisterCommand(&CLI_CrossFadeModeALLLEDRGBCommandDefinition);
//	FreeRTOS_CLIRegisterCommand(&CLI_SprinkleModeCommandDefinition);
}

/***************************************************************************/
/* This functions is useful only for input (sensors) modules.
 * paramIndex: Index of the parameter (1-based index).
 * value: Pointer to store the sampled float value.
 */
Module_Status GetModuleParameter(uint8_t paramIndex, float *value) {
	Module_Status status = BOS_OK;

	switch (paramIndex) {

	/* Invalid parameter index */
	default:
		status = BOS_ERR_WrongParam;
		break;
	}

	return status;
}

/***************************************************************************/
/****************************** Local Functions ****************************/
/***************************************************************************/
void RandomArray(int RandomIndex[65], int NbOfLeds) {
	int i = 0;
	RandomIndex[1] = (rand() % NbOfLeds) + 1;
	for (int led = 2; led <= NbOfLeds; led++) {
		bool flag = true;
		int r = 0;
		while (flag) {
			r = (rand() % NbOfLeds) + 1;
			for (i = 1; i < led; i++) {
				if (r == RandomIndex[i]) {
					break;
				}
			}
			if (i == led) {
				flag = false;
			}
		}
		RandomIndex[led] = r;
	}

}

/* RGBledTask function */
void LedMatrixTask(void *argument){

	/* Infinite loop */
	for(;;){
		/* Switch RGB LED according to its mode */
		switch(LedMatrixMode){
			case SCROLL_MODE:
				LED_Matrix_Scroll_Mode();
				LedMatrixMode = 0;
				break;

			case FLASH_MODE:
				LED_Matrix_Flash_Mode();
				LedMatrixMode = 0;
				break;

			case PICKER_MODE:
				LED_Matrix_RGB_Color_Picker_Mode();
				LedMatrixMode = 0;
				break;

			case FADE_MODE:
				LED_Matrix_Cross_Fade_Mode();
				LedMatrixMode = 0;
				break;

			case FADE_ONE_RGB_LED_MODE:
				LED_Matrix_Cross_Fade_Mode_LED_RGB();
				LedMatrixMode = 0;
				break;

			case FADE_ALL_RGB_LED_MODE:
				LED_Matrix_Cross_Fade_Mode_ALL_LED_RGB();
				LedMatrixMode = 0;
				break;

//			case SPRINKLE_MODE:
//				 LED_Matrix_Sprinkle_Mode();
//				 LedMatrixMode = 0;
//				break;
		}

		taskYIELD();
	}
}
/***************************************************************************/
/***************************** General Functions ***************************/
/***************************************************************************/
/* Set the colors of a single led using RGB color
 * led: position of the led in the string led>=1
 * red: intensity of the red color from 0 to 255
 * green: intensity of the green color from 0 to 255
 * blue: intensity of the blue color from 0 to 255
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 */
Module_Status LEDMatrixSetRGB(uint8_t led, uint8_t red, uint8_t green, uint8_t blue, uint8_t intensity) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	if (led > LED_FRAME_SIZE || led == 0) {
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	if (intensity >= INTINSITY_LED) {
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	memset(&OldColorR[led], red, 1);
	memset(&OldColorG[led], green, 1);
	memset(&OldColorB[led], blue, 1);
	DigiLedSetRGB(led, red, green, blue, intensity);
	DigiLedUpdate(1);
	return Status;
}

/***************************************************************************/
/* Set the colors of all LEDs using RGB color scheme
 * red: intensity of the red color from 0 to 255
 * green: intensity of the green color from 0 to 255
 * blue: intensity of the blue color from 0 to 255
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 */
Module_Status LEDMatrixSetAllRGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t intensity) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	if (intensity >= INTINSITY_LED) {
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	memset(OldColorR, red, sizeof(OldColorR));
	memset(OldColorG, green, sizeof(OldColorG));
	memset(OldColorB, blue, sizeof(OldColorB));
	DigiLedSetAllRGB(red, green, blue, intensity);
	DigiLedUpdate(1);
	return Status;
}

/***************************************************************************/
/* Set the colors of a single led using single colors
 * led: position of the led in the string led>=1
 * color: Set LED color from a predefined color list in "APA102_LedMatrix.h"
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 */
Module_Status LEDMatrixSetColor(uint8_t led, uint8_t color, uint8_t intensity) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	if (led > LED_FRAME_SIZE || led == 0) {
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	if (intensity >= INTINSITY_LED) {
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetColor(led, color, intensity);
	DigiLedUpdate(1);

	return Status;
}

/***************************************************************************/
/* set color of all LEDs in a string
 * color: Set LED color from a predefined color list in "BOS.h"
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 */
Module_Status LEDMatrixSetAllColor(uint8_t color, uint8_t intensity) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	if (intensity >= INTINSITY_LED) {
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetAllColor(color, intensity);
	DigiLedUpdate(1);
	return Status;
}

/***************************************************************************/
/* switch a single led off  led>=1
 * led: position of the led in the string to be switched off
 */
Module_Status LEDMatrixSetLedOff(uint8_t led) {

	Module_Status Status = H16R6_OK;

	if (led >= LED_FRAME_SIZE || led == 0) {
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	memset(&OldColorR[led], 0, 1);
	memset(&OldColorG[led], 0, 1);
	memset(&OldColorB[led], 0, 1);
	DigiLedSetLedOff(led);
	DigiLedUpdate(1);
	return Status;
}

/***************************************************************************/
/* All leds off */
Module_Status LEDMatrixSetAllLedOff() {

	Module_Status Status = H16R6_OK;
	memset(OldColorR, 0, sizeof(OldColorR));
	memset(OldColorG, 0, sizeof(OldColorG));
	memset(OldColorB, 0, sizeof(OldColorB));
	DigiLedSetAllLedOff();
	DigiLedUpdate(1);
	return Status;
}

/***************************************************************************/
/* switch a single led on
 * Using this function will preserve the active color settings for the led
 * led: position of the led in the string to be switched on led>=1
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 */
Module_Status LEDMatrixSetLedOn(uint8_t led, uint8_t intensity) {
	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	if (led >= LED_FRAME_SIZE || led == 0) {
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	if (intensity >= INTINSITY_LED) {
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetLedOn(led, intensity);
	DigiLedUpdate(1);
	return Status;
}

/***************************************************************************/
/* All leds on
 * Using this function will preserve the active color settings for the led led>=1
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 */
Module_Status LEDMatrixSetAllLedOn(uint8_t intensity) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	if (intensity >= INTINSITY_LED) {
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetAllLedOn(intensity);
	DigiLedUpdate(1);
	return Status;
}

/***************************************************************************/
/* Scroll - one row of one colour, the rest another colour, row moves down one for each update
 * baseColour: Basic color
 * scrollRow: Secondary color
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 * scrollTime: Secondary color retention time  value in millisecond.
 */
Module_Status LEDMatrixScrollMode(uint8_t baseColour, uint8_t scrollRow, uint8_t intensity, uint16_t scrollTime) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	BasicColor = baseColour;
	SecondColor = scrollRow;
	Intensity = intensity;
	ScrollTime = scrollTime;
	LedMatrixMode = SCROLL_MODE;
	return Status;
}

Module_Status LED_Matrix_Scroll_Mode() {

	Module_Status Status = H16R6_OK;

	DigiLedScrollMode(BasicColor, SecondColor, Intensity, ScrollTime);
	return Status;
}
/***************************************************************************/
/*Flash - flash from one colour to another with user-settable flash times and intervals
 * baseColour: Basic color
 * flashColour: Secondary color
 * intensity: intensity is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 * flashTime: Color display time value in millisecond.
 * timeBetweenFlash: The time between the display of the two colors value in millisecond.
 */
Module_Status LEDMatrixFlashMode(uint8_t baseColour, uint8_t flashColour, uint8_t intensity, uint16_t flashTime, uint16_t timeBetweenFlash) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	BasicColor = baseColour;
	SecondColor = flashColour;
	Intensity = intensity;
	FlashTime = flashTime;
	TimeBetweenFlash = timeBetweenFlash;
	LedMatrixMode = FLASH_MODE;

	return Status;
}

Module_Status LED_Matrix_Flash_Mode() {

	Module_Status Status = H16R6_OK;

	DigiLedFlashMode(BasicColor, SecondColor, Intensity, FlashTime,
			TimeBetweenFlash);

	return Status;
}

/***************************************************************************/
/* All leds on in the RGBColorPickerMode
 * color: Set LED color from a predefined color list in "BOS.h"
 * time: time between turning on each LED and the next
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 */
Module_Status LEDMatrixRGBColorPickerMode(uint8_t color, uint16_t time, uint8_t intensity) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	Color = color;
	Time = time;
	Intensity = intensity;
	LedMatrixMode = PICKER_MODE;

	return Status;
}

Module_Status LED_Matrix_RGB_Color_Picker_Mode() {

	Module_Status Status = H16R6_OK;

	DigiLedRGBColorPickerMode(Color, Time, Intensity);

	return Status;
}

/***************************************************************************/
/* Set the colors of some of led using single colors
 * StartLed: position of the led in the string led>=1
 * EndLed: position of the led in the string led<=64
 * color: Set LED color from a predefined color list in "BOS.h"
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 */
Module_Status LEDMatrixSetColorSomeLed(uint8_t StartLed, uint8_t EndLed, uint8_t color, uint8_t intensity) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	if (StartLed < 1 || EndLed < 1) {
		StartLed = 1;
		EndLed = 1;
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	if (EndLed > 64) {
		EndLed = 64;
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	for (int i = StartLed; i <= EndLed; i++) {
		LEDMatrixSetColor(i, color, intensity);

	}

	return Status;
}

/***************************************************************************/
/* baseColour: Set the colors of leds According to the value of scaledqom
 * SeconedColor: Set LED color from a predefined color list
 * intensity: is a value from 0 to 100. 0 means no light, and 100 maximum intensity
 * scaledqom: Acceleration values ​​from imu or any sensor
 *
 */
Module_Status LEDMatrixMotionMode(uint8_t baseColour, uint8_t SeconedColor, uint8_t intensity, float scaledqom) {
	intensity = intensity / 10;
	LEDMatrixRGBColorPickerMode(baseColour, scaledqom * 10, intensity);
	Delay_ms(2000);

	if (scaledqom < 0.5) {
		LEDMatrixRGBColorPickerMode(baseColour, scaledqom * 10, intensity);

	} else if (scaledqom > 0.5) {
		LEDMatrixRGBColorPickerMode(SeconedColor, scaledqom * 10, intensity);

	}
}

/***************************************************************************/
/* baseColour: the first color
 * seconedColor: the second color
 * thirdColor: the third color
 * time: Color grading time
 */
Module_Status LEDMatrixCrossFadeMode(uint8_t baseColour, uint8_t seconedColor, uint8_t thirdColor, uint16_t time) {

	Module_Status Status = H16R6_OK;

	BasicColor = baseColour;
	SecondColor = seconedColor;
	ThirdColor = thirdColor;
	Time = time;
	LedMatrixMode = FADE_MODE;

	return Status;

}

Module_Status LED_Matrix_Cross_Fade_Mode() {

	for (int var = 1; var <= 10; var++) {
		LEDMatrixSetAllColor(BasicColor, var);
		osDelay(Time);
	}
	for (int var = 10; var >= 0; var--) {
		LEDMatrixSetAllColor(BasicColor, var);
		osDelay(Time);
	}
	for (int var = 1; var <= 10; var++) {
		LEDMatrixSetAllColor(SecondColor, var);
		osDelay(Time);
	}
	for (int var = 10; var >= 0; var--) {
		LEDMatrixSetAllColor(SecondColor, var);
		osDelay(Time);
	}
	for (int var = 1; var <= 10; var++) {
		LEDMatrixSetAllColor(ThirdColor, var);
		osDelay(Time);
	}
	for (int var = 10; var >= 0; var--) {
		LEDMatrixSetAllColor(ThirdColor, var);
		osDelay(Time);
	}

}

/***************************************************************************/
/* */
Module_Status LEDMatrixCrossFadeModeLEDRGB(uint8_t LED, uint8_t SecondRED,
		uint8_t SecondGREEN, uint8_t SecondBLUE, uint16_t interpolationtime, uint8_t intensity) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	Led = LED;
	BasicColor = SecondRED;
	SecondColor = SecondGREEN;
	ThirdColor = SecondBLUE;
	InterpolationTime = interpolationtime;
	Intensity = intensity;
	LedMatrixMode = FADE_ONE_RGB_LED_MODE;

	return Status;
}

Module_Status LED_Matrix_Cross_Fade_Mode_LED_RGB()
{
	if (Led >= 64) {
		Led = 64;
	}
	if (Led < 1) {
		Led = 1;
	}
	uint8_t NewRED = 0;
	uint8_t NewGREEN = 0;
	uint8_t NewBLUE = 0;
	int16_t DeltaR = BasicColor - OldColorR[Led];
	int16_t DeltaG = SecondColor - OldColorG[Led];
	int16_t DeltaB = ThirdColor - OldColorB[Led];
	float delayTime = (float) InterpolationTime / 100;

	for (uint16_t currentStep = 0; currentStep <= 100; currentStep++) {
		float t = (float) currentStep / (float) 100;
		NewRED = OldColorR[Led] + (uint8_t) (t * DeltaR);
		NewGREEN = OldColorG[Led] + (uint8_t) (t * DeltaG);
		NewBLUE = OldColorB[Led] + (uint8_t) (t * DeltaB);
		if (Intensity >= INTINSITY_LED) {
			Intensity = INTINSITY_LED;
		}
		DigiLedSetRGB(Led, NewRED, NewGREEN, NewBLUE, Intensity);
		DigiLedUpdate(1);

		osDelay(delayTime);
	}
	memset(&OldColorR[Led], NewRED, 1);
	memset(&OldColorG[Led], NewGREEN, 1);
	memset(&OldColorB[Led], NewBLUE, 1);
}
/***************************************************************************/
/* */
Module_Status LEDMatrixCrossFadeModeALLLEDRGB(uint8_t SecondRED,
		uint8_t SecondGREEN, uint8_t SecondBLUE, uint16_t interpolationtime, uint8_t intensity) {

	Module_Status Status = H16R6_OK;
	intensity = intensity / 10;
	BasicColor = SecondRED;
	SecondColor = SecondGREEN;
	ThirdColor = SecondBLUE;
	InterpolationTime = interpolationtime;
	Intensity = intensity;
	LedMatrixMode = FADE_ALL_RGB_LED_MODE;

	return Status;

}


Module_Status LED_Matrix_Cross_Fade_Mode_ALL_LED_RGB()
{
	int16_t DeltaR = 0;
	int16_t DeltaG = 0;
	int16_t DeltaB = 0;
	uint16_t CurrentStep = 0;
	int NLED = 1;
	uint8_t NewRED = 0;
	uint8_t NewGREEN = 0;
	uint8_t NewBLUE = 0;

	if (Intensity >= INTINSITY_LED) {
		Intensity = INTINSITY_LED;
	}
	float delayTime = (float) InterpolationTime / 100;
	for (CurrentStep = 0; CurrentStep <= 40; CurrentStep++) {
		for (NLED = 1; NLED <= LED_FRAME_SIZE; NLED++) {
			float t = (float) CurrentStep / (float) 40;
			DeltaR = BasicColor - OldColorR[NLED];
			DeltaG = SecondColor - OldColorG[NLED];
			DeltaB = ThirdColor - OldColorB[NLED];
			NewRED = OldColorR[NLED] + (uint8_t) (t * DeltaR);
			NewGREEN = OldColorG[NLED] + (uint8_t) (t * DeltaG);
			NewBLUE = OldColorB[NLED] + (uint8_t) (t * DeltaB);

			DigiLedSetRGB(NLED, NewRED, NewGREEN, NewBLUE, Intensity);
			DigiLedUpdate(1);
		}
		if (CurrentStep == 40) {
			memset(OldColorR, NewRED, sizeof(OldColorR));
			memset(OldColorG, NewGREEN, sizeof(OldColorG));
			memset(OldColorB, NewBLUE, sizeof(OldColorB));
		}
		HAL_Delay(delayTime / 10);

	}
}
/***************************************************************************/
/* */
//Module_Status LEDMatrixSprinkleMode(uint8_t TargetColorR, uint8_t TargetColorG,
//		uint8_t TargetColorB, uint16_t TimeToFade, uint8_t ColorDeviation) {
//
//	Module_Status Status = H16R6_OK;
//
//	BasicColor = TargetColorR;
//	SecondColor = TargetColorG;
//	ThirdColor = TargetColorB;
//	AmountOfLeds = 100;
//	TimetoFade = TimeToFade;
//	Colordeviation = ColorDeviation;
//	LedMatrixMode = SPRINKLE_MODE;
//
//	return Status;
//}
//
//Module_Status LED_Matrix_Sprinkle_Mode()
//{
//	if (AmountOfLeds > 100) {
//		AmountOfLeds = 100;
//	}
//	if (AmountOfLeds < 0) {
//		AmountOfLeds = 0;
//	}
//	int NbOfLeds = 0.01 * AmountOfLeds * LED_FRAME_SIZE;
//	int IndexOfLeds = 0, RandDev = 0, TargetColorWithDev = 0,
//			TargetColorPerStep = 0, TargetColor = 0, OldeColor = 0;
//	float DelayTimeStep = TimetoFade / 100.0;
//
//	memcpy(&RandomIndex[0], 0, LED_FRAME_SIZE+1);
//	if (AmountOfLeds == 100) {
//		for (int i = 1; i < LED_FRAME_SIZE; i++) {
//			RandomIndex[i] = i;
//		}
//	} else
//		RandomArray(RandomIndex, NbOfLeds);
//
//	for (int CurrentStep = 0; CurrentStep <= 40; CurrentStep++) {
//		float t = CurrentStep / 40.0;
//		for (int NLED = 1; NLED <= NbOfLeds; NLED++) {
//			IndexOfLeds = RandomIndex[NLED];
//			int RandDev = (rand() % Colordeviation) - (Colordeviation / 2);
//			int TargetColorWithDevR = BasicColor + RandDev;
//			int TargetColorWithDevG = SecondColor + RandDev;
//			int TargetColorWithDevB = ThirdColor + RandDev;
//
//			int maxmax = 255;
//			if (TargetColorWithDevR > 255) {
//				maxmax = TargetColorWithDevR;
//			}
//			if (TargetColorWithDevG > 255) {
//				if (maxmax < TargetColorWithDevG) {
//					maxmax = TargetColorWithDevG;
//				}
//			}
//			if (TargetColorWithDevB > 255) {
//				if (maxmax < TargetColorWithDevB) {
//					maxmax = TargetColorWithDevB;
//				}
//			}
//			if (maxmax > 255) {
//				TargetColorWithDevR = TargetColorWithDevR - (maxmax - 255);
//				TargetColorWithDevG = TargetColorWithDevG - (maxmax - 255);
//				TargetColorWithDevB = TargetColorWithDevB - (maxmax - 255);
//			}
//
//			int minmin = 0;
//			if (TargetColorWithDevR < 0) {
//				minmin = TargetColorWithDevR;
//			}
//			if (TargetColorWithDevG < 0) {
//				if (minmin > TargetColorWithDevG) {
//					minmin = TargetColorWithDevG;
//				}
//			}
//			if (TargetColorWithDevB < 0) {
//				if (minmin < TargetColorWithDevB) {
//					minmin = TargetColorWithDevB;
//				}
//			}
//			if (minmin < 0) {
//				TargetColorWithDevR = TargetColorWithDevR - (minmin);
//				TargetColorWithDevG = TargetColorWithDevG - (minmin);
//				TargetColorWithDevB = TargetColorWithDevB - (minmin);
//			}
//			int TargetColorPerStepR = OldColorR[IndexOfLeds]
//					+ (t * (TargetColorWithDevR - OldColorR[IndexOfLeds]));
//			int TargetColorPerStepG = OldColorG[IndexOfLeds]
//					+ (t * (TargetColorWithDevG - OldColorG[IndexOfLeds]));
//			int TargetColorPerStepB = OldColorB[IndexOfLeds]
//					+ (t * (TargetColorWithDevB - OldColorB[IndexOfLeds]));
//
//			LEDMatrixSetRGB(IndexOfLeds, TargetColorPerStepR,
//					TargetColorPerStepG, TargetColorPerStepB, rand() % 4 + 1);
//
//			if (CurrentStep == 40) {
//				OldColorR[IndexOfLeds] = TargetColorPerStepR;
//				OldColorG[IndexOfLeds] = TargetColorPerStepG;
//				OldColorB[IndexOfLeds] = TargetColorPerStepB;
//			}
//
//		}
//		if (AmountOfLeds < 80) {
//			HAL_Delay(DelayTimeStep);
//		} else {
//			HAL_Delay(DelayTimeStep / 10);
//		}
//	}
//}
/***************************************************************************/
/********************************* Commands ********************************/
/***************************************************************************/
portBASE_TYPE CLI_SetRGBCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t led = 0;
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	uint8_t intensity = 0;
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
			*pcParameterString4, *pcParameterString5;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;
	portBASE_TYPE xParameterStringLength3 = 0, xParameterStringLength4 = 0,
			xParameterStringLength5 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "NumOfLed is %d ,RGB LED is (%d, %d, %d) at intensity %d%%\n\r";
	static const int8_t *pcWrongLedOutRangeMessage =
			(int8_t*) "Wrong LedOutRange!\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	led = (uint8_t) atol((char*) pcParameterString1);
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	red = (uint8_t) atol((char*) pcParameterString2);
	/* Obtain the 3nd parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	green = (uint8_t) atol((char*) pcParameterString3);
	/* Obtain the 4rd parameter string. */
	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
			&xParameterStringLength4);
	blue = (uint8_t) atol((char*) pcParameterString4);
	/* Obtain the 5th parameter string. */
	pcParameterString5 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 5,
			&xParameterStringLength5);
	intensity = (uint8_t) atol((char*) pcParameterString5);

	result = LEDMatrixSetRGB(led, red, green, blue, intensity);

	/* Respond to the command */
	if (result == H16R6_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, led, red, green,
				blue, intensity);
	else if (result == H16R6_ERR_WrongLedOutRange)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_SetAllRGBCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	uint8_t intensity = 0;
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
			*pcParameterString4;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;
	portBASE_TYPE xParameterStringLength3 = 0, xParameterStringLength4 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "ALL Leds ,RGB LED is (%d, %d, %d) at intensity %d%%\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	red = (uint8_t) atol((char*) pcParameterString1);
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	green = (uint8_t) atol((char*) pcParameterString2);
	/* Obtain the 3nd parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	blue = (uint8_t) atol((char*) pcParameterString3);
	/* Obtain the 4rd parameter string. */
	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
			&xParameterStringLength4);
	intensity = (uint8_t) atol((char*) pcParameterString4);

	result = LEDMatrixSetAllRGB(red, green, blue, intensity);

	/* Respond to the command */
	if (result == H16R6_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, red, green, blue,
				intensity);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_SetColorCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t led = 0;
	uint8_t color = 0;
	uint8_t intensity = 0;
	char par[15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;
	portBASE_TYPE xParameterStringLength3 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "NumOfLed is %d ,LED color is %s at intensity %d%%\n\r";
	static const int8_t *pcWrongLedOutRangeMessage =
			(int8_t*) "Wrong LedOutRange!\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	led = (uint8_t) atol((char*) pcParameterString1);
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	/* Read the color value. */
	if (!strncmp((const char*) pcParameterString2, "black",
			xParameterStringLength2))
		color = BLACK;
	else if (!strncmp((const char*) pcParameterString2, "white",
			xParameterStringLength2))
		color = WHITE;
	else if (!strncmp((const char*) pcParameterString2, "red",
			xParameterStringLength2))
		color = RED;
	else if (!strncmp((const char*) pcParameterString2, "blue",
			xParameterStringLength2))
		color = BLUE;
	else if (!strncmp((const char*) pcParameterString2, "yellow",
			xParameterStringLength2))
		color = YELLOW;
	else if (!strncmp((const char*) pcParameterString2, "cyan",
			xParameterStringLength2))
		color = CYAN;
	else if (!strncmp((const char*) pcParameterString2, "magenta",
			xParameterStringLength2))
		color = MAGENTA;
	else if (!strncmp((const char*) pcParameterString2, "green",
			xParameterStringLength2))
		color = GREEN;
	else if (!strncmp((const char*) pcParameterString2, "aqua",
			xParameterStringLength2))
		color = AQUA;
	else if (!strncmp((const char*) pcParameterString2, "purple",
			xParameterStringLength2))
		color = PURPLE;
	else if (!strncmp((const char*) pcParameterString2, "lightblue",
			xParameterStringLength2))
		color = LIGHTBLUE;
	else if (!strncmp((const char*) pcParameterString2, "orange",
			xParameterStringLength2))
		color = ORANGE;
	else if (!strncmp((const char*) pcParameterString2, "indigo",
			xParameterStringLength2))
		color = INDIGO;

	/* Obtain the 3nd parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	intensity = (uint8_t) atol((char*) pcParameterString3);

	result = LEDMatrixSetColor(led, color, intensity);

	/* Respond to the command */
	if (result == H16R6_OK) {
		strncpy(par, (char*) pcParameterString2, xParameterStringLength2);
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, led, par,
				intensity);
	} else if (result == H16R6_ERR_WrongLedOutRange)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_SetAllColorCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t color = 0;
	uint8_t intensity = 0;
	char par[15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "ALL Leds ,LED color is %s at intensity %d%%\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	/* Read the color value. */
	if (!strncmp((const char*) pcParameterString1, "black",
			xParameterStringLength1))
		color = BLACK;
	else if (!strncmp((const char*) pcParameterString1, "white",
			xParameterStringLength1))
		color = WHITE;
	else if (!strncmp((const char*) pcParameterString1, "red",
			xParameterStringLength1))
		color = RED;
	else if (!strncmp((const char*) pcParameterString1, "blue",
			xParameterStringLength1))
		color = BLUE;
	else if (!strncmp((const char*) pcParameterString1, "yellow",
			xParameterStringLength1))
		color = YELLOW;
	else if (!strncmp((const char*) pcParameterString1, "cyan",
			xParameterStringLength1))
		color = CYAN;
	else if (!strncmp((const char*) pcParameterString1, "magenta",
			xParameterStringLength1))
		color = MAGENTA;
	else if (!strncmp((const char*) pcParameterString1, "green",
			xParameterStringLength1))
		color = GREEN;
	else if (!strncmp((const char*) pcParameterString1, "aqua",
			xParameterStringLength1))
		color = AQUA;
	else if (!strncmp((const char*) pcParameterString1, "purple",
			xParameterStringLength1))
		color = PURPLE;
	else if (!strncmp((const char*) pcParameterString1, "lightblue",
			xParameterStringLength1))
		color = LIGHTBLUE;
	else if (!strncmp((const char*) pcParameterString1, "orange",
			xParameterStringLength1))
		color = ORANGE;
	else if (!strncmp((const char*) pcParameterString1, "indigo",
			xParameterStringLength1))
		color = INDIGO;
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	intensity = (uint8_t) atol((char*) pcParameterString2);

	result = LEDMatrixSetAllColor(color, intensity);

	/* Respond to the command */
	if (result == H16R6_OK) {
		strncpy(par, (char*) pcParameterString1, xParameterStringLength1);
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, par, intensity);
	} else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_SetLedOffCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t led = 0;

	static int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 = 0;

	static const int8_t *pcOKMessage = (int8_t*) "NumOfLed OFF is %d \n\r";
	static const int8_t *pcWrongLedOutRangeMessage =
			(int8_t*) "Wrong LedOutRange!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	led = (uint8_t) atol((char*) pcParameterString1);

	result = LEDMatrixSetLedOff(led);

	/* Respond to the command */
	if (result == H16R6_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, led);
	else if (result == H16R6_ERR_WrongLedOutRange)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}
/***************************************************************************/
portBASE_TYPE CLI_SetAllLedOffCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;

	static const int8_t *pcOKMessage = (int8_t*) "ALL Leds OFF\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	result = LEDMatrixSetAllLedOff();

	/* Respond to the command */
	if (result == H16R6_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_SetLedOnCommand(int8_t *pcWriteBuffer, size_t xWriteBufferLen,
		const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t led = 0;
	uint8_t intensity = 0;
	static int8_t *pcParameterString1, *pcParameterString2;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "NumOfLed ON is %d at intensity %d%%\n\r";
	static const int8_t *pcWrongLedOutRangeMessage =
			(int8_t*) "Wrong LedOutRange!\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	led = (uint8_t) atol((char*) pcParameterString1);
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	intensity = (uint8_t) atol((char*) pcParameterString2);

	result = LEDMatrixSetLedOn(led, intensity);

	/* Respond to the command */
	if (result == H16R6_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, led, intensity);
	else if (result == H16R6_ERR_WrongLedOutRange)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_SetAllLedOnCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t intensity = 0;
	static int8_t *pcParameterString1;
	portBASE_TYPE xParameterStringLength1 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "ALL Leds ON at intensity %d%%\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	intensity = (uint8_t) atol((char*) pcParameterString1);

	result = LEDMatrixSetAllLedOn(intensity);

	/* Respond to the command */
	if (result == H16R6_OK)
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, intensity);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_ScrollModeCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t baseColour = 0;
	uint8_t scrollRow = 0;
	uint8_t intensity = 0;
	uint16_t scrollTime;
	char par[2][15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
			*pcParameterString4;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0,
			xParameterStringLength3 = 0, xParameterStringLength4 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "ScrollMode:baseColour is %s and scrollRow is %s and intensity %d %% and scrollTime is %d \n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	/* Read the color value. */
	if (!strncmp((const char*) pcParameterString1, "black",
			xParameterStringLength1))
		baseColour = BLACK;
	else if (!strncmp((const char*) pcParameterString1, "white",
			xParameterStringLength1))
		baseColour = WHITE;
	else if (!strncmp((const char*) pcParameterString1, "red",
			xParameterStringLength1))
		baseColour = RED;
	else if (!strncmp((const char*) pcParameterString1, "blue",
			xParameterStringLength1))
		baseColour = BLUE;
	else if (!strncmp((const char*) pcParameterString1, "yellow",
			xParameterStringLength1))
		baseColour = YELLOW;
	else if (!strncmp((const char*) pcParameterString1, "cyan",
			xParameterStringLength1))
		baseColour = CYAN;
	else if (!strncmp((const char*) pcParameterString1, "magenta",
			xParameterStringLength1))
		baseColour = MAGENTA;
	else if (!strncmp((const char*) pcParameterString1, "green",
			xParameterStringLength1))
		baseColour = GREEN;
	else if (!strncmp((const char*) pcParameterString1, "aqua",
			xParameterStringLength1))
		baseColour = AQUA;
	else if (!strncmp((const char*) pcParameterString1, "purple",
			xParameterStringLength1))
		baseColour = PURPLE;
	else if (!strncmp((const char*) pcParameterString1, "lightblue",
			xParameterStringLength1))
		baseColour = LIGHTBLUE;
	else if (!strncmp((const char*) pcParameterString1, "orange",
			xParameterStringLength1))
		baseColour = ORANGE;
	else if (!strncmp((const char*) pcParameterString1, "indigo",
			xParameterStringLength1))
		baseColour = INDIGO;
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	/* Read the color value. */
	if (!strncmp((const char*) pcParameterString2, "black",
			xParameterStringLength2))
		scrollRow = BLACK;
	else if (!strncmp((const char*) pcParameterString2, "white",
			xParameterStringLength2))
		scrollRow = WHITE;
	else if (!strncmp((const char*) pcParameterString2, "red",
			xParameterStringLength2))
		scrollRow = RED;
	else if (!strncmp((const char*) pcParameterString2, "blue",
			xParameterStringLength2))
		scrollRow = BLUE;
	else if (!strncmp((const char*) pcParameterString2, "yellow",
			xParameterStringLength2))
		scrollRow = YELLOW;
	else if (!strncmp((const char*) pcParameterString2, "cyan",
			xParameterStringLength2))
		scrollRow = CYAN;
	else if (!strncmp((const char*) pcParameterString2, "magenta",
			xParameterStringLength2))
		scrollRow = MAGENTA;
	else if (!strncmp((const char*) pcParameterString2, "green",
			xParameterStringLength2))
		scrollRow = GREEN;
	else if (!strncmp((const char*) pcParameterString2, "aqua",
			xParameterStringLength2))
		scrollRow = AQUA;
	else if (!strncmp((const char*) pcParameterString2, "purple",
			xParameterStringLength2))
		scrollRow = PURPLE;
	else if (!strncmp((const char*) pcParameterString2, "lightblue",
			xParameterStringLength2))
		scrollRow = LIGHTBLUE;
	else if (!strncmp((const char*) pcParameterString2, "orange",
			xParameterStringLength2))
		scrollRow = ORANGE;
	else if (!strncmp((const char*) pcParameterString2, "indigo",
			xParameterStringLength2))
		scrollRow = INDIGO;
	/* Obtain the 3st parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	intensity = (uint8_t) atol((char*) pcParameterString3);

	/* Obtain the 4st parameter string. */
	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
			&xParameterStringLength4);
	scrollTime = (uint16_t) atol((char*) pcParameterString4);

	result = LEDMatrixScrollMode(baseColour, scrollRow, intensity, scrollTime);

	/* Respond to the command */
	if (result == H16R6_OK) {
		strncpy(par[0], (char*) pcParameterString1, xParameterStringLength1);
		strncpy(par[1], (char*) pcParameterString2, xParameterStringLength2);

		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, par[0], par[1],
				intensity, scrollTime);
	} else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;
}

/***************************************************************************/
portBASE_TYPE CLI_FlashModeCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t baseColour = 0;
	uint8_t flashColour = 0;
	uint8_t intensity = 0;
	uint16_t flashTime;
	uint16_t timeBetweenFlash;
	char par[2][15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
			*pcParameterString4, *pcParameterString5;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0,
			xParameterStringLength3 = 0, xParameterStringLength4 = 0,
			xParameterStringLength5 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "FlashMode:baseColour is %s and flashColour is %s and intensity %d %% and flashTime is %d and timeBetweenFlash is %d \n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	/* Read the color value. */
	if (!strncmp((const char*) pcParameterString1, "black",
			xParameterStringLength1))
		baseColour = BLACK;
	else if (!strncmp((const char*) pcParameterString1, "white",
			xParameterStringLength1))
		baseColour = WHITE;
	else if (!strncmp((const char*) pcParameterString1, "red",
			xParameterStringLength1))
		baseColour = RED;
	else if (!strncmp((const char*) pcParameterString1, "blue",
			xParameterStringLength1))
		baseColour = BLUE;
	else if (!strncmp((const char*) pcParameterString1, "yellow",
			xParameterStringLength1))
		baseColour = YELLOW;
	else if (!strncmp((const char*) pcParameterString1, "cyan",
			xParameterStringLength1))
		baseColour = CYAN;
	else if (!strncmp((const char*) pcParameterString1, "magenta",
			xParameterStringLength1))
		baseColour = MAGENTA;
	else if (!strncmp((const char*) pcParameterString1, "green",
			xParameterStringLength1))
		baseColour = GREEN;
	else if (!strncmp((const char*) pcParameterString1, "aqua",
			xParameterStringLength1))
		baseColour = AQUA;
	else if (!strncmp((const char*) pcParameterString1, "purple",
			xParameterStringLength1))
		baseColour = PURPLE;
	else if (!strncmp((const char*) pcParameterString1, "lightblue",
			xParameterStringLength1))
		baseColour = LIGHTBLUE;
	else if (!strncmp((const char*) pcParameterString1, "orange",
			xParameterStringLength1))
		baseColour = ORANGE;
	else if (!strncmp((const char*) pcParameterString1, "indigo",
			xParameterStringLength1))
		baseColour = INDIGO;
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	/* Read the color value. */
	if (!strncmp((const char*) pcParameterString2, "black",
			xParameterStringLength2))
		flashColour = BLACK;
	else if (!strncmp((const char*) pcParameterString2, "white",
			xParameterStringLength2))
		flashColour = WHITE;
	else if (!strncmp((const char*) pcParameterString2, "red",
			xParameterStringLength2))
		flashColour = RED;
	else if (!strncmp((const char*) pcParameterString2, "blue",
			xParameterStringLength2))
		flashColour = BLUE;
	else if (!strncmp((const char*) pcParameterString2, "yellow",
			xParameterStringLength2))
		flashColour = YELLOW;
	else if (!strncmp((const char*) pcParameterString2, "cyan",
			xParameterStringLength2))
		flashColour = CYAN;
	else if (!strncmp((const char*) pcParameterString2, "magenta",
			xParameterStringLength2))
		flashColour = MAGENTA;
	else if (!strncmp((const char*) pcParameterString2, "green",
			xParameterStringLength2))
		flashColour = GREEN;
	else if (!strncmp((const char*) pcParameterString2, "aqua",
			xParameterStringLength2))
		flashColour = AQUA;
	else if (!strncmp((const char*) pcParameterString2, "purple",
			xParameterStringLength2))
		flashColour = PURPLE;
	else if (!strncmp((const char*) pcParameterString2, "lightblue",
			xParameterStringLength2))
		flashColour = LIGHTBLUE;
	else if (!strncmp((const char*) pcParameterString2, "orange",
			xParameterStringLength2))
		flashColour = ORANGE;
	else if (!strncmp((const char*) pcParameterString2, "indigo",
			xParameterStringLength2))
		flashColour = INDIGO;
	/* Obtain the 3st parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	intensity = (uint8_t) atol((char*) pcParameterString3);

	/* Obtain the 4st parameter string. */
	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
			&xParameterStringLength4);
	flashTime = (uint16_t) atol((char*) pcParameterString4);

	/* Obtain the 5st parameter string. */
	pcParameterString5 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 5,
			&xParameterStringLength5);
	timeBetweenFlash = (uint16_t) atol((char*) pcParameterString5);

	result = LEDMatrixFlashMode(baseColour, flashColour, intensity, flashTime,
			timeBetweenFlash);

	/* Respond to the command */
	if (result == H16R6_OK) {
		strncpy(par[0], (char*) pcParameterString1, xParameterStringLength1);
		strncpy(par[1], (char*) pcParameterString2, xParameterStringLength2);

		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, par[0], par[1],
				intensity, flashTime, timeBetweenFlash);
	} else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;

}

/***************************************************************************/
portBASE_TYPE CLI_ColorPickerModeCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t Colour = 0;
	uint16_t Time;
	uint8_t intensity = 0;
	char par[2][15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0,
			xParameterStringLength3 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "ColorPickerMode:Colour is %s and Time is %d and and intensity %d %% \n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	/* Read the color value. */
	if (!strncmp((const char*) pcParameterString1, "black",
			xParameterStringLength1))
		Colour = BLACK;
	else if (!strncmp((const char*) pcParameterString1, "white",
			xParameterStringLength1))
		Colour = WHITE;
	else if (!strncmp((const char*) pcParameterString1, "red",
			xParameterStringLength1))
		Colour = RED;
	else if (!strncmp((const char*) pcParameterString1, "blue",
			xParameterStringLength1))
		Colour = BLUE;
	else if (!strncmp((const char*) pcParameterString1, "yellow",
			xParameterStringLength1))
		Colour = YELLOW;
	else if (!strncmp((const char*) pcParameterString1, "cyan",
			xParameterStringLength1))
		Colour = CYAN;
	else if (!strncmp((const char*) pcParameterString1, "magenta",
			xParameterStringLength1))
		Colour = MAGENTA;
	else if (!strncmp((const char*) pcParameterString1, "green",
			xParameterStringLength1))
		Colour = GREEN;
	else if (!strncmp((const char*) pcParameterString1, "aqua",
			xParameterStringLength1))
		Colour = AQUA;
	else if (!strncmp((const char*) pcParameterString1, "purple",
			xParameterStringLength1))
		Colour = PURPLE;
	else if (!strncmp((const char*) pcParameterString1, "lightblue",
			xParameterStringLength1))
		Colour = LIGHTBLUE;
	else if (!strncmp((const char*) pcParameterString1, "orange",
			xParameterStringLength1))
		Colour = ORANGE;
	else if (!strncmp((const char*) pcParameterString1, "indigo",
			xParameterStringLength1))
		Colour = INDIGO;

	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	Time = (uint16_t) atol((char*) pcParameterString2);

	/* Obtain the 3st parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	intensity = (uint8_t) atol((char*) pcParameterString3);

	result = LEDMatrixRGBColorPickerMode(Colour, Time, intensity);

	/* Respond to the command */
	if (result == H16R6_OK) {
		strncpy(par[0], (char*) pcParameterString1, xParameterStringLength1);

		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, par[0], Time,
				intensity);
	} else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;

}

/***************************************************************************/
portBASE_TYPE CLI_SetColorSomeLedCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t StartLed = 0;
	uint8_t EndLed = 0;
	uint8_t color = 0;
	uint8_t intensity = 0;
	char par[15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
			*pcParameterString4;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;
	portBASE_TYPE xParameterStringLength3 = 0, xParameterStringLength4 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "StartLed is %d ,EndLed is %d,LED color is %s at intensity %d%%\n\r";
	static const int8_t *pcWrongLedOutRangeMessage =
			(int8_t*) "Wrong LedOutRange!\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	StartLed = (uint8_t) atol((char*) pcParameterString1);
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	EndLed = (uint8_t) atol((char*) pcParameterString2);

	/* Obtain the 3st parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	/* Read the color value. */
	if (!strncmp((const char*) pcParameterString3, "black",
			xParameterStringLength3))
		color = BLACK;
	else if (!strncmp((const char*) pcParameterString3, "white",
			xParameterStringLength3))
		color = WHITE;
	else if (!strncmp((const char*) pcParameterString3, "red",
			xParameterStringLength3))
		color = RED;
	else if (!strncmp((const char*) pcParameterString3, "blue",
			xParameterStringLength3))
		color = BLUE;
	else if (!strncmp((const char*) pcParameterString3, "yellow",
			xParameterStringLength3))
		color = YELLOW;
	else if (!strncmp((const char*) pcParameterString3, "cyan",
			xParameterStringLength3))
		color = CYAN;
	else if (!strncmp((const char*) pcParameterString3, "magenta",
			xParameterStringLength3))
		color = MAGENTA;
	else if (!strncmp((const char*) pcParameterString3, "green",
			xParameterStringLength3))
		color = GREEN;
	else if (!strncmp((const char*) pcParameterString3, "aqua",
			xParameterStringLength3))
		color = AQUA;
	else if (!strncmp((const char*) pcParameterString3, "purple",
			xParameterStringLength3))
		color = PURPLE;
	else if (!strncmp((const char*) pcParameterString3, "lightblue",
			xParameterStringLength3))
		color = LIGHTBLUE;
	else if (!strncmp((const char*) pcParameterString3, "orange",
			xParameterStringLength3))
		color = ORANGE;
	else if (!strncmp((const char*) pcParameterString3, "indigo",
			xParameterStringLength3))
		color = INDIGO;

	/* Obtain the 4nd parameter string. */
	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
			&xParameterStringLength4);
	intensity = (uint8_t) atol((char*) pcParameterString4);

	result = LEDMatrixSetColorSomeLed(StartLed, EndLed, color, intensity);

	/* Respond to the command */
	if (result == H16R6_OK) {
		strncpy(par, (char*) pcParameterString3, xParameterStringLength3);
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, StartLed, EndLed,
				par, intensity);
	} else if (result == H16R6_ERR_WrongLedOutRange)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;

}

/***************************************************************************/
portBASE_TYPE CLI_MotionModeCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t BaseColor = 0;
	uint8_t SecondColor = 0;
	uint8_t Intensity = 0;
	float ScaledQom = 0;
	char par[15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
			*pcParameterString4;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;
	portBASE_TYPE xParameterStringLength3 = 0, xParameterStringLength4 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "base color is %d, second color is %d, intensity is %d%, ScaledQom is %f\n\r";
	static const int8_t *pcWrongLedOutRangeMessage =
			(int8_t*) "Wrong LedOutRange!\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	BaseColor = (uint8_t) atol((char*) pcParameterString1);
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	SecondColor = (uint8_t) atol((char*) pcParameterString2);

	/* Obtain the 3st parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	Intensity = (uint8_t) atol((char*) pcParameterString3);

	/* Obtain the 4nd parameter string. */
	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
			&xParameterStringLength4);
	ScaledQom = (float) atof((char*) pcParameterString4);

	result = LEDMatrixMotionMode(BaseColor, SecondColor, Intensity, ScaledQom);

	/* Respond to the command */
	if (result == H16R6_OK) {
//		strncpy(par, (char*) pcParameterString3, xParameterStringLength3);
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, BaseColor, SecondColor,
				Intensity, ScaledQom);
	} else if (result == H16R6_ERR_WrongLedOutRange)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;

}

/***************************************************************************/
portBASE_TYPE CLI_CrossFadeModeCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t BaseColor = 0;
	uint8_t SecondColor = 0;
	uint8_t thirdcolor = 0;
	uint16_t time = 0;
	char par[15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
			*pcParameterString4;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;
	portBASE_TYPE xParameterStringLength3 = 0, xParameterStringLength4 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "base color is %d, second color is %d, third color is %d, time is %d\n\r";
	static const int8_t *pcWrongLedOutRangeMessage =
			(int8_t*) "Wrong LedOutRange!\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	BaseColor = (uint8_t) atol((char*) pcParameterString1);
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	SecondColor = (uint8_t) atol((char*) pcParameterString2);

	/* Obtain the 3st parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	thirdcolor = (uint8_t) atol((char*) pcParameterString3);

	/* Obtain the 4nd parameter string. */
	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
			&xParameterStringLength4);
	time = (uint16_t) atol((char*) pcParameterString4);

	result = LEDMatrixCrossFadeMode(BaseColor, SecondColor, thirdcolor, time);

	/* Respond to the command */
	if (result == H16R6_OK) {
//		strncpy(par, (char*) pcParameterString3, xParameterStringLength3);
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, BaseColor, SecondColor,
				thirdcolor, time);
	} else if (result == H16R6_ERR_WrongLedOutRange)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;

}

/***************************************************************************/
portBASE_TYPE CLI_CrossFadeModeLEDRGBCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t Led = 0;
	uint8_t SecondRED = 0;
	uint8_t SecondGreen = 0;
	uint8_t SecondBlue = 0;
	uint8_t Intensity = 0;
	uint16_t InterPolationTime = 0;
	char par[15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
			*pcParameterString4, *pcParameterString5, *pcParameterString6;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;
	portBASE_TYPE xParameterStringLength3 = 0, xParameterStringLength4 = 0;
	portBASE_TYPE xParameterStringLength5 = 0, xParameterStringLength6 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "Led is %d ,RED is %d, Green is %d, Blue is %d, Intensity is %d%%, InterPolationTime is %d\n\r";
	static const int8_t *pcWrongLedOutRangeMessage =
			(int8_t*) "Wrong LedOutRange!\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 1st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	Led = (uint8_t) atol((char*) pcParameterString1);
	/* Obtain the 2st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	SecondRED = (uint8_t) atol((char*) pcParameterString2);

	/* Obtain the 3st parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	SecondGreen = (uint8_t) atol((char*) pcParameterString3);

	/* Obtain the 4nd parameter string. */
	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
			&xParameterStringLength4);
	SecondBlue = (uint8_t) atol((char*) pcParameterString4);

	/* Obtain the 5nd parameter string. */
	pcParameterString5 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 5,
			&xParameterStringLength5);
	Intensity = (uint8_t) atol((char*) pcParameterString5);

	/* Obtain the 6nd parameter string. */
	pcParameterString6 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 6,
			&xParameterStringLength6);
	InterPolationTime = (uint16_t) atol((char*) pcParameterString6);

	result = LEDMatrixCrossFadeModeLEDRGB(Led, SecondRED, SecondGreen, SecondBlue, InterPolationTime, Intensity);

	/* Respond to the command */
	if (result == H16R6_OK) {
//		strncpy(par, (char*) pcParameterString3, xParameterStringLength3);
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, Led, SecondRED,
				SecondGreen, SecondBlue, Intensity, InterPolationTime);
	} else if (result == H16R6_ERR_WrongLedOutRange)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;

}

/***************************************************************************/
portBASE_TYPE CLI_CrossFadeModeALLLEDRGBCommand(int8_t *pcWriteBuffer,
		size_t xWriteBufferLen, const int8_t *pcCommandString) {
	Module_Status result = H16R6_OK;
	uint8_t SecondRED = 0;
	uint8_t SecondGreen = 0;
	uint8_t SecondBlue = 0;
	uint8_t Intensity = 0;
	uint16_t InterPolationTime = 0;
	char par[15] = { 0 };
	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
			*pcParameterString4, *pcParameterString5;
	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;
	portBASE_TYPE xParameterStringLength3 = 0, xParameterStringLength4 = 0;
	portBASE_TYPE xParameterStringLength5 = 0;

	static const int8_t *pcOKMessage =
			(int8_t*) "RED is %d, Green is %d, Blue is %d, Intensity is %d%%, InterPolationTime is %d\n\r";
	static const int8_t *pcWrongLedOutRangeMessage =
			(int8_t*) "Wrong LedOutRange!\n\r";
	static const int8_t *pcWrongIntensityMessage =
			(int8_t*) "Wrong intensity!\n\r";

	(void) xWriteBufferLen;
	configASSERT(pcWriteBuffer);

	/* Obtain the 2st parameter string. */
	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
			&xParameterStringLength1);
	SecondRED = (uint8_t) atol((char*) pcParameterString1);

	/* Obtain the 3st parameter string. */
	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
			&xParameterStringLength2);
	SecondGreen = (uint8_t) atol((char*) pcParameterString2);

	/* Obtain the 4nd parameter string. */
	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
			&xParameterStringLength3);
	SecondBlue = (uint8_t) atol((char*) pcParameterString3);

	/* Obtain the 5nd parameter string. */
	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
			&xParameterStringLength4);
	Intensity = (uint8_t) atol((char*) pcParameterString4);

	/* Obtain the 6nd parameter string. */
	pcParameterString5 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 5,
			&xParameterStringLength5);
	InterPolationTime = (uint16_t) atol((char*) pcParameterString5);

	result = LEDMatrixCrossFadeModeALLLEDRGB(SecondRED, SecondGreen, SecondBlue, Intensity, InterPolationTime);

	/* Respond to the command */
	if (result == H16R6_OK) {
		strncpy(par, (char*) pcParameterString3, xParameterStringLength3);
		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, SecondRED,
				SecondGreen, SecondBlue, Intensity, InterPolationTime);
	} else if (result == H16R6_ERR_WrongLedOutRange)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);
	else if (result == H16R6_ERR_WrongIntensity)
		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);

	/* There is no more data to return after this single string, so return
	 pdFALSE. */
	return pdFALSE;

}

///***************************************************************************/
//portBASE_TYPE CLI_SprinkleModeCommand(int8_t *pcWriteBuffer,
//		size_t xWriteBufferLen, const int8_t *pcCommandString) {
//	Module_Status result = H16R6_OK;
//	uint8_t TargetColorRed = 0;
//	uint8_t TargetColorGreen = 0;
//	uint8_t TargetColorBlue = 0;
//	uint8_t AmountOfLeds = 0;
//	uint8_t ColorDeviation = 0;
//	uint16_t TimeTOFade = 0;
//
//	char par[15] = { 0 };
//	static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3,
//			*pcParameterString4, *pcParameterString5, *pcParameterString6;
//	portBASE_TYPE xParameterStringLength1 = 0, xParameterStringLength2 = 0;
//	portBASE_TYPE xParameterStringLength3 = 0, xParameterStringLength4 = 0;
//	portBASE_TYPE xParameterStringLength5 = 0, xParameterStringLength6 = 0;
//
//	static const int8_t *pcOKMessage =
//			(int8_t*) "TargetColorRed is %d ,TargetColorGreen is %d,TargetColorBlue is %d, AmountOfLeds is %d, ColorDeviation is %d, TimeTOFade is %d\n\r";
//	static const int8_t *pcWrongLedOutRangeMessage =
//			(int8_t*) "Wrong LedOutRange!\n\r";
//	static const int8_t *pcWrongIntensityMessage =
//			(int8_t*) "Wrong intensity!\n\r";
//
//	(void) xWriteBufferLen;
//	configASSERT(pcWriteBuffer);
//
//	/* Obtain the 1st parameter string. */
//	pcParameterString1 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 1,
//			&xParameterStringLength1);
//	TargetColorRed = (uint8_t) atol((char*) pcParameterString1);
//	/* Obtain the 2st parameter string. */
//	pcParameterString2 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 2,
//			&xParameterStringLength2);
//	TargetColorGreen = (uint8_t) atol((char*) pcParameterString2);
//
//	/* Obtain the 3st parameter string. */
//	pcParameterString3 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 3,
//			&xParameterStringLength3);
//	TargetColorBlue = (uint8_t) atol((char*) pcParameterString3);
//
//
//	/* Obtain the 4nd parameter string. */
//	pcParameterString4 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 4,
//			&xParameterStringLength4);
//	ColorDeviation = (uint8_t) atol((char*) pcParameterString4);
//
//	/* Obtain the 4nd parameter string. */
//	pcParameterString5 = (int8_t*) FreeRTOS_CLIGetParameter(pcCommandString, 5,
//			&xParameterStringLength5);
//	TimeTOFade = (uint16_t) atol((char*) pcParameterString5);
//
//	result = LEDMatrixSprinkleMode(TargetColorRed, TargetColorGreen, TargetColorBlue, TimeTOFade, ColorDeviation);
//
//	/* Respond to the command */
//	if (result == H16R6_OK) {
////		strncpy(par, (char*) pcParameterString3, xParameterStringLength3);
//		sprintf((char*) pcWriteBuffer, (char*) pcOKMessage, TargetColorRed, TargetColorGreen,
//				TargetColorBlue, AmountOfLeds, ColorDeviation, TimeTOFade);
//	} else if (result == H16R6_ERR_WrongLedOutRange)
//		strcpy((char*) pcWriteBuffer, (char*) pcWrongLedOutRangeMessage);
//	else if (result == H16R6_ERR_WrongIntensity)
//		strcpy((char*) pcWriteBuffer, (char*) pcWrongIntensityMessage);
//
//	/* There is no more data to return after this single string, so return
//	 pdFALSE. */
//	return pdFALSE;
//
//}
/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
