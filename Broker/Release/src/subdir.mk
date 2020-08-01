################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/broker.c \
../src/brokerColas.c \
../src/conexionBroker.c 

OBJS += \
./src/broker.o \
./src/brokerColas.o \
./src/conexionBroker.o 

C_DEPS += \
./src/broker.d \
./src/brokerColas.d \
./src/conexionBroker.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


