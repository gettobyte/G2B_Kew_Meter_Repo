################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Pmeter/src/pmeter.c 

OBJS += \
./Pmeter/src/pmeter.o 

C_DEPS += \
./Pmeter/src/pmeter.d 


# Each subdirectory must supply rules for building sources it contributes
Pmeter/src/%.o Pmeter/src/%.su Pmeter/src/%.cyclo: ../Pmeter/src/%.c Pmeter/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G030xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Bhavnesh/OneDrive/Desktop/Documents/G2B_Kew_Meter_Repo/ACVA/ADC_STM32G030/Pmeter/inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Pmeter-2f-src

clean-Pmeter-2f-src:
	-$(RM) ./Pmeter/src/pmeter.cyclo ./Pmeter/src/pmeter.d ./Pmeter/src/pmeter.o ./Pmeter/src/pmeter.su

.PHONY: clean-Pmeter-2f-src

