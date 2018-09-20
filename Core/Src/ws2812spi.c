/*
 * ws2812spi.c
 *
 *  Created on: 16 sept. 2018
 *      Author: Stephane
 */

#include <stdlib.h>
#include <memory.h>

#include "stm32f1xx_hal.h"

#include "ws2812spi.h"



#define PIXEL_SPI_SIZE  9 // pixel size in bytes on spi channel
#define HEADER_SIZE	1
#define FOOTER_SIZE PIXEL_SPI_SIZE

// offset in bytes from pixel start
#define PIXEL_RED_OFFSET	3
#define PIXEL_GREEN_OFFSET	0
#define PIXEL_BLUE_OFFSET	(3 * 2)

static uint8_t ws2812_buffer[NB_PIXEL * 9 + HEADER_SIZE + FOOTER_SIZE];
static size_t ws2812_buffer_size = 0;

static SPI_HandleTypeDef* _hspi;
static DMA_HandleTypeDef* _hdma_spi1_tx;

int WS2312BSPI_Init (SPI_HandleTypeDef* hspi, DMA_HandleTypeDef* hdma_spi1_tx, int nbpixel)
{
	size_t size = nbpixel * 9 + HEADER_SIZE + FOOTER_SIZE;
//	if ((ws2812_buffer = malloc (size)) == NULL)
//			return (-1);
	ws2812_buffer_size = size;

	memset (ws2812_buffer, 0, ws2812_buffer_size);

	_hspi = hspi;
	_hdma_spi1_tx = hdma_spi1_tx;

	return (0);
}


void WS2812BSPI_SendData (void)
{
	HAL_SPI_Transmit_DMA(_hspi, ws2812_buffer, sizeof(ws2812_buffer));
}

uint8_t *WS2812BSPI_GetPixelPtr (int pixel)
{
	return (ws2812_buffer + ((pixel * PIXEL_SPI_SIZE) + HEADER_SIZE));
}

void WS2812BSPI_encode_byte (uint8_t b, uint8_t* spi_data)
{
	uint32_t data = 0;
	uint8_t* pdata = (uint8_t*) &data;

	for (int idx = 0; idx < 8; idx++)
	{
		data <<= 3;

		if ((b & (1 << 7)) != 0)
			data |= 0b110;
		else
			data |= 0b100;

		b <<= 1;
	}
	spi_data[0] = pdata[2];
	spi_data[1] = pdata[1];
	spi_data[2] = pdata[0];
}

void WS2812BSPI_encode_pixel_index (uint8_t R, uint8_t G, uint8_t B, int pixel)
{
	WS2812BSPI_encode_pixel_ptr (R, G, B, WS2812BSPI_GetPixelPtr (pixel) );
}

void WS2812BSPI_encode_pixel_ptr (uint8_t R, uint8_t G, uint8_t B, uint8_t* spi_data)
{
	WS2812BSPI_encode_byte (G, &spi_data[PIXEL_GREEN_OFFSET]);
	WS2812BSPI_encode_byte (R, &spi_data[PIXEL_RED_OFFSET]);
	WS2812BSPI_encode_byte (B, &spi_data[PIXEL_BLUE_OFFSET]);
}


