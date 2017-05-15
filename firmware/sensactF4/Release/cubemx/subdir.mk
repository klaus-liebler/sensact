################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dfsdm.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sd.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_sdmmc.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c 

OBJS += \
./cubemx/stm32f4xx_hal.o \
./cubemx/stm32f4xx_hal_adc.o \
./cubemx/stm32f4xx_hal_adc_ex.o \
./cubemx/stm32f4xx_hal_can.o \
./cubemx/stm32f4xx_hal_cortex.o \
./cubemx/stm32f4xx_hal_dfsdm.o \
./cubemx/stm32f4xx_hal_dma.o \
./cubemx/stm32f4xx_hal_gpio.o \
./cubemx/stm32f4xx_hal_i2c.o \
./cubemx/stm32f4xx_hal_rcc.o \
./cubemx/stm32f4xx_hal_sd.o \
./cubemx/stm32f4xx_hal_spi.o \
./cubemx/stm32f4xx_hal_tim.o \
./cubemx/stm32f4xx_hal_tim_ex.o \
./cubemx/stm32f4xx_hal_uart.o \
./cubemx/stm32f4xx_ll_sdmmc.o \
./cubemx/stm32f4xx_ll_usb.o 

C_DEPS += \
./cubemx/stm32f4xx_hal.d \
./cubemx/stm32f4xx_hal_adc.d \
./cubemx/stm32f4xx_hal_adc_ex.d \
./cubemx/stm32f4xx_hal_can.d \
./cubemx/stm32f4xx_hal_cortex.d \
./cubemx/stm32f4xx_hal_dfsdm.d \
./cubemx/stm32f4xx_hal_dma.d \
./cubemx/stm32f4xx_hal_gpio.d \
./cubemx/stm32f4xx_hal_i2c.d \
./cubemx/stm32f4xx_hal_rcc.d \
./cubemx/stm32f4xx_hal_sd.d \
./cubemx/stm32f4xx_hal_spi.d \
./cubemx/stm32f4xx_hal_tim.d \
./cubemx/stm32f4xx_hal_tim_ex.d \
./cubemx/stm32f4xx_hal_uart.d \
./cubemx/stm32f4xx_ll_sdmmc.d \
./cubemx/stm32f4xx_ll_usb.d 


# Each subdirectory must supply rules for building sources it contributes
cubemx/stm32f4xx_hal.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_adc.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_adc_ex.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_can.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_cortex.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_dfsdm.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dfsdm.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_dma.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_gpio.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_i2c.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_rcc.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_sd.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sd.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_spi.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_tim.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_tim_ex.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_hal_uart.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_ll_sdmmc.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_sdmmc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cubemx/stm32f4xx_ll_usb.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.14.0/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Werror -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F405xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DSTM32F4 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.9.0\Drivers\STM32F4xx_HAL_Driver\Inc" -I"C:\Users\klaus\OneDrive\sensact\firmware\bsp\sensactHs" -I"C:\Users\klaus\OneDrive\sensact\firmware\generated" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hal" -I"C:\Users\klaus\OneDrive\sensact\firmware\modules\hc" -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


