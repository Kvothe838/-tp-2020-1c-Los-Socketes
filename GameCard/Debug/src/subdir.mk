################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexionGameCard.c \
../src/gamecard.c 

OBJS += \
./src/conexionGameCard.o \
./src/gamecard.o 

C_DEPS += \
./src/conexionGameCard.d \
./src/gamecard.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-1c-Los-Socketes/Shared" -I"/home/utnso/tp-2020-1c-Los-Socketes/Shared/Debug" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


