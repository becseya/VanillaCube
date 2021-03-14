# Makefile installed by VanillaCube IDE

R             = $(shell pwd)
DIR_OUTPUT    = ./build
DIR_GENERATED = ./generated
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

DIR_OBJ              = ${DIR_OUTPUT}/obj
DIR_IMAGES           = ${DIR_OUTPUT}/images
DIR_INJECTIONS       = ${PATH_VCUBE}/injections
DIR_VSCODE           = ./.vscode

IN_GENERATOR_SCRIPT  = ${DIR_INJECTIONS}/generate.template

OUT_GENERATOR_SCRIPT = ${DIR_OUTPUT}/generate.script
OUT_GENERATED        = ${DIR_OUTPUT}/.generated
OUT_INJECTED         = ${DIR_OUTPUT}/.injected
OUT_HEX_IMAGE        = ${DIR_IMAGES}/${TARGET}.hex

.PHONY: all clean clean-deep clean-purge ${OUT_HEX_IMAGE}

all: ${OUT_HEX_IMAGE}

Makefile: ;
${PROJECT_FILE}: ;
${IN_GENERATOR_SCRIPT}: ;

# ---------------------------------------------------------------------------------------------------------------------

${OUT_GENERATOR_SCRIPT}: ${IN_GENERATOR_SCRIPT} # | output folder already exsists
	cat ${IN_GENERATOR_SCRIPT} | sed 's+@IOC_PATH@+${R}/${PROJECT_FILE}+' > ${OUT_GENERATOR_SCRIPT}

${OUT_GENERATED}: ${PROJECT_FILE} ${OUT_GENERATOR_SCRIPT}
	rm -f ${DIR_GENERATED}/Makefile
	${PATH_CUBE_MX} -q ${R}/${OUT_GENERATOR_SCRIPT}
	touch ${OUT_GENERATED}

${OUT_INJECTED}: ${OUT_GENERATED} Makefile ${DIR_INJECTIONS}/*.mk | ${DIR_VSCODE}
# inject main
	sed -i '/USER CODE END EFP/a\void vcube_init();' ${DIR_GENERATED}/Inc/main.h
	sed -i '/USER CODE END EFP/a\void vcube_loop();' ${DIR_GENERATED}/Inc/main.h
	sed -i '/USER CODE END 2/a\  vcube_init();'       ${DIR_GENERATED}/Src/main.c
	sed -i '/USER CODE END WHILE/a\    vcube_loop();' ${DIR_GENERATED}/Src/main.c
# inject makefile
	sed -i '/# paths/,/# source/c\___PATHS___'                  ${DIR_GENERATED}/Makefile
	sed -i '/# binaries/,/# CFLAGS/c\___BIN_AND_CPP_SOURCES___' ${DIR_GENERATED}/Makefile
	sed -i '/vpath %.s/a\___COMPILE___'                         ${DIR_GENERATED}/Makefile
	sed -i '/C_INCLUDES =/a\-I${PATH_VCUBE}/lib/inc \\'         ${DIR_GENERATED}/Makefile
	sed -i -e '/___PATHS___/{r ${DIR_INJECTIONS}/paths.mk' -e 'd}'                 ${DIR_GENERATED}/Makefile
	sed -i -e '/___BIN_AND_CPP_SOURCES___/{r ${DIR_INJECTIONS}/bin_cpp.mk' -e 'd}' ${DIR_GENERATED}/Makefile
	sed -i -e '/___COMPILE___/{r ${DIR_INJECTIONS}/compile.mk' -e 'd}'             ${DIR_GENERATED}/Makefile
# update vscode settings
	$(eval TARGET_DEF := $(shell cat ${DIR_GENERATED}/Makefile | grep  -Po '(?<=\-D)STM32[A-Z0-9a-z]+'))
	cat "${DIR_INJECTIONS}/c_cpp_properties.template" | sed 's+@TARGET_DEF@+${TARGET_DEF}+' > ${DIR_VSCODE}/c_cpp_properties.json
	@echo "Target define: ${TARGET_DEF}"
# finish
	touch ${OUT_INJECTED}

.SILENT: ${OUT_HEX_IMAGE}
${OUT_HEX_IMAGE}: ${OUT_INJECTED} | ${DIR_IMAGES} ${DIR_OBJ}
	cd ${DIR_GENERATED} && make
	cp ${DIR_OBJ}/${TARGET}.hex ${OUT_HEX_IMAGE}

${DIR_OBJ}:
	mkdir $@

${DIR_IMAGES}:
	mkdir $@

${DIR_VSCODE}:
	mkdir $@

clean:
	rm -Rf ${DIR_OBJ} ${DIR_IMAGES}

clean-deep: clean
	find ${DIR_GENERATED} ! -name '${TARGET}.ioc' -type f -exec rm -f {} +
	rm ${OUT_GENERATED} ${OUT_INJECTED}

clean-purge: clean-deep
	rm -Rf ${DIR_OUTPUT}
	${PATH_VCUBE}/install.sh

# ---------------------------------------------------------------------------------------------------------------------

edit-project:
	${PATH_CUBE_MX} ${R}/${PROJECT_FILE}

flash: all
	${PATH_CUBE_PROG} -c port=SWD mode=UR -w ${DIR_IMAGES}/${TARGET}.hex 0x8000000 -v -rst

flash-rst:
	${PATH_CUBE_PROG} -c port=SWD mode=UR -rst

debug-% : ; @echo $* = ${$*}

