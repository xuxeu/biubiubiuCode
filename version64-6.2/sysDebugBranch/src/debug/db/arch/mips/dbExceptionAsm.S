/*
 *变更历史:
 * 2010-08-31 彭元志     创建该文件。
 */

/**
 * @file dbExceptionAsm.S
 * @brief
 *       功能:
 *       <li>提供异常入口</li>
 */

/*头文件*/

#include "kbspAsm.h"
#include "kbspCp0.h"

/* 恢复上下文 */
.macro RESTORE_REGISTERS

#ifdef CONFIG_CORE_SMP
.set	mips64 
	mfc0    k1, $15, 1
	andi    k1, 0x3ff
.set	mips3
	dmul k1,k1,0x248 /*580*/
  	la   k0, taSDASmpExpContext
  	add k0,k0,k1  		
#else
	la   k0, taSDASmpExpContext
#endif

	LDREG k1, TA_REG_STATUS_OFFSET(k0)
	nop
	MTCO k1, Status
	
	LDREG k1, TA_REG_EPC_OFFSET(k0)
	nop
	MTCO k1, EPC
	
	LDREG k1, TA_REG_BAD_OFFSET(k0)
	nop
	MTCO k1, Bad
	
	LDREG k1, TA_REG_HI_OFFSET(k0)
	nop
	mthi k1
	
	LDREG k1, TA_REG_LO_OFFSET(k0)
	nop	
	mtlo k1
		
	LDREG k1, TA_REG_CAUSE_OFFSET(k0)
	nop
	MTCO k1, Cause
	
	LDREG zero, TA_REG_ZERO_OFFSET(k0)

	LDREG v0, TA_REG_V0_OFFSET(k0)
	LDREG v1, TA_REG_V1_OFFSET(k0)

	LDREG a0, TA_REG_A0_OFFSET(k0)
	LDREG a1, TA_REG_A1_OFFSET(k0)
	LDREG a2, TA_REG_A2_OFFSET(k0)
	LDREG a3, TA_REG_A3_OFFSET(k0)
	
	LDREG t0, TA_REG_T0_OFFSET(k0)
	LDREG t1, TA_REG_T1_OFFSET(k0)
	LDREG t2, TA_REG_T2_OFFSET(k0)
	LDREG t3, TA_REG_T3_OFFSET(k0)
	LDREG t4, TA_REG_T4_OFFSET(k0)
	LDREG t5, TA_REG_T5_OFFSET(k0)
	LDREG t6, TA_REG_T6_OFFSET(k0)
	LDREG t7, TA_REG_T7_OFFSET(k0)	
	LDREG t8, TA_REG_T8_OFFSET(k0)
	LDREG t9, TA_REG_T9_OFFSET(k0)
	
	LDREG s0, TA_REG_S0_OFFSET(k0)
	LDREG s1, TA_REG_S1_OFFSET(k0)
	LDREG s2, TA_REG_S2_OFFSET(k0)
	LDREG s3, TA_REG_S3_OFFSET(k0)
	LDREG s4, TA_REG_S4_OFFSET(k0)
	LDREG s5, TA_REG_S5_OFFSET(k0)
	LDREG s6, TA_REG_S6_OFFSET(k0)
	LDREG s7, TA_REG_S7_OFFSET(k0)
	
	LDREG gp, TA_REG_GP_OFFSET(k0)
	LDREG sp, TA_REG_SP_OFFSET(k0)
	LDREG fp, TA_REG_FP_OFFSET(k0) /*s8*/
	LDREG ra, TA_REG_RA_OFFSET(k0)
	
	.set noat
	LDREG AT, TA_REG_AT_OFFSET(k0)
	.set at
.endm


/* 保存现场 */
.macro SAVE_REGISTERS

#ifdef CONFIG_CORE_SMP
.set	mips64 
	mfc0    k1, $15, 1
	andi    k1, 0x3ff
.set	mips3
	dmul k1,k1,0x248 /*580*/
  	la   k0, taSDASmpExpContext
  	add k0,k0,k1  
#else
  	la   k0, taSDASmpExpContext
#endif

	MFCO k1, Status
	nop
	STREG k1, TA_REG_STATUS_OFFSET(k0)
	
	MFCO k1, EPC 
	nop
	STREG k1, TA_REG_EPC_OFFSET(k0)
	
	MFCO k1, Bad
	nop
	STREG k1, TA_REG_BAD_OFFSET(k0)
	
	mfhi k1
	nop
	STREG k1, TA_REG_HI_OFFSET(k0)
	
	mflo k1
	nop
	STREG k1, TA_REG_LO_OFFSET(k0)
	
	MFCO k1, Cause
	nop	
	STREG k1, TA_REG_CAUSE_OFFSET(k0)
	
	STREG zero, TA_REG_ZERO_OFFSET(k0)

	STREG v0, TA_REG_V0_OFFSET(k0)
	STREG v1, TA_REG_V1_OFFSET(k0)

	STREG a0, TA_REG_A0_OFFSET(k0)
	STREG a1, TA_REG_A1_OFFSET(k0)
	STREG a2, TA_REG_A2_OFFSET(k0)
	STREG a3, TA_REG_A3_OFFSET(k0)
	
	STREG t0, TA_REG_T0_OFFSET(k0)
	STREG t1, TA_REG_T1_OFFSET(k0)
	STREG t2, TA_REG_T2_OFFSET(k0)
	STREG t3, TA_REG_T3_OFFSET(k0)
	STREG t4, TA_REG_T4_OFFSET(k0)
	STREG t5, TA_REG_T5_OFFSET(k0)
	STREG t6, TA_REG_T6_OFFSET(k0)
	STREG t7, TA_REG_T7_OFFSET(k0)	
	STREG t8, TA_REG_T8_OFFSET(k0)
	STREG t9, TA_REG_T9_OFFSET(k0)
	
	STREG s0, TA_REG_S0_OFFSET(k0)
	STREG s1, TA_REG_S1_OFFSET(k0)
	STREG s2, TA_REG_S2_OFFSET(k0)
	STREG s3, TA_REG_S3_OFFSET(k0)
	STREG s4, TA_REG_S4_OFFSET(k0)
	STREG s5, TA_REG_S5_OFFSET(k0)
	STREG s6, TA_REG_S6_OFFSET(k0)
	STREG s7, TA_REG_S7_OFFSET(k0)
	
	STREG gp, TA_REG_GP_OFFSET(k0)
	STREG sp, TA_REG_SP_OFFSET(k0)
	STREG fp, TA_REG_FP_OFFSET(k0)/*s8*/
	STREG ra, TA_REG_RA_OFFSET(k0)
	
	.set noat
	STREG AT, TA_REG_AT_OFFSET(k0)
	.set at
.endm


FRAME(taSystemDebugExceptionEntry,fp,0,ra)

    nop
    nop 
    nop
                         
    SAVE_REGISTERS
    
    mfc0 t0,CP0_STATUS
    and t0, t0, 0xFFFFFFFC
    mtc0 t0,CP0_STATUS
    
    MFCO a0, Cause
    andi a0, Cause_ExcCode
    srl  a0, a0, 2
    
    ADD sp,sp, -(R_SZ * 4)
    la t0, taSDAExceptionHandler
    jalr t0             
    nop
    ADD sp,sp, R_SZ * 4
	  
    RESTORE_REGISTERS

    eret
    nop
ENDFRAME(taSystemDebugExceptionEntry)

FRAME(taSystemDebugInterruptEntry,fp,0,ra)
    nop
    nop
    nop
                         
    SAVE_REGISTERS
    
    MFCO    a0, Cause                    
    andi    a0, Cause_ExcCode
    srl a0, a0, 2
    
    ADD sp,sp, -(R_SZ * 4)
    la t0, taDevIntHandle
    jalr t0             
    nop
    ADD sp,sp, R_SZ * 4
	  
    RESTORE_REGISTERS
    
    eret
    nop
ENDFRAME(taSystemDebugInterruptEntry)

#ifdef _NOOS_TEST
FRAME(taIPIEntry,fp,0,ra)
    nop
    nop
    nop
                         
    SAVE_REGISTERS
    
    MFCO    a0, Cause                    
    andi    a0, Cause_ExcCode
    srl a0, a0, 2
    
    ADD sp,sp, -(R_SZ * 4)
    la t0, sdaCpuCtrlHandler
    jalr t0             
    nop
    ADD sp,sp, R_SZ * 4
	  
    RESTORE_REGISTERS    
   
    eret
    nop
ENDFRAME(taIPIEntry)
#endif
