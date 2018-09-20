/*
 * ws2812spi.h
 *
 *  Created on: 16 sept. 2018
 *      Author: Stephane
 */

#ifndef INC_WS2812SPI_H_
#define INC_WS2812SPI_H_

#define NB_PIXEL 8


void 		WS2812BSPI_SendData (void);
int 		WS2312BSPI_Init (SPI_HandleTypeDef* hspi, DMA_HandleTypeDef* hdma_spi1_tx, int nbpixel);
uint8_t*	WS2812BSPI_GetPixelPtr (int pixel);
void 		WS2812BSPI_encode_pixel_index (uint8_t R, uint8_t G, uint8_t B, int pixel);
void 		WS2812BSPI_encode_pixel_ptr (uint8_t R, uint8_t G, uint8_t B, uint8_t* spi_data);


#endif /* INC_WS2812SPI_H_ */
