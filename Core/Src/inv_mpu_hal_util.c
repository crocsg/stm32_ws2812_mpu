/*
 * inv_i2c_util.c
 *
 *  Created on: 22 sept. 2018
 *      Author: Stephane
 */


#include <inv_mpu_hal_util.h>
#include "main.h"

I2C_HandleTypeDef* _i2c_dev;

void set_i2c_device (I2C_HandleTypeDef* pi2c)
{
	_i2c_dev = pi2c;
}

HAL_StatusTypeDef i2c_write(uint8_t slave_addr, uint8_t reg_addr,
    uint8_t length, uint8_t const *data) {
  return HAL_I2C_Mem_Write (_i2c_dev, slave_addr << 1, reg_addr,
  I2C_MEMADD_SIZE_8BIT, (uint8_t*) data, length, 10);
}

HAL_StatusTypeDef i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t length,
    uint8_t *data) {
  return HAL_I2C_Mem_Read(_i2c_dev, slave_addr << 1, reg_addr,
  I2C_MEMADD_SIZE_8BIT, data, length, 10);

}

HAL_StatusTypeDef IICwriteBit(uint8_t slave_addr, uint8_t reg_addr,
    uint8_t bitNum, uint8_t data) {
  uint8_t tmp;
  i2c_read(slave_addr, reg_addr, 1, &tmp);
  tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
  return i2c_write(slave_addr, reg_addr, 1, &tmp);
}
;

HAL_StatusTypeDef IICwriteBits(uint8_t slave_addr, uint8_t reg_addr,
    uint8_t bitStart, uint8_t length, uint8_t data) {

  uint8_t tmp, dataShift;
  HAL_StatusTypeDef status = i2c_read(slave_addr, reg_addr, 1, &tmp);
  if (status == HAL_OK) {
    uint8_t mask = (((1 << length) - 1) << (bitStart - length + 1));
    dataShift = data << (bitStart - length + 1);
    tmp &= mask;
    tmp |= dataShift;
    return i2c_write(slave_addr, reg_addr, 1, &tmp);
  } else {
    return status;
  }
}

void get_hal_ticks (unsigned long *timestamp)
{
	*timestamp = HAL_GetTick();
}

int reg_int_cb(void* int_param)
{
  UNUSED(int_param);
    return 0;
}
