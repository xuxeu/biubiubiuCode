-include $(PROJECT_PATH)/$(CONFIG_NAME)/toolchain.mk
include  $(RTOS_PATH)/base.make

#源文件路径及源文件
ifeq ($(CONFIG_ARCH), x86)
	SRC_PATHS = $(PROJECT_PATH)/src \
			$(PROJECT_PATH)/src/utils \
			$(PROJECT_PATH)/src/debug \
			$(PROJECT_PATH)/src/debug/rtp \
			$(PROJECT_PATH)/src/debug/task \
			$(PROJECT_PATH)/src/debug/sda \
			$(PROJECT_PATH)/src/debug/db \
			$(PROJECT_PATH)/src/debug/mm \
			$(PROJECT_PATH)/src/debug/db/arch/$(CONFIG_ARCH) \
			$(PROJECT_PATH)/src/communication \
			$(PROJECT_PATH)/src/communication/command \
			$(PROJECT_PATH)/src/communication/taProtocol \
			$(PROJECT_PATH)/src/communication/msgdispatch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch/$(CONFIG_ARCH) \

KDA_SRC_PATHS = $(PROJECT_PATH)/src \
			$(PROJECT_PATH)/src/utils \
			$(PROJECT_PATH)/src/debug \
			$(PROJECT_PATH)/src/debug/sda \
			$(PROJECT_PATH)/src/debug/db \
			$(PROJECT_PATH)/src/debug/mm \
			$(PROJECT_PATH)/src/debug/db/arch/$(CONFIG_ARCH) \
			$(PROJECT_PATH)/src/communication \
			$(PROJECT_PATH)/src/communication/command \
			$(PROJECT_PATH)/src/communication/taProtocol \
			$(PROJECT_PATH)/src/communication/msgdispatch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch/$(CONFIG_ARCH)
endif

ifeq ($(CONFIG_ARCH), mips)
	SRC_PATHS = $(PROJECT_PATH)/src \
			$(PROJECT_PATH)/src/utils \
			$(PROJECT_PATH)/src/debug \
			$(PROJECT_PATH)/src/debug/rtp \
			$(PROJECT_PATH)/src/debug/task \
			$(PROJECT_PATH)/src/debug/sda \
			$(PROJECT_PATH)/src/debug/db \
			$(PROJECT_PATH)/src/debug/mm \
			$(PROJECT_PATH)/src/debug/db/arch/$(CONFIG_ARCH) \
			$(PROJECT_PATH)/src/communication \
			$(PROJECT_PATH)/src/communication/command \
			$(PROJECT_PATH)/src/communication/taProtocol \
			$(PROJECT_PATH)/src/communication/msgdispatch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch/$(CONFIG_ARCH)
			
	KDA_SRC_PATHS = $(PROJECT_PATH)/src \
			$(PROJECT_PATH)/src/utils \
			$(PROJECT_PATH)/src/debug \
			$(PROJECT_PATH)/src/debug/sda \
			$(PROJECT_PATH)/src/debug/db \
			$(PROJECT_PATH)/src/debug/mm \
			$(PROJECT_PATH)/src/debug/db/arch/$(CONFIG_ARCH) \
			$(PROJECT_PATH)/src/communication \
			$(PROJECT_PATH)/src/communication/command \
			$(PROJECT_PATH)/src/communication/taProtocol \
			$(PROJECT_PATH)/src/communication/msgdispatch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch/$(CONFIG_ARCH) 
endif

SRC_FILES = $(notdir ${wildcard $(patsubst %, %/*.c, $(SRC_PATHS))}  \
            ${wildcard $(patsubst %, %/*.S, $(SRC_PATHS))} )

KDA_SRC_FILES = $(notdir ${wildcard $(patsubst %, %/*.c, $(KDA_SRC_PATHS))}  \
            ${wildcard $(patsubst %, %/*.S, $(KDA_SRC_PATHS))} )
                   
SRC_OBJS = ${patsubst %.c,%.o,${patsubst %.S,%.o,${SRC_FILES}}} 

KDA_SRC_OBJS = ${patsubst %.c,%.o,${patsubst %.S,%.o,${KDA_SRC_FILES}}} 

#头文件路径
INCLUDE_PATHS :=-I$(PROJECT_PATH)/include \
			-I$(PROJECT_PATH)/include/extern \
			-I$(PROJECT_PATH)/include/communication/msgDispatch \
			-I$(PROJECT_PATH)/include/communication/taProtocol \
			-I$(PROJECT_PATH)/include/communication/command \
			-I$(PROJECT_PATH)/include/debug \
			-I$(PROJECT_PATH)/include/debug/db \
			-I$(PROJECT_PATH)/include/debug/mm \
			-I$(PROJECT_PATH)/include/debug/db/arch/$(CONFIG_ARCH) \
			-I$(PROJECT_PATH)/include/debug/db/arch/ \
			-I$(PROJECT_PATH)/include/debug/sda/ \
			-I$(PROJECT_PATH)/include/debug/task/ \
			-I$(PROJECT_PATH)/include/debug/rtp/ \
			-I$(PROJECT_PATH)/include/debug/sda/arch/ \
			-I$(PROJECT_PATH)/include/debug/sda/arch/$(CONFIG_ARCH) \
			-I$(PROJECT_PATH)/include/debug/sda \
			-I$(RTOS_PATH)/include/vxworks   \
			-I$(RTOS_PATH)/include/vxworks/private   \
			-I$(RTOS_PATH)/include/DeltaCore/arch/i86/private   \
			-I$(RTOS_PATH)/include/vxworks/types   \
			-I$(RTOS_PATH)/include/usr/h/ \
			-I$(RTOS_PATH)/include/usr/h/private \
			-I$(RTOS_PATH)/include/DeltaCore \
			-I$(RTOS_PATH)/include/DeltaCore/private \
			-I$(RTOS_PATH)/include/DeltaCore/arch/$(CONFIG_ARCH) \

#库编译时的大小端
ifeq (${CONFIG_CPU_ENDIAN},little)
	LIBNAMES = lib_little
	OTHER_OPTION += -D_LITTLE_ENDIAN_
else
	LIBNAMES = lib_big
	OTHER_OPTION += -D_BIG_ENDIAN_
endif

#软硬浮点规则
ifeq ($(CONFIG_CPU_FLOAT),soft)
	OTHER_OPTION += -msoft-float 
endif

##体系结构规则
#mips规则
ifeq ($(CONFIG_ARCH), mips)
	ifeq ($(CPU_BIT), 64)
		OTHER_OPTION += -DMIPS64_Debug
	endif
endif

#x86规则 
ifeq ($(CONFIG_ARCH),x86)
ifeq ($(CONFIG_CPU_FLOAT),soft)
	OTHER_OPTION += -mno-fp-ret-in-387
endif
endif

#
VPATH = ${SRC_PATHS}

#库名字

L_NAME = libsysDebug.a
KDA_L_NAME = libkda.a

#编译参数
ifeq ($(CONFIG_ARCH), mips)
	SDA_CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION) -DCONFIG_DELTAOS -D__MIPS__
	KDA_CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION) -DCONFIG_DELTAOS -D_KERNEL_DEBUG_ -D__MIPS__
endif

ifeq ($(CONFIG_ARCH), x86)
	SDA_CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION) -D_TA_CONFIG_RTP -DCONFIG_DELTAOS -DTA_SINGLE_STEP -D__X86__
	KDA_CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION) -DCONFIG_DELTAOS -D_KERNEL_DEBUG_ -D__X86__
endif

L_PATH = .
L_OBJS = $(SRC_OBJS)

DA_FLOAT =
ifeq (${ARCH},ppc)
 ifeq (${CPU_FLOAT},soft)
     DA_FLOAT := -DDA_FLOAT_SOFT 
  else
     DA_FLOAT := -DDA_FLOAT_HARD 
  endif
endif

export VPATH
export L_NAME
export L_PATH
export L_OBJS

LIB_PATH=$(RTOS_PATH)/$(VSB_LIB_NAME)/$(TARGET_ADAPT_LIB)/$(ARCH)/${CONFIG_SUB_ARCH}/${CPU_ENDIAN}

all : $(LIBNAMES) 

lib_little:
	${MAKE} "CFLAGS :=$(SDA_CFLAGS) " "L_OBJS=$(SRC_OBJS)" "L_NAME :=$(L_NAME)" \
	"VPATH = $(SRC_PATHS)" $(L_NAME) -f $(RTOS_PATH)/Rules.make
	$(BIN_PATH)/cp -f $(L_NAME) $(LIB_PATH)
	@$(BIN_PATH)/rm -f *.o *.a
	${MAKE} "CFLAGS :=$(KDA_CFLAGS) " "L_OBJS=$(KDA_SRC_OBJS)" "L_NAME :=$(KDA_L_NAME)" \
	"VPATH = $(KDA_SRC_PATHS)" $(KDA_L_NAME) -f $(RTOS_PATH)/Rules.make
	$(BIN_PATH)/cp -f $(KDA_L_NAME) $(LIB_PATH)
	@$(BIN_PATH)/rm -f *.o *.a
lib_big:
	${MAKE} "CFLAGS :=$(CFLAGS)" "L_OBJS=$(SRC_OBJS)" "L_NAME :=$(L_NAME)" \
	"VPATH = $(SRC_PATHS)" $(L_NAME) -f $(RTOS_PATH)/Rules.make
	$(BIN_PATH)/cp -f $(L_NAME) $(LIB_PATH)
	@$(BIN_PATH)/rm -f *.o *.a
	${MAKE} "CFLAGS :=$(CFLAGS) " "L_OBJS=$(KDA_SRC_OBJS)" "L_NAME :=$(KDA_L_NAME)" \
	"VPATH = $(KDA_SRC_PATHS)" $(KDA_L_NAME) -f $(RTOS_PATH)/Rules.make
	$(BIN_PATH)/cp -f $(KDA_L_NAME) $(LIB_PATH)
	@$(BIN_PATH)/rm -f *.o *.a
clean:
	@$(BIN_PATH)/rm -f *.o *.a
	@$(BIN_PATH)/rm -f config_*.*
