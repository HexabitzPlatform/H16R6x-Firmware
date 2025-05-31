/*
 BitzOS (BOS) V0.3.6 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved

 File Name     : main.c
 Description   : Main program body.
 */
/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include <stdlib.h>
#include <time.h>
/* Private variables ---------------------------------------------------------*/
//int randomNumber1 =0,randomNumber2=0;
/* Private function prototypes -----------------------------------------------*/



/* Main function ------------------------------------------------------------*/
int main(void) {
	Module_Init();		//Initialize Module &  BitzOS
//    srand(HAL_GetTick());
	//Don't place your code here.
	for (;;) {
	}
}
uint8_t f;
float v1,v2,v3,v4;
/*-----------------------------------------------------------*/
/* User Task */
void UserTask(void *argument) {
//	LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(30, 200, 30, 50, 3000, 10);

//	LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(200, 70, 200, 100, 1000, 10);
//	LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(200, 70, 200, 80, 3000, 10);LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(200, 70, 200, 70, 3000, 10);LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(200, 70, 200, 50, 3000, 10);LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(200, 70, 200, 25, 3000, 10);LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(200, 70, 200, 10, 3000, 10);LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(200, 70, 200, 50, 1000, 10);LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(200, 70, 200, 25, 1000, 10);LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(200, 70, 200, 10, 1000, 10);LEDMatrixSetAllLedOff();

//	LEDMatrixSetAllLedOff();
//		LEDMatrixSprinkleMode(30, 200, 30, 10, 3000, 10);
//		LEDMatrixSprinkleMode(250, 10, 140, 100, 8000, 10);
//
//	LEDMatrixSprinkleMode(30, 200, 30, 100, 1000, 10);
//	LEDMatrixSprinkleMode(200, 70, 200, 50, 3000, 10);

//	LEDMatrixSetAllRGB(255, 20, 14, 4);
//	LEDMatrixSprinkleMode(20, 20, 200, 50, 3000, 10);
//	LEDMatrixSetRGB(10,255, 20, 14, 4);
//	LEDMatrixSetAllLedOff();
//	LEDMatrixCrossFadeMode(RED, BLUE, YELLOW,2000);
//	LEDMatrixSprinkleMode(200, 70, 200, 100, 3000, 10);
//	LEDMatrixCrossFadeModeALLLEDRGB(0,200, 0,127, 127, 127, 3000,5);
	//LEDMatrixSetAllLedOff();
//	LEDMatrixSprinkleMode(0, 0, 200, 30, 5000, 20);
//	LEDMatrixSprinkleMode(30, 220, 30, 50, 3000, 50);
//	LEDMatrixSprinkleMode(0, 0, 150, 50, 3000, 40);
//	LEDMatrixCrossFadeModeLEDRGB(10, 55, 33, 88, 3000, 3);
//	SwapUartPins(&huart1, REVERSED);
//	SwapUartPins(&huart2, REVERSED);
//	SwapUartPins(&huart3, REVERSED);
//	SwapUartPins(&huart4, REVERSED);
//	SwapUartPins(&huart5, REVERSED);
//	SwapUartPins(&huart6, REVERSED);
	// put your code here, to run repeatedly.

	while (1) {
//		LEDMatrixSetRGB(15, 100, 50, 80,30);
		 if(f==1){f=0;
		 LEDMatrixSetRGB(1, 100, 50, 80,5);}
		 if(f==2){f=0;
		 LEDMatrixSetAllRGB(100, 50, 80,5);}
		 if(f==3){f=0;
		 LEDMatrixSetColor(1,5 ,5);}
		 if(f==4){f=0;
		 LEDMatrixSetAllColor(1,5);}
		 if(f==5){f=0;
		 LEDMatrixSetLedOff(1);}
		 if(f==6){f=0;
		 LEDMatrixSetAllLedOff();}
		 if(f==7){f=0;
		 LEDMatrixSetLedOn(1,5);}
		 if(f==8){f=0;
		 LEDMatrixSetAllLedOn(5);}
		 if(f==9){f=0;
		 LEDMatrixScrollMode(5,1,5,5000);}
		 if(f==10){f=0;
		 LEDMatrixFlashMode(5,1,5,1000,5000);}
		 if(f==11){f=0;
		 LEDMatrixRGBColorPickerMode(1,5000,5);}
		 if(f==12){f=0;
		 LEDMatrixSetColorSomeLed(1,10,1 ,5);}
		 if(f==13){f=0;
		 LEDMatrixMotionMode(1, 2,5,5.2);}
		 if(f==14){f=0;
		 LEDMatrixCrossFadeMode(1,2,3,2000);}
		 if(f==15){f=0;
		 LEDMatrixCrossFadeModeLEDRGB(1, 2,
				3, 4, 5000, 20);}
		 if(f==16){f=0;
		 LEDMatrixCrossFadeModeALLLEDRGB(1,
				2, 3, 1000, 20);}
		 if(f==17){f=0;
		 LEDMatrixSprinkleMode(1, 2,
				3, 10, 1000, 50);}
//		if(f==1)
//		{
//			ADCSelectPort(P1);
//			ADCSelectPort(P2);
//			f=0;
//		}
//		if(f==2)
//		{
//			ADCDeinitChannel(P1);
//			ADCDeinitChannel(P2);
//			f=0;
//		}
//
//		if(f==3)
//		{
//			ReadADCChannel(P1,"top",&v1);
//			ReadADCChannel(P1,"bottom",&v2);
//			ReadADCChannel(P2,"top",&v3);
//			ReadADCChannel(P2,"bottom",&v4);
//		}
//		LEDMatrixSetAllRGB( 80, 20,10,5);
//		Delay_ms(2000);
//		LEDMatrixCrossFadeModeLEDRGB(10, 70, 240, 150, 3000, 5);
//Delay_ms(2000);
//		LEDMatrixCrossFadeModeALLLEDRGB( 0, 255, 0, 3000,5);
//		LEDMatrixCrossFadeModeALLLEDRGB( 255, 55, 0, 3000,5);
//		LEDMatrixCrossFadeModeALLLEDRGB( 72, 7, 255, 3000,5);
//		LEDMatrixSprinkleMode(200, 50,98 , 100, 3000, 40);


//		Delay_ms(2000);
//		LEDMatrixCrossFadeModeALLLEDRGB(0,255, 0, 0, 0, 255, 3000,5);
//
//
//		LEDMatrixRGBColorPickerMode(RED,200,5);

	}
}
/*-----------------------------------------------------------*/
