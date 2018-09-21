#此文件供用户添加构建规则使用
include v_makefile.mk
include ../config_monitor.mk
include  $(RTOS_PATH)/base.make
WIND_HOME := E:/platform
WIND_BASE := E:/platform/target
TEMP := C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp

RM := rm -rf
OBJCOPY := $(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/dcore-objcopy-$(CONFIG_ARCH).exe
SIZE = $(TOOLS_CHAIN_PATH)/$(TOOLS_VERSION)/bin/dcore-size-$(CONFIG_ARCH).exe

#前面这些都是以前在vmakefile中定义的,现在整理后移植到makefile中定义
# 从这些工具命令添加输入输出到构建变量
#得到当前的路径
CURRENTDIR:=${RTOS_PATH}/tools/monitor

#添加显示配置选项
CONFIG_TA_DISPLAY_DEVICE_TYPE := VGA

#平台执行文件路径
#PLATFORM_BIN :=$(PLATFORM)/host/deltaos/bin

#设置ta/src/include为编译头文件路径
#这里需要根据具体的ARCH宏定义进行指定

INCLUDE_PATHS = -I$(CURRENTDIR) \
			-I$(RTOS_PATH)/include/debug \
			-I$(RTOS_PATH)/include/debug/$(CONFIG_ARCH) \
			-I$(CURRENTDIR)/src/board/${ARCH}/${TARGT_BOARD}/include \
			-I$(RTOS_PATH)/include/vxworks   \
			-I$(RTOS_PATH)/include/vxworks/private   \
			-I$(RTOS_PATH)/include/vxworks/types   \
			-I$(RTOS_PATH)/include/usr/h/ \
			-I$(RTOS_PATH)/include/usr/h/private \
			-I$(RTOS_PATH)/include/DeltaCore \
			
#编译参数
CFLAGS := $(CFLAGS) $(INCLUDE_PATHS)

FLAGS :=  -L$(RTOS_PATH)/lib/$(ARCH)/$(CONFIG_SUB_ARCH)/$(CONFIG_CPU_ENDIAN) -N -T $(PROJECT_PATH)/$(TARGT_BOARD)/catlinkcmds -Map map.txt 
LDFLAGS = FLAGS
PREFLAGS :=  -L$(RTOS_PATH)/lib/$(ARCH)/$(CONFIG_SUB_ARCH)/$(CONFIG_CPU_ENDIAN) -N -T $(PROJECT_PATH)/$(TARGT_BOARD)/prelinkcmds -Map map.txt

#设置压缩文件路径	
INPUT_ARCHIVES = libtra.a
ARCHIVES := $(INPUT_ARCHIVES)

#设置生成可执行文件名tra.后缀
EXECUTABLES += ../tra
TARGET_EXT +=bin

#需要编译构建的源代码的路径
SRC_PATHS = ../src \
			../src/board \
			../src/board/${ARCH} \
			../src/board/${ARCH}/${TARGT_BOARD} \
			../src/board/${ARCH}/${TARGT_BOARD}/driver \
			../src/board/${ARCH}/${TARGT_BOARD}/start \
			../src/board/${ARCH}/${TARGT_BOARD}/arch \
			../src/init \
			../src/communication

#根据源代码路径找到所需要的编译构建的所有源代码
SRC_FILES = $(notdir ${wildcard $(patsubst %, %/*.c, $(SRC_PATHS))}  \
            ${wildcard $(patsubst %, %/*.S, $(SRC_PATHS))} )

#根据配置选择编译哪个网卡驱动
ifeq ($(CONFIG_TA_ETHER_TYPE), INTEL8254X)
	SRC_FILES := ${subst taIntel8255x.c,, ${SRC_FILES}}
	SRC_FILES := ${subst taAm79C97X.c,, ${SRC_FILES}}
	SRC_FILES := ${subst taEth8139.c,, ${SRC_FILES}}	
endif
ifeq ($(CONFIG_TA_ETHER_TYPE), INTEL8255X)
	SRC_FILES := ${subst taIntel8254x.c,, ${SRC_FILES}}
	SRC_FILES := ${subst taAm79C97X.c,, ${SRC_FILES}}
	SRC_FILES := ${subst taEth8139.c,, ${SRC_FILES}}	
endif
ifeq ($(CONFIG_TA_ETHER_TYPE), RTL8139)
	SRC_FILES := ${subst taIntel8254x.c,, ${SRC_FILES}}
	SRC_FILES := ${subst taIntel8255x.c,, ${SRC_FILES}}
	SRC_FILES := ${subst taAm79C97X.c,, ${SRC_FILES}}	
endif
ifeq ($(CONFIG_TA_ETHER_TYPE), LN97)
	SRC_FILES := ${subst taIntel8254x.c,, ${SRC_FILES}}
	SRC_FILES := ${subst taIntel8255x.c,, ${SRC_FILES}}
	SRC_FILES := ${subst taEth8139.c,, ${SRC_FILES}}	
endif

ifeq ($(CONFIG_TA_DISPLAY_DEVICE_TYPE), VGA)
	SRC_FILES := ${subst taDVI.c,, ${SRC_FILES}}	
endif
ifeq ($(CONFIG_TA_DISPLAY_DEVICE_TYPE), DVI)
	SRC_FILES := ${subst taVga.c,, ${SRC_FILES}}	
endif

#建立*.o 文件对 *.c 文件的依赖关系
OBJS = ${patsubst %.c,%.o,${patsubst %.S,%.o,${SRC_FILES}}} 

CFLAGS += -DCONFIG_CORE_SMP -D_KERNEL_DEBUG_

#根据ARCH和大小端配置来建立编译链接参数
ifeq (${ARCH},x86)
	CFLAGS += -DTA_ARCH_X86
	CFLAGS += -D__X86__
	CFLAGS += -DCONFIG_CORE_SMP
	 
	ifeq (${CONFIG_CPU_ENDIAN},big)
		CFLAGS +=   -D__BIG_ENDIAN__
	endif
	
	ifeq (${CONFIG_CPU_ENDIAN},little)
	 	CFLAGS += -D__LITTLE_ENDIAN__
	endif
endif

ifeq (${ARCH},ppc)
 	CFLAGS += -DTA_ARCH_PPC
	CFLAGS += -D__PPC__
	ifeq (${CONFIG_CPU_ENDIAN},big)
		CFLAGS += -D__BIG_ENDIAN__ 
		FLAGS +=  -EB
		PREFLAGS += -EB
	endif
	
	ifeq (${CONFIG_CPU_ENDIAN},little)
	 	CFLAGS += -mlittle-endian  -D__LITTLE_ENDIAN__
		FLAGS +=  -EL
		PREFLAGS += -EL
	endif
endif

ifeq (${CONFIG_SUB_ARCH},8569)
	CFLAGS += -D__PPC85XX__
#	CFLAGS += -mcpu=8540  -mabi=no-spe -mspe=no
endif

ifeq (${ARCH},mips)
 	CFLAGS += -DTA_ARCH_MIPS
	CFLAGS += -G 0 -mno-branch-likely -mips3 -mabi=n32 -mgp64 -D__MIPS__ -DLS3_HT -DMIPSEL -DCPU=MIPS64 -DTOOL_FAMILY=gnu -DTOOL=gnule
	ifeq (${CONFIG_CPU_ENDIAN},big)
		CFLAGS += -D__BIG_ENDIAN__ 
		FLAGS +=  -EB
		PREFLAGS += -EB
	endif
	
	ifeq (${CONFIG_CPU_ENDIAN},little)
	 	CFLAGS += -EL -D__LITTLE_ENDIAN__
		FLAGS +=  -EL
		PREFLAGS += -EL
	endif
endif

#根据配置来建立编译参数
ifeq (${CONFIG_CPU_ENDIAN},big)
	CFLAGS +=  
endif

ifeq (${CONFIG_CPU_ENDIAN},little)
 	CFLAGS += 
endif

#根据软硬浮点设置判断软硬浮点的编译链接条件
TA_CC_FLOAT = 
TA_LD_FLOAT = 

ifeq (${CPU_FLOAT},hard)
	TA_CC_FLOAT := 
endif

ifeq (${CPU_FLOAT},soft)
	TA_CC_FLOAT := -msoft-float
endif

ifeq (${CPU_FLOAT},hard)
	TA_LD_FLOAT := -lc -lm
endif

ifeq (${CPU_FLOAT},soft)
	TA_LD_FLOAT := -lc_soft -lm_soft
endif

###################
LINK_OBJS = crt0.o
ifeq (${ARCH},ppc)
	LINK_OBJS = crt0.o
endif

##################


#根据前面的判断设置最后的编译链接参数
#ifeq (${ARCH},mips)
#	CFLAGS +=  -I$(CURRENTDIR)/../ -I$(CURRENTDIR)/../src $(COMPILE_DEBUG) $(COMPILE_WARNING) $(TA_CC_FLOAT) \
#	-DUINT32=unsigned -DINT8=char -DBOOL=int -DUINT8=unsigned\ char -DUINT16=unsigned\ short -G 0 -mips3 -D__MIPS__ \
#	-DMIPSEL -DCPU=MIPS64 -DTOOL_FAMILY=gnu -DTOOL=gnule
#endif

ifeq ($(CPU_BIT), 64)
	#LIBS := -lgcc64
endif

ifeq ($(CPU_BIT), 32)
	LIBS := -lgcc32
endif

ifeq (${ARCH},x86)
	LIBS := -lgcc
endif

ifeq (${ARCH},ppc)
	LIBS := -lgcc
endif
ifneq (${ARCH},mips)
	LIBS += $(TA_LD_FLOAT)
endif

LIBS += -lkda -ltautil

#根据配置决定生成文件的最终后缀名和生成方式
TA_EXECUTABLE_SUFFIX = 

ifeq (${ARCH},x86)
	TA_EXECUTABLE_SUFFIX := exe
endif

ifeq (${ARCH},ppc)
 	TA_EXECUTABLE_SUFFIX := bin
endif

ifeq (${ARCH},mips)
 	TA_EXECUTABLE_SUFFIX := elf
endif

# All Target
all : clean  $(EXECUTABLES).$(TA_EXECUTABLE_SUFFIX) finish

beforebuild :
	@echo "*********************************"
	@$(PLATFORM)/host/pub/TraTool ./config_ta.h

#建立elf文件到bin文件的依赖构建关系
$(EXECUTABLES).bin : $(EXECUTABLES).elf
	@echo 'Building target: $@'
ifeq (${ARCH},x86)
	@$(OBJCOPY) -O binary --strip-unneeded  -R .note -R .comment $<  $@
endif

ifeq (${ARCH},ppc)
	@$(BIN_PATH)/echo 'Building target: $@'
	$(OBJCOPY) -O binary --strip-unneeded -R .note -R .comment -R .stab -R .stabstr $<  $@
	@$(BIN_PATH)/echo 'Finished building target: $@'
	@$(BIN_PATH)/echo ' '
endif

#建立elf文件到exe文件的依赖关系
$(EXECUTABLES).exe : $(EXECUTABLES).elf
	@echo 'Building target: $@'
	@echo $(OBJCOPY) -O elf32-i386-vxworks --strip-debug -R .note -R .comment -R .stab -R .stabstr -R .jcr -R .rodata.str1.4 -R .rodata.str1.1 $<  $@
	@$(OBJCOPY) -O elf32-i386-vxworks --strip-debug -R .note -R .comment -R .stab -R .stabstr -R .jcr -R .rodata.str1.4 -R .rodata.str1.1 $<  $@
	@echo 'Finished building target'

#建立.a文件到elf文件的依赖构建关系
$(EXECUTABLES).elf : $(ARCHIVES)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	@echo "(PREFLAGS) $(PREFLAGS)"
	@echo "(FLAGS) $(FLAGS)"
	@echo $(LD) $(FLAGS) $(LINK_OBJS) $(USER_OBJS) --start-group $< $(LIBS) --end-group  -o $@
	@$(LD) $(FLAGS) $(LINK_OBJS) $(USER_OBJS) --start-group $< $(LIBS) --end-group  -o $@
	@echo $(SIZE) -A -x $@ 1>presize.txt
	@$(SIZE) -A -x $@ 1>presize.txt
	@echo 'Finished building target: $@'
	
#建立.o文件到.a文件的依赖构建关系
$(ARCHIVES):
	@echo 'Invoking: GCC Archiver'
	${MAKE} "CFLAGS := $(CFLAGS) " "L_OBJS=$(OBJS) " "L_NAME :=$(INPUT_ARCHIVES)" "L_PATH :=$(PROJECT_PATH)/.make" "VPATH = $(SRC_PATHS)" $(INPUT_ARCHIVES) -f $(RTOS_PATH)/Rules.make
	@echo "CFLAGS = $(CFLAGS)"
	@echo 'Finished building: $@'

#清理工作
clean:
ifeq ($(TARGET_EXT),elf)
	-$(RM) $(OBJS) $(ARCHIVES) $(DEPS)$(EXECUTABLES).elf
endif

ifeq ($(TARGET_EXT),bin)
	-$(RM) $(OBJS) $(ARCHIVES) $(DEPS)$(EXECUTABLES).bin $(EXECUTABLES).elf
endif

ifeq ($(TARGET_EXT),exe)
	-$(RM) $(OBJS) $(ARCHIVES) $(DEPS)$(EXECUTABLES).exe $(EXECUTABLES).elf
endif

finish:
	-$(RM) $(OBJS) $(ARCHIVES)

.PHONY: all clean dependents beforebuild
.SECONDARY:
