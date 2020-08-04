################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexionesTeam.c \
../src/planificacion.c \
../src/team.c \
../src/teamInit.c 

OBJS += \
./src/conexionesTeam.o \
./src/planificacion.o \
./src/team.o \
./src/teamInit.o 

C_DEPS += \
./src/conexionesTeam.d \
./src/planificacion.d \
./src/team.d \
./src/teamInit.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/wrksp/tp-2020-1c-Los-Socketes/Shared" -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


