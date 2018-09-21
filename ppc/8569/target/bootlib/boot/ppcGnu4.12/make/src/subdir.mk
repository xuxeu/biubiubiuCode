################################################################################
# 自动的生成文件. 不要编辑!
################################################################################

#  构建变量时从工具调用里增加输入输出
C_SRCS += \
$(PROJECT_PATH)/src/bootAddMem.c \
$(PROJECT_PATH)/src/bootApp.c \
$(PROJECT_PATH)/src/bootAppExc.c \
$(PROJECT_PATH)/src/bootAppShell.c \
$(PROJECT_PATH)/src/bootAppTestFuncs.c \
$(PROJECT_PATH)/src/bootDhcpcLoad.c \
$(PROJECT_PATH)/src/bootEdrSupport.c \
$(PROJECT_PATH)/src/bootEndSupport.c \
$(PROJECT_PATH)/src/bootEthAdrSet.c \
$(PROJECT_PATH)/src/bootEthMacHandler.c \
$(PROJECT_PATH)/src/bootFdLoad.c \
$(PROJECT_PATH)/src/bootFsLoad.c \
$(PROJECT_PATH)/src/bootFtpLoad.c \
$(PROJECT_PATH)/src/bootMemCmds.c \
$(PROJECT_PATH)/src/bootNetLoad.c \
$(PROJECT_PATH)/src/bootRshLoad.c \
$(PROJECT_PATH)/src/bootTftpLoad.c \
$(PROJECT_PATH)/src/bootTsfsLoad.c \
$(PROJECT_PATH)/src/bootUsbShow.c 

OBJS += \
./src/bootAddMem.o \
./src/bootApp.o \
./src/bootAppExc.o \
./src/bootAppShell.o \
./src/bootAppTestFuncs.o \
./src/bootDhcpcLoad.o \
./src/bootEdrSupport.o \
./src/bootEndSupport.o \
./src/bootEthAdrSet.o \
./src/bootEthMacHandler.o \
./src/bootFdLoad.o \
./src/bootFsLoad.o \
./src/bootFtpLoad.o \
./src/bootMemCmds.o \
./src/bootNetLoad.o \
./src/bootRshLoad.o \
./src/bootTftpLoad.o \
./src/bootTsfsLoad.o \
./src/bootUsbShow.o 

DEPS += \
./src/bootAddMem.d \
./src/bootApp.d \
./src/bootAppExc.d \
./src/bootAppShell.d \
./src/bootAppTestFuncs.d \
./src/bootDhcpcLoad.d \
./src/bootEdrSupport.d \
./src/bootEndSupport.d \
./src/bootEthAdrSet.d \
./src/bootEthMacHandler.d \
./src/bootFdLoad.d \
./src/bootFsLoad.d \
./src/bootFtpLoad.d \
./src/bootMemCmds.d \
./src/bootNetLoad.d \
./src/bootRshLoad.d \
./src/bootTftpLoad.d \
./src/bootTsfsLoad.d \
./src/bootUsbShow.d 



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
	$(CC) -DCPU=PPC85XX -IC:/LambdaPRO1/lib/boot/src -iquote -IC:/LambdaPRO1/lib/boot -I./ -I../ -IC:/LambdaPRO/target/deltaos/include/include-boot -IC:/LambdaPRO/target/deltaos/config/include -IC:/LambdaPRO/target/deltaos/include/include-boot/net -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/private -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO1/lib/boot/src -IC:/LambdaPRO/target/deltaos/include/include-boot/arch/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/wrn/coreip -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/hwif -IC:/LambdaPRO/target/deltaos/comps/src/src-boot -IC:/LambdaPRO/target/deltaos/src/src-boot -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO/target/deltaos/include/include-boot/debug/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/ugl -IC:/LambdaPRO/target/deltaos/include/include-boot/rtl -IC:/LambdaPRO/target/deltaos/include/include-boot/DeltaCore -IC:/LambdaPRO1/lib/boot/src ${OTHER_OPTION} -D${ARCH} ${CFLAGS}  -O0 -g -Wall -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp -o$@ $< && \
	$(BIN_PATH)/echo -n $(@:%.o=%.d) $(dir $@) > $(@:%.o=%.d) && \
	$(CC) -DCPU=PPC85XX -IC:/LambdaPRO1/lib/boot/src -iquote -IC:/LambdaPRO1/lib/boot -I./ -I../ -IC:/LambdaPRO/target/deltaos/include/include-boot -IC:/LambdaPRO/target/deltaos/config/include -IC:/LambdaPRO/target/deltaos/include/include-boot/net -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/private -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO1/lib/boot/src -IC:/LambdaPRO/target/deltaos/include/include-boot/arch/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/wrn/coreip -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/hwif -IC:/LambdaPRO/target/deltaos/comps/src/src-boot -IC:/LambdaPRO/target/deltaos/src/src-boot -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO/target/deltaos/include/include-boot/debug/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/ugl -IC:/LambdaPRO/target/deltaos/include/include-boot/rtl -IC:/LambdaPRO/target/deltaos/include/include-boot/DeltaCore -IC:/LambdaPRO1/lib/boot/src ${OTHER_OPTION} -D${ARCH} ${CFLAGS}  -O0 -g -Wall -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp -MM -MG -P -w  $< >> $(@:%.o=%.d)
	@$(BIN_PATH)/echo '已经完成构建: $<'
	@$(BIN_PATH)/echo ' '


COMPILE_COMMAND := $(CC) -DCPU=PPC85XX -IC:/LambdaPRO1/lib/boot -I./ -I../ -IC:/LambdaPRO/target/deltaos/include/include-boot -IC:/LambdaPRO/target/deltaos/config/include -IC:/LambdaPRO/target/deltaos/include/include-boot/net -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/private -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO1/lib/boot/src -IC:/LambdaPRO/target/deltaos/include/include-boot/arch/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/wrn/coreip -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/hwif -IC:/LambdaPRO/target/deltaos/comps/src/src-boot -IC:/LambdaPRO/target/deltaos/src/src-boot -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO/target/deltaos/include/include-boot/debug/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/ugl -IC:/LambdaPRO/target/deltaos/include/include-boot/rtl -IC:/LambdaPRO/target/deltaos/include/include-boot/DeltaCore ${OTHER_OPTION} -D${ARCH} ${CFLAGS}  -O0 -g -Wall -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp -o