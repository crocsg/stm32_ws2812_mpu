/*
 * ws2812spi.h
 *
 *  Created on: 16 sept. 2018
 *      Author: Stephane
 */

#ifndef INC_WS2812SPI_H_
#define INC_WS2812SPI_H_

void WS2812BSPI_encode_pixel (uint8_t R, uint8_t G, uint8_t B, uint8_t* spi_data);
void writeWS2812B(unsigned long Value, uint8_t* SPI_Data );

#endif /* INC_WS2812SPI_H_ */
