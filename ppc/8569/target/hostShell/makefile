include $(PROJECT_PATH)/$(CONFIG_NAME)/toolchain.mk
include  $(RTOS_PATH)/base.make

#源文件路径及源文件
SRC_PATHS = $(PROJECT_PATH)/src

SRC_FILES = $(notdir ${wildcard $(patsubst %, %/*.c, $(SRC_PATHS))}  \
            ${wildcard $(patsubst %, %/*.S, $(SRC_PATHS))} )
            
SRC_OBJS = ${patsubst %.c,%.o,${patsubst %.S,%.o,${SRC_FILES}}} 

#头文件路径
INCLUDE_PATHS :=-I$(PROJECT_PATH)/include \
			-I$(RTOS_PATH)/include/debug \
			-I$(RTOS_PATH)/include/debug/$(CONFIG_ARCH) \
			-I$(RTOS_PATH)/include/vxworks   \
			-I$(RTOS_PATH)/include/vxworks/private   \
			-I$(RTOS_PATH)/include/vxworks/types   \
			-I$(RTOS_PATH)/include/usr/h/ \
			-I$(RTOS_PATH)/include/usr/h/private \
			-I$(RTOS_PATH)/include/DeltaCore \
			-I$(RTOS_PATH)/include/vxworks/wrn/coreip
			
#库编译时的大小端
ifeq (${CONFIG_CPU_ENDIAN},little)
	LIBNAMES = lib_little 
endif

ifeq (${CONFIG_CPU_ENDIAN},big)
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
			OTHER_OPTION += -D_LITTLE_ENDIAN_ -EL -mips3 -D__MIPS__ -DMIPSEL -DCPU=MIPS64 -DTOOL_FAMILY=gnu -DTOOL=gnule 
		endif
		ifeq ($(CONFIG_CPU_BIT), 32)
			OTHER_OPTION += -D_LITTLE_ENDIAN_ -EL -mips2 -D__MIPS__ -DMIPSEL -DCPU=MIPS32 -DTOOL_FAMILY=gnu -DTOOL=gnule 
		endif
	endif
endif

#x86规则 
ifeq ($(CONFIG_ARCH),x86)
#	OTHER_OPTION += D_LITTLE_ENDIAN_
	ifeq ($(CONFIG_CPU_FLOAT),soft)
		OTHER_OPTION += -mno-fp-ret-in-387 
	endif
endif

DA_FLOAT =
ifeq (${ARCH},ppc)
 ifeq (${CPU_FLOAT},soft)
     DA_FLOAT := -DDA_FLOAT_SOFT 
  else
     DA_FLOAT := -DDA_FLOAT_HARD 
  endif
endif

ifeq (${ARCH},ppc)
ifeq (${SUB_ARCH},8569)
	CFLAGS += -mcpu=8540  -mabi=no-spe -mspe=no
endif
endif

VPATH = ${SRC_PATHS}

#库名字
ifeq (${CPU_FLOAT},soft)
  L_NAME = libhostShell_soft.a
else
  L_NAME = libhostShell_hard.a
endif

L_NAME = libhostShell.a

#编译参数
CFLAGS := $(CFLAGS) $(DA_FLOAT) $(INCLUDE_PATHS) $(OTHER_OPTION)

L_PATH = .
L_OBJS = $(SRC_OBJS)

export VPATH
export L_NAME
export L_PATH
export L_OBJS

LIB_PATH=$(RTOS_PATH)/lib/$(ARCH)/${CONFIG_SUB_ARCH}/${CPU_ENDIAN}

all : $(LIBNAMES) 

lib_little:
	${MAKE} "L_OBJS=$(SRC_OBJS)" "L_NAME :=$(L_NAME)" \
	"VPATH = $(SRC_PATHS)" $(L_NAME) -f $(RTOS_PATH)/Rules.make
	$(BIN_PATH)/cp -f $(L_NAME) $(LIB_PATH)
	@$(BIN_PATH)/rm -f *.o *.a
lib_big:
	${MAKE} "L_OBJS=$(SRC_OBJS)" "L_NAME :=$(L_NAME)" \
	"VPATH = $(SRC_PATHS)" $(L_NAME) -f $(RTOS_PATH)/Rules.make
	$(BIN_PATH)/cp -f $(L_NAME) $(LIB_PATH)
	@$(BIN_PATH)/rm -f *.o *.a

clean:
	@$(BIN_PATH)/rm -f *.o *.a
	@$(BIN_PATH)/rm -f config_*.*
	@$(BIN_PATH)/rm -f *makefile*