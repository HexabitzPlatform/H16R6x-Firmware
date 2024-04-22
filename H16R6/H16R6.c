/*
 BitzOS (BOS) V0.3.2 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved

 File Name     : H16R6.c
 Description   : Source code for module H16R6.
 	 	 	 	 (Description_of_module)

(Description of Special module peripheral configuration):
>>
>>
>>

 */

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include "H16R6_inputs.h"



/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;
/* Exported variables */
extern FLASH_ProcessTypeDef pFlash;
extern uint8_t numOfRecordedSnippets;

/* Local functions */
/* functions */
void DigiLed_init();
void DigiLed_setColor(uint8_t led, uint8_t red, uint8_t green, uint8_t blue,uint8_t illumination);
void DigiLed_setAllColor(uint8_t red, uint8_t green, uint8_t blue,uint8_t illumination);
void DigiLed_setRGB(uint8_t led, uint32_t rgb,uint8_t illumination);
void DigiLed_setAllRGB(uint32_t rgb,uint8_t illumination);
void DigiLed_setLedOff(uint8_t led);
void DigiLed_setAllLedOff();
void DigiLed_setLedOn(uint8_t led,uint8_t illumination);
void DigiLed_setAllLedOn(uint8_t illumination);
void DigiLed_update(uint8_t forceUpdate);
uint8_t DigiLed_getFrameSize(void);
uint8_t DigiLed_TestPosition(uint8_t led);

/* Module exported parameters ------------------------------------------------*/
module_param_t modParam[NUM_MODULE_PARAMS] ={{.paramPtr = NULL, .paramFormat =FMT_FLOAT, .paramName =""}};

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void ExecuteMonitor(void);
void FLASH_Page_Eras(uint32_t Addr );
Module_Status ConvertTwosComplToDec(uint16_t twosComplVal, int16_t *sgnDecimalVal);
Module_Status BAT_ReadIdReg(uint16_t regAddress, uint16_t *Buffer, uint8_t NoBytes);
/* Create CLI commands --------------------------------------------------------*/


/* CLI command structure : demo */

/*-----------------------------------------------------------*/


/* ---------------------------------------------------------------------
 |							 Private Functions	                	   |
 ----------------------------------------------------------------------- 
 */

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow : 
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 48000000
 *            HCLK(Hz)                       = 48000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            HSE Frequency(Hz)              = 8000000
 *            PREDIV                         = 1
 *            PLLMUL                         = 6
 *            Flash Latency(WS)              = 1
 * @param  None
 * @retval None
 */
void SystemClock_Config(void){
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI
			| RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
	RCC_OscInitStruct.PLL.PLLN = 12;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	/** Initializes the peripherals clocks
	 */
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_USART2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1;
	PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLKSOURCE_PCLK1;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	__SYSCFG_CLK_ENABLE();

	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	
}

/*-----------------------------------------------------------*/


/* --- Save array topology and Command Snippets in Flash RO --- 
 */
uint8_t SaveToRO(void){
	BOS_Status result =BOS_OK;
	HAL_StatusTypeDef FlashStatus =HAL_OK;
	uint16_t add =8;
    uint16_t temp =0;
	uint8_t snipBuffer[sizeof(snippet_t) + 1] ={0};
	
	HAL_FLASH_Unlock();
	/* Erase RO area */
	FLASH_PageErase(FLASH_BANK_1,RO_START_ADDRESS);
	FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
	FLASH_PageErase(FLASH_BANK_1,RO_MID_ADDRESS);
	//TOBECHECKED
	FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
	if(FlashStatus != HAL_OK){
		return pFlash.ErrorCode;
	}
	else{
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR,FLASH_CR_PER);
	}
	
	/* Save number of modules and myID */
	if(myID){
		temp =(uint16_t )(N << 8) + myID;
		//HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,RO_START_ADDRESS,temp);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,RO_START_ADDRESS,temp);
		//TOBECHECKED
		FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
		if(FlashStatus != HAL_OK){
			return pFlash.ErrorCode;
		}
		else{
			/* If the program operation is completed, disable the PG Bit */
			CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
		}
		
		/* Save topology */
		for(uint8_t i =1; i <= N; i++){
			for(uint8_t j =0; j <= MaxNumOfPorts; j++){
				if(array[i - 1][0]){

          	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,RO_START_ADDRESS + add,array[i - 1][j]);
				 //HALFWORD 	//TOBECHECKED
					FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
					if(FlashStatus != HAL_OK){
						return pFlash.ErrorCode;
					}
					else{
						/* If the program operation is completed, disable the PG Bit */
						CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
						add +=8;
					}
				}
			}
		}
	}
	
	// Save Command Snippets
	int currentAdd = RO_MID_ADDRESS;
	for(uint8_t s =0; s < numOfRecordedSnippets; s++){
		if(snippets[s].cond.conditionType){
			snipBuffer[0] =0xFE;		// A marker to separate Snippets
			memcpy((uint32_t* )&snipBuffer[1],(uint8_t* )&snippets[s],sizeof(snippet_t));
			// Copy the snippet struct buffer (20 x numOfRecordedSnippets). Note this is assuming sizeof(snippet_t) is even.
			for(uint8_t j =0; j < (sizeof(snippet_t)/4); j++){
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,currentAdd,*(uint64_t* )&snipBuffer[j*8]);
				//HALFWORD
				//TOBECHECKED
				FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
				if(FlashStatus != HAL_OK){
					return pFlash.ErrorCode;
				}
				else{
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
					currentAdd +=8;
				}
			}
			// Copy the snippet commands buffer. Always an even number. Note the string termination char might be skipped
			for(uint8_t j =0; j < ((strlen(snippets[s].cmd) + 1)/4); j++){
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,currentAdd,*(uint64_t* )(snippets[s].cmd + j*4 ));
				//HALFWORD
				//TOBECHECKED
				FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
				if(FlashStatus != HAL_OK){
					return pFlash.ErrorCode;
				}
				else{
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
					currentAdd +=8;
				}
			}
		}
	}
	
	HAL_FLASH_Lock();
	
	return result;
}

/* --- Clear array topology in SRAM and Flash RO --- 
 */
uint8_t ClearROtopology(void){
	// Clear the array 
	memset(array,0,sizeof(array));
	N =1;
	myID =0;
	
	return SaveToRO();
}
/*-----------------------------------------------------------*/

/* --- Trigger ST factory bootloader update for a remote module.
 */
void remoteBootloaderUpdate(uint8_t src,uint8_t dst,uint8_t inport,uint8_t outport){

	uint8_t myOutport =0, lastModule =0;
	int8_t *pcOutputString;

	/* 1. Get route to destination module */
	myOutport =FindRoute(myID,dst);
	if(outport && dst == myID){ /* This is a 'via port' update and I'm the last module */
		myOutport =outport;
		lastModule =myID;
	}
	else if(outport == 0){ /* This is a remote update */
		if(NumberOfHops(dst)== 1)
		lastModule = myID;
		else
		lastModule = route[NumberOfHops(dst)-1]; /* previous module = route[Number of hops - 1] */
	}

	/* 2. If this is the source of the message, show status on the CLI */
	if(src == myID){
		/* Obtain the address of the output buffer.  Note there is no mutual
		 exclusion on this buffer as it is assumed only one command console
		 interface will be used at any one time. */
		pcOutputString =FreeRTOS_CLIGetOutputBuffer();

		if(outport == 0)		// This is a remote module update
			sprintf((char* )pcOutputString,pcRemoteBootloaderUpdateMessage,dst);
		else
			// This is a 'via port' remote update
			sprintf((char* )pcOutputString,pcRemoteBootloaderUpdateViaPortMessage,dst,outport);

		strcat((char* )pcOutputString,pcRemoteBootloaderUpdateWarningMessage);
		writePxITMutex(inport,(char* )pcOutputString,strlen((char* )pcOutputString),cmd50ms);
		Delay_ms(100);
	}

	/* 3. Setup my inport and outport for bootloader update */
	SetupPortForRemoteBootloaderUpdate(inport);
	SetupPortForRemoteBootloaderUpdate(myOutport);


	/* 5. Build a DMA stream between my inport and outport */
	StartScastDMAStream(inport,myID,myOutport,myID,BIDIRECTIONAL,0xFFFFFFFF,0xFFFFFFFF,false);
}

/*-----------------------------------------------------------*/

/* --- Setup a port for remote ST factory bootloader update:
 - Set baudrate to 57600
 - Enable even parity
 - Set datasize to 9 bits
 */
void SetupPortForRemoteBootloaderUpdate(uint8_t port){
	UART_HandleTypeDef *huart =GetUart(port);

	huart->Init.BaudRate =57600;
	huart->Init.Parity = UART_PARITY_EVEN;
	huart->Init.WordLength = UART_WORDLENGTH_9B;
	HAL_UART_Init(huart);

	/* The CLI port RXNE interrupt might be disabled so enable here again to be sure */
	__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
}

/* --- H16R6 module initialization.
 */
void Module_Peripheral_Init(void){

	/* Array ports */
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_USART4_UART_Init();
	MX_USART5_UART_Init();
	MX_USART6_UART_Init();
	MX_GPIO_Init();
	MX_SPI1_Init();
	DigiLed_init();

	 //Circulating DMA Channels ON All Module
	for (int i = 1; i <= NumOfPorts; i++) {
		if (GetUart(i) == &huart1) {
			index_dma[i - 1] = &(DMA1_Channel1->CNDTR);}
		else if (GetUart(i) == &huart2) {
			index_dma[i - 1] = &(DMA1_Channel2->CNDTR);}
		else if (GetUart(i) == &huart3) {
			index_dma[i - 1] = &(DMA1_Channel3->CNDTR);}
		else if (GetUart(i) == &huart4) {
			index_dma[i - 1] = &(DMA1_Channel4->CNDTR);}
		else if (GetUart(i) == &huart5) {
			index_dma[i - 1] = &(DMA1_Channel5->CNDTR);}
		else if (GetUart(i) == &huart6) {
			index_dma[i - 1] = &(DMA1_Channel6->CNDTR);}
	}


	/* Create module special task (if needed) */

}

/*-----------------------------------------------------------*/
/* --- H16R6 message processing task.
 */
Module_Status Module_MessagingTask(uint16_t code,uint8_t port,uint8_t src,uint8_t dst,uint8_t shift){
	Module_Status result =H16R6_OK;


	switch(code){

		default:
		result =H16R6_ERR_UnknownMessage;
		break;
	}
	
	return result;
}
/* --- Get the port for a given UART. 
 */
uint8_t GetPort(UART_HandleTypeDef *huart){

	if(huart->Instance == USART4)
		return P1;
	else if(huart->Instance == USART2)
		return P2;
	else if(huart->Instance == USART3)
		return P3;
	else if(huart->Instance == USART5)
		return P4;
	else if(huart->Instance == USART1)
		return P5;
	else if(huart->Instance == USART6)
		return P6;
	
	return 0;
}

/* --- Register this module CLI Commands
 */
void RegisterModuleCLICommands(void){


}

/*-----------------------------------------------------------*/

/* Module special task function (if needed) */
//void LipoChargerTask(void *argument){
//
//	/* Infinite loop */
//	uint8_t cases; // Test variable.
//	for(;;){
//		/*  */
//		switch(cases){
//
//
//			default:
//				osDelay(10);
//				break;
//		}
//
//		taskYIELD();
//	}
//
//}

/*-----------------------------------------------------------*/

/* -----------------------------------------------------------------------
 |								  Local Function                          |
/* -----------------------------------------------------------------------

/* -----------------------------------------------------------------------
 |								  User Function
/* -----------------------------------------------------------------------
 */
/*
 * Set the colors of a single led ad position 'led' using single colors
 * @param led position of the led in the string
 * @param blue intensity of the blue color from 0 to 255
 * @param green intensity of the green color from 0 to 255
 * @param red intensity of the red color from 0 to 255
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 */
Module_Status LEDMatrix_SetColor(uint8_t led, uint8_t red, uint8_t green, uint8_t blue,uint8_t illumination)
{
	Module_Status Status = H16R6_OK;
	DigiLed_setColor(led,red,green,blue,illumination);
	DigiLed_update(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/*
 * set color of all LEDs in a string
 * @param blue intensity of the blue color from 0 to 255
 * @param green intensity of the green color from 0 to 255
 * @param red intensity of the red color from 0 to 255
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 */
Module_Status LEDMatrix_SetAllColor(uint8_t red, uint8_t green, uint8_t blue,uint8_t illumination)
{
	Module_Status Status = H16R6_OK;
	DigiLed_setAllColor(red, green, blue, illumination);
	DigiLed_update(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/*
 * Set the colors of a single led ad position 'led' using RGB color scheme
 * RGB colors are 24 bits of a 32 bit word where the intensity of the colors red, green en blue are
 * expressed as hex values from 0 to 255 (0 - FF).
 * Colors can be set using defines from "colors.h"
 * @param led position of the led in the string
 * @param rgb color of led in RGB color scheme maximum 0xFFFFFF
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 */
Module_Status LEDMatrix_SetRGB(uint8_t led, uint32_t rgb,uint8_t illumination)
{
	Module_Status Status = H16R6_OK;
	DigiLed_setRGB(led, rgb, illumination);
	DigiLed_update(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/*
 * Set the colors of a single led ad position 'led' using RGB color scheme
 * RGB colors are 24 bits of a 32 bit word where the intensity of the colors red, green and blue are
 * expressed as hex values from 0 to 255 (0 - FF).
 * Colors can be set using defines from "colors.h"
 * @param rgb color of led in RGB color scheme  maximum 0xFFFFFF
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 */
Module_Status LEDMatrix_SetAllRGB(uint32_t rgb,uint8_t illumination)
{
	Module_Status Status = H16R6_OK;
	DigiLed_setAllRGB(rgb, illumination);
	DigiLed_update(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/*
 * @switch a single led off
 * @param led position of the led in the string to be switched off
 */
Module_Status LEDMatrix_SetLedOff(uint8_t led)
{
	Module_Status Status = H16R6_OK;
	 DigiLed_setLedOff(led);
	 DigiLed_update(1);
	 return Status;
}
/* -----------------------------------------------------------------------*/
/*
 * @All leds off
 */
Module_Status LEDMatrix_SetAllLedOff()
{
	Module_Status Status = H16R6_OK;
	DigiLed_setAllLedOff();
	DigiLed_update(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/*
 * switch a single led on
 * Using this function will preserve the active color settings for the led
 * @param led position of the led in the string to be switched on
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 */
Module_Status LEDMatrix_SetLedOn(uint8_t led,uint8_t illumination)
{
	Module_Status Status = H16R6_OK;
    DigiLed_setLedOn(led, illumination);
	DigiLed_update(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/*
 * All leds on
 * Using this function will preserve the active color settings for the led
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 */
Module_Status LEDMatrix_SetAllLedOn(uint8_t illumination)
{
	Module_Status Status = H16R6_OK;
	DigiLed_setAllLedOn(illumination);
	DigiLed_update(1);
	return Status;
}
/* -----------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 |								Commands							      |
   -----------------------------------------------------------------------
 */

/*-----------------------------------------------------------*/

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
