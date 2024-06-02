/*
 BitzOS (BOS) V0.3.4 - Copyright (C) 2017-2024 Hexabitz
 All rights reserved

 File Name     : spi.h
 Description   : This file contains all the function prototypes for
  *              the spi.c file


 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */
/* SPI type define */
typedef SPI_HandleTypeDef SPI_HANDLE;

/* USER CODE BEGIN ET */
typedef enum{
	STATUS_OK = 0,
	STATUS_INV,
	STATUS_TMOUT,
	STATUS_ERR=255
}Status_TypeDef;
/* SPI port definitions */
#define TIM_OUT_10MS         		10u

/* USER CODE END Private defines */

void MX_SPI1_Init(void);
Status_TypeDef SendSPI(SPI_HANDLE *xPort, uint8_t pData[], uint16_t Size);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

