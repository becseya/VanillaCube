
# injected by VanillaCube IDE -----------------------------------------------------------------------------------------

OBJECTS += $(subst ${DIR_CPP_SRC},${BUILD_DIR}/src,${CPP_SOURCES:.cpp=.cpp.o})
OBJECTS += $(subst ${DIR_VCL_SRC},${BUILD_DIR}/lib,${LIB_SOURCES:.cpp=.cpp.o})
OBJECTS += $(subst ${DIR_IMAGES},${BUILD_DIR}/img,${IMG_SOURCES:.cpp=.cpp.o})

CPPFLAGS = ${CFLAGS} -std=c++17 -fno-rtti -fno-exceptions -specs=nosys.specs -Wno-register

${BUILD_DIR}/src/%.cpp.o: ${DIR_CPP_SRC}/%.cpp Makefile | ${BUILD_DIR}
	mkdir -p $(@D)
	${PP} -c ${CPPFLAGS} -Wa,-a,-ad,-alms=$@.lst $< -o $@

${BUILD_DIR}/lib/%.cpp.o: ${DIR_VCL_SRC}/%.cpp Makefile | ${BUILD_DIR}
	mkdir -p $(@D)
	${PP} -c ${CPPFLAGS} -Wa,-a,-ad,-alms=$@.lst $< -o $@

${BUILD_DIR}/img/%.cpp.o: ${DIR_IMAGES}/%.cpp Makefile | ${BUILD_DIR}
	mkdir -p $(@D)
	${PP} -c ${CPPFLAGS} -I${DIR_IMAGES} -Wa,-a,-ad,-alms=$@.lst $< -o $@

# end of injection ----------------------------------------------------------------------------------------------------
