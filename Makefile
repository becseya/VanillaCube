# Makefile installed by VanillaCube IDE

R             = $(shell pwd)
DIR_OUTPUT    = ${R}/build
DIR_GENERATED = ${R}/generated
DIR_IMAGES    = ${R}/images
MK_ENV        = ${DIR_OUTPUT}/vcube-install/env.mk
PROJECT_FILE  = $(shell find ${DIR_GENERATED} -name '*.ioc' 2>/dev/null)

ifeq (,$(wildcard ${MK_ENV}))
$(error Environmet is not set. Run 'VanillaCube/install.sh')
endif

include ${MK_ENV}

ifeq (,$(wildcard ${PROJECT_FILE}))
$(error Project file is missing. Run 'VanillaCube/new-project.sh')
endif

TARGET = $(shell cat ${PROJECT_FILE} | grep -Po '(?<=ProjectManager.ProjectName=)[A-Za-z_.\-]+')

# ---------------------------------------------------------------------------------------------------------------------

VERSION              = ${shell git describe}

DIR_OBJ              = ${DIR_OUTPUT}/obj
DIR_BIN_IMAGES       = ${DIR_OUTPUT}/images
DIR_INJECTIONS       = ${PATH_VCUBE}/injections
DIR_VSCODE           = ${R}/.vscode

IN_GENERATOR_SCRIPT  = ${DIR_INJECTIONS}/generate.template

OUT_GENERATOR_SCRIPT = ${DIR_OUTPUT}/generate.script
OUT_GENERATED        = ${DIR_OUTPUT}/.generated
OUT_SRC_INJECTED     = ${DIR_OUTPUT}/.src_injected
OUT_HEX_IMAGE        = ${DIR_BIN_IMAGES}/${TARGET}.hex
OUT_IMAGES           = ${DIR_OUTPUT}/.images

# ---------------------------------------------------------------------------------------------------------------------

ifndef BUILD_CONFIG
BUILD_CONFIG = 0
endif

ifeq (${BUILD_CONFIG}, 0)
BUILD_CONFIG_TXT = debug
OPT              = -Og
DEBUG            = 1
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

# these variables are passed down to second Makefile
export DIR_CPP_SRC   = ${R}/src
export DIR_IMAGES    = ${R}/images
export DIR_VCL_SRC   = ${PATH_VCUBE}/lib/src
export BUILD_DIR     = ${DIR_OBJ}

export EXT_AS_FLAGS  = ${AS_FLAGS}
export EXT_C_FLAGS   = ${C_FLAGS}
export EXT_CPP_FLAGS = ${CPP_FLAGS}
export EXT_LD_FLAGS  = ${LD_FLAGS}

export OPT

# ---------------------------------------------------------------------------------------------------------------------

RM = rm -Rf

.PHONY: all clean clean-deep clean-purge ${OUT_HEX_IMAGE}

all: ${OUT_HEX_IMAGE}

Makefile: ;
${PROJECT_FILE}: ;
${IN_GENERATOR_SCRIPT}: ;

# ---------------------------------------------------------------------------------------------------------------------

${OUT_GENERATOR_SCRIPT}: ${IN_GENERATOR_SCRIPT} # | output folder already exsists
	cat ${IN_GENERATOR_SCRIPT} | sed 's+@IOC_PATH@+${PROJECT_FILE}+' > ${OUT_GENERATOR_SCRIPT}

${OUT_GENERATED}: ${PROJECT_FILE} | ${OUT_GENERATOR_SCRIPT}
	${RM} ${DIR_GENERATED}/Makefile
	${PATH_CUBE_MX} -q ${OUT_GENERATOR_SCRIPT}
	mv ${DIR_GENERATED}/Makefile ${DIR_GENERATED}/Makefile.original
	touch ${OUT_GENERATED}

${OUT_SRC_INJECTED}: ${OUT_GENERATED}
	$(eval IT_FILE_NAME := $(shell basename $$(find ${DIR_GENERATED}/Inc -name *_it.h) .h))
# inject main
	sed -i '/USER CODE END EFP/a\void main_init();' ${DIR_GENERATED}/Inc/main.h
	sed -i '/USER CODE END EFP/a\void main_loop();' ${DIR_GENERATED}/Inc/main.h
	sed -i '/USER CODE END 2/a\  main_init();'       ${DIR_GENERATED}/Src/main.c
	sed -i '/USER CODE END WHILE/a\    main_loop();' ${DIR_GENERATED}/Src/main.c
# inject systick
	sed -i '/USER CODE END 2/a\  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;'          ${DIR_GENERATED}/Src/main.c
	sed -i '/USER CODE END EM/a\volatile uint64_t __vanillacube_systick_counter = 0;' ${DIR_GENERATED}/Inc/${IT_FILE_NAME}.h
	sed -i '/USER CODE END SysTick_IRQn 0/a\  __vanillacube_systick_counter++;'       ${DIR_GENERATED}/Src/${IT_FILE_NAME}.c
	touch ${OUT_SRC_INJECTED}

${DIR_GENERATED}/Makefile: ${OUT_SRC_INJECTED} Makefile ${DIR_INJECTIONS}/*.mk | ${DIR_VSCODE}
# inject makefile
	cp ${DIR_GENERATED}/Makefile.original ${DIR_GENERATED}/Makefile
	sed -i '/# building variables/,/# source/c\___PATHS___'     ${DIR_GENERATED}/Makefile
	sed -i '/# binaries/,/# CFLAGS/c\___BIN_AND_CPP_SOURCES___' ${DIR_GENERATED}/Makefile
	sed -i '/vpath %.s/a\___COMPILE___'                         ${DIR_GENERATED}/Makefile
	sed -i '/C_INCLUDES =/a\-I${PATH_VCUBE}/lib/inc \\'         ${DIR_GENERATED}/Makefile
	sed -i '/# dependencies/,/# \*\+ EOF/c\___DEPENDENCY___'    ${DIR_GENERATED}/Makefile
	sed -i -e '/___PATHS___/{r ${DIR_INJECTIONS}/path_info.mk' -e 'd}'             ${DIR_GENERATED}/Makefile
	sed -i -e '/___BIN_AND_CPP_SOURCES___/{r ${DIR_INJECTIONS}/bin_cpp.mk' -e 'd}' ${DIR_GENERATED}/Makefile
	sed -i -e '/___COMPILE___/{r ${DIR_INJECTIONS}/compile.mk' -e 'd}'             ${DIR_GENERATED}/Makefile
	sed -i -e '/___DEPENDENCY___/{r ${DIR_INJECTIONS}/dependency.mk' -e 'd}'       ${DIR_GENERATED}/Makefile
# update vscode settings
	$(eval TARGET_DEF := $(shell cat ${DIR_GENERATED}/Makefile.original | grep  -Po '(?<=\-D)STM32[A-Z0-9a-z]+'))
	cat "${DIR_INJECTIONS}/c_cpp_properties.template" | sed 's+@TARGET_DEF@+${TARGET_DEF}+' > ${DIR_VSCODE}/c_cpp_properties.json
	@echo "Target define: ${TARGET_DEF}"

${OUT_IMAGES}: $(shell find ${DIR_IMAGES} -maxdepth 1 -type f) ${PATH_VCUBE}/convert-images.py
	! test -d ${DIR_IMAGES} || ${PATH_VCUBE}/convert-images.py ${DIR_IMAGES}
	touch ${OUT_IMAGES}

.SILENT: ${OUT_HEX_IMAGE}
${OUT_HEX_IMAGE}: ${DIR_GENERATED}/Makefile ${OUT_IMAGES} | ${DIR_BIN_IMAGES} ${DIR_OBJ}
	cd ${DIR_GENERATED} && make -j
	cp ${DIR_OBJ}/${TARGET}.hex ${OUT_HEX_IMAGE}

${DIR_OBJ}:
	mkdir $@

${DIR_BIN_IMAGES}:
	mkdir $@

${DIR_VSCODE}:
	mkdir $@

clean:
	${RM} ${DIR_OBJ} ${DIR_BIN_IMAGES} ${DIR_GENERATED}/Makefile ${DIR_IMAGES}/generated ${OUT_IMAGES}

clean-deep: clean
	find ${DIR_GENERATED} ! -name '${TARGET}.ioc' -type f -exec rm -f {} +
	find ${DIR_GENERATED} -type d -empty -delete
	${RM} ${OUT_GENERATED} ${OUT_GENERATOR_SCRIPT}

clean-purge: clean-deep
	${RM} ${DIR_OUTPUT}
	${PATH_VCUBE}/install.sh

# ---------------------------------------------------------------------------------------------------------------------

.PHONY: format-all update-vanillacube edit-project flash flash-rst

format-all:
	find . -name "*.hpp" | xargs clang-format -i
	find . -name "*.cpp" | xargs clang-format -i

update-vanillacube:
	git -C ${PATH_VCUBE} checkout master
	git -C ${PATH_VCUBE} pull
	${PATH_VCUBE}/install.sh

edit-project:
	${PATH_CUBE_MX} ${PROJECT_FILE}

flash: all
	${PATH_CUBE_PROG} -c port=SWD mode=UR -w ${DIR_BIN_IMAGES}/${TARGET}.hex 0x8000000 -v -rst

flash-rst:
	${PATH_CUBE_PROG} -c port=SWD mode=UR -rst

print-% : ; @echo $* = ${$*}
