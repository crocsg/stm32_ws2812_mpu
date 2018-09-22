/*
 * inv_i2c_util.h
 *
 *  Created on: 22 sept. 2018
 *      Author: Stephane
 */

#ifndef INC_INV_MPU_HAL_UTIL_H_
#define INC_INV_MPU_HAL_UTIL_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"
#include "stdint.h"
#include <math.h>

#define __no_operation() (0)


int reg_int_cb(void* int_param);
void get_hal_ticks (unsigned long *timestamp);
void set_i2c_device (I2C_HandleTypeDef* pi2c);
HAL_StatusTypeDef i2c_write(uint8_t slave_addr, uint8_t reg_addr,
    uint8_t length, uint8_t const *data);
HAL_StatusTypeDef i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t length,
    uint8_t *data);
HAL_StatusTypeDef IICwriteBit(uint8_t slave_addr, uint8_t reg_addr,
    uint8_t bitNum, uint8_t data);
HAL_StatusTypeDef IICwriteBits(uint8_t slave_addr, uint8_t reg_addr,
    uint8_t bitStart, uint8_t length, uint8_t data);

#endif /* INC_INV_MPU_HAL_UTIL_H_ */
