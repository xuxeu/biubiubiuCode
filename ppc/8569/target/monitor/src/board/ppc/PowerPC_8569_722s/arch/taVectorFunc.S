#include <dbAsm.h>
#include "dbVector.h"

#define HIADJ(arg)	arg@ha
#define HI(arg)		arg@h
#define LO(arg)		arg@l

/*瀹氫箟瀵勫瓨鍣ㄥ湪DA涓婁笅鏂囩紦瀛樹腑鐨勫亸绉*/
#define TA_REG_PCR_OFFSET  ((32+32*2)*4)   /*閫氱敤瀵勫瓨鍣�double娴偣瀵勫瓨鍣�*/

#define TA_REG_MSR_OFFSET  (TA_REG_PCR_OFFSET+4)
#define TA_REG_CR_OFFSET   (TA_REG_MSR_OFFSET+4)
#define TA_REG_LR_OFFSET   (TA_REG_CR_OFFSET+4)
#define TA_REG_CTR_OFFSET  (TA_REG_LR_OFFSET+4)
#define TA_REG_XER_OFFSET  (TA_REG_CTR_OFFSET+4)
#define TA_REG_FPSCR_OFFSET (TA_REG_XER_OFFSET+4)
//#define TA_REG_PDAR_OFFSET (TA_REG_FPSCR_OFFSET+4)
//#define TA_REG_PDSISR_OFFSET (TA_REG_PDAR_OFFSET+4)
#define TA_REG_VECTOR_OFFSET (TA_REG_PDSISR_OFFSET+4)


#define TA_REG_GPR_OFFEST(i) (0+i*4)
//#define TA_REG_FGPR_OFFSET(i) (32*4+i*8)

#define EXCEPTION_FRAME_SIZE  (0xB0)

/*ʹ�ô˵�ַ����MSLӳ���OSӳ����Ҫʹ�õ��쳣��ʱջ�ռ�*/
#define EXP_TMP_STACK_BASE (0xBA0)

/* �쳣ʱ����������� */
.macro GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 

    /* ʹ��SPRG1-2����������б���ʹ�õ��ļĴ��� */
    mtspr   SPRG1, r3 
    mtspr   SPRG2, r5 

    /*��ȡ��MMUǰʹ�õ���ʱ�쳣ջ*/
    lwi     r3,EXP_TMP_STACK_BASE
    lwz     r3, 0(r3)

    mfctr   r5
    stw     r5,CTR_OFFSET(r3)

    mflr    r5
    stw     r5,LR_OFFSET(r3)

    lwi  r5,__exception_save_basic_register  
    mtctr r5
    bctrl
    
.endm


.macro GENERAL_EXCEPTION_JMP2ASM_HANDLER vector

    /*�����쳣��*/
    li r3,\vector
    stw r3,EXCEPTION_VECTOR_OFFSET(r1)
    
    //b mslRawGeneralExpAsmHandler
    b taRawGeneralExpAsmHandler
.endm

__exception_save_basic_register:
    
    /* ��SRR0��SRR1��CR��DAR��DSISR���浽��ʱջ*/
    mfsrr0  r5 
    stw     r5,SRR0_OFFSET(r3)

    mfsrr1  r5 
    stw     r5,SRR1_OFFSET(r3)

    mfcr    r5 /*����cr,���²�����Ӱ�쵽cr*/
    stw     r5,CR_OFFSET(r3)

    mfspr   r5,DAR
    stw     r5,DAR_OFFSET(r3)

    mfspr   r5,DSISR
    stw     r5,DSISR_OFFSET(r3)

    /* ���MSR�Ĵ����е�[PR]λ���жϲ����жϵĵط��Ƿ����û�̬�����û�̬�����ϵͳջ���л�����ǰ�����ϵͳջ��SPRG0�� */
    mfsrr1  r3 /*�õ������쳣��MSR*/
    andi.   r3,r3,0x4000 /*�ж��Ƿ����û�̬������쳣(PR)*/
    beq 1f
    
    /* �û�̬*/
    mr r5,r1
    mfspr r1, SPRG0 /*�û�̬�²����쳣����Ҫ�Ѳ����쳣�ķ����ջ�л���ϵͳջ������ϵͳ�ѵ�ǰ�����ϵͳջ������SPRG0��,������Ҫ�ֶ��л��¡�*/
    b 4f

1:  /*ϵͳ̬*/
    
     
    li	r5, 0
    addis	r3, 0, exceptionTwiceFlag@ha
    stw	r5, exceptionTwiceFlag@l(r3)
    
    lis r5, taExceptionStackTop@ha
    lwz r5, taExceptionStackTop@l(r5) /* ��ȡta�쳣ջ����ַ*/
    cmpw r1, r5 /* �жϵ�ǰջ��ַ�Ƿ����MSL�쳣ջ */
    bgt 2f
    lis r5, taExceptionStackBottom@ha
    lwz r5, taExceptionStackBottom@l(r5) /* ��ȡta�쳣ջ�׵�ַ*/
    cmpw r1, r5 /* �жϵ�ǰջ�Ƿ��Ƿ����MSL�쳣ջ  */
    blt 2f

    /* �쳣�����Ѿ���ta�쳣ջ��*/
    /* ��ʶ��ta�쳣ջ����ʱ�����˶����쳣*/
    li	r5, 1
    stw	r5, exceptionTwiceFlag@l(r3)

    /* �Ƿ����ж�ta �쳣ջ�����㹻��ʣ��ռ�*/
    lis r5, taExceptionStackBottom@ha
    lwz r5, taExceptionStackBottom@l(r5) /* ��ȡta�쳣ջ�׵�ַ*/
    addi r3,r5,MINIMUM_EXCHANDLE_STACK_SIZE+EXCEPTION_FRAME_SIZE
    cmpw r1, r3 /* �жϵ�ǰta�쳣ջ�Ƿ����㹻�ռ䱣���쳣*/
    bge 3f

2:
    /* 
      * ��ǰta �쳣ջ��û���㹻ʣ��ռ����
      * �������̬ʱ������쳣����ֱ�ӽ�ջ
      * ����Ϊta�쳣ջ��
      */
    mr r5, r1 /*���ڶ�ջ����ͳһ�ı��淽�� */
    lis r3, taExceptionStackTop@ha
    lwz r3, taExceptionStackTop@l(r3) /* ֱ�ӽ�ջ����Ϊta�쳣ջ��*/
    mr r1, r3
    b 4f

3:
    mr r5, r1

4:
    /*���жϲ���*/
    mfsrr1 r3   
    andi. r3,r3,MSR_BIT_IR_DR_RI   /*���ﱣ֤��Ӱ��mmu�Ŀ���*/
    ori r3,r3,MSR_BIT_FP     /*������ж� [EE=0],[PR=0],[FP=1],[ME=0],[FE0-FE1=00] */
    mtmsr r3
    sync
    isync

    /* Ϊ�쳣����������Ԥ���ռ� */
    subi r1,r1,EXCEPTION_FRAME_SIZE

    stw r4,GPR4_OFFSET(r1)
    
    /*����SRR0��SRR1��CR��CTR��LR��DAR��DSISR��R1 */
    lwi     r3,EXP_TMP_STACK_BASE
    lwz     r3, 0(r3)

    lwz     r4, SRR0_OFFSET(r3)
    stw     r4,SRR0_OFFSET(r1)

    lwz     r4, SRR1_OFFSET(r3)
    stw     r4,SRR1_OFFSET(r1)

    lwz     r4, CR_OFFSET(r3)
    stw     r4,CR_OFFSET(r1)

    lwz     r4, CTR_OFFSET(r3)
    stw     r4,CTR_OFFSET(r1)   

    lwz     r4, LR_OFFSET(r3)
    stw     r4,LR_OFFSET(r1)
        
    lwz     r4, DAR_OFFSET(r3)
    stw     r4,DAR_OFFSET(r1)
        
    lwz     r4, DSISR_OFFSET(r3)
    stw     r4,DSISR_OFFSET(r1)

    stw     r5,GPR1_OFFSET(r1)

    /*�ָ�r3��r5*/
    mfspr r3,SPRG1
    mfspr r5,SPRG2
    
    /*����r3,r5*/
    stw r3,GPR3_OFFSET(r1)
    stw r5,GPR5_OFFSET(r1)
    blr

	
.extern taExcHandleDespatch
.extern __reset
.extern __reset_700

.section ".vectors1","ax" 
.text
.global _0x700Handle_start
_0x700Handle_start:	
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS
    /*��ȡ�쳣:floating-point enabled��illegal instruction��privileged instruction��trap*/
    lwz    r3,SRR1_OFFSET(r1)
    
    /*����16λ*/
    rlwinm  r3,r3,16,16,31

    lis	r4, HI(taAsmExceptionHandle)
	ori	r4, r4, LO(taAsmExceptionHandle)
	mtlr	r4
	blrl
	nop
    nop

.global _0x700Handle_end
_0x700Handle_end:

#if 0
.global _0xD00Handle_start
_0xD00Handle_start:
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_DB_INT
.global _0xD00Handle_end
_0xD00Handle_end:
#endif

/*
 * @brief:
 *    鍒锋柊鎸囦护Cache銆�
 * @param[in]: startAddr: 璧峰鍦板潃
 * @param[in]: cnt: Cache Line 鏁�
 * @param[in]: lineSize: Cache Line 澶у皬
 * @return:
 *    鏃�
 * @tracedREQ: RKB.1.2
 * @implements: DKB.1.2.44.MPC755CA
 */
/*T_VOID kbspRawCacheInstUpdate(T_VOID* startAddr,T_UWORD cnt,T_UWORD lineSize);*/
.text
.align  4
.global kbspRawCacheInstUpdate
kbspRawCacheInstUpdate:
1:

#if 0
	addic.  r4,r4,-1
	dcbst   0,r3
	sync
	icbi    0,r3
	isync
	add     r3,r3,r5
	bgt     1b
	blr
#endif

#if 1
	mtspr   1010, r6          /* Invalidate the Data cache        */
    li      r6, 0x0000
    msync
    isync

	mtspr   1011, r6          /* Invalidate the Instruction cache */
    isync
    li      r6, 0x00000000
    msync
    isync
    blr
#endif
	
.global taAsmExceptionHandle
taAsmExceptionHandle:

    andi. r4,r3,0x0002
    cmpwi   r4,0x0002
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_TRAP_INT

.global taRawGeneralExpAsmHandler
taRawGeneralExpAsmHandler:
    /*保存r0,r2,r6-r31*/
    stw r0,GPR0_OFFSET(r1)
    stw r2,GPR2_OFFSET(r1)

    stw r6,GPR6_OFFSET(r1)
    stw r7,GPR7_OFFSET(r1)
    stw r8,GPR8_OFFSET(r1)
    stw r9,GPR9_OFFSET(r1)
    stw r10,GPR10_OFFSET(r1)
    stw r11,GPR11_OFFSET(r1)
    stw r12,GPR12_OFFSET(r1)
    stw r13,GPR13_OFFSET(r1)
    stw r14,GPR14_OFFSET(r1)
    stw r15,GPR15_OFFSET(r1)
    stw r16,GPR16_OFFSET(r1)
    stw r17,GPR17_OFFSET(r1)
    stw r18,GPR18_OFFSET(r1)
    stw r19,GPR19_OFFSET(r1)
    stw r20,GPR20_OFFSET(r1)
    stw r21,GPR21_OFFSET(r1)
    stw r22,GPR22_OFFSET(r1)
    stw r23,GPR23_OFFSET(r1)
    stw r24,GPR24_OFFSET(r1)
    stw r25,GPR25_OFFSET(r1)
    stw r26,GPR26_OFFSET(r1)
    stw r27,GPR27_OFFSET(r1)
    stw r28,GPR28_OFFSET(r1)
    stw r29,GPR29_OFFSET(r1)
    stw r30,GPR30_OFFSET(r1)
    stw r31,GPR31_OFFSET(r1)
    
    /*保存xer*/
    mfxer   r5
    stw r5,XER_OFFSET(r1)
    
    /*�ָ��쳣������*/
    lwz r3,EXCEPTION_VECTOR_OFFSET(r1)    
    
    /* ����쳣���������ö�Ӧ���쳣������� */
    addis    r6, 0, _Raw_Vector_table@ha
    rotlwi  r5, r3,2
    add     r6,r6,r5
    lwz    r7, _Raw_Vector_table@l(r6)
    mtctr  r7
    
    mr r3, r1
    lis r4, exceptionTwiceFlag@ha
    lwz r4, exceptionTwiceFlag@l(r4)
    /*����û��r1 ��8���?Ҫ��������ȫ���Ի������д��ͨ����c������û��Զ���lr���浽r1 ��8��λ�ã��Լ�д�Ļ��һ�㲻�ᱣ��lr��r1 ��8*/
    bctrl

    /* �ָ������Ĳ����� */
    lwz r3,CTR_OFFSET(r1)
    lwz r4,LR_OFFSET(r1)
    lwz r5,XER_OFFSET(r1)

    mtctr   r3
    mtlr    r4
    mtxer   r5

    lwz r0,GPR0_OFFSET(r1)
    lwz r2,GPR2_OFFSET(r1)
    lwz r5,GPR5_OFFSET(r1)
    lwz r6,GPR6_OFFSET(r1)
    lwz r7,GPR7_OFFSET(r1)
    lwz r8,GPR8_OFFSET(r1)
    lwz r9,GPR9_OFFSET(r1)
    lwz r10,GPR10_OFFSET(r1)
    lwz r11,GPR11_OFFSET(r1)
    lwz r12,GPR12_OFFSET(r1)
    lwz r13,GPR13_OFFSET(r1)
    lwz r14,GPR14_OFFSET(r1)
    lwz r15,GPR15_OFFSET(r1)
    lwz r16,GPR16_OFFSET(r1)
    lwz r17,GPR17_OFFSET(r1)
    lwz r18,GPR18_OFFSET(r1)
    lwz r19,GPR19_OFFSET(r1)
    lwz r20,GPR20_OFFSET(r1)
    lwz r21,GPR21_OFFSET(r1)
    lwz r22,GPR22_OFFSET(r1)
    lwz r23,GPR23_OFFSET(r1)
    lwz r24,GPR24_OFFSET(r1)
    lwz r25,GPR25_OFFSET(r1)
    lwz r26,GPR26_OFFSET(r1)
    lwz r27,GPR27_OFFSET(r1)
    lwz r28,GPR28_OFFSET(r1)
    lwz r29,GPR29_OFFSET(r1)
    lwz r30,GPR30_OFFSET(r1)
    lwz r31,GPR31_OFFSET(r1)

    /* ����Ĳ�����Ҫ���жϽ��� */
    mfmsr r3   
    andi. r3,r3,MSR_BIT_IR_DR_RI   /*���ﱣ֤��Ӱ��mmu�Ŀ���*/
    ori r3,r3,MSR_BIT_FP     /*������ж� [EE=0],[PR=0],[FP=1],[ME=0],[FE0-FE1=00] */
    mtmsr r3
    sync
    isync

    lwz r3,CR_OFFSET(r1) 
    mtcr    r3

    lwz     r3, GPR3_OFFSET(r1)
    mtspr   SPRG1, r3

    lwz     r4, GPR4_OFFSET(r1)
    mtspr   SPRG2, r4
    
    /*��ȡrfi����ʱ����srr0,srr1ʹ�õ�ջ*/
    lwi      r3,EXP_TMP_STACK_BASE
    lwz     r3, 0(r3)
    
    lwz     r4, SRR0_OFFSET(r1)
    stw     r4,SRR0_OFFSET(r3)

    lwz     r4, SRR1_OFFSET(r1)
    stw     r4,SRR1_OFFSET(r3)

    lwz     r1, GPR1_OFFSET(r1)

    /*������srr0,srr1ǰ����Ҫ���жϡ���MMU���� */
    li      r4,MSR_BIT_RI
    mtmsr   r4
    sync
    isync	
        
    lwz     r4, SRR0_OFFSET(r3)
    mtsrr0  r4

    lwz     r4, SRR1_OFFSET(r3)
    mtsrr1  r4    
    
    mfspr   r3,SPRG1
    mfspr   r4,SPRG2

    sync
    isync
    rfi
	
	