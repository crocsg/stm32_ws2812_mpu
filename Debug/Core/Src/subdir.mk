################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/freertos.c \
../Core/Src/inv_mpu.c \
../Core/Src/inv_mpu_dmp_motion_driver.c \
../Core/Src/inv_mpu_hal_util.c \
../Core/Src/log_stm32.c \
../Core/Src/main.c \
../Core/Src/message_layer.c \
../Core/Src/ml_math_func.c \
../Core/Src/mlmath.c \
../Core/Src/mpl.c \
../Core/Src/mpu_data_handler.c \
../Core/Src/start_manager.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_hal_timebase_TIM.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/storage_manager.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/ws2812spi.c 

OBJS += \
./Core/Src/freertos.o \
./Core/Src/inv_mpu.o \
./Core/Src/inv_mpu_dmp_motion_driver.o \
./Core/Src/inv_mpu_hal_util.o \
./Core/Src/log_stm32.o \
./Core/Src/main.o \
./Core/Src/message_layer.o \
./Core/Src/ml_math_func.o \
./Core/Src/mlmath.o \
./Core/Src/mpl.o \
./Core/Src/mpu_data_handler.o \
./Core/Src/start_manager.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_hal_timebase_TIM.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/storage_manager.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/ws2812spi.o 

C_DEPS += \
./Core/Src/freertos.d \
./Core/Src/inv_mpu.d \
./Core/Src/inv_mpu_dmp_motion_driver.d \
./Core/Src/inv_mpu_hal_util.d \
./Core/Src/log_stm32.d \
./Core/Src/main.d \
./Core/Src/message_layer.d \
./Core/Src/ml_math_func.d \
./Core/Src/mlmath.d \
./Core/Src/mpl.d \
./Core/Src/mpu_data_handler.d \
./Core/Src/start_manager.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_hal_timebase_TIM.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/storage_manager.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/ws2812spi.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' -DMPU6050 '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F103xB -I"I:/home/blackpill/stm32_ws2812_mpu/Core/Inc" -I"I:/home/blackpill/stm32_ws2812_mpu/Drivers/STM32F1xx_HAL_Driver/Inc" -I"I:/home/blackpill/stm32_ws2812_mpu/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"I:/home/blackpill/stm32_ws2812_mpu/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/Third_Party/FreeRTOS/Source/include" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"I:/home/blackpill/stm32_ws2812_mpu/Drivers/CMSIS/Include" -I"I:/home/blackpill/stm32_ws2812_mpu/USB_DEVICE" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


