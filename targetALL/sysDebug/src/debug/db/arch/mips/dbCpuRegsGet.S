/*
 * dbCpuRegsGet.S
 *
 *  Created on: 2014-7-2
 *      Author: Administrator
 */
 //intVxdbgCpuRegsGet实现
#ifndef _KERNEL_DEBUG_
#ifdef CONFIG_CORE_SMP
#define _ASMLANGUAGE
#define _DELTA_INT_REGISTER_SIZE 8
#define _DELTA_FP_REGISTER_SIZE 8
#define _WRS_FP_REGISTER_SIZE 8

#include "esfMips.h"
#include "asmMips.h"
#include "archMips.h"
#include "regsMips.h"

#define	TA_MIPS_INT_DISABLE(oldSR, scratch, mask, hazard)  \
	.set	noreorder				;\
	mfc0	oldSR, C0_SR				;\
	HAZARD_CP_READ					;\
	and	scratch, oldSR, mask			;\
	mtc0	scratch, C0_SR				;\
	hazard						;\
	.set	reorder

#ifndef _KERNEL_DEBUG_
.globl taIntVxdbgCpuRegsGet
.globl	excIntStub_intVxdbgCpuRegsGetOpAddr

excIntStub_intVxdbgCpuRegsGetOpAddr:	/* VxDBG label identifying the addr */
					/* of the instruction to use to patch */
					/* the interrupt handling code */
	j	taIntVxdbgCpuRegsGet	/* Jump to VxDBG CPU register get rtn */

.ent	taIntVxdbgCpuRegsGet
taIntVxdbgCpuRegsGet:
	HAZARD_VR5400
	TA_MIPS_INT_DISABLE(t0, t0, ~SR_IE, HAZARD_CP_WRITE)

	/* we are on the interrupt stack here */

	/*
	 * Retrieve the pointer to the per-processor register set array for
	 * current CPU: &vxdbgCpuCtrlVars[_WRS_CPU_INDEX_GET()]
	 */

       //_VXDBG_PER_CPU_PREGS_GET(t1)	/* Get per cpu REG_SET addr */
	.set	mips64             /*待验证*/
		mfc0    k1, $15, 1
		andi    k1, 0x3ff
	.set	mips3
		dmul k1,k1,0x138 /*312 不保存浮点*/
	  	la   k0, taSDASmpExpContext
	  	add k0,k0,k1
	  	move t1,k0
	/*
	 * First copy the registers that have already been saved by excIntStub()
	 * on the interrupt stack.
	 */

	LW	t0, E_STK_SP(sp)	/* read SP from interrupt stack */
	SW	t0, SPREG(t1)		/* save it to REG_SET array */
	LW	t0, E_STK_AT(sp)
	SW	t0, ATREG(t1)
	LW	t0, E_STK_T7(sp)
	SW	t0, T7REG(t1)
	LW	t0, E_STK_T8(sp)
	SW	t0, T8REG(t1)
	LW	t0, E_STK_T9(sp)
	SW	t0, T9REG(t1)
	LW	t0, E_STK_V0(sp)
	SW	t0, V0REG(t1)
	lw	t0, E_STK_SR(sp)
	sw	t0, SRREG(t1)
	lw	t0, E_STK_EPC(sp)
	sw	t0, PCREG(t1)
	LW	t0, E_STK_RA(sp)
	SW	t0, RAREG(t1)
	LW	t0, E_STK_T1(sp)
	SW	t0, T1REG(t1)
	LW	t0, E_STK_T2(sp)
	SW	t0, T2REG(t1)
	LW	t0, E_STK_T3(sp)
	SW	t0, T3REG(t1)
	LW	t0, E_STK_ZERO(sp)
	SW	t0, ZEROREG(t1)
	LW	t0, E_STK_V1(sp)
	SW	t0, V1REG(t1)
	LW	t0, E_STK_A0(sp)
	SW	t0, A0REG(t1)
	LW	t0, E_STK_A1(sp)
	SW	t0, A1REG(t1)
	LW	t0, E_STK_A2(sp)
	SW	t0, A2REG(t1)
	LW	t0, E_STK_A3(sp)
	SW	t0, A3REG(t1)
	LW	t0, E_STK_T0(sp)
	SW	t0, T0REG(t1)
	LW	t0, E_STK_T4(sp)
	SW	t0, T4REG(t1)
	LW	t0, E_STK_T5(sp)
	SW	t0, T5REG(t1)
	LW	t0, E_STK_T6(sp)
	SW	t0, T6REG(t1)
	LW	t0, E_STK_LO(sp)
	SW	t0, LOREG(t1)
	LW	t0, E_STK_HI(sp)
	SW	t0, HIREG(t1)
	LW	t0, E_STK_TLBHI(sp)
	SW	t0, TLBHIREG(t1)

        /* set WIND_TCB_TASK_CONTEXT in REG_SET to zero flagging a full
         * register restore is needed
         */
        sw      zero, ULEXTRA1(t1)

#ifdef MIPSXX_cav_cn3xxx
	ld	t0, E_STK_P0(sp)
	sd	t0, P0REG(t1)
	ld	t0, E_STK_P1(sp)
	sd	t0, P1REG(t1)
	ld	t0, E_STK_P2(sp)
	sd	t0, P2REG(t1)
	ld	t0, E_STK_MPL0(sp)
	sd	t0, MPL0REG(t1)
	ld	t0, E_STK_MPL1(sp)
	sd	t0, MPL1REG(t1)
	ld	t0, E_STK_MPL2(sp)
	sd	t0, MPL2REG(t1)

	ld	t0, E_STK_TLO(sp)
	sd	t0, TLOREG(t1)
	ld	t0, E_STK_THI(sp)
	sd	t0, THIREG(t1)
#endif /* MIPSXX_cav_cn3xxx */

#if defined(MIPSXX_xlr)
	ld	t0, E_STK_EIMR(sp)
	sd      t0, EIMRREG(t1)
#endif  /* MIPSXX_xlr */

	/* Save remaining registers: regs not saved on stack by excIntStub() */

	SW	s0, S0REG(t1)
	SW	s1, S1REG(t1)
	SW	s2, S2REG(t1)
	SW	s3, S3REG(t1)
	SW	s4, S4REG(t1)
	SW	s5, S5REG(t1)
	SW	s6, S6REG(t1)
	SW	s7, S7REG(t1)

	SW	k0, K0REG(t1)
	SW	k1, K1REG(t1)
	SW	gp, GPREG(t1)
	SW	s8, S8REG(t1)

	/*
	 * Now that all the registers have been saved to the per-processor
	 * register set array, we must enter into the VxDBG CPU control loop.
	 * This control loop will be exited when the debugger will decide to
	 * resume the OS activity on all CPUs.
	 */

#ifndef _WRS_MIPS_N32_ABI
	SETFRAME(taIntVxdbgCpuRegsGet,0,0)
	subu	sp, FRAMESZ(taIntVxdbgCpuRegsGet)	/* allocate stack frame */
#endif /* _WRS_MIPS_N32_ABI */

	//jal	vxdbgCpuCtrlLoop	/* Enter VxDBG CPU control loop */

	jal sdaCpuCtrlHandler

#ifndef	_WRS_MIPS_N32_ABI
	addu sp, FRAMESZ(taIntVxdbgCpuRegsGet)	/* adjust frame back */
#endif	/* _WRS_MIPS_N32_ABI */

	/* flag we need a complete restore of regs from esf */

	li	t3, 1
	sw	t3,E_STK_PARAM2(sp)

	/* Exit interrupt handling using the standard interrupt exit routine */

	j	intExit			/* jump to default interrupt exit */

	.end	taIntVxdbgCpuRegsGet
#endif
#endif
#endif
