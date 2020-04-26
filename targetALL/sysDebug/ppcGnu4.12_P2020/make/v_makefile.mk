################################################################################
# 自动的生成文件. 不要编辑!
################################################################################

#Basic variables defined here:
export PLATFORM:=I:/PPCdebug/platform
export RTOS:=deltaos6.0
export RTOS_PATH:=$(PLATFORM)/$(RTOS)/target
export TEMP = C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp
export WIND_HOME = I:/PPCdebug/platform
export WIND_BASE = I:/PPCdebug/platform/$(RTOS)/target
export RTOS_PATH:=$(PLATFORM)/$(RTOS)/target
export BIN_PATH:=$(PLATFORM)/host/bin
export TOOLS_CHAIN:=4.1.2
export TARGET_ADAPT_LIB:=gnu4.1.2
export TOOLS_BIN_PATH:=x86-win32/bin
export TOOLS_CHAIN_PATH:=I:/PPCdebug/platform/host/gnu/$(TOOLS_CHAIN)
export PROJECT_PATH:=I:/PPCdebug/platform/workspace/sysDebug
export CONFIG_NAME:=ppcGnu4.12_P2020
export CONFIG_PATH:=I:/PPCdebug/platform/workspace/sysDebug/ppcGnu4.12_P2020
export CONFIG_ARCH := ppc
export VSB_LIB_NAME:=lib

TOOLS_VERSION := null



FLAGS := 
PREFLAGS := 
ARCHIVES += lib$(PROJECT_NAME).a
EXECUTABLES +=I:/PPCdebug/platform/workspace/sysDebug/ppcGnu4.12_P2020/make/sysDebug
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
