/*
 * ws2812spi.c
 *
 *  Created on: 16 sept. 2018
 *      Author: Stephane
 */

#include "stm32f1xx_hal.h"

#define BIT23 (1<<23)
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

void WS2812BSPI_encode_pixel (uint8_t R, uint8_t G, uint8_t B, uint8_t* spi_data)
{
	WS2812BSPI_encode_byte (G, spi_data);
	WS2812BSPI_encode_byte (R, &spi_data[3]);
	WS2812BSPI_encode_byte (B, &spi_data[6]);
}


void writeWS2812B(unsigned long Value, uint8_t* SPI_Data )
{
    // have to expand each bit to 3 bits
    // Can then output 110 for WS2812B logic '1'
    // and 100 for WS2812B logic '0'
    uint32_t Encoding=0;

    int Index;

    // Process the GREEN byte
    Index=0;
    Encoding=0;
    while (Index < 8)
    {
        Encoding = Encoding << 3;
        if (Value & BIT23)
        {
            Encoding |= 0b110;
        }
        else
        {
            Encoding |= 0b100;
        }
        Value = Value << 1;
        Index++;

    }
    SPI_Data[0] = ((Encoding >> 16) & 0xff);
    SPI_Data[1] = ((Encoding >> 8) & 0xff);
    SPI_Data[2] = (Encoding & 0xff);

    // Process the RED byte
    Index=0;
    Encoding=0;
    while (Index < 8)
    {
        Encoding = Encoding << 3;
        if (Value & BIT23)
        {
            Encoding |= 0b110;
        }
        else
        {
            Encoding |= 0b100;
        }
        Value = Value << 1;
        Index++;

    }
    SPI_Data[3] = ((Encoding >> 16) & 0xff);
    SPI_Data[4] = ((Encoding >> 8) & 0xff);
    SPI_Data[5] = (Encoding & 0xff);

    // Process the BLUE byte
    Index=0;
    Encoding=0;
    while (Index < 8)
    {
        Encoding = Encoding << 3;
        if (Value & BIT23)
        {
            Encoding |= 0b110;
        }
        else
        {
            Encoding |= 0b100;
        }
        Value = Value << 1;
        Index++;

    }
    SPI_Data[6] = ((Encoding >> 16) & 0xff);
    SPI_Data[7] = ((Encoding >> 8) & 0xff);
    SPI_Data[8] = (Encoding & 0xff);

    // Now send out the 24 (x3) bits to the SPI bus


}
