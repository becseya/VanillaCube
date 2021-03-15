# binaries
#######################################

# injected by VanillaCube IDE -----------------------------------------------------------------------------------------

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

EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_adc.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_crc.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_dac.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_dma.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_exti.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_gpio.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_i2c.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_pwr.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_rcc.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_rtc.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_spi.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_tim.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_usart.c")
EXTRA_LL_SOURCES += $(shell find Drivers -name "*_ll_utils.c")

ADDITIONAL_C_SOURCES := $(filter-out ${C_SOURCES},${EXTRA_LL_SOURCES})
C_SOURCES += ${ADDITIONAL_C_SOURCES}

CPP_SOURCES = $(shell find ${DIR_CPP_SRC} -name "*.cpp")
LIB_SOURCES = $(shell find ${DIR_VCL_SRC} -name "*.cpp")

# end of injection ----------------------------------------------------------------------------------------------------

#######################################
# CFLAGS
