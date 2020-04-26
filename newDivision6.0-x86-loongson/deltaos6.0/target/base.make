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

#include  $(RTOS_PATH)/cur_board.def

#
# 编译的基本项
#   打开警告(COMPILE_WARNING)： 	 	-Wall
#   优化级别(COMPILE_OPTIMIZATION):  	-O2 或者 无
#   调试信息(COMPILE_DEBUG)： 		-g3

BASE_CFLAGS := ${COMPILE_DEBUG} ${COMPILE_WARNING} ${COMPILE_OPTIMIZATION}


#
# 内核版本定义
#
KERNEL_EDITION := DELTACORE_V2dot2

#
# arm的相关选项
#
ifeq (${ARCH},arm)

  ifeq (${SUB_ARCH},arm7)
    CPU_FLAGS = -mcpu=arm7tdmi  -D__ARM__ -D__ARM7__ 
    CPU_HARD_FLOAT_CFLAGS= -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float  -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS= -Wa,-EB -mbig-endian -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS := -d -N -oformat elf32-bigarm -format elf32-bigarm 
    CPU_LILLTE_ENDAIN_LDFLAGS := -d -N 
    SUB_ARCH_MACRO = __ARM7__
  endif
  
  ifeq (${SUB_ARCH},arm9)
    CPU_FLAGS = -mcpu=arm9tdmi -D__ARM__ -D__ARM9__
    CPU_HARD_FLOAT_CFLAGS= -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS= -Wa,-EB -mbig-endian -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS := -d -N -oformat elf32-bigarm -format elf32-bigarm 
    CPU_LILLTE_ENDAIN_LDFLAGS := -d -N 
    SUB_ARCH_MACRO = __ARM9__
  endif
  
  ARCH_MACRO     = __ARM__

endif

#
# x86的相关选项
#

ifeq (${ARCH},x86)
  ifeq (${SUB_ARCH},i386)
    CPU_FLAGS = -march=i386  -D__X86__ -D__I386__ -DCPU=_DELTA_I80386 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -mno-fp-ret-in-387 -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := 
    SUB_ARCH_MACRO = __I386__
  endif

  ifeq (${SUB_ARCH},i486)
    CPU_FLAGS = -march=i486  -D__X86__ -D__I486__ -DCPU=_DELTA_I80486 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -mno-fp-ret-in-387 -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := 
    SUB_ARCH_MACRO = __I486__
  endif

  ifeq (${SUB_ARCH},pentium)
    CPU_FLAGS = -march=pentium  -D__X86__ -D__PENTIUM__ -DCPU=PENTIUM 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -mno-fp-ret-in-387 -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := 
    SUB_ARCH_MACRO = __PENTIUM__
  endif

  ifeq (${SUB_ARCH},pentiumpro)
    CPU_FLAGS = -march=pentiumpro -D__X86__ -D__PENTIUMPRO__ -DCPU=_DELTA_PENTIUMPRO
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -mno-fp-ret-in-387 -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := 
    SUB_ARCH_MACRO = __PENTIUMPRO__
  endif

  ifeq (${SUB_ARCH},pentium2)
    CPU_FLAGS = -march=pentium  -D__X86__ -D__PENTIUM2__ -DCPU=_DELTA_PENTIUM2 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -mno-fp-ret-in-387 -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := 
    SUB_ARCH_MACRO = __PENTIUM2__
  endif

  ifeq (${SUB_ARCH},pentium3)
    CPU_FLAGS = -march=pentium3  -D__X86__ -D__PENTIUM3__ -DCPU=_DELTA_PENTIUM3 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -mno-fp-ret-in-387 -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := 
    SUB_ARCH_MACRO = __PENTIUM3__
  endif

  ifeq (${SUB_ARCH},pentium4)
    CPU_FLAGS = -march=pentium  -D__X86__ -D__PENTIUM4__ -DCPU=_DELTA_PENTIUM4 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -mno-fp-ret-in-387 -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := 
    SUB_ARCH_MACRO = __PENTIUM4__
  endif

ARCH_MACRO     = __X86__

endif

#
# ppc的相关选项
#

ifeq (${ARCH},ppc)

  ifeq (${SUB_ARCH},ppc860)
    CPU_FLAGS = -mcpu=860  -D__PPC__ -D__PPC860__ 
    CPU_HARD_FLOAT_CFLAGS= -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -mlittle-endian  -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __PPC860__
  endif

  ifeq (${SUB_ARCH},405)
    CPU_FLAGS = -mcpu=405  -D__PPC__ -D__PPC405__ 
    CPU_HARD_FLOAT_CFLAGS= -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS=  -mlittle-endian -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __PPC405__
  endif

  ifeq (${SUB_ARCH},505)
    CPU_FLAGS = -mcpu=505  -D__PPC__ -D__PPC505__ 
    CPU_HARD_FLOAT_CFLAGS= -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS=  -mlittle-endian -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __PPC505__
  endif

  ifeq (${SUB_ARCH},603e)
    CPU_FLAGS = -mcpu=603e  -D__PPC__ -D__PPC603E__ 
    CPU_HARD_FLOAT_CFLAGS= -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS=  -mlittle-endian -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __PPC603E__
  endif

  ifeq (${SUB_ARCH},7450)
    CPU_FLAGS = -mcpu=7450  -D__PPC__ -D__PPC7450__ 
    CPU_HARD_FLOAT_CFLAGS= -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS=  -mlittle-endian -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __PPC7450__
  endif

  ifeq (${SUB_ARCH},750)
    CPU_FLAGS = -mcpu=750  -D__PPC__ -D__PPC750__ 
    CPU_HARD_FLOAT_CFLAGS= -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -mlittle-endian -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS= -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __PPC750__
  endif

ARCH_MACRO     = __PPC__

endif


#
# mips的相关选项
#

ifeq (${ARCH},mips)
  CPU_BIT := 64
  ifeq (${SUB_ARCH},loongson1b)
    CPU_BIT := 32
  endif

  ifeq (${SUB_ARCH},r3000)
    CPU_FLAGS = -march=r3000  -D__MIPS__ -D__R3000__ 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS=  -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS :=  
    CPU_LILLTE_ENDAIN_LDFLAGS := 
    SUB_ARCH_MACRO = __R3000__
  endif

  ifeq (${SUB_ARCH},r4000)
    CPU_FLAGS = -march=r4000  -D__MIPS__ -D__R4000__ 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -EL -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS= -EB -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS := -EB  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __R4000__
  endif	
  
  ifeq (${SUB_ARCH},loongson2f)
  	ifeq ($(CPU_BIT), 64)
		CPU_FLAGS = -G 0 -mips3 -D__MIPS__ -DMIPSEL -DCPU=MIPS64 -DTOOL_FAMILY=gnu -DTOOL=gnule 
	endif
	ifeq ($(CPU_BIT), 32)
		CPU_FLAGS = -G 0 -mips2 -D__MIPS__ -DMIPSEL -DCPU=MIPS32 -DTOOL_FAMILY=gnu -DTOOL=gnule 
	endif
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -EL -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS= -EB -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS := -EB  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __R4000__
  endif	
  
  ifeq (${SUB_ARCH},loongson2h)
	CPU_FLAGS = -G 0 -mno-branch-likely -EL -mips64 -mabi=n32 -mgp64 -D__MIPS__ -DMIPSEL -DCPU=_DELTA_MIPS64 -DCPU_FAMILY=MIPS -DTOOL_FAMILY=gnu -DTOOL=gnule 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -EL -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS= -EB -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS := -EB  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __R4000__
  endif	
 
   ifeq (${SUB_ARCH},loongson1b)
 	ifeq ($(CPU_BIT), 64)
		CPU_FLAGS = -G 0 -mips3  -D__MIPS__ -DMIPSEL -DCPU=MIPS64 -DTOOL_FAMILY=gnu -DTOOL=gnule 
	endif
	ifeq ($(CPU_BIT), 32)
		CPU_FLAGS = -G 0 -mips32r2 -mgp32 -D__MIPS__  -D__MIPS_1B__  -DMIPSEL -DCPU=MIPS32 -DTOOL_FAMILY=gnu -DTOOL=gnule  -DMIPS32_1B
	endif
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -EL -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS= -EB -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS := -EB  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __R4000__
  endif	

  ifeq (${SUB_ARCH},loongson2k)
	CPU_FLAGS = -G 0 -mno-branch-likely -EL -mips64 -mabi=n32 -mgp64 -D__MIPS__ -DMIPSEL -DCPU=_DELTA_MIPS64 -DCPU_FAMILY=MIPS -DTOOL_FAMILY=gnu -DTOOL=gnule 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -EL -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS= -EB -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS := -EB  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __R4000__
  endif	
 
 #change by luoad on 2016-7-1合并龙芯平台相关的处理。 
  ifeq (${SUB_ARCH},loongson3a)
	CPU_FLAGS = -G 0 -mno-branch-likely -EL -mips64 -mabi=n32 -mgp64 -D__MIPS__ -DMIPSEL -DCPU=_DELTA_MIPS64 -DCPU_FAMILY=MIPS -DTOOL_FAMILY=gnu -DTOOL=gnule 
    CPU_HARD_FLOAT_CFLAGS=-mhard-float -D__HARD_FLOAT__
    CPU_SOFT_FLOAT_CFLAGS=-msoft-float -D__SOFT_FLOAT__
    CPU_LILLTE_ENDAIN_CFLAGS= -EL -D__LITTLE_ENDIAN__
    CPU_BIG_ENDAIN_CFLAGS= -EB -D__BIG_ENDIAN__
    CPU_BIG_ENDAIN_LDFLAGS := -EB  
    CPU_LILLTE_ENDAIN_LDFLAGS := -EL
    SUB_ARCH_MACRO = __R4000__
  endif	  
  
    
  
ARCH_MACRO     = __MIPS__

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

#VXWORK_CFLAGS := -D_WRS_CONFIG_TASK_CREATE_HOOKS -D_WRS_KERNEL -D_WRS_CONFIG_OBJECT_LIB  -D_WRS_CONFIG_OBJECT_VERIFICATION -D_WRS_CONFIG_TASK_SWITCH_HOOKS -D_WRS_CONFIG_TASK_SWAP_HOOKS
#D_DELTA_CFLAGS :=-D$(KERNEL_EDITION) -D$(SOC_MACRO) -D$(BOARD_MACRO) -DMAX_CPU_NUM=8 -DCONFIG_CORE_SMP -D_WRS_CONFIG_SMP  -D_DELTA_CORE -D_DELTA_CONFIG_SMP -DINET -DIP_PORT_VXWORKS=68

	#编译调试库时需要使用宏MAX_CPU_NUM，所以在此处增加了MAX_CPU_NUM的宏定义。后继应该在ta.h中在ta.h中应该使用DELTA_MAX_SMP_CPUS来重新定义MAX_CPU_NUM。
	VXWORK_CFLAGS := -D_WRS_CONFIG_SMP -D_WRS_KERNEL -DINET
	D_DELTA_CFLAGS :=-D$(KERNEL_EDITION) -D$(SOC_MACRO) -D$(BOARD_MACRO)  -D_DELTA_CORE -DCONFIG_CORE_SMP -DMAX_CPU_NUM=4

ifeq (${ARCH},mips)
	#在兼容层中某些函数提供C语言和汇编语言两种实现，当定义宏PORTABLE时，采用C语言实现的接口，反之试用汇编语言实现的接口。
	#该宏在编译arch库和兼容层库时使用。
	D_DELTA_CFLAGS +=-DPORTABLE 
endif


#在host\gnu\Cfg_ToolChain.cfg中配置了工具链适配的vx版本value(在v_makefile.mk中生成对应的ADAPT_VERSION)，
#如果选择了适配vx66，添加CONFIG_INCLUDE_NETWORK_VX66宏
ifeq (${ADAPT_VERSION},6.6)
	BASE_CFLAGS :=-DCONFIG_INCLUDE_NETWORK_VX66
	DEPEND_CFLAGS:= $(D_DELTA_CFLAGS) -DIP_PORT_VXWORKS=66 -DVXB_LEGACY_ACCESS
else
	BASE_CFLAGS :=-DCONFIG_INCLUDE_NETWORK_VX68
	DEPEND_CFLAGS:= $(D_DELTA_CFLAGS) -DIP_PORT_VXWORKS=68
endif

BASE_CFLAGS := $(BASE_CFLAGS) $(CPU_FLAGS) $(VXWORK_CFLAGS) $(DEPEND_CFLAGS)

#编译BSP文件时也可以使用OS配置的编译选项。
CFLAGS := $(BASE_CFLAGS)  $(CPU_ENDIAN_CFLAGS) $(CPU_FLOAT_CFLAGS) $(COMPILE_OPTIMIZATION) $(COMPILE_DEBUG) $(COMPILE_WARNING) $(COMPILE_OTHER) 

#
# 缺省的编译搜索路径 
# 增加BSP路径下的头文件路径和项目的配置文件路径
#

DEFAULT_SEARCH_PATH= -I. \
					 -I../include \
					 -I$(RTOS_PATH)/$(VSB_LIB_NAME)/h/config \
					 -I$(L_INCLUDE_H) \
					 -I$(BSPS_PATH) \
					 -I$(RTOS_PATH)/all \
					 -I$(RTOS_PATH)/comps/src \
					 -I$(RTOS_PATH)/comps/$(CONFIG_ARCH)/src \
					 -I$(RTOS_PATH)/include/arch/${CONFIG_ARCH} \
					 -I$(RTOS_PATH)/include \
             -I$(RTOS_PATH)/include/vxworks \
			 -I$(RTOS_PATH)/include/DeltaCore \
             -I$(RTOS_PATH)/include/debug \
             -I$(RTOS_PATH)/include/arch/${CONFIG_ARCH} \
			 -I$(RTOS_PATH)/include/vxworks/wrn/coreip \
			 -I$(RTOS_PATH)/src \
			 -I$(RTOS_PATH)/src/vxworks/src/hwif/h \
			 -I$(RTOS_PATH)/src/vxworks/src  \
			 -I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.8/ipcom/port/vxworks/include  \
         	 -I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.8/ipcom/port/vxworks/config  \
         	 -I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.8/ipcom/include  \
         	 -I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.8/vxmux/include  \
         	 -I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.8/vxmux/config  \
         	 -I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.8/ipnet2/src  \
         	 -I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.8/ipnet2/include  \
         	 -I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.8/ipnet2/config  \
         	 -I$(RTOS_PATH)/src/vxworks/components/ip_net2-6.8/ipcom/config  
					 
         
                     


AR_FLAGS := -ruvs
#change by liy on 2016-9-14：交叉工具链已经由host\gnu\Cfg_ToolChain.cfg中工具链配置的makefile属性中指定的文件配置。
#TOOLS_VERSION=x86-win32

#CROSS_COMPILE 	=dcore-

#AS	=$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)as-$(CONFIG_ARCH)
#LD	=$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)ld-$(CONFIG_ARCH)
#ifeq ($(IS_TEAMCC),YES)
#	CC	=$(TOOLS_CHAIN_PATH)/bin/teamcc $(TOOLS_CHAIN_PATH)/arch/$(ARCH)/bin/$(CROSS_COMPILE)gcc-$(CONFIG_ARCH)
#else
#	CC	=$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)gcc-$(CONFIG_ARCH)
#endif
#CPP	=$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/$(CC)-$(CONFIG_ARCH) -E
#AR	=$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)ar-$(CONFIG_ARCH)
#NM	=$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)nm-$(CONFIG_ARCH)
#STRIP	=$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)strip-$(CONFIG_ARCH)
#OBJDUMP	=$(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/$(CROSS_COMPILE)objdump-$(CONFIG_ARCH)
MAKE	=$(BIN_PATH)/make


.EXPORT_ALL_VARIABLES:
