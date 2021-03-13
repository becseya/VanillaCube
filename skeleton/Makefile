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

IN_GENERATOR_SCRIPT  = ${DIR_INJECTIONS}/generate.template

OUT_GENERATOR_SCRIPT = ${DIR_OUTPUT}/generate.script
OUT_GENERATED        = ${DIR_OUTPUT}/.generated
OUT_HEX_IMAGE        = ${DIR_IMAGES}/${TARGET}.hex

.PHONY: all clean clean-deep clean-purge ${OUT_HEX_IMAGE}

all: ${OUT_HEX_IMAGE}

Makefile: ;
${PROJECT_FILE}: ;
${IN_GENERATOR_SCRIPT}: ;

# ---------------------------------------------------------------------------------------------------------------------

${OUT_GENERATOR_SCRIPT}: ${IN_GENERATOR_SCRIPT} # | output folder already exsists
	cat ${IN_GENERATOR_SCRIPT} | sed 's+@IOC_PATH@+${R}/${PROJECT_FILE}+' > ${OUT_GENERATOR_SCRIPT}

${OUT_GENERATED}: ${PROJECT_FILE} ${OUT_GENERATOR_SCRIPT} Makefile ${DIR_INJECTIONS}/*.mk
# makefile doest seems to re-genrate
	rm -f ${DIR_GENERATED}/Makefile
# generate
	${PATH_CUBE_MX} -q ${R}/${OUT_GENERATOR_SCRIPT}
# inject main
	sed -i '/USER CODE BEGIN EFP/a\void vcube_init();' ${DIR_GENERATED}/Inc/main.h
	sed -i '/USER CODE BEGIN EFP/a\void vcube_loop();' ${DIR_GENERATED}/Inc/main.h
	sed -i '/USER CODE BEGIN WHILE/a\  vcube_init();'  ${DIR_GENERATED}/Src/main.c
	sed -i '/USER CODE BEGIN 3/a\    vcube_loop();'    ${DIR_GENERATED}/Src/main.c
# inject makefile
	sed -i '/# paths/,/# source/c\___PATHS___'                                     ${DIR_GENERATED}/Makefile
	sed -i '/# binaries/,/# CFLAGS/c\___BIN_AND_CPP_SOURCES___'                    ${DIR_GENERATED}/Makefile
	sed -i "/vpath %.s/a\___COMPILE___"                                            ${DIR_GENERATED}/Makefile
	sed -i -e '/___PATHS___/{r ${DIR_INJECTIONS}/paths.mk' -e 'd}'                 ${DIR_GENERATED}/Makefile
	sed -i -e '/___BIN_AND_CPP_SOURCES___/{r ${DIR_INJECTIONS}/bin_cpp.mk' -e 'd}' ${DIR_GENERATED}/Makefile
	sed -i -e '/___COMPILE___/{r ${DIR_INJECTIONS}/compile.mk' -e 'd}'             ${DIR_GENERATED}/Makefile
# file marks successful injection
	touch ${OUT_GENERATED}

.SILENT: ${OUT_HEX_IMAGE}
${OUT_HEX_IMAGE}: ${OUT_GENERATED} | ${DIR_IMAGES} ${DIR_OBJ}
	cd ${DIR_GENERATED} && make
	cp ${DIR_OBJ}/${TARGET}.hex ${OUT_HEX_IMAGE}

${DIR_OBJ}:
	mkdir $@

${DIR_IMAGES}:
	mkdir $@

clean:
	rm -Rf ${DIR_OBJ} ${DIR_IMAGES}

clean-deep: clean
	find ${DIR_GENERATED} ! -name '${TARGET}.ioc' -type f -exec rm -f {} +
	rm ${OUT_GENERATED}

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

