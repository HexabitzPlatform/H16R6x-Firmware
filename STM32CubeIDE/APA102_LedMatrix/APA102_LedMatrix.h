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
#include "H16R6_spi.h"
#include <stdint.h>
/* defines */

#define LED_MATRIX_SPI_HANDLER    &hspi1

#define 	LED_FRAME_SIZE   		64		///< \define number of LEDs in the chain

#define 	FALSE 					0		// false value
#define 	TRUE 					1		// true value
#define 	OUT_OF_RANGE			1		// chosen LED does not exist
#define 	RANGE_OK 				0		// chosen LED exist
#define 	LED_START_FRAME_SIZE 	4		// 0x00, 0x00, 0x00, 0x00
#define 	LED_END_FRAME_SIZE 		4 		// 0xFF, 0xFF, 0xFF, 0xFF
#define     Illumination_LED        31      //maximum illumination 31

/* variables */
union
{
	struct
	{					// LSB
		uint8_t RED    : 8;	///< RED led intensity from 0 (off) to 255 (max)
		uint8_t GREEN  : 8; 	///< GREEN led intensity from 0 (off) to 255 (max)
		uint8_t BLUE   : 8; 	///< BLUE led intensity from 0 (off) to 255 (max)
		uint8_t GLOBAL : 5; 	///< Global intensity for all LEDs from 0 (off) to 32 (max)
		uint8_t INIT   : 3; 	///< Initialize, the led configuration starts with '111'
						// MSB
	} FieldsIn;					///< All fields in a LED packet
	struct
	{					// LSB
		uint8_t RED    : 8;		///< RED led intensity from 0 (off) to 255 (max)
		uint8_t GREEN  : 8; 	///< GREEN led intensity from 0 (off) to 255 (max)
		uint8_t BLUE   : 8; 	///< BLUE led intensity from 0 (off) to 255 (max)
		uint8_t CMD	   : 8; 	///< Global intensity for all LEDs && frame start with
						// MSB
	} FieldsOut;
	uint32_t data; 				///< RAW LED packet data
} digitalLedframe[LED_FRAME_SIZE];	///< Frame of LED packets


/* functions */
void DigiLed_init();
void DigiLed_setColor(uint8_t led, uint8_t red, uint8_t green, uint8_t blue,uint8_t illumination);
void DigiLed_setAllColor(uint8_t red, uint8_t green, uint8_t blue,uint8_t illumination);
void DigiLed_setRGB(uint8_t led, uint32_t rgb,uint8_t illumination);
void DigiLed_setAllRGB(uint32_t rgb,uint8_t illumination);
void DigiLed_setLedOff(uint8_t led);
void DigiLed_setAllLedOff();
void DigiLed_setLedOn(uint8_t led,uint8_t illumination);
void DigiLed_setAllLedOn(uint8_t illumination);
void DigiLed_update(uint8_t forceUpdate);
uint8_t DigiLed_getFrameSize(void);
uint8_t DigiLed_TestPosition(uint8_t led);

#endif /* APA102_LEDMATRIX_H_ */
