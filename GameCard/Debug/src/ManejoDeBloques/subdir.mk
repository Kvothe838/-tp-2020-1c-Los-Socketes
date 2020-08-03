################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ManejoDeBloques/manejoDeArchivos.c 

OBJS += \
./src/ManejoDeBloques/manejoDeArchivos.o 

C_DEPS += \
./src/ManejoDeBloques/manejoDeArchivos.d 


# Each subdirectory must supply rules for building sources it contributes
src/ManejoDeBloques/%.o: ../src/ManejoDeBloques/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/nuevoWrksp/tp-2020-1c-Los-Socketes/Shared" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


