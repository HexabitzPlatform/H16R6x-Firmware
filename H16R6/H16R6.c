/*
 BitzOS (BOS) V0.3.6 - Copyright (C) 2017-2024 Hexabitz
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
int randomNumber1[65] ={0},randomNumber2=0;
int randomIndex[65];
/* Local functions */
uint8_t OldeColorR[65]={0};
uint8_t OldeColorG[65]={0};
uint8_t OldeColorB[65]={0};
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
portBASE_TYPE CLI_ScrollModeCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_FlashModeCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_ColorPickerModeCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);
portBASE_TYPE CLI_SetColorSomeLedCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString);

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
/* CLI command structure : LEDMatrixSetLedOn */
const CLI_Command_Definition_t CLI_ScrollModeCommandDefinition ={
    (const int8_t* )"scrollmode", /* The command string to type. */
    (const int8_t* )"scrollmode:\r\n Set scrollmode  Basic color (1st par.) Secondary color(2st par.) at a specific intensity (0-31%) (3nd par.) scrollTime(4nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
	CLI_ScrollModeCommand, /* The function to ScrollMode. */
    4 /* four parameters are expected. */
};

/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetAllLedOn */
const CLI_Command_Definition_t CLI_FlashModeCommandDefinition ={
    (const int8_t* )"flashmode", /* The command string to type. */
    (const int8_t* )"flashmode:\r\n Set flashmode  Basic color (1st par.) Secondary color(2st par.) at a specific intensity (0-31%) (3nd par.) flashTime(4nd par.)  timeBetweenFlash(5nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
	CLI_FlashModeCommand, /* The function to FlashMode. */
    5 /* five parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixRGBColorPickerMode */
const CLI_Command_Definition_t CLI_ColorPickerModeCommandDefinition ={
    (const int8_t* )"colorpickermode", /* The command string to type. */
    (const int8_t* )"colorpickermode:\r\n Set colorpickermode  color (1st par.)  Time(2nd par.) at a specific intensity (0-31%) (3nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
	CLI_SetColorSomeLedCommand, /* The function to SetColorSomeLed. */
    4 /* five parameters are expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : LEDMatrixSetColorSomeLed */
const CLI_Command_Definition_t CLI_SetColorSomeLedCommandDefinition ={
    (const int8_t* )"setcolorsomeled", /* The command string to type. */
    (const int8_t* )"setcolorsomeled:\r\n Set setcolorsomeled  StartLed (1st par.)  EndLed (2nd par.) color (3nd par.) at a specific intensity (0-31%) (4nd par.) \n\rRegistered colors are:\
					\r\nblack, white, red, blue, green, yellow, cyan, magenta ,aqua,purple,lightblue,orange and indigo, \r\n\r\n",
	CLI_SetColorSomeLedCommand, /* The function to FlashMode. */
    4 /* five parameters are expected. */
};

/*

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
	case CODE_H16R6_SCROLLMODE:
	{
		uint16_t scrollTime=0;
		scrollTime=(((uint16_t)cMessage[port-1][shift+3])+((uint16_t)cMessage[port-1][shift+4]<<8));
		LEDMatrixScrollMode(cMessage[port-1][shift], cMessage[port-1][shift+1], cMessage[port-1][shift+2], scrollTime);
		break;
	}
	case CODE_H16R6_FLASHMODE:
	{
		uint16_t flashTime=0;
		uint16_t timeBetweenFlash=0;
		flashTime=(((uint16_t)cMessage[port-1][shift+3])+((uint16_t)cMessage[port-1][shift+4]<<8));
		timeBetweenFlash=(((uint16_t)cMessage[port-1][shift+5])+((uint16_t)cMessage[port-1][shift+6]<<8));
		LEDMatrixFlashMode(cMessage[port-1][shift], cMessage[port-1][shift+1], cMessage[port-1][shift+2], flashTime, timeBetweenFlash);
		break;
	}
	case CODE_H16R6_COLORPICKERMODE:
		{
			uint16_t time =0;
			time=(((uint16_t)cMessage[port-1][shift+1])+((uint16_t)cMessage[port-1][shift+2]<<8));
			LEDMatrixRGBColorPickerMode(cMessage[port-1][shift], time, cMessage[port-1][shift+3]);
			break;
		}
	case CODE_H16R6_SETCOLORSOMELED:
		{
			LEDMatrixSetColorSomeLed(cMessage[port-1][shift], cMessage[port-1][shift+1],cMessage[port-1][shift+2], cMessage[port-1][shift+3]);
			break;
		}
	case CODE_H16R6_MOTIONMODE:
		{
			uint32_t Number_int;
			float scaledqom;
			Number_int=(((uint32_t)cMessage[port-1][shift+3])+((uint32_t)cMessage[port-1][shift+4]<<8)+((uint32_t)cMessage[port-1][shift+5]<<16)+((uint32_t)cMessage[port-1][shift+6]<<24));
			scaledqom= *((float*)&Number_int);
			LEDMatrixMotionMode(cMessage[port-1][shift],cMessage[port-1][shift+1],cMessage[port-1][shift+2], scaledqom);

			break;
		}
	case CODE_H16R6_CROSSFADEMODE:
		{
			uint16_t time=0;
			time=(((uint16_t)cMessage[port-1][shift+3])+((uint16_t)cMessage[port-1][shift+4]<<8));
			LEDMatrixCrossFadeMode(cMessage[port-1][shift],cMessage[port-1][shift+1],cMessage[port-1][shift+2], time);

			break;
		}
	case CODE_H16R6_CROSSFADEMODELEDRGB:
		{
			uint16_t interpolationtime;
			interpolationtime=(((uint16_t)cMessage[port-1][shift+4])+((uint16_t)cMessage[port-1][shift+5]<<8));
			LEDMatrixCrossFadeModeLEDRGB(cMessage[port-1][shift], cMessage[port-1][shift+1], cMessage[port-1][shift+2], cMessage[port-1][shift+3], interpolationtime, cMessage[port-1][shift+6]);
			break;
		}
	case CODE_H16R6_CROSSFADEMODEALLLEDRGB:
		{
			uint16_t interpolationtime;
			interpolationtime=(((uint16_t)cMessage[port-1][shift+3])+((uint16_t)cMessage[port-1][shift+4]<<8));
			LEDMatrixCrossFadeModeALLLEDRGB(cMessage[port-1][shift], cMessage[port-1][shift+1], cMessage[port-1][shift+2], interpolationtime, cMessage[port-1][shift+5]);
			break;
		}
	case CODE_H16R6_SPRINKLEMODE:
		{
			uint16_t TimeToFade;
			TimeToFade=(((uint16_t)cMessage[port-1][shift+4])+((uint16_t)cMessage[port-1][shift+5]<<8));
			LEDMatrixSprinkleMode(cMessage[port-1][shift], cMessage[port-1][shift+1], cMessage[port-1][shift+2], cMessage[port-1][shift+3], TimeToFade, cMessage[port-1][shift+6]);
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
    FreeRTOS_CLIRegisterCommand(&CLI_ScrollModeCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_FlashModeCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_ColorPickerModeCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&CLI_SetColorSomeLedCommandDefinition);

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
	if (led > LEDFRAMESIZE||led==0)
	{
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	if (intensity>=INTINSITYLED)
	{
		Status = H16R6_ERR_WrongIntensity;
		return Status;
	}
	memset(&OldeColorR[led], red, 1);
	memset(&OldeColorG[led], green, 1);
	memset(&OldeColorB[led], blue,1);
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
	memset(OldeColorR, red, sizeof(OldeColorR));
	memset(OldeColorG, green, sizeof(OldeColorG));
	memset(OldeColorB, blue, sizeof(OldeColorB));
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
	if (led > LEDFRAMESIZE||led==0)
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
	memset(&OldeColorR[led], 0, 1);
	memset(&OldeColorG[led], 0, 1);
	memset(&OldeColorB[led], 0,1);
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
	memset(OldeColorR, 0, sizeof(OldeColorR));
	memset(OldeColorG, 0, sizeof(OldeColorG));
	memset(OldeColorB, 0, sizeof(OldeColorB));
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
/*-----------------------------------------------------------*/
/*
 * All leds on in the RGBColorPickerMode
 * @param color Set LED color from a predefined color list in "BOS.h"
 * @param time   time between turning on each LED and the next
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
Module_Status LEDMatrixRGBColorPickerMode(uint8_t color,uint16_t time,uint8_t intensity)
{
	Module_Status Status = H16R6_OK;
	DigiLedRGBColorPickerMode(color, time, intensity);
	return Status;
}
/* -----------------------------------------------------------------------*/
/*
 * Set the colors of some of led using single colors
 * @param StartLed  position of the led in the string led>=1
 * @param EndLed  position of the led in the string led<=64
 * @param Set LED color from a predefined color list in "BOS.h"
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
Module_Status LEDMatrixSetColorSomeLed(uint8_t StartLed,uint8_t EndLed,uint8_t color ,uint8_t intensity)
{
	Module_Status Status = H16R6_OK;
	if (StartLed <1 || EndLed<1)
	{
		StartLed=1;
		EndLed=1;
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	if (EndLed >64)
	{
		EndLed=64;
		Status = H16R6_ERR_WrongLedOutRange;
		return Status;
	}
	for (int i = StartLed; i <=EndLed; i++) {
		LEDMatrixSetColor(i, color, intensity);

		}

	return Status;
}

/* -----------------------------------------------------------------------*/
/*
 * Set the colors of leds According to the value of scaledqom
 * @param Set LED color from a predefined color list in "BOS.h"
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 * @param scaledqom Acceleration values ​​from imu or any sensor
 *
 */
Module_Status LEDMatrixMotionMode(uint8_t baseColour,uint8_t SeconedColor,uint8_t intensity,float scaledqom)
{
	LEDMatrixRGBColorPickerMode(baseColour, scaledqom*10, intensity);
	Delay_ms(2000);

	if(scaledqom<0.5)
	{
		LEDMatrixRGBColorPickerMode(baseColour, scaledqom*10, intensity);

	}
	else if(scaledqom>0.5)
	{
		LEDMatrixRGBColorPickerMode(SeconedColor, scaledqom*10, intensity);

	}
}

/* -----------------------------------------------------------------------*/
/*
 * Transition between the ratios of the three colors over time
 * @param Set LED color from a predefined color list in "BOS.h"
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 * @param time  Color grading time
 *
 */
Module_Status LEDMatrixCrossFadeMode(uint8_t baseColour,uint8_t seconedColor,uint8_t thirdColor,uint16_t time)
{

	for (int var = 1; var <= 10; var++) {
		LEDMatrixSetAllColor(baseColour,var);
		Delay_ms(time);
	}
	for (int var = 10; var >= 0; var--) {
			LEDMatrixSetAllColor(baseColour,var);
			Delay_ms(time);
		}
	for (int var = 1; var <= 10; var++) {
		LEDMatrixSetAllColor(seconedColor,var);
		Delay_ms(time);
	}
	for (int var = 10; var >= 0; var--) {
			LEDMatrixSetAllColor(seconedColor,var);
			Delay_ms(time);
		}
	for (int var = 1; var <= 10; var++) {
		LEDMatrixSetAllColor(thirdColor,var);
		Delay_ms(time);
	}
	for (int var = 10; var >= 0; var--) {
			LEDMatrixSetAllColor(thirdColor,var);
			Delay_ms(time);
		}

}
/* -----------------------------------------------------------------------*/
Module_Status LEDMatrixCrossFadeModeLEDRGB(uint8_t LED, uint8_t SecondRED,
		uint8_t SecondGREEN, uint8_t SecondBLUE, uint16_t interpolationtime,
		uint8_t intensity) {
	if (LED >= 64) {LED = 64;}
	if (LED <1 ) {LED = 1;}
	uint8_t NewRED=0;
	uint8_t NewGREEN=0;
	uint8_t NewBLUE=0;
	int16_t DeltaR = SecondRED - OldeColorR[LED];
	int16_t DeltaG = SecondGREEN - OldeColorG[LED];
	int16_t DeltaB = SecondBLUE - OldeColorB[LED];
	float delayTime = (float) interpolationtime / 100;
	for (uint16_t currentStep = 0; currentStep <= 100; currentStep++) {
		float t = (float) currentStep / (float) 100;
		 NewRED = OldeColorR[LED] + (uint8_t) (t * DeltaR);
		 NewGREEN = OldeColorG[LED] + (uint8_t) (t * DeltaG);
		 NewBLUE = OldeColorB[LED] + (uint8_t) (t * DeltaB);
		if (intensity >= INTINSITYLED) {
			intensity = INTINSITYLED;
		}
		DigiLedSetRGB(LED, NewRED, NewGREEN, NewBLUE, intensity);
		DigiLedUpdate(1);

		HAL_Delay(delayTime);
	}
	memset(&OldeColorR[LED], NewRED, 1);
	memset(&OldeColorG[LED], NewGREEN, 1);
	memset(&OldeColorB[LED], NewBLUE, 1);
}
/* -----------------------------------------------------------------------*/

Module_Status LEDMatrixCrossFadeModeALLLEDRGB( uint8_t SecondRED, uint8_t SecondGREEN,
		uint8_t SecondBLUE, uint16_t interpolationtime, uint8_t intensity) {
	int16_t DeltaR = 0;
	int16_t DeltaG = 0;
	int16_t DeltaB = 0;
	uint16_t CurrentStep=0;
	int NLED=1;
	uint8_t NewRED=0;
	uint8_t NewGREEN=0;
	uint8_t NewBLUE=0;
	if (intensity >= INTINSITYLED) {
		intensity = INTINSITYLED;
	}
	float delayTime = (float) interpolationtime / 100;
	for ( CurrentStep = 0; CurrentStep <= 40; CurrentStep++) {
		for ( NLED = 1; NLED <= 64;NLED++)
		{
		float t = (float) CurrentStep / (float) 40;
		 DeltaR = SecondRED - OldeColorR[NLED];
		 DeltaG = SecondGREEN - OldeColorG[NLED];
		 DeltaB = SecondBLUE - OldeColorB[NLED];
		 NewRED = OldeColorR[NLED] + (uint8_t) (t * DeltaR);
		 NewGREEN = OldeColorG[NLED]+ (uint8_t) (t * DeltaG);
		 NewBLUE = OldeColorB[NLED] + (uint8_t) (t * DeltaB);

		DigiLedSetRGB(NLED,NewRED, NewGREEN, NewBLUE, intensity);
		DigiLedUpdate(1);
		}
		if(CurrentStep==40)
		{
			memset(OldeColorR, NewRED, sizeof(OldeColorR));
			memset(OldeColorG, NewGREEN, sizeof(OldeColorG));
			memset(OldeColorB, NewBLUE, sizeof(OldeColorB));
		}
		HAL_Delay(delayTime/10);

	}

}
/* -----------------------------------------------------------------------*/
void RandomArray(int randomIndex[65],int NbOfLeds)
{
	int i=0;
	randomIndex[1] =(rand() % 64)+1;
	for (int led = 2; led <= NbOfLeds; led++)
	{
		bool flag=true;
		int r=0;
		while(flag)
		{
			r= (rand() % 64)+1;
			for( i=1;i<led;i++)
			{
				if(r==randomIndex[i])
				{break;}
			}
			if(i==led)
			{flag=false;}
		}
		randomIndex[led]=r;
	}

}

/* -----------------------------------------------------------------------*/
Module_Status LEDMatrixSprinkleMode(uint8_t TargetColorR, uint8_t TargetColorG,
		uint8_t TargetColorB, uint8_t AmountOfLEDs, uint16_t TimeToFade,
		uint8_t ColorDeviation) {

if (AmountOfLEDs>100) {AmountOfLEDs=100;}
if (AmountOfLEDs<0) {AmountOfLEDs=0;}
	int NbOfLeds = 0.64 * AmountOfLEDs;
	int IndexOfLeds=0,RandDev=0,TargetColorWithDev=0,TargetColorPerStep=0,TargetColor=0,OldeColor=0;
	float DelayTimeStep=TimeToFade/100.0;

	memcpy(&randomIndex[0], 0, 65);
	if (AmountOfLEDs==100) {
		for (int i = 1; i <+ 65; i++) {
			randomIndex[i]=i;
		}
	}
	else
	RandomArray(randomIndex,NbOfLeds);

	for (int CurrentStep = 0; CurrentStep <= 40;CurrentStep++)
	{
		float t=CurrentStep/40.0;
		for (int NLED = 1; NLED <= NbOfLeds;NLED++)
		{
			IndexOfLeds=randomIndex[NLED];
			int RandDev=(rand() % ColorDeviation)-(ColorDeviation/2);
			int TargetColorWithDevR=TargetColorR+RandDev;
			int TargetColorWithDevG=TargetColorG+RandDev;
			int TargetColorWithDevB=TargetColorB+RandDev;

			int maxmax=255;
			if(TargetColorWithDevR>255){maxmax=TargetColorWithDevR;}
			if(TargetColorWithDevG>255){if(maxmax<TargetColorWithDevG){maxmax=TargetColorWithDevG;}}
			if(TargetColorWithDevB>255){if(maxmax<TargetColorWithDevB){maxmax=TargetColorWithDevB;}}
			if(maxmax>255)
			{
				TargetColorWithDevR=TargetColorWithDevR-(maxmax-255);
				TargetColorWithDevG=TargetColorWithDevG-(maxmax-255);
				TargetColorWithDevB=TargetColorWithDevB-(maxmax-255);
			}

			int minmin=0;
			if(TargetColorWithDevR<0){minmin=TargetColorWithDevR;}
			if(TargetColorWithDevG<0){if(minmin>TargetColorWithDevG){minmin=TargetColorWithDevG;}}
			if(TargetColorWithDevB<0){if(minmin<TargetColorWithDevB){minmin=TargetColorWithDevB;}}
			if(minmin<0)
			{
				TargetColorWithDevR=TargetColorWithDevR-(minmin);
				TargetColorWithDevG=TargetColorWithDevG-(minmin);
				TargetColorWithDevB=TargetColorWithDevB-(minmin);
			}
			int TargetColorPerStepR=OldeColorR[IndexOfLeds]+(t*(TargetColorWithDevR-OldeColorR[IndexOfLeds]));
			int TargetColorPerStepG=OldeColorG[IndexOfLeds]+(t*(TargetColorWithDevG-OldeColorG[IndexOfLeds]));
			int TargetColorPerStepB=OldeColorB[IndexOfLeds]+(t*(TargetColorWithDevB-OldeColorB[IndexOfLeds]));

			LEDMatrixSetRGB(IndexOfLeds, TargetColorPerStepR, TargetColorPerStepG, TargetColorPerStepB, rand()%4+1);

			if(CurrentStep==40)
			{
				OldeColorR[IndexOfLeds] = TargetColorPerStepR;
				OldeColorG[IndexOfLeds] = TargetColorPerStepG;
				OldeColorB[IndexOfLeds] = TargetColorPerStepB;
			}

		}
		if(AmountOfLEDs<80)
		{HAL_Delay(DelayTimeStep);
		}
		else
		{HAL_Delay(DelayTimeStep/10);}
	}

}


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
portBASE_TYPE CLI_ScrollModeCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t baseColour =0;
    uint8_t scrollRow =0;
    uint8_t intensity =0;
    uint16_t scrollTime;
    char par[2][15] ={0};
    static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3, *pcParameterString4;
    portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0, xParameterStringLength3 =0, xParameterStringLength4 =0;

    static const int8_t *pcOKMessage =(int8_t* )"ScrollMode:baseColour is %s and scrollRow is %s and intensity %d %% and scrollTime is %d \n\r";
    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";


    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    /* Read the color value. */
    	if(!strncmp((const char* )pcParameterString1,"black",xParameterStringLength1))
    		baseColour =BLACK;
    	else if(!strncmp((const char* )pcParameterString1,"white",xParameterStringLength1))
    		baseColour =WHITE;
    	else if(!strncmp((const char* )pcParameterString1,"red",xParameterStringLength1))
    		baseColour =RED;
    	else if(!strncmp((const char* )pcParameterString1,"blue",xParameterStringLength1))
    		baseColour =BLUE;
    	else if(!strncmp((const char* )pcParameterString1,"yellow",xParameterStringLength1))
    		baseColour =YELLOW;
    	else if(!strncmp((const char* )pcParameterString1,"cyan",xParameterStringLength1))
    		baseColour =CYAN;
    	else if(!strncmp((const char* )pcParameterString1,"magenta",xParameterStringLength1))
    		baseColour =MAGENTA;
    	else if(!strncmp((const char* )pcParameterString1,"green",xParameterStringLength1))
    		baseColour =GREEN;
    	else if(!strncmp((const char* )pcParameterString1,"aqua",xParameterStringLength1))
    		baseColour =AQUA;
    	else if(!strncmp((const char* )pcParameterString1,"purple",xParameterStringLength1))
    		baseColour =PURPLE;
    	else if(!strncmp((const char* )pcParameterString1,"lightblue",xParameterStringLength1))
    		baseColour =LIGHTBLUE;
    	else if(!strncmp((const char* )pcParameterString1,"orange",xParameterStringLength1))
    		baseColour =ORANGE;
    	else if(!strncmp((const char* )pcParameterString1,"indigo",xParameterStringLength1))
    		baseColour =INDIGO;
    	/* Obtain the 2st parameter string. */
    	    pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);
    	    /* Read the color value. */
    	    	if(!strncmp((const char* )pcParameterString2,"black",xParameterStringLength2))
    	    		scrollRow =BLACK;
    	    	else if(!strncmp((const char* )pcParameterString2,"white",xParameterStringLength2))
    	    		scrollRow =WHITE;
    	    	else if(!strncmp((const char* )pcParameterString2,"red",xParameterStringLength2))
    	    		scrollRow =RED;
    	    	else if(!strncmp((const char* )pcParameterString2,"blue",xParameterStringLength2))
    	    		scrollRow =BLUE;
    	    	else if(!strncmp((const char* )pcParameterString2,"yellow",xParameterStringLength2))
    	    		scrollRow =YELLOW;
    	    	else if(!strncmp((const char* )pcParameterString2,"cyan",xParameterStringLength2))
    	    		scrollRow =CYAN;
    	    	else if(!strncmp((const char* )pcParameterString2,"magenta",xParameterStringLength2))
    	    		scrollRow =MAGENTA;
    	    	else if(!strncmp((const char* )pcParameterString2,"green",xParameterStringLength2))
    	    		scrollRow =GREEN;
    	    	else if(!strncmp((const char* )pcParameterString2,"aqua",xParameterStringLength2))
    	    		scrollRow =AQUA;
    	    	else if(!strncmp((const char* )pcParameterString2,"purple",xParameterStringLength2))
    	    		scrollRow =PURPLE;
    	    	else if(!strncmp((const char* )pcParameterString2,"lightblue",xParameterStringLength2))
    	    		scrollRow =LIGHTBLUE;
    	    	else if(!strncmp((const char* )pcParameterString2,"orange",xParameterStringLength2))
    	    		scrollRow =ORANGE;
    	    	else if(!strncmp((const char* )pcParameterString2,"indigo",xParameterStringLength2))
    	    		scrollRow =INDIGO;
    /* Obtain the 3st parameter string. */
    pcParameterString3 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,3,&xParameterStringLength3);
    intensity =(uint8_t )atol((char* )pcParameterString3);

    /* Obtain the 4st parameter string. */
       pcParameterString4 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,4,&xParameterStringLength4);
       scrollTime =(uint16_t )atol((char* )pcParameterString4);

    result =LEDMatrixScrollMode(baseColour, scrollRow, intensity, scrollTime);

    /* Respond to the command */
    if(result == H16R6_OK)
    {
		strncpy(par[0],(char* )pcParameterString1,xParameterStringLength1);
		strncpy(par[1],(char* )pcParameterString2,xParameterStringLength2);

    	sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,par[0],par[1], intensity, scrollTime);
    }
    else if(result == H16R6_ERR_WrongIntensity)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongIntensityMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}
/*-----------------------------------------------------------*/

portBASE_TYPE CLI_FlashModeCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t baseColour =0;
    uint8_t flashColour =0;
    uint8_t intensity =0;
    uint16_t flashTime;
    uint16_t timeBetweenFlash;
    char par[2][15] ={0};
    static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3, *pcParameterString4, *pcParameterString5;
    portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0, xParameterStringLength3 =0, xParameterStringLength4 =0, xParameterStringLength5 =0;

    static const int8_t *pcOKMessage =(int8_t* )"FlashMode:baseColour is %s and flashColour is %s and intensity %d %% and flashTime is %d and timeBetweenFlash is %d \n\r";
    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";


    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    /* Read the color value. */
    	if(!strncmp((const char* )pcParameterString1,"black",xParameterStringLength1))
    		baseColour =BLACK;
    	else if(!strncmp((const char* )pcParameterString1,"white",xParameterStringLength1))
    		baseColour =WHITE;
    	else if(!strncmp((const char* )pcParameterString1,"red",xParameterStringLength1))
    		baseColour =RED;
    	else if(!strncmp((const char* )pcParameterString1,"blue",xParameterStringLength1))
    		baseColour =BLUE;
    	else if(!strncmp((const char* )pcParameterString1,"yellow",xParameterStringLength1))
    		baseColour =YELLOW;
    	else if(!strncmp((const char* )pcParameterString1,"cyan",xParameterStringLength1))
    		baseColour =CYAN;
    	else if(!strncmp((const char* )pcParameterString1,"magenta",xParameterStringLength1))
    		baseColour =MAGENTA;
    	else if(!strncmp((const char* )pcParameterString1,"green",xParameterStringLength1))
    		baseColour =GREEN;
    	else if(!strncmp((const char* )pcParameterString1,"aqua",xParameterStringLength1))
    		baseColour =AQUA;
    	else if(!strncmp((const char* )pcParameterString1,"purple",xParameterStringLength1))
    		baseColour =PURPLE;
    	else if(!strncmp((const char* )pcParameterString1,"lightblue",xParameterStringLength1))
    		baseColour =LIGHTBLUE;
    	else if(!strncmp((const char* )pcParameterString1,"orange",xParameterStringLength1))
    		baseColour =ORANGE;
    	else if(!strncmp((const char* )pcParameterString1,"indigo",xParameterStringLength1))
    		baseColour =INDIGO;
    	/* Obtain the 2st parameter string. */
    	    pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);
    	    /* Read the color value. */
    	    	if(!strncmp((const char* )pcParameterString2,"black",xParameterStringLength2))
    	    		flashColour =BLACK;
    	    	else if(!strncmp((const char* )pcParameterString2,"white",xParameterStringLength2))
    	    		flashColour =WHITE;
    	    	else if(!strncmp((const char* )pcParameterString2,"red",xParameterStringLength2))
    	    		flashColour =RED;
    	    	else if(!strncmp((const char* )pcParameterString2,"blue",xParameterStringLength2))
    	    		flashColour =BLUE;
    	    	else if(!strncmp((const char* )pcParameterString2,"yellow",xParameterStringLength2))
    	    		flashColour =YELLOW;
    	    	else if(!strncmp((const char* )pcParameterString2,"cyan",xParameterStringLength2))
    	    		flashColour =CYAN;
    	    	else if(!strncmp((const char* )pcParameterString2,"magenta",xParameterStringLength2))
    	    		flashColour =MAGENTA;
    	    	else if(!strncmp((const char* )pcParameterString2,"green",xParameterStringLength2))
    	    		flashColour =GREEN;
    	    	else if(!strncmp((const char* )pcParameterString2,"aqua",xParameterStringLength2))
    	    		flashColour =AQUA;
    	    	else if(!strncmp((const char* )pcParameterString2,"purple",xParameterStringLength2))
    	    		flashColour =PURPLE;
    	    	else if(!strncmp((const char* )pcParameterString2,"lightblue",xParameterStringLength2))
    	    		flashColour =LIGHTBLUE;
    	    	else if(!strncmp((const char* )pcParameterString2,"orange",xParameterStringLength2))
    	    		flashColour =ORANGE;
    	    	else if(!strncmp((const char* )pcParameterString2,"indigo",xParameterStringLength2))
    	    		flashColour =INDIGO;
    /* Obtain the 3st parameter string. */
    pcParameterString3 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,3,&xParameterStringLength3);
    intensity =(uint8_t )atol((char* )pcParameterString3);

    /* Obtain the 4st parameter string. */
       pcParameterString4 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,4,&xParameterStringLength4);
       flashTime =(uint16_t )atol((char* )pcParameterString4);

       /* Obtain the 5st parameter string. */
          pcParameterString5 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,5,&xParameterStringLength5);
          timeBetweenFlash =(uint16_t )atol((char* )pcParameterString5);

    result =LEDMatrixFlashMode(baseColour, flashColour, intensity, flashTime, timeBetweenFlash);

    /* Respond to the command */
    if(result == H16R6_OK)
    {
		strncpy(par[0],(char* )pcParameterString1,xParameterStringLength1);
		strncpy(par[1],(char* )pcParameterString2,xParameterStringLength2);

    	sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,par[0],par[1], intensity, flashTime,timeBetweenFlash);
    }
    else if(result == H16R6_ERR_WrongIntensity)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongIntensityMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;

}
/*-----------------------------------------------------------*/

portBASE_TYPE CLI_ColorPickerModeCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
    Module_Status result =H16R6_OK;
    uint8_t Colour =0;
    uint16_t Time;
    uint8_t intensity =0;
    char par[2][15] ={0};
    static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3;
    portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0, xParameterStringLength3 =0;

    static const int8_t *pcOKMessage =(int8_t* )"ColorPickerMode:Colour is %s and Time is %d and and intensity %d %% \n\r";
    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";


    (void )xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Obtain the 1st parameter string. */
    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
    /* Read the color value. */
    	if(!strncmp((const char* )pcParameterString1,"black",xParameterStringLength1))
    		Colour =BLACK;
    	else if(!strncmp((const char* )pcParameterString1,"white",xParameterStringLength1))
    		Colour =WHITE;
    	else if(!strncmp((const char* )pcParameterString1,"red",xParameterStringLength1))
    		Colour =RED;
    	else if(!strncmp((const char* )pcParameterString1,"blue",xParameterStringLength1))
    		Colour =BLUE;
    	else if(!strncmp((const char* )pcParameterString1,"yellow",xParameterStringLength1))
    		Colour =YELLOW;
    	else if(!strncmp((const char* )pcParameterString1,"cyan",xParameterStringLength1))
    		Colour =CYAN;
    	else if(!strncmp((const char* )pcParameterString1,"magenta",xParameterStringLength1))
    		Colour =MAGENTA;
    	else if(!strncmp((const char* )pcParameterString1,"green",xParameterStringLength1))
    		Colour =GREEN;
    	else if(!strncmp((const char* )pcParameterString1,"aqua",xParameterStringLength1))
    		Colour =AQUA;
    	else if(!strncmp((const char* )pcParameterString1,"purple",xParameterStringLength1))
    		Colour =PURPLE;
    	else if(!strncmp((const char* )pcParameterString1,"lightblue",xParameterStringLength1))
    		Colour =LIGHTBLUE;
    	else if(!strncmp((const char* )pcParameterString1,"orange",xParameterStringLength1))
    		Colour =ORANGE;
    	else if(!strncmp((const char* )pcParameterString1,"indigo",xParameterStringLength1))
    		Colour =INDIGO;

    	/* Obtain the 2st parameter string. */
		   pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);
		   Time =(uint16_t )atol((char* )pcParameterString2);

    /* Obtain the 3st parameter string. */
    pcParameterString3 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,3,&xParameterStringLength3);
    intensity =(uint8_t )atol((char* )pcParameterString3);

    result =LEDMatrixRGBColorPickerMode(Colour, Time, intensity);

    /* Respond to the command */
    if(result == H16R6_OK)
    {
		strncpy(par[0],(char* )pcParameterString1,xParameterStringLength1);

    	sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,par[0],Time,intensity);
    }
    else if(result == H16R6_ERR_WrongIntensity)
        strcpy((char* )pcWriteBuffer,(char* )pcWrongIntensityMessage);

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;

}
/*-----------------------------------------------------------*/

portBASE_TYPE CLI_SetColorSomeLedCommand(int8_t *pcWriteBuffer,size_t xWriteBufferLen,const int8_t *pcCommandString){
	 Module_Status result =H16R6_OK;
	    uint8_t StartLed =0;
	    uint8_t EndLed =0;
	    uint8_t color =0;
	    uint8_t intensity =0;
	    char par[15] ={0};
	    static int8_t *pcParameterString1, *pcParameterString2, *pcParameterString3, *pcParameterString4;
	    portBASE_TYPE xParameterStringLength1 =0, xParameterStringLength2 =0;
	    portBASE_TYPE xParameterStringLength3 =0,xParameterStringLength4 =0;

	    static const int8_t *pcOKMessage =(int8_t* )"StartLed is %d ,EndLed is %d,LED color is %s at intensity %d%%\n\r";
	    static const int8_t *pcWrongLedOutRangeMessage =(int8_t* )"Wrong LedOutRange!\n\r";
	    static const int8_t *pcWrongIntensityMessage =(int8_t* )"Wrong intensity!\n\r";


	    (void )xWriteBufferLen;
	    configASSERT(pcWriteBuffer);

	    /* Obtain the 1st parameter string. */
	    pcParameterString1 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,1,&xParameterStringLength1);
	    StartLed =(uint8_t )atol((char* )pcParameterString1);
	    /* Obtain the 2st parameter string. */
	    pcParameterString2 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,2,&xParameterStringLength2);
	    EndLed =(uint8_t )atol((char* )pcParameterString2);

	    /* Obtain the 3st parameter string. */
	    pcParameterString3 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,3,&xParameterStringLength3);
	    /* Read the color value. */
	    	if(!strncmp((const char* )pcParameterString3,"black",xParameterStringLength3))
	    		color =BLACK;
	    	else if(!strncmp((const char* )pcParameterString3,"white",xParameterStringLength3))
	    		color =WHITE;
	    	else if(!strncmp((const char* )pcParameterString3,"red",xParameterStringLength3))
	    		color =RED;
	    	else if(!strncmp((const char* )pcParameterString3,"blue",xParameterStringLength3))
	    		color =BLUE;
	    	else if(!strncmp((const char* )pcParameterString3,"yellow",xParameterStringLength3))
	    		color =YELLOW;
	    	else if(!strncmp((const char* )pcParameterString3,"cyan",xParameterStringLength3))
	    		color =CYAN;
	    	else if(!strncmp((const char* )pcParameterString3,"magenta",xParameterStringLength3))
	    		color =MAGENTA;
	    	else if(!strncmp((const char* )pcParameterString3,"green",xParameterStringLength3))
	    		color =GREEN;
	    	else if(!strncmp((const char* )pcParameterString3,"aqua",xParameterStringLength3))
	    		color =AQUA;
	    	else if(!strncmp((const char* )pcParameterString3,"purple",xParameterStringLength3))
	    		color =PURPLE;
	    	else if(!strncmp((const char* )pcParameterString3,"lightblue",xParameterStringLength3))
	    		color =LIGHTBLUE;
	    	else if(!strncmp((const char* )pcParameterString3,"orange",xParameterStringLength3))
	    		color =ORANGE;
	    	else if(!strncmp((const char* )pcParameterString3,"indigo",xParameterStringLength3))
	    		color =INDIGO;

	    /* Obtain the 4nd parameter string. */
	    pcParameterString4 =(int8_t* )FreeRTOS_CLIGetParameter(pcCommandString,4,&xParameterStringLength4);
	    intensity =(uint8_t )atol((char* )pcParameterString4);


	    result =LEDMatrixSetColorSomeLed(StartLed, EndLed, color, intensity);

	    /* Respond to the command */
	    if(result == H16R6_OK)
	    {	strncpy(par,(char* )pcParameterString3,xParameterStringLength3);
	        sprintf((char* )pcWriteBuffer,(char* )pcOKMessage,StartLed,EndLed,par,intensity);
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

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
