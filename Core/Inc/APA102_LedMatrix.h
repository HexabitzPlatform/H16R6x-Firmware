/*
 * APA102_LedMatrix.h
 * Description: APA102 Led Matrix Header file
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

#ifndef APA102_LEDMATRIX_H_
#define APA102_LEDMATRIX_H_

/* includes */
#include "spi.h"
#include <stdint.h>
/* defines */

#define LEDMATRIXSPIHANDLER        &hspi1

#define 	LEDFRAMESIZE   		    64		///< \define number of LEDs in the chain

#define 	FALSE 					0		// false value
#define 	TRUE 					1		// true value
#define 	OUTOFRANGE			    1		// chosen LED does not exist
#define 	RANGEOK 				0		// chosen LED exist
#define 	LEDSTARTFRAMESIZE 	    4		// 0x00, 0x00, 0x00, 0x00
#define 	LEDENDFRAMESIZE 		4 		// 0xFF, 0xFF, 0xFF, 0xFF
#define     INTINSITYLED            31      //maximum intensity 31
/* variables */
typedef union {
	struct {					// LSB
		uint8_t RED :8;	///< RED led intensity from 0 (off) to 255 (max)
		uint8_t GREEN :8; 	///< GREEN led intensity from 0 (off) to 255 (max)
		uint8_t BLUE :8; 	///< BLUE led intensity from 0 (off) to 255 (max)
		uint8_t GLOBAL :5; ///< Global intensity for all LEDs from 0 (off) to 32 (max)
		uint8_t INIT :3; ///< Initialize, the led configuration starts with '111'
	// MSB
	} FieldsIn;					///< All fields in a LED packet
	struct {					// LSB
		uint8_t RED :8;		///< RED led intensity from 0 (off) to 255 (max)
		uint8_t GREEN :8; 	///< GREEN led intensity from 0 (off) to 255 (max)
		uint8_t BLUE :8; 	///< BLUE led intensity from 0 (off) to 255 (max)
		uint8_t CMD :8; ///< Global intensity for all LEDs && frame start with
	// MSB
	} FieldsOut;
	uint32_t data; 				///< RAW LED packet data
} DigitalLedframe;	///< Frame of LED packets

/* Color Enumerations */
enum BasicColors {
	BLACK = 1,
	WHITE,
	RED,
	BLUE,
	YELLOW,
	CYAN,
	MAGENTA,
	GREEN,
	AQUA,
	PURPLE,
	LIGHTBLUE,
	ORANGE,
	INDIGO,
};
/* functions */
void DigiLedInit();
void DigiLedSetRGB(uint8_t led, uint8_t red, uint8_t green, uint8_t blue,
		uint8_t intensity);
void DigiLedSetAllRGB(uint8_t red, uint8_t green, uint8_t blue,
		uint8_t intensity);
void DigiLedSetColor(uint8_t led, uint8_t color, uint8_t intensity);
void DigiLedSetAllColor(uint8_t color, uint8_t intensity);
void DigiLedSetLedOff(uint8_t led);
void DigiLedSetAllLedOff();
void DigiLedSetLedOn(uint8_t led, uint8_t intensity);
void DigiLedSetAllLedOn(uint8_t intensity);
void DigiLedUpdate(uint8_t forceUpdate);
uint8_t DigiLedGetFrameSize(void);
uint8_t DigiLedTestPosition(uint8_t led);
void DigiLedScrollMode(uint8_t baseColour, uint8_t scrollRow, uint8_t intensity,
		uint16_t scrollTime);
void DigiLedFlashMode(uint8_t baseColour, uint8_t flashColour,
		uint8_t intensity, uint16_t flashTime, uint16_t timeBetweenFlash);
void DigiLedRGBColorPickerMode(uint8_t color,uint16_t time,uint8_t intensity);

#endif /* APA102_LEDMATRIX_H_ */
