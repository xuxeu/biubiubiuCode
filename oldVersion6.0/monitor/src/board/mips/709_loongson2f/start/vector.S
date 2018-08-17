#include "regdef.h"
#include "asm.h"
#include "mips_cp0.h"


/* 恢复上下文 */
.macro RESTORE_REGISTERS

    la   k0, g_sdaExpContext

#if 0
#ifdef _HARD_FLOAT_
    /* 恢复浮点上下文 */
    TA_RESTORE_FGPR
#endif
#endif
	
	LDREG k1, TA_REG_STATUS_OFFSET(k0)
	nop
	MTC0 k1, STATUS
	
	
	LDREG k1, TA_REG_EPC_OFFSET(k0)
	nop
	MTC0 k1, EPC 
	
	
	
	
	LDREG k1, TA_REG_BAD_OFFSET(k0)
	nop
	MFC0 k1, BAD
	
	
	
	LDREG k1, TA_REG_HI_OFFSET(k0)
	nop
	mthi k1
	
	
	
	LDREG k1, TA_REG_LO_OFFSET(k0)
	nop	
	mtlo k1
	
	
	
		
	LDREG k1, TA_REG_CAUSE_OFFSET(k0)
	nop
	MTC0 k1, CAUSE
	
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
	
	LDREG s1, TA_REG_S1_OFFSET(k0)
	LDREG s2, TA_REG_S2_OFFSET(k0)
	LDREG s3, TA_REG_S3_OFFSET(k0)
	LDREG s4, TA_REG_S4_OFFSET(k0)
	LDREG s5, TA_REG_S5_OFFSET(k0)
	LDREG s6, TA_REG_S6_OFFSET(k0)
	LDREG s7, TA_REG_S7_OFFSET(k0)
	
	LDREG gp, TA_REG_GP_OFFSET(k0)
	
	//LDREG sp, TA_REG_SP_OFFSET(k0)
	LDREG fp, TA_REG_FP_OFFSET(k0)
	LDREG ra, TA_REG_RA_OFFSET(k0)
	
	.set noat
	LDREG AT, TA_REG_AT_OFFSET(k0)
	.set at
.endm



/* 保存现场 */
.macro SAVE_REGISTERS

    la   k0, g_sdaExpContext

/*FIXME:
#ifdef _HARD_FLOAT_
    TA_SAVE_FPGR
#endif
*/
	MFC0 k1, STATUS
	nop
	STREG k1, TA_REG_STATUS_OFFSET(k0)
	
	MFC0 k1, EPC 
	nop
	STREG k1, TA_REG_EPC_OFFSET(k0)
	
	MFC0 k1, BAD
	nop
	STREG k1, TA_REG_BAD_OFFSET(k0)
	
	mfhi k1
	nop
	STREG k1, TA_REG_HI_OFFSET(k0)
	
	mflo k1
	nop
	STREG k1, TA_REG_LO_OFFSET(k0)
	
	MFC0 k1, CAUSE
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
	
	STREG s1, TA_REG_S1_OFFSET(k0)
	STREG s2, TA_REG_S2_OFFSET(k0)
	STREG s3, TA_REG_S3_OFFSET(k0)
	STREG s4, TA_REG_S4_OFFSET(k0)
	STREG s5, TA_REG_S5_OFFSET(k0)
	STREG s6, TA_REG_S6_OFFSET(k0)
	STREG s7, TA_REG_S7_OFFSET(k0)
	
	STREG gp, TA_REG_GP_OFFSET(k0)
	move k1,sp
	ADDU k1, sp, R_SZ * 1
	STREG k1, TA_REG_SP_OFFSET(k0)
	STREG fp, TA_REG_FP_OFFSET(k0)
	STREG ra, TA_REG_RA_OFFSET(k0)
	
	.set noat
	STREG AT, TA_REG_AT_OFFSET(k0)
	.set at
.endm

.macro TEMP_SAVE
	dsubu sp, sp, R_SZ * 1
	STREG ra, 0(sp)
.endm

.macro TEMP_RESTORE	
	LDREG ra, 0(sp)
	ADDU sp, sp, R_SZ * 1
.endm

#define OS_EXCEPTION_TABLE 0x80000200  //FIXME:这里需要和OS小组协商

GLOBAL(Handler_GeneralExc,0)

   TEMP_SAVE
    nop
    nop
    nop
                         
    SAVE_REGISTERS
    
    MFC0    a0, CAUSE                    
    andi    a0, CAUSE_EXCCODE
    srl a0, a0, 2
    
    dsubu sp,sp, R_SZ * 4
    la t0, monitorCallHandler
    jalr t0             
    nop
    ADDU sp,sp, R_SZ * 4
	
	li t0, 1
	bne v0, t0, exit_monitor_exception
	

    
    RESTORE_REGISTERS
    
    TEMP_RESTORE	
    
    MFC0    k0, CAUSE                    
    andi    k0, CAUSE_EXCCODE
    srl k0, k0, 2	
    
    lw        k1, OS_EXCEPTION_TABLE(k0) 
    jr        k1 /*跳转到OS*/
    nop
   
    
exit_monitor_exception:    
    RESTORE_REGISTERS
    
    TEMP_RESTORE
    
    eret
    nop
    
END_GLOBAL(Handler_GeneralExc)

GLOBAL(BPExceptionEntry,0)
    nop
    nop
    nop
                         
    SAVE_REGISTERS
    
    MFC0    a0, CAUSE                    
    andi    a0, CAUSE_EXCCODE
    srl a0, a0, 2
    
    dsubu sp,sp, R_SZ * 4
    la t0, sdaExceptionHandler
    jalr t0             
    nop
    ADDU sp,sp, R_SZ * 4
	
    RESTORE_REGISTERS
    
    eret
    nop
    
END_GLOBAL(BPExceptionEntry)

.text

.global _vectors_table_start
_vectors_table_start:

VECTOR_BASE:



/*
************************************************************************
*                                                                      *
*     0x0000 0000:  TLB refill                                         *        
*                                                                      *
************************************************************************
*/
.global __except_tlb_refill
__except_tlb_refill:
    la        t0, except_tlb_refill_default
    jr        t0


/*
************************************************************************
*                                                                      *
*     0x0000 0080:  XTLB refill                                        *        
*                                                                      *
************************************************************************
*/
.skip    0x080 - ( . - VECTOR_BASE )
.global __except_xtlb_refill
__except_xtlb_refill:
    la        t0, except_xtlb_refill_default
    jr        t0


 
/*
************************************************************************
*                                                                      *
*     0x0000 0100:   Cache error                                       *
*                                                                      *
************************************************************************
*/
.skip    0x0100 - ( . - VECTOR_BASE )
.global __except_cache_error
__except_cache_error:
    la        t0, except_cache_error_default
    jr        t0



/*
************************************************************************
*                                                                      *
*     0x0000 0180:   Genenal                                           *
*                                                                      *
************************************************************************
*/
.skip    0x0180 - ( . - VECTOR_BASE )
.global __except_general
__except_general:    

                             
    la        k0, Handler_GeneralExc
    jr        k0    
	nop

.skip    0x0200-(.-VECTOR_BASE)

.global _vectors_table_end
_vectors_table_end:
