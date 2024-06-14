################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/custom_libraries/printf/printf.c 

OBJS += \
./Core/custom_libraries/printf/printf.o 

C_DEPS += \
./Core/custom_libraries/printf/printf.d 


# Each subdirectory must supply rules for building sources it contributes
Core/custom_libraries/printf/%.o Core/custom_libraries/printf/%.su Core/custom_libraries/printf/%.cyclo: ../Core/custom_libraries/printf/%.c Core/custom_libraries/printf/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-custom_libraries-2f-printf

clean-Core-2f-custom_libraries-2f-printf:
	-$(RM) ./Core/custom_libraries/printf/printf.cyclo ./Core/custom_libraries/printf/printf.d ./Core/custom_libraries/printf/printf.o ./Core/custom_libraries/printf/printf.su

.PHONY: clean-Core-2f-custom_libraries-2f-printf

