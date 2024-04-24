/*
 * APA102_LedMatrix.c
 * Description:APA102 Led Matrix  Source file
 * Created on: April 17, 2024
 * Author: Mohamad Khatib @ Hexabitz
******************************************************************************
 * @attention: this driver requires to set up SPI
 *
 * Copyright (c) 2024 Hexabitz.
 * All rights reserved.
 *
 ******************************************************************************
 */

#include "APA102_LedMatrix.h"

/* variables */
uint8_t SpiSendFrame[LED_START_FRAME_SIZE + 4 * LED_FRAME_SIZE + LED_END_FRAME_SIZE];
uint8_t frameModified; 		// when frame is changed the stimuli is set high

DigitalLedframe digitalLedframe[LED_FRAME_SIZE];
/* functions */
void DigiLed_init()
{
	frameModified = TRUE; 		// Initial set to true to force update after initialization of frame buffer
	// TODO Auto-generated constructor stub
	for (int led = 1; led <= LED_FRAME_SIZE; led++)
	{
		digitalLedframe[led-1].FieldsIn.INIT = 0x07; // Set MSB first 3 bits to identify start of LED packet
		digitalLedframe[led-1].FieldsIn.GLOBAL = 0x00; // Switch off LED global
		digitalLedframe[led-1].FieldsIn.BLUE = 0x00;
		digitalLedframe[led-1].FieldsIn.GREEN = 0x00;
		digitalLedframe[led-1].FieldsIn.RED = 0x00;
	}
	DigiLed_update(FALSE); // Update frame buffer using the value of frameModified as set in initialiser.
}
/*-----------------------------------------------------------*/
/**
 * Set color from a predefined color list in "APA102_LedMatrix.h" in enum BasicColors
 */
uint32_t DigiLed_SwitchColors(uint8_t Color)
{
	uint32_t rgb;
	switch(Color)
	{
		case BLACK:
			rgb =0x000000;
			break;
		case WHITE:
			rgb =0xFFFFFF;
			break;
		case RED:
			rgb =0xFF0000;
			break;
		case BLUE:
			rgb =0x0000FF;
			break;
		case YELLOW:
			rgb =0xFFFF00;
			break;
		case CYAN:
			rgb =0x00FFFF;
			break;
		case MAGENTA:
			rgb =0xFF00FF;
			break;
		case GREEN:
			rgb =0x00FF00;
			break;
		case AQUA:
			rgb =0x00FFFF;
			break;
		case PURPLE:
			rgb =0x800080;
			break;
		case LIGHTBLUE:
			rgb =0xadd8e6;
			break;
		case ORANGE:
			rgb =0xFFA500;
			break;
		case INDIGO:
			rgb =0x4b0082;
			break;
		default:
			break;
	}
	return rgb;
}
/*-----------------------------------------------------------*/
/*
 * Set the colors of a single led using RGB color
 * @param led position of the led in the string led>=1
 * @param red intensity of the red color from 0 to 255
 * @param green intensity of the green color from 0 to 255
 * @param blue intensity of the blue color from 0 to 255
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLed_SetRGB(uint8_t led, uint8_t red, uint8_t green, uint8_t blue,uint8_t intensity)
{
	if (led<1) {led=1;}
	if (DigiLed_TestPosition(led) == RANGE_OK)
	{
	if (intensity>intensity_LED)
	{
		intensity=intensity_LED;
	}
		digitalLedframe[led-1].FieldsIn.INIT = 0x7; // Set MSB first 3 bits to identify start of LED packet
		digitalLedframe[led-1].FieldsIn.GLOBAL = intensity; // Set led at maximum intensity 0x1F=31
		digitalLedframe[led-1].FieldsIn.BLUE = blue;
		digitalLedframe[led-1].FieldsIn.GREEN = green;
		digitalLedframe[led-1].FieldsIn.RED = red;
	}
	frameModified = TRUE;
}
/*-----------------------------------------------------------*/
/**
  * Set the colors of all LEDs using RGB color scheme
 * @param red intensity of the red color from 0 to 255
 * @param green intensity of the green color from 0 to 255
 * @param blue intensity of the blue color from 0 to 255
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLed_SetAllRGB(uint8_t red, uint8_t green, uint8_t blue,uint8_t intensity)
{

	for (int led = 1; led <= LED_FRAME_SIZE; led++)
	{
		DigiLed_SetRGB(led, red, green, blue,intensity);
	}
}
/*-----------------------------------------------------------*/
/**
 * Set the colors of a single led using single colors
 * @param led position of the led in the string led>=1
 * Set LED color from a predefined color list in "APA102_LedMatrix.h"
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLed_SetColor(uint8_t led,uint8_t color ,uint8_t intensity)
{
	uint32_t rgb=0x000000;
	rgb=DigiLed_SwitchColors(color);
	if (led<1) {led=1;}
	if (DigiLed_TestPosition(led) == RANGE_OK)
	{
	if (intensity>intensity_LED)
	{
		intensity=intensity_LED;
	}
		digitalLedframe[led-1].FieldsIn.INIT = 0x7;
		digitalLedframe[led-1].FieldsIn.GLOBAL = intensity;// Set led at maximum intensity 0x1F=31
		digitalLedframe[led-1].FieldsIn.BLUE = (uint8_t)(rgb);
		digitalLedframe[led-1].FieldsIn.GREEN = (uint8_t)(rgb >> 8);
		digitalLedframe[led-1].FieldsIn.RED = (uint8_t)(rgb >> 16);
		frameModified = TRUE;
	}
}
/*-----------------------------------------------------------*/
/**
 * set color of all LEDs in a string
 * Set LED color from a predefined color list in "BOS.h"
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLed_SetAllColor(uint8_t color,uint8_t intensity)
{
	for (int led = 1; led <= LED_FRAME_SIZE; led++)
	{
		DigiLed_SetColor(led,color,intensity);
	}
}
/*-----------------------------------------------------------*/
/**
  * @switch a single led off  led>=1
 * @param led position of the led in the string to be switched off
 */
void DigiLed_SetLedOff(uint8_t led)
{
	if (led<1) {led=1;}
	if (DigiLed_TestPosition(led) == RANGE_OK)
	{
		digitalLedframe[led-1].FieldsIn.GLOBAL = 0x00;
	}
	frameModified = TRUE;
}
/*-----------------------------------------------------------*/
/**
 * @All leds off
 */
void DigiLed_SetAllLedOff()
{
	for (int led = 1; led <= LED_FRAME_SIZE; led++)
	{
		DigiLed_SetLedOff(led);
	}
}
/*-----------------------------------------------------------*/
/**
 * switch a single led on
 * Using this function will preserve the active color settings for the led
 * @param led position of the led in the string to be switched on led>=1
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLed_SetLedOn(uint8_t led,uint8_t intensity)
{
	if (led<1) {led=1;}
	if (intensity>intensity_LED)
	{
		intensity=intensity_LED;
	}
	if (DigiLed_TestPosition(led) == RANGE_OK)
	{
		digitalLedframe[led-1].FieldsIn.GLOBAL = intensity;
	}
	frameModified = TRUE;
}
/*-----------------------------------------------------------*/
/**
 * All leds on
 * Using this function will preserve the active color settings for the led led>=1
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLed_SetAllLedOn(uint8_t intensity)
{
	for (int led = 1; led <= LED_FRAME_SIZE; led++)
	{
		DigiLed_SetLedOn(led,intensity);
	}
}
/*-----------------------------------------------------------*/
/**
 * @brief update led string
 * @param set true to force update leds and false to update only when frame is modified
 */
void DigiLed_update(uint8_t forceUpdate)
{
	if(frameModified | forceUpdate)
	{
	// add start of frame (0x00000000)
	for(int i = 0; i < LED_START_FRAME_SIZE; i++)
	{
		SpiSendFrame[i] = 0x00;
	}
	// add all LED packets of the frame
	uint32_t SpiDataPacket = 0;
	for (uint32_t led = 0; led < LED_FRAME_SIZE; led++)
	{
		SpiSendFrame[LED_START_FRAME_SIZE + SpiDataPacket + 0] = digitalLedframe[led].FieldsOut.CMD;		// Add INIT and GLOBAL to SPI send frame
		SpiSendFrame[LED_START_FRAME_SIZE + SpiDataPacket + 1] = digitalLedframe[led].FieldsOut.BLUE; 	// Add BLUE to SPI send frame
		SpiSendFrame[LED_START_FRAME_SIZE + SpiDataPacket + 2] = digitalLedframe[led].FieldsOut.GREEN;	// Add GREEN to SPI send frame
		SpiSendFrame[LED_START_FRAME_SIZE + SpiDataPacket + 3] = digitalLedframe[led].FieldsOut.RED;		// Add RED to SPI send frame
		SpiDataPacket = SpiDataPacket + 4;
	}
	// add end of frame (0xffffffff)
	for(int i = 0; i < 4; i++)
	{
		SpiSendFrame[LED_START_FRAME_SIZE + 4*LED_FRAME_SIZE + i] = 0xFF;
	}
	// send spi frame with all led values
	 SendSPI(LED_MATRIX_SPI_HANDLER, SpiSendFrame, sizeof(SpiSendFrame));

	}

	frameModified = FALSE; // reset frame modified identifier.
}
/*-----------------------------------------------------------*/
/**
 * @brief get LED frame size
 * @return LED frame size
 */
uint8_t DigiLed_getFrameSize(void)
{
	return LED_FRAME_SIZE;
}
/*-----------------------------------------------------------*/
/**
 * @brief Test led position is within range.
 * @param led led position
 * @return result of evaluation ad define.
 */
uint8_t DigiLed_TestPosition(uint8_t led)
{
	uint8_t returnValue = OUT_OF_RANGE;
	if (led <= LED_FRAME_SIZE)
	{
		returnValue = RANGE_OK;
	}
	return returnValue;
}
/*-----------------------------------------------------------*/
/**
 * Scroll - one row of one colour, the rest another colour, row moves down one for each update
 */
void DigiLed_ScrollMode(uint8_t Base_Colour,uint8_t Scroll_Row,uint8_t intensity,uint8_t Scroll_Time)
{

	DigiLed_SetAllColor(Base_Colour, intensity);
	HAL_Delay(Scroll_Time);
	int led=1;
	for (led=1;led<=64;)
	{
		DigiLed_SetAllColor(Base_Colour, intensity);
		DigiLed_SetColor(led, Scroll_Row, intensity);
		HAL_Delay(Scroll_Time);
		led=led+8;
	}

}
/*-----------------------------------------------------------*/
/**
* Flash - flash from one colour to another with user-settable flash times and intervals
 */
void DigiLed_FlashMode(uint8_t Base_Colour,uint8_t flash_Colour,uint8_t intensity,uint8_t flash_Time,uint8_t Time_Between_Flash)
{

	DigiLed_SetAllColor(Base_Colour, intensity);
	HAL_Delay(flash_Time);
	DigiLed_SetAllLedOff();
	HAL_Delay(Time_Between_Flash);
	DigiLed_SetAllColor(flash_Colour, intensity);
	HAL_Delay(flash_Time);
	DigiLed_SetAllLedOff();
	HAL_Delay(Time_Between_Flash);
}
/*-----------------------------------------------------------*/
