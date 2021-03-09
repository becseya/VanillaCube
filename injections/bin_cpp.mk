# binaries
#######################################

# injected by CubeVanilla IDE -----------------------------------------------------------------------------------------

PREFIX = arm-none-eabi-
ifdef GCC_PATH
PREFIX := ${GCC_PATH}/${PREFIX}
endif
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
CC = $(PREFIX)gcc
PP = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# extra sources
#######################################

EXTRA_LL_SOURCES =  \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_adc.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_crc.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dac.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_dma.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_exti.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_gpio.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_i2c.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_pwr.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rcc.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_rtc.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_spi.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_tim.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usart.c \
Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_utils.c

ADDITIONAL_C_SOURCES := $(filter-out ${C_SOURCES},${EXTRA_LL_SOURCES})
C_SOURCES += ${ADDITIONAL_C_SOURCES}

CPP_SOURCES = $(shell find ${DIR_CPP_SRC} -name "*.cpp")

# end of injection ----------------------------------------------------------------------------------------------------

#######################################
# CFLAGS
