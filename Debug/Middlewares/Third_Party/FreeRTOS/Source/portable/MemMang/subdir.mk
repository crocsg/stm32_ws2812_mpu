################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c 

OBJS += \
./Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.o 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/%.o: ../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F103xB -D_DEBUGoff -I"D:/usr/home/blackpill/stm32_ws2812_mpu/DMP_Driver/Include" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/DMP_Driver/Include/MPU6050" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/DMP_Driver/Source" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Core/Inc" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Drivers/STM32F1xx_HAL_Driver/Inc" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Middlewares/Third_Party/FreeRTOS/Source/include" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Drivers/CMSIS/Include" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/USB_DEVICE/Target" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"D:/usr/home/blackpill/stm32_ws2812_mpu/USB_DEVICE/App"  -O3 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


