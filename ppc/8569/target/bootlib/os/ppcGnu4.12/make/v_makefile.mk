################################################################################
# 自动的生成文件. 不要编辑!
################################################################################

#Basic variables defined here:
export PLATFORM:=C:/LambdaPRO
export TEMP = C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp
export WIND_HOME = C:/LambdaPRO
export WIND_BASE = C:/LambdaPRO/target
export TOOLS_CHAIN_PATH:=C:/LambdaPRO/host/gnu
export BIN_PATH:=$(PLATFORM)/host/bin
export PROJECT_PATH:=C:/LambdaPRO1/lib/os
export PROJECT_NAME:=os
export CONFIG_NAME:=ppcGnu4.12
export CONFIG_PATH:=C:/LambdaPRO1/lib/os/ppcGnu4.12
export CONFIG_ARCH := ppc
IS_BUILD_APP := Y

USE_DEFAULT_TOOLS_CHAIN = yes
export TOOLS_VERSION := x86-win32
DOT_A_OUTPUT_PATH := C:/LambdaPRO/target/deltaos/lib/x86/pentium/little
IS_SET_OUTPUT := N



FLAGS := 
PREFLAGS := 
ARCHIVES += lib$(PROJECT_NAME).a
EXECUTABLES +=C:/LambdaPRO1/lib/os/ppcGnu4.12/make/os
TARGET_EXT +=elf
LIB_PATH := $(CONFIG_PATH)/lib
IS_INCREMENTAL_BUILD_WITHBSP := N
# BSP目标文件存放的路径
BSP_OBJ_PATH :=$(CONFIG_PATH)/obj

IS_COMPILE_SOURCE := N
HAS_CPP := N

COMPILE_SYMBOL := -DCPU=PPC85XX 
COMPILE_OPTIMIZATION := -O0 
COMPILE_DEBUG := -g 
COMPILE_WARNING := -Wall 
COMPILE_OTHER := -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp 


#prebuild target defined here:
