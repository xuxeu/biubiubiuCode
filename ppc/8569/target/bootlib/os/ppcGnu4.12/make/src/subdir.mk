################################################################################
# 自动的生成文件. 不要编辑!
################################################################################

#  构建变量时从工具调用里增加输入输出
O_SRCS += \
$(PROJECT_PATH)/src/bootLib.o \
$(PROJECT_PATH)/src/cksumLib.o 

C_SRCS += \
$(PROJECT_PATH)/src/avlLib.c \
$(PROJECT_PATH)/src/avlUint64Lib.c \
$(PROJECT_PATH)/src/avlUintLib.c \
$(PROJECT_PATH)/src/bLib.c \
$(PROJECT_PATH)/src/bmpLib.c \
$(PROJECT_PATH)/src/bootLib.c \
$(PROJECT_PATH)/src/bootParseLib.c \
$(PROJECT_PATH)/src/cksumLib.c \
$(PROJECT_PATH)/src/dllCreateLib.c \
$(PROJECT_PATH)/src/dllLib.c \
$(PROJECT_PATH)/src/getopt.c \
$(PROJECT_PATH)/src/hookLib.c \
$(PROJECT_PATH)/src/hookShow.c \
$(PROJECT_PATH)/src/inflateLib.c \
$(PROJECT_PATH)/src/lstLib.c \
$(PROJECT_PATH)/src/memmem.c \
$(PROJECT_PATH)/src/poolLib.c \
$(PROJECT_PATH)/src/poolShow.c \
$(PROJECT_PATH)/src/procNumSetFuncBind.c \
$(PROJECT_PATH)/src/qFifoLib.c \
$(PROJECT_PATH)/src/qPriBMapLib.c \
$(PROJECT_PATH)/src/qPriDeltaLib.c \
$(PROJECT_PATH)/src/qPriListLib.c \
$(PROJECT_PATH)/src/qPxPriBMapLib.c \
$(PROJECT_PATH)/src/qPxSSPriBMapLib.c \
$(PROJECT_PATH)/src/rBuffLib.c \
$(PROJECT_PATH)/src/rBuffShow.c \
$(PROJECT_PATH)/src/rngLib.c \
$(PROJECT_PATH)/src/sllCreateLib.c \
$(PROJECT_PATH)/src/sllLib.c \
$(PROJECT_PATH)/src/smUtilLib.c \
$(PROJECT_PATH)/src/strSearchLib.c \
$(PROJECT_PATH)/src/symUtilLib.c \
$(PROJECT_PATH)/src/uncompress.c 

OBJS += \
./src/avlLib.o \
./src/avlUint64Lib.o \
./src/avlUintLib.o \
./src/bLib.o \
./src/bmpLib.o \
./src/bootLib.o \
./src/bootParseLib.o \
./src/cksumLib.o \
./src/dllCreateLib.o \
./src/dllLib.o \
./src/getopt.o \
./src/hookLib.o \
./src/hookShow.o \
./src/inflateLib.o \
./src/lstLib.o \
./src/memmem.o \
./src/poolLib.o \
./src/poolShow.o \
./src/procNumSetFuncBind.o \
./src/qFifoLib.o \
./src/qPriBMapLib.o \
./src/qPriDeltaLib.o \
./src/qPriListLib.o \
./src/qPxPriBMapLib.o \
./src/qPxSSPriBMapLib.o \
./src/rBuffLib.o \
./src/rBuffShow.o \
./src/rngLib.o \
./src/sllCreateLib.o \
./src/sllLib.o \
./src/smUtilLib.o \
./src/strSearchLib.o \
./src/symUtilLib.o \
./src/uncompress.o 

DEPS += \
./src/avlLib.d \
./src/avlUint64Lib.d \
./src/avlUintLib.d \
./src/bLib.d \
./src/bmpLib.d \
./src/bootLib.d \
./src/bootParseLib.d \
./src/cksumLib.d \
./src/dllCreateLib.d \
./src/dllLib.d \
./src/getopt.d \
./src/hookLib.d \
./src/hookShow.d \
./src/inflateLib.d \
./src/lstLib.d \
./src/memmem.d \
./src/poolLib.d \
./src/poolShow.d \
./src/procNumSetFuncBind.d \
./src/qFifoLib.d \
./src/qPriBMapLib.d \
./src/qPriDeltaLib.d \
./src/qPriListLib.d \
./src/qPxPriBMapLib.d \
./src/qPxSSPriBMapLib.d \
./src/rBuffLib.d \
./src/rBuffShow.d \
./src/rngLib.d \
./src/sllCreateLib.d \
./src/sllLib.d \
./src/smUtilLib.d \
./src/strSearchLib.d \
./src/symUtilLib.d \
./src/uncompress.d 



OTHER_OPTION :=
ifeq ($(CONFIG_CPU_FLOAT),soft)
	OTHER_OPTION += -msoft-float 
endif
ifeq ($(CONFIG_ARCH),x86)
ifeq ($(CONFIG_CPU_FLOAT),soft)
	OTHER_OPTION += -mno-fp-ret-in-387 
endif
endif
ARCH =__ppc__
# 每个子目录必须提供规则给它正在构建资源
src/%.o: $(PROJECT_PATH)/src/%.c
	@$(BIN_PATH)/echo '正在构建文件 $<'
	@$(BIN_PATH)/echo '调用: GCC C 编译器'
	$(CC) -DCPU=PPC85XX -IC:/LambdaPRO1/lib/os/src -iquote -IC:/LambdaPRO1/lib/os -I./ -I../ -IC:/LambdaPRO/target/deltaos/include/include-boot -IC:/LambdaPRO/target/deltaos/config/include -IC:/LambdaPRO/target/deltaos/include/include-boot/net -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/private -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO1/lib/os/src -IC:/LambdaPRO/target/deltaos/include/include-boot/arch/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/wrn/coreip -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/hwif -IC:/LambdaPRO/target/deltaos/comps/src/src-boot -IC:/LambdaPRO/target/deltaos/src/src-boot -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO/target/deltaos/include/include-boot/debug/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/ugl -IC:/LambdaPRO/target/deltaos/include/include-boot/rtl -IC:/LambdaPRO/target/deltaos/include/include-boot/DeltaCore -IC:/LambdaPRO1/lib/os/src ${OTHER_OPTION} -D${ARCH} ${CFLAGS}  -O0 -g -Wall -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp -o$@ $< && \
	$(BIN_PATH)/echo -n $(@:%.o=%.d) $(dir $@) > $(@:%.o=%.d) && \
	$(CC) -DCPU=PPC85XX -IC:/LambdaPRO1/lib/os/src -iquote -IC:/LambdaPRO1/lib/os -I./ -I../ -IC:/LambdaPRO/target/deltaos/include/include-boot -IC:/LambdaPRO/target/deltaos/config/include -IC:/LambdaPRO/target/deltaos/include/include-boot/net -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/private -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO1/lib/os/src -IC:/LambdaPRO/target/deltaos/include/include-boot/arch/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/wrn/coreip -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/hwif -IC:/LambdaPRO/target/deltaos/comps/src/src-boot -IC:/LambdaPRO/target/deltaos/src/src-boot -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO/target/deltaos/include/include-boot/debug/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/ugl -IC:/LambdaPRO/target/deltaos/include/include-boot/rtl -IC:/LambdaPRO/target/deltaos/include/include-boot/DeltaCore -IC:/LambdaPRO1/lib/os/src ${OTHER_OPTION} -D${ARCH} ${CFLAGS}  -O0 -g -Wall -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp -MM -MG -P -w  $< >> $(@:%.o=%.d)
	@$(BIN_PATH)/echo '已经完成构建: $<'
	@$(BIN_PATH)/echo ' '


COMPILE_COMMAND := $(CC) -DCPU=PPC85XX -IC:/LambdaPRO1/lib/os -I./ -I../ -IC:/LambdaPRO/target/deltaos/include/include-boot -IC:/LambdaPRO/target/deltaos/config/include -IC:/LambdaPRO/target/deltaos/include/include-boot/net -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/private -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO1/lib/os/src -IC:/LambdaPRO/target/deltaos/include/include-boot/arch/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/wrn/coreip -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/hwif -IC:/LambdaPRO/target/deltaos/comps/src/src-boot -IC:/LambdaPRO/target/deltaos/src/src-boot -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO/target/deltaos/include/include-boot/debug/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/ugl -IC:/LambdaPRO/target/deltaos/include/include-boot/rtl -IC:/LambdaPRO/target/deltaos/include/include-boot/DeltaCore ${OTHER_OPTION} -D${ARCH} ${CFLAGS}  -O0 -g -Wall -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp -o