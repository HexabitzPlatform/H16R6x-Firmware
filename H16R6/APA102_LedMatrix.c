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
	for (int led = 0; led < LED_FRAME_SIZE; led++)
	{
		digitalLedframe[led].FieldsIn.INIT = 0x07; // Set MSB first 3 bits to identify start of LED packet
		digitalLedframe[led].FieldsIn.GLOBAL = 0x00; // Switch off LED global
		digitalLedframe[led].FieldsIn.BLUE = 0x00;
		digitalLedframe[led].FieldsIn.GREEN = 0x00;
		digitalLedframe[led].FieldsIn.RED = 0x00;
	}
	DigiLed_update(FALSE); // Update frame buffer using the value of frameModified as set in initialiser.
}
/*-----------------------------------------------------------*/
/*
 * @brief set color of a single led
 * Set the colors of a single led ad position 'led' using single colors
 * @param led position of the led in the string
 * @param blue intensity of the blue color from 0 to 255
 * @param green intensity of the green color from 0 to 255
 * @param red intensity of the red color from 0 to 255
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 */
void DigiLed_setColor(uint8_t led, uint8_t red, uint8_t green, uint8_t blue,uint8_t illumination)
{
	if (DigiLed_TestPosition(led) == RANGE_OK)
	{
	if (illumination>Illumination_LED)
	{
		illumination=Illumination_LED;
	}
		digitalLedframe[led].FieldsIn.INIT = 0x7; // Set MSB first 3 bits to identify start of LED packet
		digitalLedframe[led].FieldsIn.GLOBAL = illumination; // Set led at maximum illumination 0x1F=31
		digitalLedframe[led].FieldsIn.BLUE = blue;
		digitalLedframe[led].FieldsIn.GREEN = green;
		digitalLedframe[led].FieldsIn.RED = red;
	}
	frameModified = TRUE;
}
/*-----------------------------------------------------------*/
/**
 * @brief set color of all LEDs in a string
 * @param blue intensity of the blue color from 0 to 255
 * @param green intensity of the green color from 0 to 255
 * @param red intensity of the red color from 0 to 255
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 *
 */
void DigiLed_setAllColor(uint8_t red, uint8_t green, uint8_t blue,uint8_t illumination)
{

	for (int led = 0; led < LED_FRAME_SIZE; led++)
	{
		DigiLed_setColor(led, red, green, blue,illumination);
	}
}
/*-----------------------------------------------------------*/
/**
 * @brief set color of a single led
 * Set the colors of a single led ad position 'led' using RGB color scheme
 * RGB colors are 24 bits of a 32 bit word where the intensity of the colors red, green en blue are
 * expressed as hex values from 0 to 255 (0 - FF).
 * Colors can be set using defines from "APA102_colors.h"
 * @param led position of the led in the string
 * @param rgb color of led in RGB color scheme
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 */
void DigiLed_setRGB(uint8_t led, uint32_t rgb,uint8_t illumination)
{
	if (DigiLed_TestPosition(led) == RANGE_OK)
	{
	if (illumination>Illumination_LED)
	{
		illumination=Illumination_LED;
	}
		digitalLedframe[led].FieldsIn.INIT = 0x7;
		digitalLedframe[led].FieldsIn.GLOBAL = illumination;// Set led at maximum illumination 0x1F=31
		digitalLedframe[led].FieldsIn.BLUE = (uint8_t)(rgb);
		digitalLedframe[led].FieldsIn.GREEN = (uint8_t)(rgb >> 8);
		digitalLedframe[led].FieldsIn.RED = (uint8_t)(rgb >> 16);
		frameModified = TRUE;
	}
}
/*-----------------------------------------------------------*/
/**
 * @brief set color of a single led
 * Set the colors of a single led ad position 'led' using RGB color scheme
 * RGB colors are 24 bits of a 32 bit word where the intensity of the colors red, green and blue are
 * expressed as hex values from 0 to 255 (0 - FF).
 * Colors can be set using defines from "APA102_colors.h"
 * @param rgb color of led in RGB color scheme
 * @param Illumination is a value from 0 to 31. 0 means no light, and 31 maximum illumination
 */
void DigiLed_setAllRGB(uint32_t rgb,uint8_t illumination)
{
	for (int led = 0; led < LED_FRAME_SIZE; led++)
	{
		DigiLed_setRGB(led, rgb,illumination);
	}
}
/*-----------------------------------------------------------*/
/**
 * @brief switch a single led off
 * @param led position of the led in the string to be switched off
 */
void DigiLed_setLedOff(uint8_t led)
{
	if (DigiLed_TestPosition(led) == RANGE_OK)
	{
		digitalLedframe[led].FieldsIn.GLOBAL = 0x00;
	}
	frameModified = TRUE;
}
/*-----------------------------------------------------------*/
/**
 * @All leds off
 */
void DigiLed_setAllLedOff()
{
	for (int led = 0; led < LED_FRAME_SIZE; led++)
	{
		DigiLed_setLedOff(led);
	}
}
/*-----------------------------------------------------------*/
/**
 * @brief switch a single led on
 * Using this function will preserve the active color settings for the led
 * @param led position of the led in the string to be switched on
 */
void DigiLed_setLedOn(uint8_t led,uint8_t illumination)
{
	if (illumination>Illumination_LED)
	{
		illumination=Illumination_LED;
	}
	if (DigiLed_TestPosition(led) == RANGE_OK)
	{
		digitalLedframe[led].FieldsIn.GLOBAL = illumination;
	}
	frameModified = TRUE;
}
/*-----------------------------------------------------------*/
/**
 * @brief  All leds on
 * Using this function will preserve the active color settings for the led
 */
void DigiLed_setAllLedOn(uint8_t illumination)
{
	for (int led = 0; led < LED_FRAME_SIZE; led++)
	{
		DigiLed_setLedOn(led,illumination);
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
	if (led < LED_FRAME_SIZE)
	{
		returnValue = RANGE_OK;
	}
	return returnValue;
}
/*-----------------------------------------------------------*/
