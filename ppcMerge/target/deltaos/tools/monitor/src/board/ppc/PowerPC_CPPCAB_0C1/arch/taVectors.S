/*
* 变更历史：
* 2014-04-13  叶俊    0x300、0x400的缺页异常处理的hash表的更新不清vec位。  
* 2013-01-15  周英豪  在中断异常处理程序中添加浮点上下文的保存与恢复。
* 2012-04-21  邱学强  支持Hash表动态更新，增加0x300、0x400的缺页异常处理的hash表的更新。(BUG6549)
* 2012-04-21  邱学强  增加相关体系结构宏进行代码控制，包括浮点相关。(BUG6195)
* 2010-08-30  尹立孟  创建该文件。
*/

/*
* @file     vector.S
* @brief
*    功能：
*    <li>实现对CPU异常和中断通用处理的相关操作和定义。</li>
*/
#define _ASMLANGUAGE

/* 头文件 */
#include "dbAsm.h"
#include "dbVector.h"

/* 定义部分 */
/* 异常时保存基本上下文 */
.macro GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 

    /* 使用SPRG1-2来保存操作中必须使用到的寄存器 */
    mtspr   SPRG1, r3 
    mtspr   SPRG2, r5 

    /*获取开MMU前使用的临时异常栈*/
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

    /*保存异常号*/
    li r3,\vector
    stw r3,EXCEPTION_VECTOR_OFFSET(r1)
    
    //b mslRawGeneralExpAsmHandler
    b taRawGeneralExpAsmHandler
.endm

/* 中断时保存基本上下文 */
.macro GENERAL_INTERRUPT_SAVE_BASIC_REGISTERS  

    /* 使用SPRG1-3来保存中断操作中必须使用到的寄存器 */
    mtspr   SPRG1, r3
    mtspr   SPRG3, r4
    mtspr   SPRG2, r5

    lwi     r3,EXP_TMP_STACK_BASE
    lwz     r3, 0(r3)
    
    mfsrr0  r5 
    stw     r5,SRR0_OFFSET(r3)

    mfsrr1  r5 
    stw     r5,SRR1_OFFSET(r3)
    

    /* 由于后面的比较指令会影响CR寄存器，因此先将CR寄存器保存到R4中 */
    mfcr r4

    /* 根据MSR寄存器中的[PR]位来判断产生中断的地方是否是用户态，是用户态则进行系统栈的切换，当前VM的系统栈在SPRG0中 */
    mfsrr1  r3 /*得到MSR*/
    andi.   r3,r3,0x4000 /*判定是否是用户态产生的中断*/
    beq 1f

    /* 用户态产生的中断*/
    mr r5,r1
    mfspr r1,SPRG0/*用户态下产生异常，需要把产生异常的分区的栈切换到系统栈，操作系统把当前分区的系统栈定义在sprg0中,这里需要手动切换下。*/
    b 2f

/*系统态产生的中断*/
1:
    mr r5,r1 /*便于对栈进行统一的保存方法*/

/* 运行到此处，栈都是系统栈了，产生中断的程序的栈保存在R5中的 */
2:
    /*关中断操作*/
    mfsrr1 r3   
    andi. r3,r3,MSR_BIT_IR_DR_RI   /* 这里保证不影响mmu的开启 */
    ori r3,r3,MSR_BIT_FP     /* 这里关中断 [EE=0],[PR=0],[FP=1],[ME=0],[FE0-FE1=00] */
    
    /* 在恢复MMU前将SPRG3空闲出来，便于hash更新函数使用. */
    mtcr r4 /* 恢复CR */
    mfspr r4,SPRG3
    
    mtmsr r3
    sync
    isync

    /* 为保存上下文预留空间 */
    subi r1,r1,ISR_FRAME_SIZE + ISR_FLOAT_FRAME_SIZE

    /* 将先前保存在寄存器中的栈指针寄存器保存到上下文中 */
    stw r5,GPR1_OFFSET(r1)

    /* 保存r4 */
    stw r4,GPR4_OFFSET(r1)

    /* 保存CR */
    mfcr r4
    stw r4,CR_OFFSET(r1)

    lwi     r3,EXP_TMP_STACK_BASE
    lwz     r3, 0(r3)

    lwz     r4, SRR0_OFFSET(r3)
    stw     r4,SRR0_OFFSET(r1)

    lwz     r4, SRR1_OFFSET(r3)
    stw     r4,SRR1_OFFSET(r1)
    
    /* 恢复r3 r5寄存器，并保存到上下文中 */
    mfspr r3,SPRG1
    mfspr r5,SPRG2
    stw r3,GPR3_OFFSET(r1)
    stw r5,GPR5_OFFSET(r1)

.endm 


.macro GENERAL_INTERRUPT_JMP2ASM_HANDLER

    /* 跳转到对中断进一步处理的程序 */
    //b taRawGeneralIsrHandler
    b taRawGeneralIsrHandler
.endm

/*实现*/

.section ".vectors","ax" 
.global _vectors_table_start
_vectors_table_start:
VECTOR_BASE:
/*
************************************************************************
*                                                                      *
*     0100:  SYSTEM RESET                                              *
*                                                                      *
************************************************************************
*/
.skip    0x0100-(.-VECTOR_BASE)
.global __reset
__reset:
    lis r3, _start@h
    ori r3, r3, _start@l
    mtlr r3
    blr


/*
************************************************************************
*                                                                      *
*     0200  Machine Check                                              *
*                                                                      *
************************************************************************
*/
.skip    0x0200-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_MC_INT


/*
************************************************************************
*                                                                      *
*     0300:  DATA ACCESS ERROR                                         *
*                                                                      *
************************************************************************
*/
.skip    0x0300-(.-VECTOR_BASE)

    mtspr SPRG3,r1
    lwi  r1,__mmu_stack__

    stw  r3,0(r1)
    stw  r4,4(r1)
    stw  r5,8(r1)

    stw  r9,32(r1)
    stw  r10,36(r1)
    stw  r11,40(r1)

    mflr r9
    mfctr r10
    mfcr  r11

    mfspr r3,18  /* 设置r3的值为DSISR的值 */

    /* 通过DSISR 判断是否是地址转换的异常 */
    andis.	r4,r3,0x4000
    beq           1f
    mfspr r4, 19  /* 从DAR中获取缺页地址 */

    lwi  r5,__hash_table_update /* 更新哈希表 */
    mtctr r5
    bctrl
  
1:

    lwz   r3,0(r1)
    lwz   r4,4(r1)
    lwz   r5,8(r1)


    mtlr  r9
    mtctr r10
    mtcr  r11

    lwz   r9,32(r1)
    lwz   r10,36(r1) 
    lwz   r11,40(r1)

    mfspr r1,SPRG3

    b .DAE_CONTINUE
 
/*
************************************************************************
*                                                                      *
*    0400:  INSTRUCTION ACCESS ERROR                                   *
*                                                                      *
************************************************************************
*/
.skip    0x0400-(.-VECTOR_BASE)
    mtspr SPRG3,r1
    lwi  r1,__mmu_stack__

    stw  r3,0(r1)
    stw  r4,4(r1)
    stw  r5,8(r1)
    stw  r9,32(r1)
    stw  r10,36(r1)
    stw  r11,40(r1)

    mflr r9
    mfctr r10
    mfcr  r11


    mfsrr1  r3  /* 获取SRR1寄存器 */

    /* 判断是否是地址转换的异常 */
    andis.	r4,r3,0x4000
    beq           1f
    mfsrr0  r4   /* SRR0 缺页的地址 */

    lwi  r5,__hash_table_update  /* 更新哈希表 */
    mtctr r5
    bctrl
 
1:

    lwz   r3,0(r1)
    lwz   r4,4(r1)
    lwz   r5,8(r1)

    mtlr  r9
    mtctr r10
    mtcr  r11

    lwz   r9,32(r1)
    lwz   r10,36(r1) 
    lwz   r11,40(r1)

    mfspr r1,SPRG3

    b .IAE_CONTINUE

/*
************************************************************************
*                                                                      *
*   0500:  EXTERNAL INTERRUPT                                          *
*                                                                      *
************************************************************************
*/
.skip    0x0500-(.-VECTOR_BASE)
	GENERAL_INTERRUPT_SAVE_BASIC_REGISTERS
	GENERAL_INTERRUPT_JMP2ASM_HANDLER	


/*
************************************************************************
*                                                                      *
*   0600:  ALIGNMENT ERROR                                             *
*                                                                      *
************************************************************************
*/
.skip    0x0600-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_AE_INT

    
/*
************************************************************************
*                                                                      *
*   0700:  PROGRAM ERROR                                               *
*                                                                      *
************************************************************************
*/
.skip    0x0700-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    
	/*获取异常:floating-point enabled、illegal instruction、privileged instruction、trap*/
    lwz    r3,SRR1_OFFSET(r1) 
    
    /*右移16位*/
    rlwinm  r3,r3,16,16,31
    
    b .PE_CONTINUE
 
/*
************************************************************************
**                                                                     *
** 0800:  FLOATING-POINT UNAVAILABLE                                   *
**                                                                     *
************************************************************************
*/
.skip    0x0800-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_UF_INT
    

/*
************************************************************************
**                                                                     *
**   0900:  DECREMENTER INTERRUPT                                      *
**                                                                     *
************************************************************************
*/
.skip    0x0900-(.-VECTOR_BASE)
    rfi

/* 0x700空间不足部分移动至此 */
.PE_CONTINUE:

#ifdef __HARD_FLOAT__
    /*判断是否是浮点异常*/
    andi. r4,r3,0x0010
    cmpwi   r4,0x0010
    beq .FLOATING_POINT
#endif

    /*判断是否是非法指令异常*/ 
    andi. r4,r3,0x0008
    cmpwi   r4,0x0008
    beq .ILL_INSTRUCTION_POINT
    
    /*判断是否是优先级异常*/
    andi. r4,r3,0x0004
    cmpwi   r4,0x0004
    beq .PRIVILEGE_INSTRUCTION_POINT
    
    /*判断是否是TRAP异常*/
    andi. r4,r3,0x0002
    cmpwi   r4,0x0002
    beq .TRAP_POINT

#ifdef __HARD_FLOAT__
/*浮点异常处理*/    
.FLOATING_POINT:

    /* 保存FPSCR寄存器 */
    subi r1,r1,8
    stfd f15,0(r1)
    mffs f15
    stfd f15,8+FPSCR_OFFSET(r1)
    lfd  f15,0(r1)
    addi r1,r1,8
	
    mtfsfi    cr5,0
    mtfsfi    cr3,0
    mtfsfi    cr2,0
    mtfsfi    cr1,0
    mtfsfi    cr0,0
    isync 

    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_MF_INT 
#endif

/*非法指令异常处理*/		
.ILL_INSTRUCTION_POINT:		
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_ILLINS_INT
    
/*特权指令异常处理*/		
.PRIVILEGE_INSTRUCTION_POINT:		
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_PRIVI_INT
    
/*TRAP异常处理*/		
.TRAP_POINT:
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_TRAP_INT

/*
************************************************************************
**                                                                     *
**  0x0C00: System Call Interrupt                                      *
**                                                                     *
************************************************************************
*/
.skip    0x0C00-(.-VECTOR_BASE)

    /*用户态产生系统调用时:
    *r0用来保存系统调用号，r3-r10可能被用来保存系统调用使用的参数,除此之外不可能
    *使用其它寄存器。因此操作系统的系统调用处理可以直接使用volatile寄存器。
    */

    mfsrr0 r12
    
    /* 根据MSR寄存器中的[PR]位来判断产生中断的地方是否是用户态，是用户态则进行系统栈的切换，当前VM的系统栈在SPRG0中 */
    mfsrr1  r13 /*得到产生异常的MSR*/

    mfsrr1 r11   
    andi. r11,r11,MSR_BIT_IR_DR_RI   /*这里保证不影响mmu的开启*/
    ori r11,r11,MSR_BIT_FP     /*这里开中断 [EE=0],[PR=0],[FP=1],[ME=0],[FE0-FE1=00],[SE=0],[BE=0] */
    mtmsr r11
	
    sync
    isync
    
 
    cmpwi  r0, 0xFFFFFFEF /* CALLLIB调用时r0会设置为0xFFFFFFEF */
    bne    1f
    b .taCallibHandler

1:
    andi. r11, r13, 0x4000 /*判定是否是用户态产生的异常(PR)*/
    beq   .SCI_CONTINUE
    /* 根据中断向量来调用对应的中断服务程序 */
    /*为了性能考虑，用户态产生系统调用直接由操作系统接管*/
    li r11, GENERAL_SC_INT
   
    rotlwi  r11, r11, 2
    addis   r11, r11, _Raw_Vector_table@ha
    lwz    r11, _Raw_Vector_table@l(r11)
    mtctr  r11
    bctr

/*
************************************************************************
**                                                                     *
**  0D00:  TRACE                                                       *
**                                                                     *
************************************************************************
*/
.skip    0x0D00-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_DB_INT

/*
************************************************************************
**                                                                     *
**  0E00:  RESERVED                                                    *
**                                                                     *
************************************************************************
*/
.skip    0x0E00-(.-VECTOR_BASE)
    rfi

/* 0xC00系统调用空间不足部分移动至此 */
.SCI_CONTINUE:
    /*开MMU后，srr0、srr1的值有可能被MMU异常更改，此处需要关中断、关MMU、恢复srr0和srr1的值 */
    li r11,MSR_BIT_RI 
    mtmsr r11
    sync
    isync
	
    mtsrr0 r12
    mtsrr1 r13
    
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_ILLSC_INT
/*
************************************************************************
**                                                                     *
**  0F00:  Performance Monitor                                         *
**                                                                     *
************************************************************************
*/
.skip    0x0F00-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_PM_INT


/* TLB MISS EXCEPTION */
#if defined(__PPC7450__)|| defined(__PPC8640__)
/*
************************************************************************
*                                                                      *
*  1000:  INSTRUCTION TRANSLATION MISS                                 *
*                                                                      *
************************************************************************
*/
.skip    0x01000-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_ITM_INT

/*
************************************************************************
**                                                                     *
**  1100:  DATA LOAD TRANSLATION MISS                                  *
**                                                                     *
************************************************************************
*/
.skip    0x1100-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_DLTM_INT
    

/*
************************************************************************
**                                                                     *
**  1200:  DATA STORE TRANSLATION MISS                                 *
**                                                                     *
************************************************************************
*/
.skip    0x1200-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_DSTM_INT
    

#endif /* defined(__PPC7450__)|| defined(__PPC8640__) */
/*
************************************************************************
**                                                                     *
**  1300:  INSTRUCTION ADDRESS BREAKPOINT                              *
**                                                                     *
************************************************************************
*/
.skip    0x1300-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_IAB_INT

/*
************************************************************************
**                                                                     *
**  1400:  SYSTEM MANAGEMENT INTERRUPT                                 *
**                                                                     *
************************************************************************
*/
.skip    0x1400-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_SM_INT


#if defined(__PPC750__)
/*
************************************************************************
**                                                                     *
**  1700:  THERMAL MANAGEMENT INTERRUPT                                *
**                                                                     *
************************************************************************
*/
.skip    0x1700-(.-VECTOR_BASE)
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_TH_INT

#endif /* __PPC750__ */
/*
************************************************************************
**                                                                     *
**  1800:  Exception ASM Handler                                       *
**                                                                     *
************************************************************************
*/
.skip    0x1800-(.-VECTOR_BASE)
__exception_save_basic_register:
    
    /* 将SRR0、SRR1、CR、DAR、DSISR保存到临时栈*/
    mfsrr0  r5 
    stw     r5,SRR0_OFFSET(r3)

    mfsrr1  r5 
    stw     r5,SRR1_OFFSET(r3)

    mfcr    r5 /*保存cr,以下操作会影响到cr*/
    stw     r5,CR_OFFSET(r3)

    mfspr   r5,DAR
    stw     r5,DAR_OFFSET(r3)

    mfspr   r5,DSISR
    stw     r5,DSISR_OFFSET(r3)

    /* 根据MSR寄存器中的[PR]位来判断产生中断的地方是否是用户态，是用户态则进行系统栈的切换，当前分区的系统栈在SPRG0中 */
    mfsrr1  r3 /*得到产生异常的MSR*/
    andi.   r3,r3,0x4000 /*判定是否是用户态产生的异常(PR)*/
    beq 1f
    
    /* 用户态*/
    mr r5,r1
    mfspr r1, SPRG0 /*用户态下产生异常，需要把产生异常的分区的栈切换到系统栈，操作系统把当前分区的系统栈定义在SPRG0中,这里需要手动切换下。*/
    b 4f

1:  /*系统态*/
    
     
    li	r5, 0
    addis	r3, 0, exceptionTwiceFlag@ha
    stw	r5, exceptionTwiceFlag@l(r3)
    
    lis r5, taExceptionStackTop@ha
    lwz r5, taExceptionStackTop@l(r5) /* 获取ta异常栈顶地址*/
    cmpw r1, r5 /* 判断当前栈地址是否高于MSL异常栈 */
    bgt 2f
    lis r5, taExceptionStackBottom@ha
    lwz r5, taExceptionStackBottom@l(r5) /* 获取ta异常栈底地址*/
    cmpw r1, r5 /* 判断当前栈是否是否低于MSL异常栈  */
    blt 2f

    /* 异常处理已经在ta异常栈中*/
    /* 标识在ta异常栈处理时产生了二次异常*/
    li	r5, 1
    stw	r5, exceptionTwiceFlag@l(r3)

    /* 是否还需判断ta 异常栈上有足够的剩余空间*/
    lis r5, taExceptionStackBottom@ha
    lwz r5, taExceptionStackBottom@l(r5) /* 获取ta异常栈底地址*/
    addi r3,r5,MINIMUM_EXCHANDLE_STACK_SIZE+EXCEPTION_FRAME_SIZE
    cmpw r1, r3 /* 判断当前ta异常栈是否还有足够空间保存异常*/
    bge 3f

2:
    /* 
      * 当前ta 异常栈中没有足够剩余空间或者
      * 其他核心态时发生的异常，则直接将栈
      * 设置为ta异常栈顶
      */
    mr r5, r1 /*便于对栈进行统一的保存方法 */
    lis r3, taExceptionStackTop@ha
    lwz r3, taExceptionStackTop@l(r3) /* 直接将栈设置为ta异常栈顶*/
    mr r1, r3
    b 4f

3:
    mr r5, r1

4:
    /*关中断操作*/
    mfsrr1 r3   
    andi. r3,r3,MSR_BIT_IR_DR_RI   /*这里保证不影响mmu的开启*/
    ori r3,r3,MSR_BIT_FP     /*这里关中断 [EE=0],[PR=0],[FP=1],[ME=0],[FE0-FE1=00] */ 
    mtmsr r3
    sync
    isync

    /* 为异常保存上下文预留空间 */
    subi r1,r1,EXCEPTION_FRAME_SIZE

    stw r4,GPR4_OFFSET(r1)
    
    /*保存SRR0、SRR1、CR、CTR、LR、DAR、DSISR、R1 */
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

    /*恢复r3、r5*/
    mfspr r3,SPRG1
    mfspr r5,SPRG2
    
    /*保存r3,r5*/
    stw r3,GPR3_OFFSET(r1)
    stw r5,GPR5_OFFSET(r1)
    blr
    
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
    
    /*恢复异常向量号*/
    lwz r3,EXCEPTION_VECTOR_OFFSET(r1)    
    
    /* 根据异常向量来调用对应的异常服务程序 */
    addis    r6, 0, _Raw_Vector_table@ha
    rotlwi  r5, r3,2
    add     r6,r6,r5
    lwz    r7, _Raw_Vector_table@l(r6)
    mtctr  r7
    
    mr r3, r1
    lis r4, exceptionTwiceFlag@ha
    lwz r4, exceptionTwiceFlag@l(r4)
    /*这里没作r1 减8处理。要求服务程序全部以汇编代码编写。通常的c函数调用会自动将lr保存到r1 减8的位置，自己写的汇编一般不会保存lr到r1 减8*/
    bctrl

    /* 恢复上下文并返回 */
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

    /* 下面的操作需要关中断进行 */
    mfmsr r3   
    andi. r3,r3,MSR_BIT_IR_DR_RI   /*这里保证不影响mmu的开启*/
    ori r3,r3,MSR_BIT_FP     /*这里关中断 [EE=0],[PR=0],[FP=1],[ME=0],[FE0-FE1=00] */ 
    mtmsr r3
    sync
    isync

    lwz r3,CR_OFFSET(r1) 
    mtcr    r3

    lwz     r3, GPR3_OFFSET(r1)
    mtspr   SPRG1, r3

    lwz     r4, GPR4_OFFSET(r1)
    mtspr   SPRG2, r4
    
    /*获取rfi返回时保存srr0,srr1使用的栈*/
    lwi      r3,EXP_TMP_STACK_BASE
    lwz     r3, 0(r3)
    
    lwz     r4, SRR0_OFFSET(r1)
    stw     r4,SRR0_OFFSET(r3)

    lwz     r4, SRR1_OFFSET(r1)
    stw     r4,SRR1_OFFSET(r3)

    lwz     r1, GPR1_OFFSET(r1)

    /*在设置srr0,srr1前，需要关中断、关MMU进行 */
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

/*
************************************************************************
**                                                                     *
**  ISR ASM Handler                                       *
**                                                                     *
************************************************************************
*/
taRawGeneralIsrHandler:

 /* 保存剩余的寄存器  */
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
    mfctr   r3
    mflr    r4
    mfxer   r5
    stw r3,CTR_OFFSET(r1)
    stw r4,LR_OFFSET(r1)
    stw r5,XER_OFFSET(r1)

    /* 保存浮点寄存器 f0 - f31*/
    stfd f0, (ISR_FRAME_SIZE + FP_0)(r1)
    stfd f1, (ISR_FRAME_SIZE + FP_1)(r1)
    stfd f2, (ISR_FRAME_SIZE + FP_2)(r1)
    stfd f3, (ISR_FRAME_SIZE + FP_3)(r1)
    stfd f4, (ISR_FRAME_SIZE + FP_4)(r1)
    stfd f5, (ISR_FRAME_SIZE + FP_5)(r1)
    stfd f6, (ISR_FRAME_SIZE + FP_6)(r1)
    stfd f7, (ISR_FRAME_SIZE + FP_7)(r1)
    stfd f8, (ISR_FRAME_SIZE + FP_8)(r1)
    stfd f9, (ISR_FRAME_SIZE + FP_9)(r1)
    stfd f10, (ISR_FRAME_SIZE + FP_10)(r1)
    stfd f11, (ISR_FRAME_SIZE + FP_11)(r1)
    stfd f12, (ISR_FRAME_SIZE + FP_12)(r1)
    stfd f13, (ISR_FRAME_SIZE + FP_13)(r1)
    stfd f14, (ISR_FRAME_SIZE + FP_14)(r1)
    stfd f15, (ISR_FRAME_SIZE + FP_15)(r1)
    stfd f16, (ISR_FRAME_SIZE + FP_16)(r1)
    stfd f17, (ISR_FRAME_SIZE + FP_17)(r1)
    stfd f18, (ISR_FRAME_SIZE + FP_18)(r1)
    stfd f19, (ISR_FRAME_SIZE + FP_19)(r1)
    stfd f20, (ISR_FRAME_SIZE + FP_20)(r1)
    stfd f21, (ISR_FRAME_SIZE + FP_21)(r1)
    stfd f22, (ISR_FRAME_SIZE + FP_22)(r1)
    stfd f23, (ISR_FRAME_SIZE + FP_23)(r1)
    stfd f24, (ISR_FRAME_SIZE + FP_24)(r1)
    stfd f25, (ISR_FRAME_SIZE + FP_25)(r1)
    stfd f26, (ISR_FRAME_SIZE + FP_26)(r1)
    stfd f27, (ISR_FRAME_SIZE + FP_27)(r1)
    stfd f28, (ISR_FRAME_SIZE + FP_28)(r1)
    stfd f29, (ISR_FRAME_SIZE + FP_29)(r1)
    stfd f30, (ISR_FRAME_SIZE + FP_30)(r1)
    stfd f31, (ISR_FRAME_SIZE + FP_31)(r1)

    /* 保存FPSCR */
    mffs   f0
    stfd   f0, (ISR_FRAME_SIZE + FP_FPSCR)(r1)

    isync
    sync
    
    /*获取并检查中断号。由于中断设备的不同，获取中断代码放在对应的板级目录中*/  
    subi r1,r1,8
    bl getVectorNum
    addi r1,r1,8
    
    cmpwi r3,0xff
    beq .isr_exit
    
    /*中断号加上异常最大位数（处理表中，异常排在中断前面）*/
    addi r3,r3,GENERAL_STD_IRQ_LAST   
    
    /*保存中断号*/
    stw r3,ISR_VECTOR_OFFSET(r1)
    
    /* 根据中断向量来调用对应的中断服务程序 */
    /*需要在中断处理函数中，在适当时机做开启中断和响应中断的操作*/
    addis    r6, 0, _Raw_Vector_table@ha
    rotlwi  r5, r3, 2
    add     r6,r6, r5
    lwz    r7, _Raw_Vector_table@l(r6)
    mtctr  r7
    
    /*这里没作r1-8处理。要求服务程序全部以汇编代码编写。通常的c函数调用会自动将lr保存到r1-8的位置，自己写的汇编一般不会保存lr到r1-8*/
    bctrl
       
.isr_exit:

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

    /* 下面的操作需要关中断进行 */
    mfmsr r3
    andi. r3,r3,MSR_BIT_IR_DR_RI   /*这里保证不影响mmu的开启*/
    ori r3,r3,MSR_BIT_FP     /*这里关中断 [EE=0],[PR=0],[FP=1],[ME=0],[FE0-FE1=00] */
    mtmsr r3
    sync
    isync

    /*恢复FPSCR */
    lfd    f0,(ISR_FRAME_SIZE + FP_FPSCR)(r1)
    mtfsf   0xff, f0

    /*  恢复浮点寄存器f0-f31 */
    lfd   f0,(ISR_FRAME_SIZE + FP_0)(r1) 
    lfd   f1,(ISR_FRAME_SIZE + FP_1)(r1)
    lfd   f2,(ISR_FRAME_SIZE + FP_2)(r1)
    lfd   f3,(ISR_FRAME_SIZE + FP_3)(r1)
    lfd   f4,(ISR_FRAME_SIZE + FP_4)(r1)
    lfd   f5,(ISR_FRAME_SIZE + FP_5)(r1)
    lfd   f6,(ISR_FRAME_SIZE + FP_6)(r1)
    lfd   f7,(ISR_FRAME_SIZE + FP_7)(r1)
    lfd   f8,(ISR_FRAME_SIZE + FP_8)(r1)
    lfd   f9,(ISR_FRAME_SIZE + FP_9)(r1)
    lfd   f10,(ISR_FRAME_SIZE + FP_10)(r1)
    lfd   f11,(ISR_FRAME_SIZE + FP_11)(r1)
    lfd   f12,(ISR_FRAME_SIZE + FP_12)(r1)
    lfd   f13,(ISR_FRAME_SIZE + FP_13)(r1)
    lfd   f14,(ISR_FRAME_SIZE + FP_14)(r1)
    lfd   f15,(ISR_FRAME_SIZE + FP_15)(r1)
    lfd   f16,(ISR_FRAME_SIZE + FP_16)(r1)
    lfd   f17,(ISR_FRAME_SIZE + FP_17)(r1)
    lfd   f18,(ISR_FRAME_SIZE + FP_18)(r1)
    lfd   f19,(ISR_FRAME_SIZE + FP_19)(r1)
    lfd   f20,(ISR_FRAME_SIZE + FP_20)(r1)
    lfd   f21,(ISR_FRAME_SIZE + FP_21)(r1)
    lfd   f22,(ISR_FRAME_SIZE + FP_22)(r1)
    lfd   f23,(ISR_FRAME_SIZE + FP_23)(r1)
    lfd   f24,(ISR_FRAME_SIZE + FP_24)(r1)
    lfd   f25,(ISR_FRAME_SIZE + FP_25)(r1)
    lfd   f26,(ISR_FRAME_SIZE + FP_26)(r1)
    lfd   f27,(ISR_FRAME_SIZE + FP_27)(r1)
    lfd   f28,(ISR_FRAME_SIZE + FP_28)(r1)
    lfd   f29,(ISR_FRAME_SIZE + FP_29)(r1)
    lfd   f30,(ISR_FRAME_SIZE + FP_30)(r1)
    lfd   f31,(ISR_FRAME_SIZE + FP_31)(r1)

    isync
    sync
    
    lwz r3,CR_OFFSET(r1) 
    mtcr    r3
    
    lwz     r3, GPR3_OFFSET(r1)
    mtspr   SPRG1, r3

    lwz     r4, GPR4_OFFSET(r1)
    mtspr   SPRG2, r4

    /*获取rfi返回时保存srr0,srr1使用的栈*/
    lwi      r3,EXP_TMP_STACK_BASE
    lwz     r3, 0(r3)
    
    lwz     r4, SRR0_OFFSET(r1)
    stw     r4,SRR0_OFFSET(r3)

    lwz     r4, SRR1_OFFSET(r1)
    stw     r4,SRR1_OFFSET(r3)

    lwz     r1, GPR1_OFFSET(r1)

    /*在设置srr0,srr1前，需要关中断、关MMU进行 */
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
        
.p2align 4
.local .taCallibHandler
.taCallibHandler:

    cmpw r0, r3
    beq 2f
	
    /* 保存当前栈，切换到系统栈 */
    mr r10, r1
	
    andi.   r11, r13, 0x4000 /*判定是否是用户态产生的异常(PR)*/
    beq 1f
	
    /* 切换到系统栈 */
    mfspr r1, SPRG0

1: 
    subi r1, r1, 0x10
    stw r12, 0(r1)
    stw r13, 4(r1)
    mflr r11
    stw  r11, 8(r1)
    stw  r10, 0xC(r1)
    li	r12, GENERAL_CALLLIB_INT
    addis    r11, 0, _Raw_Vector_table@ha
    rotlwi  r12, r12, 2
    add     r11,r11, r12
    lwz    r11, _Raw_Vector_table@l(r11)
    mtctr  r11
    bctrl
    
    lwz r12, 0(r1)
    lwz r13, 4(r1)
    lwz r11, 8(r1)
    mtlr r11
    lwz r1, 0xC(r1)
    
    
    /* 关中断、关MMU进行 */
    li r11,MSR_BIT_RI 
    mtmsr r11
    sync
    isync
	
    mtsrr0 r12
    mtsrr1 r13

    sync
    isync
    rfi

2:
    /*开MMU后，srr0、srr1的值有可能被MMU异常更改，此处需要关中断、关MMU、恢复srr0和srr1的值 */
    li r11,MSR_BIT_RI 
    mtmsr r11
    sync
    isync
	
    mtsrr0 r12
    mtsrr1 r13
    
    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS 
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_CALLLIB_ERROR_INT


/*
************************************************************************
**                                                                     *
**  .DAE_CONTINUE   .IAE_CONTINUE                     *
**                                                                     *
************************************************************************
*/
/* 0x300空间不足部分移动至此 */
.DAE_CONTINUE:

    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS

    lwz     r3, DSISR_OFFSET(r1)
    
    /*散列开 页异常和断点异常*/
    rlwinm  r3,r3,16,16,31 /*右移16位*/
    andi. r3,r3,0x4840

    /*判断是否是地址没有映射异常*/ 
    cmpwi   r3,0x4000
    beq .DPAGE_FAULT
    
    /*判断是否是页保护异常*/ 
    cmpwi   r3,0x0800
    beq .DPAGE_FAULT
    
    /*判断是否是断点异常*/
    cmpwi   r3,0x0040
    beq .BREAK_FAULT
    
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_DAE_INT
    
.DPAGE_FAULT:
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_PAGE_INT
    
.BREAK_FAULT:
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_HBP_INT

/* 0x400空间不足部分移动至此 */
.IAE_CONTINUE:

    GENERAL_EXCEPTION_SAVE_BASIC_REGISTERS

    lwz     r3, SRR1_OFFSET(r1)
    rlwinm  r3,r3,16,16,31 /*右移16位*/
    andi. r3,r3,0x4800
    cmpwi   r3,0x4000
    beq .IPAGE_FAULT
    cmpwi   r3,0x800
    beq .IPAGE_FAULT
    
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_IAE_INT
    
.IPAGE_FAULT:
    GENERAL_EXCEPTION_JMP2ASM_HANDLER GENERAL_IPAGE_INT 

/*
************************************************************************
**                                                                     *
**  更新哈希表                                           *
**                                                                     *
************************************************************************
*/

/*  把这段代码存放在这个位置是便于节约空间 */

/* r4 包含具体的有效地址 */

__hash_table_update:

    stw  r6,12(r1)
    stw  r7,16(r1)
    stw  r8,20(r1)
    stw  r0,24(r1)
    stw  r2,28(r1)

    rlwinm	r3,r4,12,20,29  /* 得到该地址的页目录偏移 */

    addis r5,0,mmPageDirBase@ha  /* 得到页目录基地址 */
    lwz   r5,mmPageDirBase@l(r5)

    cmpwi r5,0
    beq   1f

    rlwinm	r6,r5,0,0,19 /* 得到页目录项的基址 */

    add r3,r3,r6   /* 得到页目录项的地址 */

    lwz  r3,0(r3) /* 得到页目录项的内容 */
    cmpwi r3,0  /* 判断该页目录项是否有效，无效跳到出口 */
    beq   1f

    rlwinm	r3,r3,0,0,19 /* 得到页表的基址 */
    rlwinm	r6,r4,22,20,29 /* 得到页表项的偏移 */
    add           r3,r3,r6   /* 得到对应的页表项地址 */

    lwz  r6,0(r3)  /* 得到页表项的内容 */
    mr   r3,r6
    rlwinm	r3,r3,25,31,31  /* 得到存在位（PPC定义的change位）*/

    cmpwi r3,0  /* 判断该页表项是否有效，无效则跳出 */

    beq   1f

    rlwinm	r6,r6,0,25,22 /* 清除R和C位 R6 = PETL */

    rlwinm	r3,r4,4,28,31 /* SRn  */
    slwi       r5,r3,28
    mfsrin	r5,r5  /* SR的值 */
    rlwinm     r5,r5, 0,8,31  /*屏蔽状态位,只留VSID*/

    /* 得到PTEU */
    mr r2,r4
    mr r8,r5


    rlwinm	r2,r2,10,26,31  /*获取有效地址的API ，PTEU[26-31]*/
    rlwinm	r8,r8,7,0,24     /*获取VSID ，PTEU[1-24]*/
    oris	r2,r2,32768      /*设置PTEU 为有效，PTEU[0]*/
    or	r2,r8,r2    /*生成主hash表项组(PTEG)的第一个hash表项的PTEU */


    /*获取主PTEG的第一个hash表项PTEL*/
    mfspr  r3,25  /* 获取HASH表的基地址 */

    rlwinm	r6,r6,0,25,22  /* 清除C和R位 */


    /* r3=sdr,r4=ea,r5=sr  : 计算Hash的Primary PTEG的入口 */
    rlwinm	r4,r4,20,16,31   /*获取ea[4-19]*/
    clrlwi	r5,r5,13    /*获取VSID[5-23]*/
    xor		r4,r4,r5   /*ea[4-19]异或VSID[5-23]生成19位的主PTEG的hash值1*/
    rlwinm	r7,r3,0,7,15    /*获取hash表基址的7-15位，SDR1[7-15]*/
    rlwinm	r8,r4,22,10,31  /*获取hash值1的0-8位*/
    and		r8,r8,r3  /*SDR1[HTABMASK]掩码& hash值1的0-8位生成PTEG[7-15]a*/
    rlwinm	r4,r4,6,16,25  /*获取hash值1的9-18位并左移6位生成PTEG[16-25]*/
    rlwinm	r8,r8,16,0,15  /*PTEG[7-15]a 左移16位生成PTEG[7-15]b*/
    rlwinm	r3,r3,0,0,6 /*获取hash表基址的0-6，PTEG[0-6]*/
    or		r8,r8,r7    /*PTEG[7-15]b | SDR1[7-15]生成PTEG[7-15]*/

    /*PTEG的地址= PTEG[0-6]|PTEG[7-15]|PTEG[16-25]|000000b*/
    add		r3,r3,r8
    or		r3,r3,r4

    /* 更新Hash表 */
    stw   r2,0(r3)
    stw   r6,4(r3)

    lwi   r4,0xA7FFFFFF
    mfsrr1 r3
    and.  r3,r4,r3
    mtsrr1 r3

    lwz   r3,0(r1)
    lwz   r4,4(r1)
    lwz   r5,8(r1)
    lwz   r6,12(r1)
    lwz   r7,16(r1)
    lwz   r8,20(r1)
    lwz   r0,24(r1)
    lwz   r2,28(r1)

    mtlr  r9
    mtctr r10
    mtcr  r11

    lwz   r9,32(r1)
    lwz   r10,36(r1)
    lwz   r11,40(r1)

    mfspr r1,SPRG3

    isync
    sync
    rfi

1:
    lwz   r6,12(r1)
    lwz   r7,16(r1)
    lwz   r8,20(r1)
    lwz   r0,24(r1)
    lwz   r2,28(r1)

    blr /* 回到调用程序 */

.skip   0x04000-(.-VECTOR_BASE)

_vectors_table_end:

