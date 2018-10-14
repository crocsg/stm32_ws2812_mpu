
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */
#include <limits.h>
#include "ws2812spi.h"
#include "mpu6050.h"
#include "dmptask.h"

//#define _DEBUG 1
#ifdef _DEBUG
#pragma message  "DEBUG CONSOLE ACTIVATED"
#endif

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

UART_HandleTypeDef huart2;

DMA_HandleTypeDef hdma_memtomem_dma1_channel1;
osThreadId defaultTaskHandle;
osThreadId blinkTaskHandle;
osThreadId BLETaskHandle;
osMessageQId dataBleQueueHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);
void StartTaskBlink(void const * argument);
void StartTaskBLE(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void initialise_monitor_handles (void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
#ifdef _DEBUG
	initialise_monitor_handles ();
#endif
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	#ifdef _DEBUG
  	  printf("Starting:\r\n");
	#endif

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of blinkTask */
  osThreadDef(blinkTask, StartTaskBlink, osPriorityNormal, 0, 256);
  blinkTaskHandle = osThreadCreate(osThread(blinkTask), NULL);

  /* definition and creation of BLETask */
  osThreadDef(BLETask, StartTaskBLE, osPriorityBelowNormal, 0, 128);
  BLETaskHandle = osThreadCreate(osThread(BLETask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of dataBleQueue */
/* what about the sizeof here??? cd native code */
  osMessageQDef(dataBleQueue, 8, dmp_data);
  dataBleQueueHandle = osMessageCreate(osMessageQ(dataBleQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_memtomem_dma1_channel1
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* Configure DMA request hdma_memtomem_dma1_channel1 on DMA1_Channel1 */
  hdma_memtomem_dma1_channel1.Instance = DMA1_Channel1;
  hdma_memtomem_dma1_channel1.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma1_channel1.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem_dma1_channel1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma1_channel1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_memtomem_dma1_channel1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_memtomem_dma1_channel1.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma1_channel1.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_memtomem_dma1_channel1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USERLED_GPIO_Port, USERLED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : USERLED_Pin */
  GPIO_InitStruct.Pin = USERLED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(USERLED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MPU_INTERUPT_Pin */
  GPIO_InitStruct.Pin = MPU_INTERUPT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MPU_INTERUPT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 10, 0);
  //HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
#if 0
static SD_MPU6050 mpu1;
static SD_MPU6050_Interrupt mpu1_interrupt;
#endif

int period = 2000 ;
uint16_t it_cnt;
uint32_t it_ticks = 0;
uint16_t it_freq = 0;
uint16_t last_fs = 0;
static uint8_t transfert_pending = 0;

uint8_t fifo_buffer[1024];
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	//HAL_GPIO_TogglePin(USERLED_GPIO_Port,USERLED_Pin);

	// read interrupt status
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			uint32_t ulStatusRegister = 0;

	transfert_pending = 0;
	xTaskNotifyFromISR( blinkTaskHandle,
			                ulStatusRegister,
			                        eSetBits,
			                        &xHigherPriorityTaskWoken );

}
/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
 	if (GPIO_Pin == MPU_INTERUPT_Pin)
	{

		//HAL_GPIO_WritePin(USERLED_GPIO_Port, USERLED_Pin, GPIO_PIN_RESET);
		//HAL_GPIO_TogglePin(USERLED_GPIO_Port,USERLED_Pin);
		//
		//if (mpu1.Accelerometer_X < 0)
		//	HAL_GPIO_WritePin(USERLED_GPIO_Port, USERLED_Pin, GPIO_PIN_RESET);
		//else
		//	HAL_GPIO_WritePin(USERLED_GPIO_Port, USERLED_Pin, GPIO_PIN_SET);

		// read fifo size
		int fs = mpu_get_fifo_size ();
		if (fs < 0)
			return;
		last_fs = fs;
		// start read fifo packet buffer
		if (fs > 512)
		{
			HAL_GPIO_TogglePin(USERLED_GPIO_Port, USERLED_Pin);
			mpu_reset_fifo();
			return;
		}


		if (dmp_get_fifo_packet_length () <= fs && transfert_pending == 0)
		{
			uint16_t size = dmp_get_fifo_packet_length () * (fs / dmp_get_fifo_packet_length ());
			transfert_pending = 1;
			HAL_I2C_Mem_Read_DMA (&hi2c1, mpu_get_i2c_addr () << 1, mpu_get_fifo_addr (), I2C_MEMADD_SIZE_8BIT, fifo_buffer, size);
		}


		it_cnt++;
		if (it_ticks +1000 < HAL_GetTick())
		{
						it_freq = it_cnt * 1000 / (HAL_GetTick () - it_ticks);
						//it_cnt = 0;
						if (it_ticks == 0)
							it_ticks = HAL_GetTick ();
		}



	}
}

void BlinkError ()
{
	for (;;) {
		for (int t = 0; t < 4; t++) {
			HAL_GPIO_TogglePin(USERLED_GPIO_Port, USERLED_Pin);
			osDelay(100);
		}
		osDelay(500);
	}
}


static uint32_t prevticks = 0;
extern short gyro[3], accel[3], sensors;
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN 5 */
  if (WS2312BSPI_Init (&hspi1, &hdma_spi1_tx, NB_PIXEL) == -1)
  	   {
  	 	#ifdef _DEBUG
  	 	  printf("WS2312BSPI_Init error:\r\n");
  	 	#endif
  	 	  BlinkError ();
  	   }

  /* Infinite loop */
  for(;;)
  {
	  //HAL_GPIO_TogglePin(USERLED_GPIO_Port, USERLED_Pin);
	  int dir;

	  for (int i = 0; i < NB_PIXEL; i++)
	  	  {
	  	  		  WS2812BSPI_encode_pixel_index(32, 32, 0 , i);
	  	  }
	  	  dir = 0;
	  #if 0
	  	  if (buffer[0].ay > 2000)
	  	  {
	  		  for (int i = 0; i < NB_PIXEL / 2; i++)
	  		  	  {
	  		  	  		  WS2812BSPI_encode_pixel_index(64,16,0, i);
	  		  	  }
	  		  dir = +1;
	  	  }
	  	  else if (buffer[0].ay > 1000)
	  	  {
	  	  		  for (int i = 0; i < NB_PIXEL / 4; i++)
	  	  		  	  {
	  	  		  	  		  WS2812BSPI_encode_pixel_index (64,16,0, i);
	  	  		  	  }

	  	  }
	  	  else if (buffer[0].ay < -2000)
	  	  {
	  		  for (int i = NB_PIXEL / 2; i < NB_PIXEL ; i++)
	  		  		  	  {
	  		  		  	  		  WS2812BSPI_encode_pixel_index (64,16,0, i);
	  		  		  	  }
	  		  dir = -1;
	  	  }
	  	  else if (buffer[0].ay < -1000)
	  	  {
	  		  for (int i = NB_PIXEL - NB_PIXEL / 4; i < NB_PIXEL ; i++)
	  		  		  	  {
	  		  		  	  		  WS2812BSPI_encode_pixel_index (64,16,0, i);
	  		  		  	  }

	  	  }
#endif
	  	/*
	  	  if (buffer[0].az >= 0)
	  		  p = NB_PIXEL - 1 - (buffer[0].az * NB_PIXEL / 18240);
	  	  else
	  		  p = NB_PIXEL - 1;
		*/
	  	  int p;
	  	if (accel[2] >= 0)
	  		  		  p = NB_PIXEL - 1 - ((accel[2] -10000) * NB_PIXEL / 8000);
	  		  	  else
	  		  		  p = NB_PIXEL - 1;
	  	  if (p >= NB_PIXEL)
	  		  p = NB_PIXEL -1;
	  	  else if (p < 0)
	  		  p = 0;
	  	  for (int i = 0; i <= p; i++)
	  		  WS2812BSPI_encode_pixel_index (accel[0] == 0 ? 0 : 64,accel[0] == 0 ? 64 : 0,0, i);
#if 0
	#ifdef _DEBUG
	  	  printf ("%d\n", p);
	#endif
#endif
	  	WS2812BSPI_SendData();
	  osDelay(4);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_StartTaskBlink */
/**
* @brief Function implementing the blinkTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskBlink */
void StartTaskBlink(void const * argument)
{
  /* USER CODE BEGIN StartTaskBlink */
	dmptask (argument);


  /* USER CODE END StartTaskBlink */
}

/* USER CODE BEGIN Header_StartTaskBLE */
/**
* @brief Function implementing the BLETask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskBLE */
void StartTaskBLE(void const * argument)
{
  /* USER CODE BEGIN StartTaskBLE */
  /* Infinite loop */
  for(;;)
  {
	 bleTask ();
  }
  /* USER CODE END StartTaskBLE */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
