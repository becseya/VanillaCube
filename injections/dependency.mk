# dependencies
#######################################

# injected by VanillaCube IDE -----------------------------------------------------------------------------------------

-include $(shell find ${BUILD_DIR} -name "*.d")

print-% : ; @echo $* = ${$*}

# end of injection ----------------------------------------------------------------------------------------------------

# *** EOF ***
