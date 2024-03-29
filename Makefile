# Makefile installed by VanillaCube IDE

R             = $(shell pwd)
DIR_BUILD     = ${R}/build
DIR_GENERATED = ${R}/generated
DIR_IMAGES    = ${R}/images
MK_ENV        = ${DIR_BUILD}/vcube-install/env.mk
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

GIT_DESCRIBE         = $(shell git describe --dirty)
GIT_BRANCH           = $(shell git branch --show-current)
DATE                 = $(shell date -u +'%Y-%m-%d %H:%M:%S %Z')

DIR_OBJ              = ${DIR_BUILD}/obj
DIR_OUTPUT           = ${DIR_BUILD}/images
DIR_INJECTIONS       = ${PATH_VCUBE}/injections
DIR_VSCODE           = ${R}/.vscode
DIR_GENERATED_IMAGES = ${DIR_GENERATED}/images

IN_BUILD_CONFIG      = ${R}/build.conf.mk
IN_GENERATOR_SCRIPT  = ${DIR_INJECTIONS}/generate.template

OUT_GENERATOR_SCRIPT = ${DIR_BUILD}/generate.script
OUT_GENERATED_MX     = ${DIR_BUILD}/.generated-cubemx
OUT_GENERATED        = ${DIR_BUILD}/.generated
OUT_HEX_IMAGE        = ${DIR_OUTPUT}/${TARGET}.hex
OUT_IMAGES           = ${DIR_BUILD}/.images
OUT_BUILD_CONFIG     = ${DIR_BUILD}/.build-config
OUT_GIT_DESCRIBE     = ${DIR_BUILD}/.git-head

# ---------------------------------------------------------------------------------------------------------------------

# must rebuild, if these files's content change
REBUILD_FLAG_FILES   = ${OUT_BUILD_CONFIG} ${OUT_GIT_DESCRIBE}

LAST_BUILD_CONFIG    = $(shell (test -f ${OUT_BUILD_CONFIG} && cat ${OUT_BUILD_CONFIG}) || echo -n "-1")
LAST_GIT_DESCRIBE    = $(shell (test -f ${OUT_GIT_DESCRIBE} && cat ${OUT_GIT_DESCRIBE}) || echo -n "-")

# ---------------------------------------------------------------------------------------------------------------------

RM                   = rm -Rf
STYLER_DO            = find ${R}/src -name "*.hpp" -o -name "*.cpp" | xargs clang-format -i
STYLER_OK            = ${STYLER_DO} --dry-run -Werror

.PHONY: all artifact clean clean-deep clean-purge ${OUT_HEX_IMAGE}

all: artifact

Makefile: ;
${PROJECT_FILE}: ;
${IN_GENERATOR_SCRIPT}: ;
${IN_BUILD_CONFIG}: ;

# ---------------------------------------------------------------------------------------------------------------------

ifeq (,$(wildcard ${IN_BUILD_CONFIG}))
$(error Buiild config is missing. Run 'VanillaCube/install.sh')
endif

include ${IN_BUILD_CONFIG}

# these variables are passed down to second Makefile
export DIR_CPP_SRC   = ${R}/src
export DIR_VCL_SRC   = ${PATH_VCUBE}/lib/src
export BUILD_DIR     = ${DIR_OBJ}

export EXT_AS_FLAGS  = ${AS_FLAGS}
export EXT_C_FLAGS   = ${C_FLAGS}
export EXT_CPP_FLAGS = ${CPP_FLAGS}
export EXT_LD_FLAGS  = ${LD_FLAGS}

export OPT

# ---------------------------------------------------------------------------------------------------------------------

define display_info
	@printf "\n\n"
	@echo "Git: ${GIT_DESCRIBE} at branch ${GIT_BRANCH}"
	@echo "Build config: ${BUILD_CONFIG_TXT}"
	$(shell test -n "$$(git status -s)" && echo '@echo ""; echo "WARNING: Uncommited changes !"')
	@printf "\n *** Finished successfully! *** \n"
endef

# Output folder already exsists, we never have to depend on it. (See: include ${MK_ENV})

${OUT_BUILD_CONFIG}:
	echo -n ${BUILD_CONFIG} > ${OUT_BUILD_CONFIG}
	$(eval LAST_BUILD_CONFIG := ${BUILD_CONFIG})

${OUT_GIT_DESCRIBE}:
	echo -n ${GIT_DESCRIBE} > ${OUT_GIT_DESCRIBE}
	$(eval LAST_GIT_DESCRIBE := ${GIT_DESCRIBE})

${OUT_GENERATOR_SCRIPT}: ${IN_GENERATOR_SCRIPT}
	cat ${IN_GENERATOR_SCRIPT} | sed 's+@IOC_PATH@+${PROJECT_FILE}+' > ${OUT_GENERATOR_SCRIPT}

${OUT_GENERATED_MX}: ${PROJECT_FILE} | ${OUT_GENERATOR_SCRIPT}
	${RM} ${DIR_GENERATED}/Makefile
	${PATH_CUBE_MX} -q ${OUT_GENERATOR_SCRIPT}
	mv ${DIR_GENERATED}/Makefile ${DIR_GENERATED}/Makefile.original
	touch ${OUT_GENERATED_MX}

${OUT_GENERATED}: ${OUT_GENERATED_MX}
# must be a separate target to eval IT_FILE_NAME properly
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
	touch ${OUT_GENERATED}

${DIR_GENERATED}/Makefile: ${OUT_GENERATED} Makefile ${IN_BUILD_CONFIG} ${DIR_INJECTIONS}/*.mk | ${DIR_VSCODE}
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

IMAGE_CHANGE_FILES = ${DIR_IMAGES} $(shell find ${DIR_IMAGES} -type f) $(shell find ${DIR_IMAGES} -type d)

${OUT_IMAGES}: ${IMAGE_CHANGE_FILES} ${PATH_VCUBE}/convert-images.py | ${DIR_GENERATED_IMAGES}
	! test -d ${DIR_IMAGES} || ${PATH_VCUBE}/convert-images.py ${DIR_IMAGES} ${DIR_GENERATED_IMAGES}
	touch ${OUT_IMAGES}

.SILENT: ${OUT_HEX_IMAGE}
${OUT_HEX_IMAGE}: ${DIR_GENERATED}/Makefile ${OUT_IMAGES} | ${DIR_OUTPUT} ${DIR_OBJ}
	cd ${DIR_GENERATED} && make -j
	cp ${DIR_OBJ}/${TARGET}.hex ${OUT_HEX_IMAGE}
	cp ${DIR_OBJ}/${TARGET}.hex ${DIR_OUTPUT}/${TARGET}-${GIT_DESCRIBE}-${BUILD_CONFIG_TXT}.hex
	$(call display_info)

artifact: dynamic-dependencies ${OUT_HEX_IMAGE}

# rebuild dependencies
f-rebuild: ${REBUILD_FLAG_FILES}
bconf-eq-last: $(shell test ${LAST_BUILD_CONFIG} = ${BUILD_CONFIG} || echo -n "clean-soft f-rebuild")
discr-eq-last: $(shell test ${LAST_GIT_DESCRIBE} = ${GIT_DESCRIBE} || echo -n "clean-soft f-rebuild")

# style checking
style-ok: $(shell ${STYLER_OK} > /dev/null 2>&1 || echo -n "format-all")

# dependencies determined before exceution
dynamic-dependencies: bconf-eq-last discr-eq-last style-ok

${DIR_OBJ}:
	mkdir $@

${DIR_OUTPUT}:
	mkdir $@

${DIR_VSCODE}:
	mkdir $@

${DIR_GENERATED_IMAGES}:
	mkdir $@

clean-soft:
	@echo "Reseting rebuild flags..."
# build control files
	${RM} ${REBUILD_FLAG_FILES}
# binaries
	${RM} ${DIR_OBJ}

clean: clean-soft
	@echo "Cleaning..."
# build control files
	${RM} ${DIR_GENERATED}/Makefile ${OUT_IMAGES}
# binaries
	${RM} ${DIR_OUTPUT} ${DIR_IMAGES}/generated

clean-deep: clean
	@echo "Deep cleaning..."
# build control files
	${RM} ${OUT_GENERATED} ${OUT_GENERATED_MX}
# binaries
	find ${DIR_GENERATED} ! -name '${TARGET}.ioc' -type f -exec ${RM} {} +
	find ${DIR_GENERATED} -type d -empty -delete
	${RM} ${OUT_GENERATOR_SCRIPT}

clean-purge: clean-deep
	@echo "Purging..."
	${RM} ${DIR_BUILD}
	${PATH_VCUBE}/install.sh

# ---------------------------------------------------------------------------------------------------------------------

.PHONY: format-all update-vanillacube edit-project flash flash-rst

format-all:
	${STYLER_DO}

update-vanillacube:
	git -C ${PATH_VCUBE} checkout master
	git -C ${PATH_VCUBE} pull
	${PATH_VCUBE}/install.sh

edit-project:
	${PATH_CUBE_MX} ${PROJECT_FILE}

flash: ${OUT_HEX_IMAGE}
	${PATH_CUBE_PROG} -c port=SWD mode=UR -w ${DIR_OUTPUT}/${TARGET}.hex 0x8000000 -v -rst

flash-rst:
	${PATH_CUBE_PROG} -c port=SWD mode=UR -rst

print-% : ; @echo $* = ${$*}
