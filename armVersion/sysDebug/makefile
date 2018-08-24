include  $(RTOS_PATH)/base.make


#源文件路径及源文件
#			$(PROJECT_PATH)/src/debug \
#			$(PROJECT_PATH)/src/debug/rtp \
#			$(PROJECT_PATH)/src/debug/task \
#			$(PROJECT_PATH)/src/debug/sda \
#			$(PROJECT_PATH)/src/debug/db \
#			$(PROJECT_PATH)/src/debug/mm \
#			$(PROJECT_PATH)/src/debug/db/arch/$(CONFIG_ARCH)


SRC_PATHS = $(PROJECT_PATH)/src/debug \
			$(PROJECT_PATH)/src/debug/rtp \
			$(PROJECT_PATH)/src/debug/task \
			$(PROJECT_PATH)/src/debug/sda \
			$(PROJECT_PATH)/src/debug/db \
			$(PROJECT_PATH)/src/debug/mm \
			$(PROJECT_PATH)/src/debug/db/arch/$(CONFIG_ARCH) \
			$(PROJECT_PATH)/src \
			$(PROJECT_PATH)/src/utils \
			$(PROJECT_PATH)/src/communication \
			$(PROJECT_PATH)/src/communication/command \
			$(PROJECT_PATH)/src/communication/taProtocol \
			$(PROJECT_PATH)/src/communication/msgdispatch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch \
			$(PROJECT_PATH)/src/communication/msgdispatch/arch/$(CONFIG_ARCH) 


#			$(PROJECT_PATH)/src/utils \
#			$(PROJECT_PATH)/src/debug \
#			$(PROJECT_PATH)/src/debug/sda \
#			$(PROJECT_PATH)/src/debug/db \
#			$(PROJECT_PATH)/src/debug/mm \
#			$(PROJECT_PATH)/src/debug/db/arch/$(CONFIG_ARCH) \
#			$(PROJECT_PATH)/src/communication \
#			$(PROJECT_PATH)/src/communication/command \
#			$(PROJECT_PATH)/src/communication/taProtocol \
#			$(PROJECT_PATH)/src/communication/msgdispatch \
#			$(PROJECT_PATH)/src/communication/msgdispatch/arch \
#			$(PROJECT_PATH)/src/communication/msgdispatch/arch/$(CONFIG_ARCH)
KDA_SRC_PATHS = $(PROJECT_PATH)/src \

			
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
			-I$(RTOS_PATH)/include/vxworks/private   \
			-I$(RTOS_PATH)/include/vxworks/arch/$(CONFIG_ARCH)   \

#库编译时的大小端
ifeq (${CONFIG_CPU_ENDIAN},little)
	LIBNAMES = lib_little
	OTHER_OPTION += -D_LITTLE_ENDIAN_ -D__LITTLE_ENDIAN__
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
	ifeq ($(CONFIG_SUB_ARCH),loongson2f)
		ifeq ($(CONFIG_ENABLE_LP64), true)
			OTHER_OPTION += -EL -mips3 -D__MIPS__ -DMIPSEL -DCPU=MIPS64 -DTOOL_FAMILY=gnu -DTOOL=gnule 
		endif
		ifeq ($(CONFIG_ENABLE_LP64), false)
			OTHER_OPTION += -EL -mips2 -D__MIPS__ -DMIPSEL -DCPU=MIPS32 -DTOOL_FAMILY=gnu -DTOOL=gnule 
		endif
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

SDA_CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION)
KDA_CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION)

#编译参数
ifeq ($(CONFIG_ENABLE_LP64),true)
SDA_CFLAGS += -DCONFIG_TA_LP64
KDA_CFLAGS += -DCONFIG_TA_LP64
endif

#根据参数CONFIG_ENABLE_SMP定义宏CONFIG_CORE_SMP。
ifeq ($(CONFIG_ENABLE_SMP),true)
SDA_CFLAGS +=  -DMAX_CPU_NUM=4
KDA_CFLAGS +=  -DMAX_CPU_NUM=4
SDA_CFLAGS += -DCONFIG_TA_SMP
KDA_CFLAGS += -DCONFIG_TA_SMP
else
SDA_CFLAGS +=  -DMAX_CPU_NUM=1
KDA_CFLAGS +=  -DMAX_CPU_NUM=1
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


#VSB_LIB_NAME :=lib_smp
LIB_PATH=$(RTOS_PATH)/$(VSB_LIB_NAME)/$(TARGET_ADAPT_LIB)/${CONFIG_ARCH}/$(CONFIG_FAMILY_ARCH)/${CPU_ENDIAN}

all : $(LIBNAMES) 

lib_little:
	@echo "11111111111111111111111"+ $(SDA_CFLAGS)
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
