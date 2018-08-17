################################################################################
# 自动的生成文件. 不要编辑!
################################################################################

#Basic variables defined here:
export PLATFORM:=F:/workspace/6.1.3.19/platform
export RTOS:=deltaos6.2
export RTOS_PATH:=$(PLATFORM)/$(RTOS)/target
export TEMP = C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp
export WIND_HOME = F:/workspace/6.1.3.19/platform
export WIND_BASE = F:/workspace/6.1.3.19/platform/$(RTOS)/target
export RTOS_PATH:=$(PLATFORM)/$(RTOS)/target
export BIN_PATH:=$(PLATFORM)/host/bin
export TOOLS_CHAIN:=4.3.3
export TARGET_ADAPT_LIB:=gnu4.3.3
export TOOLS_BIN_PATH:=x86-win32/bin
export TOOLS_CHAIN_PATH:=F:/workspace/6.1.3.19/platform/host/gnu/$(TOOLS_CHAIN)
export PROJECT_PATH:=F:/workspace/6.1.3.19/platform/workspace/sysDebugBranch
export CONFIG_NAME:=x86Gnu4.3.3le_lp64_smp
export CONFIG_PATH:=F:/workspace/6.1.3.19/platform/workspace/sysDebugBranch/x86Gnu4.3.3le_lp64_smp
export CONFIG_ARCH := x86
export VSB_LIB_NAME:=lib_lp64_smp

TOOLS_VERSION := null



FLAGS := 
PREFLAGS := 
ARCHIVES += lib$(PROJECT_NAME).a
EXECUTABLES +=F:/workspace/6.1.3.19/platform/workspace/sysDebugBranch/x86Gnu4.3.3le_lp64_smp/make/sysDebugBranch
TARGET_EXT +=elf
LIB_PATH := $(CONFIG_PATH)/lib
IS_INCREMENTAL_BUILD_WITHBSP := N
# BSP目标文件存放的路径
BSP_OBJ_PATH :=$(CONFIG_PATH)/obj

IS_COMPILE_SOURCE := N
HAS_CPP := N

COMPILE_SYMBOL := 
COMPILE_OPTIMIZATION := -O0 
COMPILE_DEBUG := -g 
COMPILE_WARNING := -Wall 
COMPILE_OTHER := -c -fno-builtin 


#prebuild target defined here:
