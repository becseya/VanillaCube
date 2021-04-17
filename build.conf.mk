# This is an example build config file for VanillaCube
#
# It gets included as it is, therfore be careful changing it
# Control variablbes: BUILD_CONFIG OPT AS_FLAGS C_FLAGS CPP_FLAGS LD_FLAGS
#

ifndef BUILD_CONFIG
BUILD_CONFIG = 0
endif

ifeq (${BUILD_CONFIG}, 0)
BUILD_CONFIG_TXT = debug
OPT              = -Og
C_FLAGS          = -g -gdwarf-2
else ifeq (${BUILD_CONFIG}, 1)
BUILD_CONFIG_TXT = release
OPT              = -O3
C_FLAGS          = -DNDEBUG
else
$(error Unknown build config')
endif

# macros
C_FLAGS +=  \
-DBUILD_CONFIG=${BUILD_CONFIG} \
'-DVERSION_TXT="${VERSION}"' \
'-DBUILD_CONFIG_TXT="${BUILD_CONFIG_TXT}"' \
'-DBUILD_DATE_TXT="$(shell date +'%Y-%m-%d %H:%M')"' \

AS_FLAGS  =
CPP_FLAGS = -std=c++17 -fno-rtti -fno-exceptions -specs=nosys.specs -Wno-register
LD_FLAGS  = -u _printf_float

# ---------------------------------------------------------------------------------------------------------------------

CONFIG_SHORTCUTS = debug release

.PHONY: ${CONFIG_SHORTCUTS}
.SILENT: ${CONFIG_SHORTCUTS}

debug:
	@make BUILD_CONFIG=0

release:
	@make BUILD_CONFIG=1
