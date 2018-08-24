include $(PROJECT_PATH)/$(CONFIG_NAME)/toolchain.mk
include  $(RTOS_PATH)/base.make

#源文件路径及源文件
SRC_PATHS = $(PROJECT_PATH)/src \

SRC_FILES = $(notdir ${wildcard $(patsubst %, %/*.c, $(SRC_PATHS))}  \
            ${wildcard $(patsubst %, %/*.S, $(SRC_PATHS))} )
            
SRC_OBJS = ${patsubst %.c,%.o,${patsubst %.S,%.o,${SRC_FILES}}} 

#头文件路径
INCLUDE_PATHS :=-I$(PROJECT_PATH)/include \
			-I$(RTOS_PATH)/include/debug \
			-I$(RTOS_PATH)/include/vxworks \
			-I$(RTOS_PATH)/include/vxworks/types \
			-I$(RTOS_PATH)/include/vxworks/wrn/coreip \
			-I$(RTOS_PATH)/include/vxworks/wdb \
			-I$(RTOS_PATH)/include/vxworks/private   \
			-I$(RTOS_PATH)/include/vxworks/drv/wdb \
			-I$(RTOS_PATH)/include/usr/h/ \
			-I$(RTOS_PATH)/include/usr/h/private \
			-I$(RTOS_PATH)/include/DeltaCore \


#库编译时的大小端
ifeq (${CONFIG_CPU_ENDIAN},little)
	LIBNAMES = lib_little 
else
	LIBNAMES = lib_big
endif

#软硬浮点规则
ifeq ($(CONFIG_CPU_FLOAT),soft)
	OTHER_OPTION += -msoft-float 
endif

##体系结构规则
#mips规则
ifeq ($(CONFIG_ARCH), mips)
	ifeq ($(CONFIG_SUB_ARCH),loongson2f)
		ifeq ($(CONFIG_CPU_BIT), 64)
			OTHER_OPTION += -EL -D_LITTLE_ENDIAN_ -mips3 -D__MIPS__ -DMIPSEL -DCPU=MIPS64 -DTOOL_FAMILY=gnu -DTOOL=gnule 
		endif
		ifeq ($(CONFIG_CPU_BIT), 32)
			OTHER_OPTION += -EL -D_LITTLE_ENDIAN_  -mips2 -D__MIPS__ -DMIPSEL -DCPU=MIPS32 -DTOOL_FAMILY=gnu -DTOOL=gnule 
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

L_NAME = libwdb.a

#编译参数
ifeq ($(CONFIG_ARCH), mips)
	CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION) -D_MIPS_
endif

ifeq ($(CONFIG_ARCH), x86)
	CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION) -D_X86_
endif

ifeq ($(CONFIG_ARCH), arm)
	CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION) -DARMEL -D_WRS_CONFIG_SMP  -D__arm__ 
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

LIB_PATH=$(RTOS_PATH)/lib_smp/$(TARGET_ADAPT_LIB)/$(ARCH)/${CONFIG_FAMILY_ARCH}/${CPU_ENDIAN}


all : $(LIBNAMES) 

lib_little:
	${MAKE} "CFLAGS :=$(CFLAGS) " "L_OBJS=$(SRC_OBJS)" "L_NAME :=$(L_NAME)" \
	"VPATH = $(SRC_PATHS)" $(L_NAME) -f $(RTOS_PATH)/Rules.make
	$(BIN_PATH)/cp -f $(L_NAME) $(LIB_PATH)
	@$(BIN_PATH)/rm -f *.o *.a
lib_big:
	${MAKE} "CFLAGS :=$(CFLAGS)" "L_OBJS=$(SRC_OBJS)" "L_NAME :=$(L_NAME)" \
	"VPATH = $(SRC_PATHS)" $(L_NAME) -f $(RTOS_PATH)/Rules.make
	$(BIN_PATH)/cp -f $(L_NAME) $(LIB_PATH)
	@$(BIN_PATH)/rm -f *.o *.a

clean:
	@$(BIN_PATH)/rm -f *.o *.a
	@$(BIN_PATH)/rm -f config_*.*
	#@$(BIN_PATH)/rm -f *makefile*
