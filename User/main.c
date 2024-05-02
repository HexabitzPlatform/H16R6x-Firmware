/*
 BitzOS (BOS) V0.3.3 - Copyright (C) 2017-2024 Hexabitz
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

	// put your code here, to run repeatedly.
	while(1){

		LEDMatrixSetRGB(4, 50, 100, 150, 10);
		LEDMatrixSetRGB(2, 0, 255, 0, 10);
		Delay_ms(1000);
		LEDMatrixSetLedOff(2);
		Delay_ms(1000);
		LEDMatrixSetLedOn(2, 30);
		Delay_ms(1000);
		LEDMatrixSetAllRGB(0, 0, 255, 20);
		Delay_ms(1000);
		LEDMatrixSetAllLedOff();
		Delay_ms(1000);
		LEDMatrixSetAllLedOn(5);
		Delay_ms(1000);
		LEDMatrixSetColor(1,LIGHTBLUE, 16);
		Delay_ms(1000);
		LEDMatrixSetAllColor(WHITE, 10);
		Delay_ms(1000);


	}
}

/*-----------------------------------------------------------*/
