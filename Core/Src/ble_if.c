
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"

#include "mpu6050.h"


extern UART_HandleTypeDef huart2;
extern osMessageQId dataBleQueueHandle;
dmp_data data;
static uint8_t buf[80];
void bleTask (void)
{

	if(xQueueGenericReceive ( dataBleQueueHandle, &data, ( TickType_t ) 80, pdFALSE ) )
	{
		sprintf ((char *)buf, "%d %d %d %ld %ld %ld %ld\r\n", data.accel[0], data.accel[1], data.accel[2], data.quat[0], data.quat[1], data.quat[2], data.quat[3]);
		HAL_UART_Transmit (&huart2, buf, sizeof(buf), 1000);

	}
}
