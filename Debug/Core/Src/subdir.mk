################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ble_if.c \
../Core/Src/dmptask.c \
../Core/Src/freertos.c \
../Core/Src/main.c \
../Core/Src/mpu_data_handler.c \
../Core/Src/sd_hal_mpu6050.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_hal_timebase_TIM.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/ws2812spi.c 

OBJS += \
./Core/Src/ble_if.o \
./Core/Src/dmptask.o \
./Core/Src/freertos.o \
./Core/Src/main.o \
./Core/Src/mpu_data_handler.o \
./Core/Src/sd_hal_mpu6050.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_hal_timebase_TIM.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/ws2812spi.o 

C_DEPS += \
./Core/Src/ble_if.d \
./Core/Src/dmptask.d \
./Core/Src/freertos.d \
./Core/Src/main.d \
./Core/Src/mpu_data_handler.d \
./Core/Src/sd_hal_mpu6050.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_hal_timebase_TIM.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/ws2812spi.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F103xB -D_DEBUGoff -I"I:/home/blackpill/stm32_ws2812_mpu/DMP_Driver/Include" -I"I:/home/blackpill/stm32_ws2812_mpu/DMP_Driver/Include/MPU6050" -I"I:/home/blackpill/stm32_ws2812_mpu/DMP_Driver/Source" -I"I:/home/blackpill/stm32_ws2812_mpu/Core/Inc" -I"I:/home/blackpill/stm32_ws2812_mpu/Drivers/STM32F1xx_HAL_Driver/Inc" -I"I:/home/blackpill/stm32_ws2812_mpu/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"I:/home/blackpill/stm32_ws2812_mpu/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/Third_Party/FreeRTOS/Source/include" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"I:/home/blackpill/stm32_ws2812_mpu/Drivers/CMSIS/Include" -I"I:/home/blackpill/stm32_ws2812_mpu/USB_DEVICE/Target" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"I:/home/blackpill/stm32_ws2812_mpu/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"I:/home/blackpill/stm32_ws2812_mpu/USB_DEVICE/App"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


