/*
 BitzOS (BOS) V0.3.5 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved

 File Name     : main.c
 Description   : Main program body.
 */
/* Includes ------------------------------------------------------------------*/
#include "BOS.h"

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Main function ------------------------------------------------------------*/

int main(void){
	Module_Init();		//Initialize Module &  BitzOS

	//Don't place your code here.
	for(;;){}
}
/*-----------------------------------------------------------*/

/* User Task */
void UserTask(void *argument){
	LEDMatrixCrossFadeMode( YELLOW, AQUA, MAGENTA, 100);
//	LEDMatrixMotionMode(BLUE, YELLOW, 5, 0.7);
//	MotionMode(AQUA, MAGENTA, 5, 0.4);
	// put your code here, to run repeatedly.
	while(1){
//		LEDMatrixSetColorSomeLed(1, 2, MAGENTA, 5);
		LEDMatrixCrossFadeMode(BLUE, YELLOW, 5, 0.7);
		LEDMatrixCrossFadeMode(baseColour, seconedColor, thirdColor, time)
	}
}
/*-----------------------------------------------------------*/
