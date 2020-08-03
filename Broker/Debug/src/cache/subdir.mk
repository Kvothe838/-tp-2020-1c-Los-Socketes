################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cache/basicCache.c \
../src/cache/dynamicCache.c 

OBJS += \
./src/cache/basicCache.o \
./src/cache/dynamicCache.o 

C_DEPS += \
./src/cache/basicCache.d \
./src/cache/dynamicCache.d 


# Each subdirectory must supply rules for building sources it contributes
src/cache/%.o: ../src/cache/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/nuevoWrksp/tp-2020-1c-Los-Socketes/Shared" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


