/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name  : H16R6_spi.h
 Description: Header file for SPI function prototypes for module H16R6.
 SPI: Declares SPI1 handle and initialization functions for APA102 LED matrix communication.
*/


/* Define to prevent recursive inclusion ***********************************/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ****************************************************************/
#include "stm32g0xx_hal.h"

/* Exported Variables ******************************************************/
extern SPI_HandleTypeDef  hspi1;

/* SPI Typedef definition **************************************************/
typedef SPI_HandleTypeDef SPI_HANDLE;

typedef enum{
	STATUS_OK = 0,
	STATUS_INV,
	STATUS_TMOUT,
	STATUS_ERR=255
}Status_TypeDef;

/* Exported Functions ******************************************************/
void MX_SPI1_Init(void);
Status_TypeDef SendSPI(SPI_HANDLE *xPort, uint8_t pData[], uint16_t Size);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
