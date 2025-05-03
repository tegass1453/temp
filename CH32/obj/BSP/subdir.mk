################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/ESP8266.c \
../BSP/MFRC522.c \
../BSP/SD.c \
../BSP/ringbuffer.c \
../BSP/schedule.c \
../BSP/tim.c \
../BSP/usart.c 

OBJS += \
./BSP/ESP8266.o \
./BSP/MFRC522.o \
./BSP/SD.o \
./BSP/ringbuffer.o \
./BSP/schedule.o \
./BSP/tim.o \
./BSP/usart.o 

C_DEPS += \
./BSP/ESP8266.d \
./BSP/MFRC522.d \
./BSP/SD.d \
./BSP/ringbuffer.d \
./BSP/schedule.d \
./BSP/tim.d \
./BSP/usart.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/%.o: ../BSP/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"D:\project\CH32\repository\CH32\Debug" -I"D:\project\CH32\repository\CH32\Core" -I"D:\project\CH32\repository\CH32\User" -I"D:\project\CH32\repository\CH32\Peripheral\inc" -I"D:\project\CH32\test1\test1\BSP" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

