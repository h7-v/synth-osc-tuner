################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/User/Fonts/font12.c \
../Core/User/Fonts/font12CN.c \
../Core/User/Fonts/font16.c \
../Core/User/Fonts/font20.c \
../Core/User/Fonts/font24.c \
../Core/User/Fonts/font24CN.c \
../Core/User/Fonts/font8.c 

OBJS += \
./Core/User/Fonts/font12.o \
./Core/User/Fonts/font12CN.o \
./Core/User/Fonts/font16.o \
./Core/User/Fonts/font20.o \
./Core/User/Fonts/font24.o \
./Core/User/Fonts/font24CN.o \
./Core/User/Fonts/font8.o 

C_DEPS += \
./Core/User/Fonts/font12.d \
./Core/User/Fonts/font12CN.d \
./Core/User/Fonts/font16.d \
./Core/User/Fonts/font20.d \
./Core/User/Fonts/font24.d \
./Core/User/Fonts/font24CN.d \
./Core/User/Fonts/font8.d 


# Each subdirectory must supply rules for building sources it contributes
Core/User/Fonts/%.o Core/User/Fonts/%.su Core/User/Fonts/%.cyclo: ../Core/User/Fonts/%.c Core/User/Fonts/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-User-2f-Fonts

clean-Core-2f-User-2f-Fonts:
	-$(RM) ./Core/User/Fonts/font12.cyclo ./Core/User/Fonts/font12.d ./Core/User/Fonts/font12.o ./Core/User/Fonts/font12.su ./Core/User/Fonts/font12CN.cyclo ./Core/User/Fonts/font12CN.d ./Core/User/Fonts/font12CN.o ./Core/User/Fonts/font12CN.su ./Core/User/Fonts/font16.cyclo ./Core/User/Fonts/font16.d ./Core/User/Fonts/font16.o ./Core/User/Fonts/font16.su ./Core/User/Fonts/font20.cyclo ./Core/User/Fonts/font20.d ./Core/User/Fonts/font20.o ./Core/User/Fonts/font20.su ./Core/User/Fonts/font24.cyclo ./Core/User/Fonts/font24.d ./Core/User/Fonts/font24.o ./Core/User/Fonts/font24.su ./Core/User/Fonts/font24CN.cyclo ./Core/User/Fonts/font24CN.d ./Core/User/Fonts/font24CN.o ./Core/User/Fonts/font24CN.su ./Core/User/Fonts/font8.cyclo ./Core/User/Fonts/font8.d ./Core/User/Fonts/font8.o ./Core/User/Fonts/font8.su

.PHONY: clean-Core-2f-User-2f-Fonts

