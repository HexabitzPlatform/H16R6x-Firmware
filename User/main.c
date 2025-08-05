/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name     : main.c
 Description   : Main program body.
 */

/* Includes ****************************************************************/
#include "BOS.h"

/* Private variables *******************************************************/
float adcValue1 = 0;
float adcValue2 = 0;
float adcValue3 = 0;
float adcValue4 = 0;
float percentageAdcValue1 = 0;
float percentageAdcValue2 = 0;
float percentageAdcValue3 = 0;
float percentageAdcValue4 = 0;
/* Private Function Prototypes *********************************************/

/* Main Function ***********************************************************/
int main(void){

	/* Initialize Module &  BitzOS */
	Module_Init();

	/* Don't place your code here */
	for(;;){
	}
}
int f ;
Module_Status status ;
/***************************************************************************/
/* User Task */
void UserTask(void *argument){

	/* put your code here, to run repeatedly. */
	while(1){

    // 1. Turn off LED 3
    status = LEDOff(3);
    HAL_Delay(500);

    // 2. Turn off all LEDs
    status = MatrixOff();
    HAL_Delay(500);

    // 3. Set LED 5 to Blue with 80% brightness
    status = SetLedRGB(5, 0, 0, 255, 8);
    HAL_Delay(500);

    // 4. Set entire matrix to Red with 100% brightness
    status = SetMatrixRGB(255, 0, 0, 10);
    HAL_Delay(1000);

    // 5. Set LEDs 10 to 20 to Yellow (predefined color)
    status = SetMatrixRangeColor(10, 20, YELLOW, 7);
    HAL_Delay(1000);
    status = SetMatrixRangeColor(10, 20, YELLOW, 1);
    HAL_Delay(1000);
    // 6. Set LED 7 to Green (predefined color)
    status = SetLedColor(7, GREEN, 9);
    HAL_Delay(1000);

    status = SetLedColor(7, GREEN, 1);
     HAL_Delay(1000);

    // 7. Set entire matrix to Cyan (predefined color)
    status = SetMatrixColor(CYAN, 10);
    HAL_Delay(1000);

    // 8. Scroll mode (Red base, Blue row, 70% brightness, 300ms delay)
    status = ScrollModeMatrix(RED, BLUE, 7, 30);
    HAL_Delay(2000); // Allow time for effect to run

    // 9. Flash mode between Magenta and Green, 500ms on/off
    status = FlashModeMatrix(MAGENTA, GREEN, 9, 500, 500);
    HAL_Delay(3000);

    // 10. Picker mode: slowly fill matrix with Orange, 60ms delay per LED
//    status = PickerModeMatrix(ORANGE, 60, 8);
//    HAL_Delay(3000);

    // 11. Cross-fade matrix to Purple (RGB: 128, 0, 128), 2s transition
    status = FadeModeMatrixRGB(128, 0, 128, 2000, 10);
    HAL_Delay(2500);
    status = FadeModeMatrixRGB(128, 0, 128, 2000, 1);
    HAL_Delay(2500);
    // Final step: turn off all
    status = MatrixOff();}
}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
