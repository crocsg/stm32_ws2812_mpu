
#include "stm32f1xx_hal.h"
#include "mpu_data_handler.h"

#include "MPU6050/mpu6050.h"
#include "../../Include/MPU6050/inv_mpu.h"
#include "../../Include/MPU6050/inv_mpu_dmp_motion_driver.h"
#include "../../Include/MPU6050/I2C.h"

/*
EventGroupHandle_t mpu_event;

void mpu_open ()
{
	 mpu_event = xEventGroupCreate();
}

*/
