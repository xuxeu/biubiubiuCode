#
# �汾��������Ϣ
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
# ����Ļ�����
#   �򿪾���(COMPILE_WARNING)�� 	 	-Wall
#   �Ż�����(COMPILE_OPTIMIZATION):  	-O2 ���� ��
#   ������Ϣ(COMPILE_DEBUG)�� 		-g3
#
BASE_CFLAGS := ${COMPILE_DEBUG} ${COMPILE_WARNING} ${COMPILE_OPTIMIZATION}


#
# �ں˰汾����
#
KERNEL_EDITION := DELTACORE_V2dot2


#
# arm�����ѡ��
#

ifeq (${ARCH},arm)
  CPU_FLAGS = -DCPU=_VX_ARMARCH7 -DTOOL_FAMILY=gnu -DTOOL=gnu
  
ARCH_MACRO     = __ARM__

#
# CPU���ѡ�� end
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

#����BSP��OS��ĿԴ���ʹ�ô˲���,$(COMPILE_OPTIMIZATION) $(COMPILE_DEBUG) $(COMPILE_WARNING) $(COMPILE_OTHER) ��OS��Ŀ�����õģ�
#������v_makefile.mk�У�OS��Ŀ��subdir.mk�л�ʹ�ô˲������˴������Ϊ�˱���弶Դ��Ҳ����ʹ�ô˺ꡣ�����ͻᵼ�±���Դ���ǻ����
#˫��ͬ���Ĳ�������������Ӱ���������
BASE_CFLAGS := $(BASE_CFLAGS)
CFLAGS := $(BASE_CFLAGS)  $(CPU_ENDIAN_CFLAGS) $(CPU_FLOAT_CFLAGS) $(COMPILE_OPTIMIZATION) $(COMPILE_DEBUG) $(COMPILE_WARNING) $(COMPILE_OTHER) 

#change by liy on 2016-9-9: ���ݹ������ɵ�VSB_LIB_NAME��TARGET_ADAPT_LIB�����ʹ��Դ���������ͷ�ļ�
#vsbConfig.h������·����
#
# ��������·��,����BSPʱ��ʹ�ô�·��
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
# ����ѡ��
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

#OS��Ŀ��ִ��TCLSHʱ�����ڵ�ǰĿ¼����nmpentium.exe������Ӧ��ִ��$(TOOLS_CHAIN_PATH)/$(TOOLS_BIN_PATH)�µ�tclsh
TCLSH =$(TOOLS_CHAIN_PATH)/$(TOOLS_BIN_PATH)/tclsh
wxTCLSH =$(WIND_HOME)/host/pub/wtxtcl
MAKE	=$(BIN_PATH)/make
.EXPORT_ALL_VARIABLES:
