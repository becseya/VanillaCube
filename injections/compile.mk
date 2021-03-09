
# injected by CubeVanilla IDE -----------------------------------------------------------------------------------------

OBJECTS += $(addprefix ${BUILD_DIR}/,$(notdir ${CPP_SOURCES:.cpp=.o}))
vpath %.cpp $(sort $(dir ${CPP_SOURCES}))

CPPFLAGS = ${CFLAGS} -std=c++11 -fno-rtti -fno-exceptions -specs=nosys.specs

${BUILD_DIR}/%.o: %.cpp Makefile | ${BUILD_DIR}
	${PP} -c ${CPPFLAGS} -Wa,-a,-ad,-alms=${BUILD_DIR}/$(notdir $(<:.cpp=.lst)) $< -o $@

# end of injection ----------------------------------------------------------------------------------------------------
