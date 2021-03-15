
# injected by VanillaCube IDE -----------------------------------------------------------------------------------------

OBJECTS += $(addprefix ${BUILD_DIR}/,$(notdir ${CPP_SOURCES:.cpp=.cpp.o}))
vpath %.cpp $(sort $(dir ${CPP_SOURCES}))

CPPFLAGS = ${CFLAGS} -std=c++17 -fno-rtti -fno-exceptions -specs=nosys.specs -Wno-register

${BUILD_DIR}/%.cpp.o: %.cpp Makefile | ${BUILD_DIR}
	${PP} -c ${CPPFLAGS} -Wa,-a,-ad,-alms=${BUILD_DIR}/$(notdir $(<:.cpp=.cpp.lst)) $< -o $@

# end of injection ----------------------------------------------------------------------------------------------------
