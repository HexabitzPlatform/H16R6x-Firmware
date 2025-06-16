/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name  : H16R6_spi.c
 Description: Configures SPI1 instance for module H16R6.
 SPI: Initializes SPI1 for APA102 LED matrix communication with master mode and DMA support.
 GPIO: Sets up SPI pins (SCLK, MOSI) in alternate function mode.
*/

/* Includes ****************************************************************/
#include "BOS.h"
#include "H16R6_spi.h"

/* Exported Variables ******************************************************/
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;
/***************************************************************************/
/* Configure SPI ***********************************************************/
/***************************************************************************/

/* SPI1 init function */
void MX_SPI1_Init(void) {
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_1LINE;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;

	HAL_SPI_Init(&hspi1);

}

/***************************************************************************/
void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle) {

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	if (spiHandle->Instance == SPI1) {
		/* SPI1 clock enable */
		__HAL_RCC_SPI1_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		/**SPI1 GPIO Configuration
		 PB3     ------> SPI1_SCK
		 PB5     ------> SPI1_MOSI
		 */
		GPIO_InitStruct.Pin = LED_MATRIX_SPI_SCK_PIN | LED_MATRIX_SPI_MOSI_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
		HAL_GPIO_Init(LED_MATRIX_SPI_PORT, &GPIO_InitStruct);

	    /* SPI1 DMA Init */
	    /* SPI1_TX Init */
	    hdma_spi1_tx.Instance = DMA2_Channel1;
	    hdma_spi1_tx.Init.Request = DMA_REQUEST_SPI1_TX;
	    hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	    hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	    hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
	    hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	    hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	    hdma_spi1_tx.Init.Mode = DMA_CIRCULAR;
	    hdma_spi1_tx.Init.Priority = DMA_PRIORITY_LOW;
	    if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
	    {
//	      Error_Handler();
	    }

	    __HAL_LINKDMA(spiHandle,hdmatx,hdma_spi1_tx);

	}
}

/***************************************************************************/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle) {

	if (spiHandle->Instance == SPI1) {
		/* Peripheral clock disable */
		__HAL_RCC_SPI1_CLK_DISABLE();

		/**SPI1 GPIO Configuration
		 PB3     ------> SPI1_SCK
		 PB5     ------> SPI1_MOSI
		 */
		HAL_GPIO_DeInit(LED_MATRIX_SPI_PORT, LED_MATRIX_SPI_SCK_PIN | LED_MATRIX_SPI_MOSI_PIN);
	    /* SPI1 DMA DeInit */
	    HAL_DMA_DeInit(spiHandle->hdmatx);
	}
}

/***************************************************************************/
/* send data buffer via SPI port
 * xPort: SPI port handle
 * pData: Pointer to data buffer
 * Size: data size in bytes unit
 */
Status_TypeDef SendSPI(SPI_HANDLE *xPort, uint8_t pData[], uint16_t Size) {
	Status_TypeDef Status = STATUS_ERR;
//	taskENTER_CRITICAL();
	if (NULL != xPort && NULL != pData) {
		if (HAL_OK == HAL_SPI_Transmit_DMA(xPort, pData, Size))
			Status = STATUS_OK;
	} else
		Status = STATUS_ERR;
//	taskEXIT_CRITICAL();
	return Status;
}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
