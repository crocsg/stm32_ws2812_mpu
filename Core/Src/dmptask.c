/*
 * dmptask.c
 *
 *  Created on: 9 oct. 2018
 *      Author: Stephane
 */

#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */
#include <limits.h>
#if 0
#include "sd_hal_mpu6050.h"
#include "mpu_data_handler.h"
#else
#include "mpu6050.h"
#endif
#include "ws2812spi.h"

static uint32_t prevticks = 0;

extern short gyro[3], accel[3], sensors;
extern osMessageQId dataBleQueueHandle;
extern long quat[4];
extern float q0, q1, q2, q3;
extern float Pitch;

extern uint16_t it_cnt;
extern uint32_t it_ticks;
extern uint16_t it_freq;

#if 0
static mpu_data buffer[MPU_BUFFER_SIZE];
static int cnt = 0;
#endif

extern uint8_t fifo_buffer[1024];
extern uint16_t last_fs;

static dmp_data mpu_data;

void dmptask (void const * arg)
{
	int fill_ble = 1;
	DMP_Init ();
	//dmp_set_fifo_rate (200);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	for (;;) {

			uint32_t ulInterruptStatus;

			xTaskNotifyWait(0x00, /* Don't clear any bits on entry. */
			ULONG_MAX, /* Clear all bits on exit. */
			&ulInterruptStatus, /* Receives the notification value. */
			portMAX_DELAY); /* Block indefinitely. */

			//static short gyro[6];
			//static short accel[6];
			//static long quat[6];
			//static short sensors[24];
			//uint32_t timestamp;
			//uint8_t more;
			//dmp_read_fifo(gyro, accel, quat,&timestamp, sensors, &more);
			//Read_DMP();
			//HAL_GPIO_TogglePin(USERLED_GPIO_Port, USERLED_Pin);
			Decode_DMP_data(fifo_buffer, &mpu_data);
			mpu_data.it_freq = it_freq;

			//Decode_DMP (fifo_buffer);
#ifdef _DEBUG
			printf ("ble queue: %d\r\n", uxQueueSpacesAvailable( dataBleQueueHandle ));
#endif

			// send data to ble queue
			if (fill_ble == 1)
			{
				xQueueSendToBack ( dataBleQueueHandle, &mpu_data, 0 );
				if (uxQueueSpacesAvailable( dataBleQueueHandle ) == 0)
				{
					fill_ble = 0; // stop filling the queue until it's empty
					HAL_GPIO_TogglePin(USERLED_GPIO_Port, USERLED_Pin);
				}
			}
			else
			{
				UBaseType_t n;
				if ((n = uxQueueSpacesAvailable( dataBleQueueHandle )) >= 32)
				{
					HAL_GPIO_TogglePin(USERLED_GPIO_Port, USERLED_Pin);
					fill_ble = 1; // restart to fill the queue
				}
			}


			//if (accel[0] > 0)

	#if 1
	#ifdef _DEBUG
			printf("%04ld | %d %d %d %d %d %d %d %d\n", HAL_GetTick() - prevticks,
					accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2],it_freq, last_fs);
	#endif
	#endif
			//if (prevticks == 0)
			prevticks = HAL_GetTick();

			osDelay(1);
		}
}
#if 0
void mputask (void)
{
	 SD_MPU6050_Result result;
	#ifdef _DEBUG
	  printf("Starting:\r\n");
	#endif

	  result = SD_MPU6050_Init(&hi2c1,&mpu1,SD_MPU6050_Device_0,SD_MPU6050_Accelerometer_2G,SD_MPU6050_Gyroscope_250s );
	  if(result != SD_MPU6050_Result_Ok)
	  {
	  	BlinkError ();

	  }
	  if (SD_MPU6050_SetDataRate(&mpu1, SD_MPU6050_DataRate_100Hz) != SD_MPU6050_Result_Ok)
	  {
		  BlinkError ();
	  }
	  if (SD_MPU6050_EnableAccelFifo(&mpu1) != SD_MPU6050_Result_Ok)
	  {
		  BlinkError ();
	  }
	  if (SD_MPU6050_EnableFifo(&mpu1) != SD_MPU6050_Result_Ok)
	    {
	  	  BlinkError ();
	    }

	  //HAL_NVIC_SetPriority(EXTI15_10_IRQn, 10, 0);
	  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);


	  SD_MPU6050_EnableInterrupts(&mpu1);
	  int dir = 0;
	  int p = 4;
	  int waiter = 0;
	  for(;;)
	  {

		  uint32_t ulInterruptStatus;

		  xTaskNotifyWait( 0x00,               /* Don't clear any bits on entry. */
		                           ULONG_MAX,          /* Clear all bits on exit. */
		                           &ulInterruptStatus, /* Receives the notification value. */
		                           portMAX_DELAY );    /* Block indefinitely. */

		  //HAL_GPIO_TogglePin(USERLED_GPIO_Port,USERLED_Pin);

		  SD_MPU6050_ReadInterrupts (&mpu1, &mpu1_interrupt);
		  /*
		  if (mpu1_interrupt.F.DataReady != 0)
		  {
		  		SD_MPU6050_ReadAll (&mpu1);

		  		memmove (&buffer[1], &buffer[0], (MPU_BUFFER_SIZE - 1)*sizeof(buffer[0]));
		  		buffer[0].ax = mpu1.Accelerometer_X;
		  		buffer[0].ay = mpu1.Accelerometer_Y;
		  		buffer[0].az = mpu1.Accelerometer_Z;

		  		if (cnt < MPU_BUFFER_SIZE)
		  			cnt++;
		  }
		  */
		  static uint16_t fifosize = 0;
		  static uint8_t buf[2048];

		  SD_MPU6050_GetFifoCount(&mpu1, &fifosize);
	#if 0
		#ifdef _DEBUG
		  	  printf ("| %ld %u\n", HAL_GetTick() - prevticks, fifosize);
		#endif
	#endif

		  if (fifosize > 0)
		  	  SD_MPU6050_ReadFifo(&mpu1, fifosize, buf);

		  int16_t x,y,z,a,b,c;
		  HAL_GPIO_TogglePin(USERLED_GPIO_Port, USERLED_Pin);
		  buffer[0].ax = (buf[0] << 8) + buf[1];
		  buffer[0].ay =
		  buffer[0].ax = x = (buf[0] << 8) + buf[1];
		  buffer[0].ay = y = (buf[2] << 8) + buf[3];
		  buffer[0].az = z = (buf[4] << 8) + buf[5];
		  a = (buf[6] << 8) + buf[7];
		  b = (buf[8] << 8) + buf[9];
		  c = (buf[10] << 8) + buf[11];

		  if (cnt > 2)
		  {
			  if (buffer[0].ax >= 0 && buffer[1].ax < 0)
			  {
	#ifdef _DEBUG
				  printf ("%ld X change %d %d \n", HAL_GetTick(), buffer[0].ax, buffer[1].ax);
	#endif

				  //HAL_GPIO_TogglePin(USERLED_GPIO_Port, USERLED_Pin);
			  }
		  }
	#ifdef _DEBUG
	#if 1
		  for (int i = 0; i < 48; i++)
			  printf ("%02x ", buf[i]);
		  printf ("| %ld %u, %d %d %d %d %d %d", HAL_GetTick() - prevticks, fifosize, x,y,z,a,b,c);
		  printf ("\n");
	#endif
		  //printf ("%ld %d %d %d\n", HAL_GetTick() - prevticks, buffer[0].ax, buffer[0].ay, buffer[0].az);
		  //if (prevticks == 0)
			  prevticks = HAL_GetTick();

	#endif



	    osDelay(0);
	  }
}
#endif
