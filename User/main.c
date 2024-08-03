/*
 BitzOS (BOS) V0.3.5 - Copyright (C) 2017-2024 Hexabitz
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
    srand(HAL_GetTick());
	//Don't place your code here.
	for (;;) {
	}
}
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
	LEDMatrixCrossFadeModeLEDRGB(10, 55, 33, 88, 3000, 3);

	// put your code here, to run repeatedly.
	while (1) {
//		LEDMatrixSetAllRGB( 80, 20,10,5);
//		Delay_ms(2000);
//		LEDMatrixCrossFadeModeLEDRGB(10, 70, 240, 150, 3000, 5);
//Delay_ms(2000);
//		LEDMatrixCrossFadeModeALLLEDRGB( 0, 255, 0, 3000,5);
//		LEDMatrixCrossFadeModeALLLEDRGB( 255, 55, 0, 3000,5);
//		LEDMatrixCrossFadeModeALLLEDRGB( 72, 7, 255, 3000,5);
		LEDMatrixSprinkleMode(200, 50,98 , 100, 3000, 40);


//		Delay_ms(2000);
//		LEDMatrixCrossFadeModeALLLEDRGB(0,255, 0, 0, 0, 255, 3000,5);
//
//
//		LEDMatrixRGBColorPickerMode(RED,200,5);

	}
}
/*-----------------------------------------------------------*/
