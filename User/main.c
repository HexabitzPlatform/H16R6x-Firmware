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

	// put your code here, to run repeatedly.
	while(1){
		LEDMatrixRGBColorPickerMode(WHITE, 10, 4);
		Delay_ms(500);
		LEDMatrixRGBColorPickerMode(YELLOW, 10, 4);
		Delay_ms(500);
		LEDMatrixRGBColorPickerMode(BLACK, 10, 4);
		Delay_ms(500);

}
}
/*-----------------------------------------------------------*/
