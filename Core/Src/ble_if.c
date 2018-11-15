
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"

#include "mpu6050.h"


extern UART_HandleTypeDef huart2;
extern osMessageQId dataBleQueueHandle;
dmp_data data;
static uint8_t buf[160];
void bleTask (void)
{

	while(xQueueGenericReceive ( dataBleQueueHandle, &data, ( TickType_t ) 80, pdFALSE ) )
	{
		//HAL_GPIO_TogglePin(USERLED_GPIO_Port, USERLED_Pin);
		sprintf ((char *)buf, "%d %d %d %d %ld %ld %ld %ld\r\n", data.it_freq, data.accel[0], data.accel[1], data.accel[2], data.quat[0], data.quat[1], data.quat[2], data.quat[3]);
		HAL_UART_Transmit (&huart2, buf, strlen(buf), 1000);

	}
}
