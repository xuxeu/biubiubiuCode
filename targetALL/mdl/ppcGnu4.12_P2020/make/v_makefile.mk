################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Basic variables defined here:
export PLATFORM:=G:/203/tonghuanide/platform
export RTOS:=deltaos6.0
export RTOS_PATH:=$(PLATFORM)/$(RTOS)/target
export TEMP = C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp
export WIND_HOME = G:/203/tonghuanide/platform
export WIND_BASE = G:/203/tonghuanide/platform/$(RTOS)/target
export RTOS_PATH:=$(PLATFORM)/$(RTOS)/target
export BIN_PATH:=$(PLATFORM)/host/bin
export TOOLS_CHAIN:=4.1.2
export TARGET_ADAPT_LIB:=gnu4.1.2
export TOOLS_BIN_PATH:=x86-win32/bin
export TOOLS_CHAIN_PATH:=G:/203/tonghuanide/platform/host/gnu/$(TOOLS_CHAIN)
export PROJECT_PATH:=G:/203/tonghuanide/runtime-LambdaAE.product/mdl
export CONFIG_NAME:=ppcGnu4.12_P2020
export CONFIG_PATH:=G:/203/tonghuanide/runtime-LambdaAE.product/mdl/ppcGnu4.12_P2020
export CONFIG_ARCH := ppc
export VSB_LIB_NAME:=lib

TOOLS_VERSION := null



FLAGS := 
PREFLAGS := 
ARCHIVES += lib$(PROJECT_NAME).a
EXECUTABLES +=G:/203/tonghuanide/runtime-LambdaAE.product/mdl/ppcGnu4.12_P2020/make/mdl
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
