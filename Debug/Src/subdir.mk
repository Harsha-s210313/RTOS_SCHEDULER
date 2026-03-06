################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/SysTick.c \
../Src/main.c \
../Src/scheduler.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/task.c 

OBJS += \
./Src/SysTick.o \
./Src/main.o \
./Src/scheduler.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/task.o 

C_DEPS += \
./Src/SysTick.d \
./Src/main.d \
./Src/scheduler.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/task.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I"C:/Users/HARSHA/Desktop/BMP/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/HARSHA/Desktop/BMP/chip_headers/CMSIS/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/SysTick.cyclo ./Src/SysTick.d ./Src/SysTick.o ./Src/SysTick.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/scheduler.cyclo ./Src/scheduler.d ./Src/scheduler.o ./Src/scheduler.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/task.cyclo ./Src/task.d ./Src/task.o ./Src/task.su

.PHONY: clean-Src

