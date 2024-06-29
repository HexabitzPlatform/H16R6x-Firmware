/*
 BitzOS (BOS) V0.3.4 - Copyright (C) 2017-2024 Hexabitz
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

/* Module exported parameters ------------------------------------------------*/
module_param_t modParam[NUM_MODULE_PARAMS] ={{.paramPtr = NULL, .paramFormat =FMT_FLOAT, .paramName =""}};

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void ExecuteMonitor(void);
void FLASH_Page_Eras(uint32_t Addr );
Module_Status ConvertTwosComplToDec(uint16_t twosComplVal, int16_t *sgnDecimalVal);
Module_Status BAT_ReadIdReg(uint16_t regAddress, uint16_t *Buffer, uint8_t NoBytes);
/* Create CLI commands --------------------------------------------------------*/
portBASE_TYPE CLI_SetRGBCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_SetAllRGBCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_SetColorCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_SetAllColorCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_SetLedOffCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_SetAllLedOffCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_SetLedOnCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_SetAllLedOnCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetRGB */
const CLI_Command_Definition_t CLI_SetRGBCommandDefinition ={
    (const int8_t* )"setrgb", /* The command string to type. */
    (const int8_t* )"setrgb:\r\n Set RGB LED (1st par.), red (2st par.), green (3nd par.), and blue (4rd par.) values (0-255) at a specific intensity (0-31%) (5th par.)\r\n\r\n",
	CLI_SetRGBCommand, /* The function to run. */
    5 /* five parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetAllRGB */
const CLI_Command_Definition_t CLI_SetAllRGBCommandDefinition ={
    (const int8_t* )"setallrgb", /* The command string to type. */
    (const int8_t* )"setallrgb:\r\n Set RGB  red (1st par.), green (2nd par.), and blue (3rd par.) values (0-255) at a specific intensity (0-31%) (4th par.)\r\n\r\n",
	CLI_SetAllRGBCommand, /* The function to run. */
    4 /* four parameters are expected. */
};

/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetColor */
const CLI_Command_Definition_t CLI_SetColorCommandDefinition ={
    (const int8_t* )"setcolor", /* The command string to type. */
    (const int8_t* )"setcolor:\r\n Set RGB LED (1st par.),LED color (2st par.) at a specific intensity (0-31%) (3nd par.)\n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
	CLI_SetColorCommand, /* The function to run. */
    3 /* Three parameters are expected. */
};

/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetAllColor */
const CLI_Command_Definition_t CLI_SetAllColorCommandDefinition ={
    (const int8_t* )"setallcolor", /* The command string to type. */
    (const int8_t* )"setallcolor:\r\n Set LED color (1st par.) at a specific intensity (0-31%) (2nd par.)\n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
	CLI_SetAllColorCommand, /* The function to run. */
    2 /* Two parameters are expected. */
};

/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetLedOff */
const CLI_Command_Definition_t CLI_SetLedOffCommandDefinition ={
    (const int8_t* )"setledoff", /* The command string to type. */
    (const int8_t* )"setledoff:\r\n Set RGB LED off(1st par.)\r\n\r\n" ,
	CLI_SetLedOffCommand, /* The function to run. */
    1 /* One parameters are expected. */
};

/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetAllLedOff */
const CLI_Command_Definition_t CLI_SetAllLedOffCommandDefinition ={
    (const int8_t* )"setallledoff", /* The command string to type. */
    (const int8_t* )"setallledoff:\r\n All LEDs off\r\n\r\n",
	CLI_SetAllLedOffCommand, /* The function to Off. */
    0 /* No parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetLedOn */
const CLI_Command_Definition_t CLI_SetLedOnCommandDefinition ={
    (const int8_t* )"setledon", /* The command string to type. */
    (const int8_t* )"setledon:\r\n Set RGB LED ON(1st par.),at a specific intensity (0-31%) (2th par.)\r\n\r\n",
	CLI_SetLedOnCommand, /* The function to Off. */
    2 /* Two parameters are expected. */
};

/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetAllLedOn */
const CLI_Command_Definition_t CLI_SetAllLedOnCommandDefinition ={
    (const int8_t* )"setallledon", /* The command string to type. */
    (const int8_t* )"setallledon:\r\n Set All RGB LEDs ON at a specific intensity (0-31%) (1th par.)\r\n\r\n",
	CLI_SetAllLedOnCommand, /* The function to run. */
    1 /* One parameters are expected. */
};
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
	DigiLedInit();

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
	case CODE_H16R6_SETRGB:
	{
		LEDMatrixSetRGB(cMessage[port-1][shift],cMessage[port-1][shift+1],cMessage[port-1][shift+2],cMessage[port-1][shift+3],cMessage[port-1][shift+4]);
		break;
	}
	case CODE_H16R6_SETALLRGB:
	{
		LEDMatrixSetAllRGB(cMessage[port-1][shift],cMessage[port-1][shift+1],cMessage[port-1][shift+2],cMessage[port-1][shift+3]);
		break;
	}
	case CODE_H16R6_SETCOLOR:
	{
		LEDMatrixSetColor(cMessage[port-1][shift],cMessage[port-1][shift+1],cMessage[port-1][shift+2]);
		break;
	}
	case CODE_H16R6_SETALLCOLOR:
	{
		LEDMatrixSetAllColor(cMessage[port-1][shift],cMessage[port-1][shift+1]);
		break;
	}
	case CODE_H16R6_SETLEDON:
	{
		LEDMatrixSetLedOn(cMessage[port-1][shift],cMessage[port-1][shift+1]);
		break;
	}
	case CODE_H16R6_SETALLLEDON:
	{
		LEDMatrixSetAllLedOn(cMessage[port-1][shift]);
		break;
	}
	case CODE_H16R6_SETLEDOFF:
	{
		LEDMatrixSetLedOff(cMessage[port-1][shift]);
		break;
	}
	case CODE_H16R6_SETALLLEDOFF:
	{
		LEDMatrixSetAllLedOff();
		break;
	}

		default:
		result =H16R6_ERR_UnknownMessage;
		break;
	}
	
	return result;
}
/*-----------------------------------------------------------*/

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
/*-----------------------------------------------------------*/

/* --- Register this module CLI Commands
 */
void RegisterModuleCLICommands(void){
    FreeRTOS_CLIRegisterCommand(&CLI_SetRGBCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_SetAllRGBCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_SetColorCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_SetAllColorCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_SetLedOffCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_SetAllLedOffCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_SetLedOnCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_SetAllLedOnCommandDefinition);

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
 |								  User Function
/* -----------------------------------------------------------------------
 */
/*
 * Set the colors of a single led using RGB color
 * @param led position of the led in the string led>=1
 * @param red intensity of the red color from 0 to 255
 * @param green intensity of the green color from 0 to 255
 * @param blue intensity of the blue color from 0 to 255
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
Module_Status LEDMatrixSetRGB(uint8_t led, uint8_t red, uint8_t green, uint8_t blue,uint8_t intensity)
{
	Module_Status Status = H16R6_OK;
	if (led >= LEDFRAMESIZE||led==0)
	{
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	if (intensity>=INTINSITYLED)
	{
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetRGB(led,red,green,blue,intensity);
	DigiLedUpdate(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/**
  * Set the colors of all LEDs using RGB color scheme
 * @param red intensity of the red color from 0 to 255
 * @param green intensity of the green color from 0 to 255
 * @param blue intensity of the blue color from 0 to 255
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
Module_Status LEDMatrixSetAllRGB(uint8_t red, uint8_t green, uint8_t blue,uint8_t intensity)
{
	Module_Status Status = H16R6_OK;
	if (intensity>=INTINSITYLED)
	{
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetAllRGB(red, green, blue, intensity);
	DigiLedUpdate(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/**
 * Set the colors of a single led using single colors
 * @param led position of the led in the string led>=1
 * Set LED color from a predefined color list in "APA102_LedMatrix.h"
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
Module_Status LEDMatrixSetColor(uint8_t led,uint8_t color ,uint8_t intensity)
{
	Module_Status Status = H16R6_OK;
	if (led >= LEDFRAMESIZE||led==0)
	{
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	if (intensity>=INTINSITYLED)
	{
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetColor(led, color, intensity);
	DigiLedUpdate(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/**
 * set color of all LEDs in a string
 * Set LED color from a predefined color list in "BOS.h"
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
Module_Status LEDMatrixSetAllColor(uint8_t color,uint8_t intensity)
{
	Module_Status Status = H16R6_OK;
	if (intensity>=INTINSITYLED)
	{
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetAllColor(color, intensity);
	DigiLedUpdate(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/**
  * @switch a single led off  led>=1
 * @param led position of the led in the string to be switched off
 */
Module_Status LEDMatrixSetLedOff(uint8_t led)
{
	Module_Status Status = H16R6_OK;
	if (led >= LEDFRAMESIZE||led==0)
	{
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	DigiLedSetLedOff(led);
	 DigiLedUpdate(1);
	 return Status;
}
/* -----------------------------------------------------------------------*/
/*
 * @All leds off
 */
Module_Status LEDMatrixSetAllLedOff()
{
	Module_Status Status = H16R6_OK;
	DigiLedSetAllLedOff();
	DigiLedUpdate(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/**
 * switch a single led on
 * Using this function will preserve the active color settings for the led
 * @param led position of the led in the string to be switched on led>=1
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
Module_Status LEDMatrixSetLedOn(uint8_t led,uint8_t intensity)
{
	Module_Status Status = H16R6_OK;
	if (led >= LEDFRAMESIZE||led==0)
	{
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	if (intensity>=INTINSITYLED)
	{
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetLedOn(led, intensity);
	DigiLedUpdate(1);
	return Status;
}
/* -----------------------------------------------------------------------*/
/**
 * All leds on
 * Using this function will preserve the active color settings for the led led>=1
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
Module_Status LEDMatrixSetAllLedOn(uint8_t intensity)
{
	Module_Status Status = H16R6_OK;
	if (intensity>=INTINSITYLED)
	{
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	DigiLedSetAllLedOn(intensity);
	DigiLedUpdate(1);
	return Status;
}
/*-----------------------------------------------------------*/
/**
 * Scroll - one row of one colour, the rest another colour, row moves down one for each update
 *  @param baseColour Basic color
 *  @param scrollRow Secondary color
 *  @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 *  @param scrollTime Secondary color retention time  value in millisecond.
 */
Module_Status LEDMatrixScrollMode(uint8_t baseColour,uint8_t scrollRow,uint8_t intensity,uint16_t scrollTime)
{
	Module_Status Status = H16R6_OK;
	DigiLedScrollMode(baseColour, scrollRow, intensity, scrollTime);
	return Status;
}
/*-----------------------------------------------------------*/
/**
 * Flash - flash from one colour to another with user-settable flash times and intervals
 * @param baseColour  Basic color
 * @param flashColour Secondary color
 * @param intensity   intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 * @param flashTime   Color display time value in millisecond.
 * @param timeBetweenFlash  The time between the display of the two colors value in millisecond.
 */
Module_Status LEDMatrixFlashMode(uint8_t baseColour,uint8_t flashColour,uint8_t intensity,uint16_t flashTime,uint16_t timeBetweenFlash)
{
	Module_Status Status = H16R6_OK;
	DigiLedFlashMode(baseColour, flashColour, intensity, flashTime, timeBetweenFlash);
	return Status;
}
/* -----------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 |								Commands							      |
   -----------------------------------------------------------------------
 */
/*-----------------------------------------------------------*/

portBASE_TYPE CLI_SetRGBCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t led =0;
    uint8_t red =0;
    uint8_t green =0;
    uint8_t blue =0;
    uint8_t intensity =0;
    static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3, *pcParameterString4,*pcParameterString5;
    portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0;
    portBASE_TYPE xParameterStringLength3 =0, xParameterStringLength4 =0,xParameterStringLength5 =0;

    static const int8_t *pcOKMessage =(int8_t* )"NumOfLed is %d ,RGB LED is (%d, %d, %d) at intensity %d%%\n\r";
    static const int8_t *pcWrongLedOutRangeMessage =(int8_t* )"Wrong LedOutRange!\n\r";
    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";


    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    led =(uint8_t )atol((char* )pcParameterString1);
    /* Obtain the 2st parameter string. */
    pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);
    red =(uint8_t )atol((char* )pcParameterString2);
    /* Obtain the 3nd parameter string. */
    pcParameterString3 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,3,&xParameterStringLength3);
    green =(uint8_t )atol((char* )pcParameterString3);
    /* Obtain the 4rd parameter string. */
    pcParameterString4 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,4,&xParameterStringLength4);
    blue =(uint8_t )atol((char* )pcParameterString4);
    /* Obtain the 5th parameter string. */
    pcParameterString5 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,5,&xParameterStringLength5);
    intensity =(uint8_t )atol((char* )pcParameterString5);

    result =LEDMatrixSetRGB(led,red,green,blue,intensity);

    /* Respond to the command */
    if(result == H16R6_OK)
        sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,led,red,green,blue,intensity);
    else if(result == H16R6_ERR_WrongLedOutRange)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongLedOutRangeMessage);
    else if(result == H16R6_ERR_WrongIntensity)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongIntensityMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}
/*-----------------------------------------------------------*/

portBASE_TYPE CLI_SetAllRGBCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t red =0;
    uint8_t green =0;
    uint8_t blue =0;
    uint8_t intensity =0;
    static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3, *pcParameterString4;
    portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0;
    portBASE_TYPE xParameterStringLength3 =0, xParameterStringLength4 =0;

    static const int8_t *pcOKMessage =(int8_t* )"ALL Leds ,RGB LED is (%d, %d, %d) at intensity %d%%\n\r";
    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";


    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    red =(uint8_t )atol((char* )pcParameterString1);
    /* Obtain the 2st parameter string. */
    pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);
    green =(uint8_t )atol((char* )pcParameterString2);
    /* Obtain the 3nd parameter string. */
    pcParameterString3 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,3,&xParameterStringLength3);
    blue =(uint8_t )atol((char* )pcParameterString3);
    /* Obtain the 4rd parameter string. */
    pcParameterString4 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,4,&xParameterStringLength4);
    intensity =(uint8_t )atol((char* )pcParameterString4);

    result =LEDMatrixSetAllRGB(red,green,blue,intensity);

    /* Respond to the command */
    if(result == H16R6_OK)
        sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,red,green,blue,intensity);
    else if(result == H16R6_ERR_WrongIntensity)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongIntensityMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}
/*-----------------------------------------------------------*/
portBASE_TYPE CLI_SetColorCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t led =0;
    uint8_t color =0;
    uint8_t intensity =0;
    char par[15] ={0};
    static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3;
    portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0;
    portBASE_TYPE xParameterStringLength3 =0;

    static const int8_t *pcOKMessage =(int8_t* )"NumOfLed is %d ,LED color is %s at intensity %d%%\n\r";
    static const int8_t *pcWrongLedOutRangeMessage =(int8_t* )"Wrong LedOutRange!\n\r";
    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";


    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    led =(uint8_t )atol((char* )pcParameterString1);
    /* Obtain the 2st parameter string. */
    pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);
    /* Read the color value. */
    	if(!strncmp((const char* )pcParameterString2,"black",xParameterStringLength2))
    		color =BLACK;
    	else if(!strncmp((const char* )pcParameterString2,"white",xParameterStringLength2))
    		color =WHITE;
    	else if(!strncmp((const char* )pcParameterString2,"red",xParameterStringLength2))
    		color =RED;
    	else if(!strncmp((const char* )pcParameterString2,"blue",xParameterStringLength2))
    		color =BLUE;
    	else if(!strncmp((const char* )pcParameterString2,"yellow",xParameterStringLength2))
    		color =YELLOW;
    	else if(!strncmp((const char* )pcParameterString2,"cyan",xParameterStringLength2))
    		color =CYAN;
    	else if(!strncmp((const char* )pcParameterString2,"magenta",xParameterStringLength2))
    		color =MAGENTA;
    	else if(!strncmp((const char* )pcParameterString2,"green",xParameterStringLength2))
    		color =GREEN;
    	else if(!strncmp((const char* )pcParameterString2,"aqua",xParameterStringLength2))
    		color =AQUA;
    	else if(!strncmp((const char* )pcParameterString2,"purple",xParameterStringLength2))
    		color =PURPLE;
    	else if(!strncmp((const char* )pcParameterString2,"lightblue",xParameterStringLength2))
    		color =LIGHTBLUE;
    	else if(!strncmp((const char* )pcParameterString2,"orange",xParameterStringLength2))
    		color =ORANGE;
    	else if(!strncmp((const char* )pcParameterString2,"indigo",xParameterStringLength2))
    		color =INDIGO;

    /* Obtain the 3nd parameter string. */
    pcParameterString3 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,3,&xParameterStringLength3);
    intensity =(uint8_t )atol((char* )pcParameterString3);


    result =LEDMatrixSetColor(led,color,intensity);

    /* Respond to the command */
    if(result == H16R6_OK)
    {	strncpy(par,(char* )pcParameterString2,xParameterStringLength2);
        sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,led,par,intensity);
    }
    else if(result == H16R6_ERR_WrongLedOutRange)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongLedOutRangeMessage);
    else if(result == H16R6_ERR_WrongIntensity)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongIntensityMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}
/*-----------------------------------------------------------*/

portBASE_TYPE CLI_SetAllColorCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t color =0;
    uint8_t intensity =0;
    char par[15] ={0};
    static int8_t *pcParameterString1, *pcParameterString2;
    portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0;

    static const int8_t *pcOKMessage =(int8_t* )"ALL Leds ,LED color is %s at intensity %d%%\n\r";
    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";


    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    /* Read the color value. */
    	if(!strncmp((const char* )pcParameterString1,"black",xParameterStringLength1))
    		color =BLACK;
    	else if(!strncmp((const char* )pcParameterString1,"white",xParameterStringLength1))
    		color =WHITE;
    	else if(!strncmp((const char* )pcParameterString1,"red",xParameterStringLength1))
    		color =RED;
    	else if(!strncmp((const char* )pcParameterString1,"blue",xParameterStringLength1))
    		color =BLUE;
    	else if(!strncmp((const char* )pcParameterString1,"yellow",xParameterStringLength1))
    		color =YELLOW;
    	else if(!strncmp((const char* )pcParameterString1,"cyan",xParameterStringLength1))
    		color =CYAN;
    	else if(!strncmp((const char* )pcParameterString1,"magenta",xParameterStringLength1))
    		color =MAGENTA;
    	else if(!strncmp((const char* )pcParameterString1,"green",xParameterStringLength1))
    		color =GREEN;
    	else if(!strncmp((const char* )pcParameterString1,"aqua",xParameterStringLength1))
    		color =AQUA;
    	else if(!strncmp((const char* )pcParameterString1,"purple",xParameterStringLength1))
    		color =PURPLE;
    	else if(!strncmp((const char* )pcParameterString1,"lightblue",xParameterStringLength1))
    		color =LIGHTBLUE;
    	else if(!strncmp((const char* )pcParameterString1,"orange",xParameterStringLength1))
    		color =ORANGE;
    	else if(!strncmp((const char* )pcParameterString1,"indigo",xParameterStringLength1))
    		color =INDIGO;
    /* Obtain the 2st parameter string. */
    pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);
    intensity =(uint8_t )atol((char* )pcParameterString2);

    result =LEDMatrixSetAllColor(color,intensity);

    /* Respond to the command */
    if(result == H16R6_OK)
    {
		strncpy(par,(char* )pcParameterString1,xParameterStringLength1);
    	sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,par,intensity);
    }
    else if(result == H16R6_ERR_WrongIntensity)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongIntensityMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}
/*-----------------------------------------------------------*/
portBASE_TYPE CLI_SetLedOffCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t led =0;

    static int8_t *pcParameterString1;
    portBASE_TYPE xParameterStringLength1 =0;


    static const int8_t *pcOKMessage =(int8_t* )"NumOfLed OFF is %d \n\r";
    static const int8_t *pcWrongLedOutRangeMessage =(int8_t* )"Wrong LedOutRange!\n\r";

    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    led =(uint8_t )atol((char* )pcParameterString1);


    result =LEDMatrixSetLedOff(led);

    /* Respond to the command */
    if(result == H16R6_OK)
        sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,led);
    else if(result == H16R6_ERR_WrongLedOutRange)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongLedOutRangeMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}
/*-----------------------------------------------------------*/
portBASE_TYPE CLI_SetAllLedOffCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;


    static const int8_t *pcOKMessage =(int8_t* )"ALL Leds OFF\n\r";

    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    result =LEDMatrixSetAllLedOff();

    /* Respond to the command */
    if(result == H16R6_OK)
        sprintf((char* )pcWriteBuffer,(char* )pcOKMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}

/*-----------------------------------------------------------*/
portBASE_TYPE CLI_SetLedOnCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t led =0;
    uint8_t intensity =0;
    static int8_t *pcParameterString1, *pcParameterString2;
    portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0;

    static const int8_t *pcOKMessage =(int8_t* )"NumOfLed ON is %d at intensity %d%%\n\r";
    static const int8_t *pcWrongLedOutRangeMessage =(int8_t* )"Wrong LedOutRange!\n\r";
    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";


    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    led =(uint8_t )atol((char* )pcParameterString1);
    /* Obtain the 2st parameter string. */
    pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);
    intensity =(uint8_t )atol((char* )pcParameterString2);

    result =LEDMatrixSetLedOn(led,intensity);

    /* Respond to the command */
    if(result == H16R6_OK)
        sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,led,intensity);
    else if(result == H16R6_ERR_WrongLedOutRange)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongLedOutRangeMessage);
    else if(result == H16R6_ERR_WrongIntensity)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongIntensityMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}

/*-----------------------------------------------------------*/
portBASE_TYPE CLI_SetAllLedOnCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t intensity =0;
    static int8_t *pcParameterString1;
    portBASE_TYPE xParameterStringLength1 =0;

    static const int8_t *pcOKMessage =(int8_t* )"ALL Leds ON at intensity %d%%\n\r";
    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";

    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    intensity =(uint8_t )atol((char* )pcParameterString1);

    result =LEDMatrixSetAllLedOn(intensity);

    /* Respond to the command */
    if(result == H16R6_OK)
        sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,intensity);
    else if(result == H16R6_ERR_WrongIntensity)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongIntensityMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}

/*-----------------------------------------------------------*/

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
