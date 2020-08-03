################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shared/commonsExtensions.c \
../shared/connection.c \
../shared/messages.c \
../shared/serialize.c \
../shared/utils.c 

OBJS += \
./shared/commonsExtensions.o \
./shared/connection.o \
./shared/messages.o \
./shared/serialize.o \
./shared/utils.o 

C_DEPS += \
./shared/commonsExtensions.d \
./shared/connection.d \
./shared/messages.d \
./shared/serialize.d \
./shared/utils.d 


# Each subdirectory must supply rules for building sources it contributes
shared/%.o: ../shared/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


