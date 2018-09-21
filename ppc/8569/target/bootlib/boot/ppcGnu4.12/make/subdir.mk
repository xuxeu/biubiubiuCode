################################################################################
# �Զ��������ļ�. ��Ҫ�༭!
################################################################################

#  ��������ʱ�ӹ��ߵ����������������
C_SRCS += \
$(PROJECT_PATH)/bootApp.c \
$(PROJECT_PATH)/bootAppShell.c \
$(PROJECT_PATH)/bootEthMacHandler.c \
$(PROJECT_PATH)/bootFsLoad.c \
$(PROJECT_PATH)/bootMemCmds.c \
$(PROJECT_PATH)/bootNetLoad.c \
$(PROJECT_PATH)/bootTftpLoad.c 

OBJS += \
./bootApp.o \
./bootAppShell.o \
./bootEthMacHandler.o \
./bootFsLoad.o \
./bootMemCmds.o \
./bootNetLoad.o \
./bootTftpLoad.o 

DEPS += \
./bootApp.d \
./bootAppShell.d \
./bootEthMacHandler.d \
./bootFsLoad.d \
./bootMemCmds.d \
./bootNetLoad.d \
./bootTftpLoad.d 



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
# ÿ����Ŀ¼�����ṩ����������ڹ�����Դ
%.o: $(PROJECT_PATH)/%.c
	@$(BIN_PATH)/echo '���ڹ����ļ� $<'
	@$(BIN_PATH)/echo '����: GCC C ������'
	$(CC) -DCPU=PPC85XX -IC:/LambdaPRO1/lib/boot -iquote -IC:/LambdaPRO1/lib/boot -I./ -I../ -IC:/LambdaPRO/target/deltaos/include/include-boot -IC:/LambdaPRO/target/deltaos/config/include -IC:/LambdaPRO/target/deltaos/include/include-boot/net -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/private -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO1/lib/boot/src -IC:/LambdaPRO/target/deltaos/include/include-boot/arch/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/wrn/coreip -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/hwif -IC:/LambdaPRO/target/deltaos/comps/src/src-boot -IC:/LambdaPRO/target/deltaos/src/src-boot -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO/target/deltaos/include/include-boot/debug/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/ugl -IC:/LambdaPRO/target/deltaos/include/include-boot/rtl -IC:/LambdaPRO/target/deltaos/include/include-boot/DeltaCore ${OTHER_OPTION} -D${ARCH} ${CFLAGS}  -O0 -g -Wall -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp -o$@ $< && \
	$(BIN_PATH)/echo -n $(@:%.o=%.d) $(dir $@) > $(@:%.o=%.d) && \
	$(CC) -DCPU=PPC85XX -IC:/LambdaPRO1/lib/boot -iquote -IC:/LambdaPRO1/lib/boot -I./ -I../ -IC:/LambdaPRO/target/deltaos/include/include-boot -IC:/LambdaPRO/target/deltaos/config/include -IC:/LambdaPRO/target/deltaos/include/include-boot/net -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/private -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO1/lib/boot/src -IC:/LambdaPRO/target/deltaos/include/include-boot/arch/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/wrn/coreip -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/hwif -IC:/LambdaPRO/target/deltaos/comps/src/src-boot -IC:/LambdaPRO/target/deltaos/src/src-boot -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO/target/deltaos/include/include-boot/debug/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/ugl -IC:/LambdaPRO/target/deltaos/include/include-boot/rtl -IC:/LambdaPRO/target/deltaos/include/include-boot/DeltaCore ${OTHER_OPTION} -D${ARCH} ${CFLAGS}  -O0 -g -Wall -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp -MM -MG -P -w  $< >> $(@:%.o=%.d)
	@$(BIN_PATH)/echo '�Ѿ���ɹ���: $<'
	@$(BIN_PATH)/echo ' '


COMPILE_COMMAND := $(CC) -DCPU=PPC85XX -IC:/LambdaPRO1/lib/boot -I./ -I../ -IC:/LambdaPRO/target/deltaos/include/include-boot -IC:/LambdaPRO/target/deltaos/config/include -IC:/LambdaPRO/target/deltaos/include/include-boot/net -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/private -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO1/lib/boot/src -IC:/LambdaPRO/target/deltaos/include/include-boot/arch/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/wrn/coreip -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/hwif -IC:/LambdaPRO/target/deltaos/comps/src/src-boot -IC:/LambdaPRO/target/deltaos/src/src-boot -IC:/LambdaPRO/target/deltaos/include/include-boot/debug -IC:/LambdaPRO/target/deltaos/include/include-boot/debug/ppc -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks -IC:/LambdaPRO/target/deltaos/include/include-boot/vxworks/ugl -IC:/LambdaPRO/target/deltaos/include/include-boot/rtl -IC:/LambdaPRO/target/deltaos/include/include-boot/DeltaCore ${OTHER_OPTION} -D${ARCH} ${CFLAGS}  -O0 -g -Wall -c -mfloat-gprs=double -mspe=yes -mabi=spe -fno-implicit-fp -o