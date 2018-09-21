################################################################################
# Automatically-generated file. Do not edit!
################################################################################
-include ./objects.mk
-include ./sources.mk
-include ./v_makefile.mk
-include $(CONFIG_PATH)/make/preandpost.mk
-include $(PROJECT_PATH)/$(CONFIG_PATH)/toolchain.mk
-include $(PLATFORM)/target/deltaos/base.make

HOST_DIR = x86-win32
#Tool definitions:
CROSS_COMPILE =dcore-
AS =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)as-$(CONFIG_ARCH)
LD =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)ld-$(CONFIG_ARCH)
ifeq ($(IS_TEAMCC),YES)
CC	= $(TOOLS_CHAIN_PATH)/bin/teamcc (TOOLS_CHAIN_PATH)/$(HOST_DIR)/bin/$(CROSS_COMPILE)gcc-$(CONFIG_ARCH)
else
CC	=$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)gcc-$(CONFIG_ARCH)
endif
CPP =$(CC) -E
AR =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)ar-$(CONFIG_ARCH)
NM =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)nm-$(CONFIG_ARCH)
STRIP =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)strip-$(CONFIG_ARCH)
OBJDUMP =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)objdump-$(CONFIG_ARCH)
OBJCOPY =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)objcopy-$(CONFIG_ARCH)
RANLIB =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)ranlib-$(CONFIG_ARCH)
READELF =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)readelf-$(CONFIG_ARCH)
STRINGS =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)strings-$(CONFIG_ARCH)
SIZE =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)size-$(CONFIG_ARCH)
ADDR2LINE =$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)addr2line-$(CONFIG_ARCH)
MAKE =$(BIN_PATH)/make
RM := $(BIN_PATH)/rm -rf

# All of the sources participating in the build are defined here
ifeq ($(IS_COMPILE_SOURCE),Y)
-include $(SUBDIR_PATH)/subdir.mk
else
-include $(SUBDIR:%=%/subdir.mk)
endif
-include $(PROJECT_PATH)/subdir.mk
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
ifeq ($(IS_SET_OUTPUT),Y)
	ARCHIVES = $(DOT_A_OUTPUT_PATH)lib$(PROJECT_NAME).a
endif
# All Target
all : $(ARCHIVES)

compile_source :$(OBJS)

# Tool invocations
$(ARCHIVES): $(OBJS)
	@$(BIN_PATH)/echo 'Invoking: GCC Archiver'
	$(AR) -r $(ARCHIVES) @prjObjs.lst
	$(BIN_PATH)/echo 'Finished building: $@'
	@$(BIN_PATH)/echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(ARCHIVES)$(DEPS) #$(OUTPUT)
	-@$(BIN_PATH)/echo ' '
	
clean_source:
	-$(RM) $(OBJS)	

.PHONY: all clean dependents
.SECONDARY:
