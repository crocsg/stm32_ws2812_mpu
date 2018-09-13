/*
 * mpu_data_handler.h
 *
 *  Created on: 13 sept. 2018
 *      Author: lenovo
 */

#ifndef INC_MPU_DATA_HANDLER_H_
#define INC_MPU_DATA_HANDLER_H_

typedef struct _mpu_data
{
	int16_t ax;
	int16_t ay;
	int16_t az;

	int16_t gx;
	int16_t gy;
	int16_t gz;

} mpu_data;


#define MPU_BUFFER_SIZE 16


#endif /* INC_MPU_DATA_HANDLER_H_ */
