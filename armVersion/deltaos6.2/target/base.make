#
# 版本和日期信息
#

MAJOR_VERSION = 1
MINOR_VERSION = 0
BUILD_NUMBER = 0
RELEASE_DATE = 20060901
EDITION = 0

ARCH = ${CONFIG_ARCH}
SUB_ARCH = ${CONFIG_SUB_ARCH}
CPU_BIT = ${CONFIG_CPU_BIT}
CPU_ENDIAN = ${CONFIG_CPU_ENDIAN}
CPU_FLOAT =${CONFIG_CPU_FLOAT}
CUR_BOARD = ${CURRENT_BOARD}
SOC_MACRO = SOC_${CONFIG_SUB_ARCH}
BOARD_MACRO= BOARD_${CURRENT_BOARD}
HOSTPUB_PATH=$(WIND_HOME)/host/pub

ifeq ($(FOUNDATION_BASE), )
FOUNDATION_BASE=$(WIND_HOME)/host/deltaos/foundation
endif

OBJ_PATH=$(CONFIG_PATH)/obj
MAKE_PATH=$(CONFIG_PATH)/make
LIB_PATH=$(CONFIG_PATH)/lib

#
# 编译的基本项
#   打开警告(COMPILE_WARNING)： 	 	-Wall
#   优化级别(COMPILE_OPTIMIZATION):  	-O2 或者 无
#   调试信息(COMPILE_DEBUG)： 		-g3
#
BASE_CFLAGS := ${COMPILE_DEBUG} ${COMPILE_WARNING} ${COMPILE_OPTIMIZATION}


#
# 内核版本定义
#
KERNEL_EDITION := DELTACORE_V2dot2


#
# arm的相关选项
#

ifeq (${ARCH},arm)
  CPU_FLAGS = -DCPU=_VX_ARMARCH7 -DTOOL_FAMILY=gnu -DTOOL=gnu
  
ARCH_MACRO     = __ARM__

#
# CPU相关选项 end
#
endif


ifeq (${CPU_ENDIAN},little)
  CPU_ENDIAN_CFLAGS := $(CPU_LILLTE_ENDAIN_CFLAGS)
  LDFLAGS := $(CPU_LILLTE_ENDAIN_LDFLAGS)
  ENDIAN_MCRO       := __LITTLE_ENDIAN__
else
  CPU_ENDIAN_CFLAGS := $(CPU_BIG_ENDAIN_CFLAGS)
  ENDIAN_MCRO       := __BIG_ENDIAN__
  LDFLAGS := $(CPU_BIG_ENDAIN_LDFLAGS)
endif

ifeq (${CPU_FLOAT},soft)
  CPU_FLOAT_CFLAGS := $(CPU_SOFT_FLOAT_CFLAGS)
  FLOAT_MACRO      :=__SOFT_FLOAT__
else
  CPU_FLOAT_CFLAGS := $(CPU_HARD_FLOAT_CFLAGS)
  FLOAT_MACRO      :=__HARD_FLOAT__
endif

VXWORK_CFLAGS := -D_WRS_KERNEL -DARMEL -DCPU_CORTEXA8 -DARMMMU=ARMMMU_CORTEXA8 -DARMCACHE=ARMCACHE_CORTEXA8 -DARM_USE_VFP -DIP_PORT_VXWORKS=69 -DINET -xassembler-with-cpp -D_WRS_VX_SMP
D_DELTA_CFLAGS :=-D$(KERNEL_EDITION) -D$(SOC_MACRO) -D$(BOARD_MACRO)  -D_DELTA_CORE

BASE_CFLAGS += -t7 -mfpu=vfp -mfloat-abi=softfp -fno-zero-initialized-in-bss -fvolatile -DCONFIG_USR_DEBUG -D__ARM__

#编译BSP和OS项目源码会使用此参数,$(COMPILE_OPTIMIZATION) $(COMPILE_DEBUG) $(COMPILE_WARNING) $(COMPILE_OTHER) 是OS项目中配置的，
#生成在v_makefile.mk中，OS项目在subdir.mk中会使用此参数，此处添加是为了编译板级源码也可以使用此宏。这样就会导致编译源码是会存在
#双份同样的参数，不过不会影响编译结果。
BASE_CFLAGS := $(BASE_CFLAGS)
CFLAGS := $(BASE_CFLAGS)  $(CPU_ENDIAN_CFLAGS) $(CPU_FLOAT_CFLAGS) $(COMPILE_OPTIMIZATION) $(COMPILE_DEBUG) $(COMPILE_WARNING) $(COMPILE_OTHER) 

#change by liy on 2016-9-9: 根据工具生成的VSB_LIB_NAME和TARGET_ADAPT_LIB，添加使用源代码库配置头文件
#vsbConfig.h的搜索路径。
#
# 编译搜索路径,编译BSP时会使用此路径
#
DEFAULT_SEARCH_PATH= -I.. \
					-I$(RTOS_PATH)/comps/src \
					-I$(RTOS_PATH)/include/vxworks \
					-I$(RTOS_PATH)/include/DeltaCore \
					-I$(RTOS_PATH)/include/vxworks/wrn/coreip \
					-I$(RTOS_PATH)/src/vxworks/src/drv \
					-I$(RTOS_PATH)/src/vxworks/src  \
					-I$(RTOS_PATH)/$(VSB_LIB_NAME)/h/arm/ARMARCH7/common \
					-I$(RTOS_PATH)/include/debug \
					-I$(RTOS_PATH)/include/debug/arm \
					-I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.9/ipnet2/src  \
					-I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.9/ipnet2/include	\
					-I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.9/ipappl/config  \
					-I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.9/ipappl/include
#
# 编译选项
#         
OPTION_PP		= -E
OPTION_PP_AS		=
OPTION_PP_NOLINENO	= -P
OPTION_PP_COMMENT	= -C
OPTION_DOLLAR_SYMBOLS	= -fdollars-in-identifiers
OPTION_DEPEND		= -M -w
OPTION_DEPEND_GEN	= -M -MG -w
OPTION_DEPEND_GEN_D	= -MD
OPTION_DEPEND_C		= $(OPTION_DEPEND_GEN)
OPTION_DEPEND_AS	= $(OPTION_DEPEND)
OPTION_DEPEND_TARGET	= -MT
OPTION_NO_WARNINGS	= -w
OPTION_OBJECT_ONLY	= -c
OPTION_INCLUDE_DIR	= -I
OPTION_LIB_DIR	= -L
OPTION_CC_GENMAP	= -Wl,-Map,$@.map,-cref
OPTION_LD_GENMAP	= -Map $@.map -cref
OPTION_OBJECT_NAME	= -o 
OPTION_EXE_NAME		= $(OPTION_OBJECT_NAME)
OPTION_DEFINE_MACRO	= -D
OPTION_PIC		=
OPTION_POINTERS_VOLATILE = -fvolatile
OPTION_VSBCONFIG = -D_VSB_CONFIG_FILE
	 
EXTRA_CFLAGS += $(DEFAULT_SEARCH_PATH) -DCPU=_VX_ARMARCH7 -DTOOL_FAMILY=gnu -DTOOL=gnu -D_WRS_KERNEL -DARMEL -DCPU_CORTEXA8 -DARMMMU=ARMMMU_CORTEXA8 -DARMCACHE=ARMCACHE_CORTEXA8 -DARM_USE_VFP -DIP_PORT_VXWORKS=69 -DINET   -D_WRS_VX_SMP -D_VSB_CONFIG_FILE=\"$(RTOS_PATH)/$(VSB_LIB_NAME)/h/config/vsbConfig.h\"
	 
AR_FLAGS := -ruvs
TOOLS_VERSION=x86-win32

RM := $(BIN_PATH)/rm -rf
MV := $(BIN_PATH)/mv

#OS项目在执行TCLSH时，会在当前目录调用nmpentium.exe，所以应该执行$(TOOLS_CHAIN_PATH)/$(TOOLS_BIN_PATH)下的tclsh
TCLSH =$(TOOLS_CHAIN_PATH)/$(TOOLS_BIN_PATH)/tclsh
wxTCLSH =$(WIND_HOME)/host/pub/wtxtcl
MAKE	=$(BIN_PATH)/make
.EXPORT_ALL_VARIABLES:
