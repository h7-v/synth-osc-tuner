################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/User/GUI/GUI_Paint.c 

OBJS += \
./Core/User/GUI/GUI_Paint.o 

C_DEPS += \
./Core/User/GUI/GUI_Paint.d 


# Each subdirectory must supply rules for building sources it contributes
Core/User/GUI/%.o Core/User/GUI/%.su Core/User/GUI/%.cyclo: ../Core/User/GUI/%.c Core/User/GUI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-User-2f-GUI

clean-Core-2f-User-2f-GUI:
	-$(RM) ./Core/User/GUI/GUI_Paint.cyclo ./Core/User/GUI/GUI_Paint.d ./Core/User/GUI/GUI_Paint.o ./Core/User/GUI/GUI_Paint.su

.PHONY: clean-Core-2f-User-2f-GUI

