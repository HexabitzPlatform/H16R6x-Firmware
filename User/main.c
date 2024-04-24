/*
 BitzOS (BOS) V0.3.2 - Copyright (C) 2017-2024 Hexabitz
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
//		LEDMatrix_SetRGB(1, White, 16);
//		Delay_ms(1000);
//		LEDMatrix_SetAllRGB(Black, 10);
//		Delay_ms(500);
//		LEDMatrix_SetAllRGB(Yellow, 10);
//		Delay_ms(500);
//		LEDMatrix_SetAllRGB(Aqua, 10);
//		Delay_ms(500);
//		LEDMatrix_SetAllRGB(Fuchsia, 10);
//		Delay_ms(500);
		LEDMatrix_SetAllRGB(Purple, 10);
		Delay_ms(500);
		LEDMatrix_SetAllRGB(lightblue, 10);
		Delay_ms(500);
		LEDMatrix_SetAllRGB(orange, 10);
		Delay_ms(500);
		LEDMatrix_SetAllRGB(pink, 10);
		Delay_ms(500);
		LEDMatrix_SetAllRGB(Indigo, 10);
		Delay_ms(500);

//		LEDMatrix_SetColor(0, 50, 100, 150, 10);
//		LEDMatrix_SetColor(2, 0, 255, 0, 10);
//		Delay_ms(1000);
//		LEDMatrix_SetLedOff(2);
//		Delay_ms(1000);
//		LEDMatrix_SetLedOn(2, 30);
//		Delay_ms(1000);
//		LEDMatrix_SetAllColor(0, 0, 255, 20);
//		Delay_ms(1000);
//		LEDMatrix_SetAllLedOff();
//		Delay_ms(1000);
//		LEDMatrix_SetAllLedOn(5);
//		Delay_ms(1000);
//		LEDMatrix_SetRGB(1, 0x0A010F, 16);
//		Delay_ms(1000);
//		LEDMatrix_SetAllRGB(0xF00F01, 10);
//		Delay_ms(1000);



	}
}

/*-----------------------------------------------------------*/
