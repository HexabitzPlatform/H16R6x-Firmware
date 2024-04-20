################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APA102Driver/APA102_LedMatrix.c 

OBJS += \
./APA102Driver/APA102_LedMatrix.o 

C_DEPS += \
./APA102Driver/APA102_LedMatrix.d 


# Each subdirectory must supply rules for building sources it contributes
APA102Driver/%.o APA102Driver/%.su: ../APA102Driver/%.c APA102Driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G0B1xx -c -I../Core/Inc -I../APA102Driver -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-APA102Driver

clean-APA102Driver:
	-$(RM) ./APA102Driver/APA102_LedMatrix.d ./APA102Driver/APA102_LedMatrix.o ./APA102Driver/APA102_LedMatrix.su

.PHONY: clean-APA102Driver

