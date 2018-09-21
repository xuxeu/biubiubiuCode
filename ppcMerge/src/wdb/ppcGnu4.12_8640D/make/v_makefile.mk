################################################################################
# 自动的生成文件. 不要编辑!
################################################################################

#Basic variables defined here:
export PLATFORM:=F:/workspace/mergePPC/platform
export TEMP = C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp
export WIND_HOME = F:/workspace/mergePPC/platform
export WIND_BASE = F:/workspace/mergePPC/platform/target
export RTOS:=deltaos
export RTOS_PATH:=$(PLATFORM)/target/$(RTOS)
export BIN_PATH:=$(PLATFORM)/host/bin
export TOOLS_CHAIN_PATH:=F:/workspace/mergePPC/platform/host/gnu
export PROJECT_PATH:=F:/workspace/mergePPC/platform/workspace/wdb
export CONFIG_NAME:=ppcGnu4.12_8640D
export CONFIG_PATH:=F:/workspace/mergePPC/platform/workspace/wdb/ppcGnu4.12_8640D
export CONFIG_ARCH := null

TOOLS_VERSION := x86-win32



FLAGS := 
PREFLAGS := 
ARCHIVES += lib$(PROJECT_NAME).a
EXECUTABLES +=F:/workspace/mergePPC/platform/workspace/wdb/ppcGnu4.12_8640D/make/wdb
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
