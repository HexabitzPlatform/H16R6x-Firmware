/*
 BitzOS (BOS) V0.3.4 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved

 File Name     : APA102_LedMatrix.c
 Description   : APA102 Led Matrix  Source file

 @attention: this driver requires to set up SPI

 */

#include "APA102_LedMatrix.h"

/* variables */
uint8_t SpiSendFrame[LEDSTARTFRAMESIZE + 4 * LEDFRAMESIZE + LEDENDFRAMESIZE];
uint8_t frameModified; 		// when frame is changed the stimuli is set high

DigitalLedframe digitalLedframe[LEDFRAMESIZE];
/* functions */
void DigiLedInit() {
	frameModified = TRUE; // Initial set to true to force update after initialization of frame buffer
	// TODO Auto-generated constructor stub
	for (int led = 1; led <= LEDFRAMESIZE; led++) {
		digitalLedframe[led - 1].FieldsIn.INIT = 0x07; // Set MSB first 3 bits to identify start of LED packet
		digitalLedframe[led - 1].FieldsIn.GLOBAL = 0x00; // Switch off LED global
		digitalLedframe[led - 1].FieldsIn.BLUE = 0x00;
		digitalLedframe[led - 1].FieldsIn.GREEN = 0x00;
		digitalLedframe[led - 1].FieldsIn.RED = 0x00;
	}
	DigiLedUpdate(FALSE); // Update frame buffer using the value of frameModified as set in initialiser.
}
/*-----------------------------------------------------------*/
/**
 * Set color from a predefined color list in "APA102_LedMatrix.h" in enum BasicColors
 */
uint32_t DigiLedSwitchColors(uint8_t Color) {
	uint32_t rgb;
	switch (Color) {
	case BLACK:
		rgb = 0x000000;
		break;
	case WHITE:
		rgb = 0xFFFFFF;
		break;
	case RED:
		rgb = 0xFF0000;
		break;
	case BLUE:
		rgb = 0x0000FF;
		break;
	case YELLOW:
		rgb = 0xFFFF00;
		break;
	case CYAN:
		rgb = 0x00FFFF;
		break;
	case MAGENTA:
		rgb = 0xFF00FF;
		break;
	case GREEN:
		rgb = 0x00FF00;
		break;
	case AQUA:
		rgb = 0x00FFFF;
		break;
	case PURPLE:
		rgb = 0x800080;
		break;
	case LIGHTBLUE:
		rgb = 0xadd8e6;
		break;
	case ORANGE:
		rgb = 0xFFA500;
		break;
	case INDIGO:
		rgb = 0x4b0082;
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
void DigiLedSetRGB(uint8_t led, uint8_t red, uint8_t green, uint8_t blue,
		uint8_t intensity) {
	if (led < 1) {
		led = 1;
	}
	if (DigiLedTestPosition(led) == RANGEOK) {
		if (intensity > INTINSITYLED) {
			intensity = INTINSITYLED;
		}
		digitalLedframe[led - 1].FieldsIn.INIT = 0x7; // Set MSB first 3 bits to identify start of LED packet
		digitalLedframe[led - 1].FieldsIn.GLOBAL = intensity; // Set led at maximum intensity 0x1F=31
		digitalLedframe[led - 1].FieldsIn.BLUE = blue;
		digitalLedframe[led - 1].FieldsIn.GREEN = green;
		digitalLedframe[led - 1].FieldsIn.RED = red;
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
void DigiLedSetAllRGB(uint8_t red, uint8_t green, uint8_t blue,
		uint8_t intensity) {

	for (int led = 1; led <= LEDFRAMESIZE; led++) {
		DigiLedSetRGB(led, red, green, blue, intensity);
	}
}
/*-----------------------------------------------------------*/
/**
 * Set the colors of a single led using single colors
 * @param led position of the led in the string led>=1
 * Set LED color from a predefined color list in "APA102_LedMatrix.h"
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLedSetColor(uint8_t led, uint8_t color, uint8_t intensity) {
	uint32_t rgb = 0x000000;
	rgb = DigiLedSwitchColors(color);
	if (led < 1) {
		led = 1;
	}
	if (DigiLedTestPosition(led) == RANGEOK) {
		if (intensity > INTINSITYLED) {
			intensity = INTINSITYLED;
		}
		digitalLedframe[led - 1].FieldsIn.INIT = 0x7;
		digitalLedframe[led - 1].FieldsIn.GLOBAL = intensity; // Set led at maximum intensity 0x1F=31
		digitalLedframe[led - 1].FieldsIn.BLUE = (uint8_t) (rgb);
		digitalLedframe[led - 1].FieldsIn.GREEN = (uint8_t) (rgb >> 8);
		digitalLedframe[led - 1].FieldsIn.RED = (uint8_t) (rgb >> 16);
		frameModified = TRUE;
	}
}
/*-----------------------------------------------------------*/
/**
 * set color of all LEDs in a string
 * Set LED color from a predefined color list in "BOS.h"
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLedSetAllColor(uint8_t color, uint8_t intensity) {
	for (int led = 1; led <= LEDFRAMESIZE; led++) {
		DigiLedSetColor(led, color, intensity);
	}
}
/*-----------------------------------------------------------*/
/**
 * @switch a single led off  led>=1
 * @param led position of the led in the string to be switched off
 */
void DigiLedSetLedOff(uint8_t led) {
	if (led < 1) {
		led = 1;
	}
	if (DigiLedTestPosition(led) == RANGEOK) {
		digitalLedframe[led - 1].FieldsIn.GLOBAL = 0x00;
	}
	frameModified = TRUE;
}
/*-----------------------------------------------------------*/
/**
 * @All leds off
 */
void DigiLedSetAllLedOff() {
	for (int led = 1; led <= LEDFRAMESIZE; led++) {
		DigiLedSetLedOff(led);
	}
}
/*-----------------------------------------------------------*/
/**
 * switch a single led on
 * Using this function will preserve the active color settings for the led
 * @param led position of the led in the string to be switched on led>=1
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLedSetLedOn(uint8_t led, uint8_t intensity) {
	if (led < 1) {
		led = 1;
	}
	if (intensity > INTINSITYLED) {
		intensity = INTINSITYLED;
	}
	if (DigiLedTestPosition(led) == RANGEOK) {
		digitalLedframe[led - 1].FieldsIn.GLOBAL = intensity;
	}
	frameModified = TRUE;
}
/*-----------------------------------------------------------*/
/**
 * All leds on
 * Using this function will preserve the active color settings for the led led>=1
 * @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 */
void DigiLedSetAllLedOn(uint8_t intensity) {
	for (int led = 1; led <= LEDFRAMESIZE; led++) {
		DigiLedSetLedOn(led, intensity);
	}
}
/*-----------------------------------------------------------*/
/**
 * @brief update led string
 * @param set true to force update leds and false to update only when frame is modified
 */
void DigiLedUpdate(uint8_t forceUpdate) {
	if (frameModified | forceUpdate) {
		// add start of frame (0x00000000)
		for (int i = 0; i < LEDSTARTFRAMESIZE; i++) {
			SpiSendFrame[i] = 0x00;
		}
		// add all LED packets of the frame
		uint32_t SpiDataPacket = 0;
		for (uint32_t led = 0; led < LEDFRAMESIZE; led++) {
			SpiSendFrame[LEDSTARTFRAMESIZE + SpiDataPacket + 0] =
					digitalLedframe[led].FieldsOut.CMD;	// Add INIT and GLOBAL to SPI send frame
			SpiSendFrame[LEDSTARTFRAMESIZE + SpiDataPacket + 1] =
					digitalLedframe[led].FieldsOut.BLUE; // Add BLUE to SPI send frame
			SpiSendFrame[LEDSTARTFRAMESIZE + SpiDataPacket + 2] =
					digitalLedframe[led].FieldsOut.GREEN;// Add GREEN to SPI send frame
			SpiSendFrame[LEDSTARTFRAMESIZE + SpiDataPacket + 3] =
					digitalLedframe[led].FieldsOut.RED;	// Add RED to SPI send frame
			SpiDataPacket = SpiDataPacket + 4;
		}
		// add end of frame (0xffffffff)
		for (int i = 0; i < 4; i++) {
			SpiSendFrame[LEDSTARTFRAMESIZE + 4 * LEDFRAMESIZE + i] = 0xFF;
		}
		// send spi frame with all led values
		SendSPI(LEDMATRIXSPIHANDLER, SpiSendFrame, sizeof(SpiSendFrame));

	}

	frameModified = FALSE; // reset frame modified identifier.
}
/*-----------------------------------------------------------*/
/**
 * @brief get LED frame size
 * @return LED frame size
 */
uint8_t DigiLedGetFrameSize(void) {
	return LEDFRAMESIZE;
}
/*-----------------------------------------------------------*/
/**
 * @brief Test led position is within range.
 * @param led led position
 * @return result of evaluation ad define.
 */
uint8_t DigiLedTestPosition(uint8_t led) {
	uint8_t returnValue = OUTOFRANGE;
	if (led <= LEDFRAMESIZE) {
		returnValue = RANGEOK;
	}
	return returnValue;
}
/*-----------------------------------------------------------*/
/**
 * scroll - one row of one colour, the rest another colour, row moves down one for each update
 *  @param baseColour Basic color
 *  @param scrollRow Secondary color
 *  @param intensity is a value from 0 to 31. 0 means no light, and 31 maximum intensity
 *  @param scrollTime Secondary color retention time  value in millisecond.
 */
void DigiLedScrollMode(uint8_t baseColour, uint8_t scrollRow, uint8_t intensity,
		uint16_t scrollTime) {

	DigiLedSetAllColor(baseColour, intensity);
	DigiLedUpdate(1);
	HAL_Delay(scrollTime);
	int led = 1;
	int ledON = 1;

	for (led = 1; led <= 64;) {
		DigiLedSetAllColor(baseColour, intensity);
		DigiLedUpdate(1);
		for (ledON = led; ledON < led + 8;) {
			DigiLedSetColor(ledON, scrollRow, intensity);
			DigiLedUpdate(1);
			ledON++;
		}
		HAL_Delay(scrollTime);
		led = led + 8;

	}

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
void DigiLedFlashMode(uint8_t baseColour, uint8_t flashColour,
		uint8_t intensity, uint16_t flashTime, uint16_t timeBetweenFlash) {

	DigiLedSetAllColor(baseColour, intensity);
	DigiLedUpdate(1);
	HAL_Delay(flashTime);
	DigiLedSetAllLedOff();
	DigiLedUpdate(1);
	HAL_Delay(timeBetweenFlash);
	DigiLedSetAllColor(flashColour, intensity);
	DigiLedUpdate(1);
	HAL_Delay(flashTime);
	DigiLedSetAllLedOff();
	DigiLedUpdate(1);
	HAL_Delay(timeBetweenFlash);

}
/*-----------------------------------------------------------*/
