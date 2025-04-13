/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name     : H16R6_spi.h
 Description   : This file contains all the function prototypes for
               the spi.c file
 */


/* Define to prevent recursive inclusion ***********************************/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ****************************************************************/
#include "stm32g0xx_hal.h"

/* SPI port definitions */
#define TIM_OUT_10MS         		10u

/* Exported Variables ******************************************************/
extern SPI_HandleTypeDef hspi1;

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
