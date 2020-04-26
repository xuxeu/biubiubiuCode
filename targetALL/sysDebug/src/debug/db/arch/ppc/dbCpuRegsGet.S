#ifndef _KERNEL_DEBUG_
#ifdef CONFIG_CORE_SMP

#include "dbAsm.h"

#define	_PPC_REG_SIZE		4	/* default register size */
#define GREG_NUM	32	/* has 32 32/64-bit data registers */

#define REG_SET_GRBASE	0x00	/* general purpose register base */
#define REG_SET_GR(n)	(REG_SET_GRBASE + _PPC_REG_SIZE*(n))
#define REG_SET_PC	(REG_SET_GR(GREG_NUM))
#define REG_SET_MSR	(REG_SET_PC + _PPC_REG_SIZE)
#define REG_SET_CR (REG_SET_MSR + _PPC_REG_SIZE)
#define REG_SET_LR (REG_SET_CR + _PPC_REG_SIZE)
#define REG_SET_CTR (REG_SET_LR + _PPC_REG_SIZE)
#define REG_SET_XER (REG_SET_CTR + _PPC_REG_SIZE)


#define	FRAMEBASESZ		16	/* minimum stack frame size */

/* Macro for hiadjust and lo */
#define HIADJ(arg)	arg@ha
#define HI(arg)		arg@h
#define LO(arg)		arg@l


/* Assembler barfs on sizeof(), so do it this way instead */
#define	SIZ_UINT32	4	/* sizeof(UINT32) */
#define	SIZ_UINT16	2	/* sizeof(UINT16) */

#define _PPC_ESF_SP_OFF		(0)
#define _PPC_ESF_VEC_OFF	(_PPC_ESF_SP_OFF + _PPC_REG_SIZE) /* fullword */
#define _PPC_ESF_RES_OFF_H	(_PPC_ESF_VEC_OFF  + 0)		  /* halfword */
#define _PPC_ESF_VEC_OFF_H	(_PPC_ESF_VEC_OFF  + SIZ_UINT16)  /* halfword */
#define _PPC_ESF_ERRNO		(_PPC_ESF_VEC_OFF  + SIZ_UINT32)
#if     ((CPU == PPC403) || (CPU == PPC405) || (CPU == PPC405F))
# define _PPC_ESF_DEAR		(_PPC_ESF_ERRNO    + SIZ_UINT32)
# define _PPC_ESF_BEAR		(_PPC_ESF_DEAR     + _PPC_REG_SIZE)
# define _PPC_ESF_BESR		(_PPC_ESF_BEAR     + SIZ_UINT32)
# define _PPC_ESF_REG_BASE	(_PPC_ESF_BESR     + SIZ_UINT32)
#elif	(CPU == PPC440)
# define _PPC_ESF_DEAR		(_PPC_ESF_ERRNO    + SIZ_UINT32)
# ifdef  _PPC_MSR_FP
#  define _PPC_ESF_FPCSR	(_PPC_ESF_DEAR     + _PPC_REG_SIZE)
# else  /* _PPC_MSR_FP */
#  define _PPC_ESF_PAD3		(_PPC_ESF_DEAR     + _PPC_REG_SIZE)
# endif /* _PPC_MSR_FP */
# define _PPC_ESF_MCSR		(_PPC_ESF_DEAR     + _PPC_REG_SIZE + SIZ_UINT32)
# define _PPC_ESF_REG_BASE	(_PPC_ESF_MCSR     + SIZ_UINT32)
#elif	(CPU == PPC85XX)
# define _PPC_ESF_DEAR		(_PPC_ESF_ERRNO    + SIZ_UINT32)
# define _PPC_ESF_ESR		(_PPC_ESF_DEAR     + _PPC_REG_SIZE)
# define _PPC_ESF_IARR		(_PPC_ESF_ESR      + SIZ_UINT32)
# ifdef PPC_e500mc
#  define _PPC_ESF_FPCSR	(_PPC_ESF_IARR	   + SIZ_UINT32)
#  define _PPC_ESF_REG_BASE	(_PPC_ESF_FPCSR    + SIZ_UINT32)
# else
#  define _PPC_ESF_REG_BASE	(_PPC_ESF_IARR     + SIZ_UINT32)
# endif
#else	/* CPU == PPC4xx/PPC85xx */
# define _PPC_ESF_DAR		(_PPC_ESF_ERRNO    + SIZ_UINT32)
# define _PPC_ESF_DSISR		(_PPC_ESF_DAR	   + _PPC_REG_SIZE)
# define _PPC_ESF_FPCSR		(_PPC_ESF_DSISR	   + SIZ_UINT32)
# define _PPC_ESF_REG_BASE	(_PPC_ESF_FPCSR    + SIZ_UINT32)
#endif	/* CPU == PPC4xx/PPC85xx */
#if	(CPU == PPC440 && defined(PPC_440x5))
# define _PPC_ESF_BESR		(_PPC_ESF_REG_BASE + _PPC_REG_SET_SIZE)
# define _PPC_ESF_BEAR_H	(_PPC_ESF_BESR     + SIZ_UINT32)
# define _PPC_ESF_BEAR_L	(_PPC_ESF_BEAR_H   + SIZ_UINT32)
#else	/* PPC440 && PPC_440x5 */
# if	(CPU == PPC405F)
#  define _PPC_ESF_FPCSR	(_PPC_ESF_REG_BASE + _PPC_REG_SET_SIZE)
#  define _PPC_ESF_PAD1		(_PPC_ESF_FPCSR    + SIZ_UINT32)
# else	/* CPU == PPC405F */
#  define _PPC_ESF_PAD0		(_PPC_ESF_REG_BASE + _PPC_REG_SET_SIZE)
#  define _PPC_ESF_PAD1		(_PPC_ESF_PAD0     + SIZ_UINT32)
# endif	/* CPU == PPC405F */
# define _PPC_ESF_PAD2		(_PPC_ESF_PAD1     + SIZ_UINT32)
#endif 	/* PPC440 && PPC_440x5*/

#define _PPC_ESF_R(n)		(_PPC_ESF_REG_BASE + (n) * _PPC_REG_SIZE)

#define _PPC_ESF_MSR		_PPC_ESF_R(GREG_NUM)
#define _PPC_ESF_LR		_PPC_ESF_R(GREG_NUM + 1)
#define _PPC_ESF_CTR		_PPC_ESF_R(GREG_NUM + 2)
#define _PPC_ESF_PC		_PPC_ESF_R(GREG_NUM + 3)
#define _PPC_ESF_CR		_PPC_ESF_R(GREG_NUM + 4)
#define _PPC_ESF_XER		(_PPC_ESF_CR + 4)

#define _PPC_ESF_R0		_PPC_ESF_R(0)
#define _PPC_ESF_R1		_PPC_ESF_R(1)
#define _PPC_ESF_R2		_PPC_ESF_R(2)
#define _PPC_ESF_R3		_PPC_ESF_R(3)
#define _PPC_ESF_R4		_PPC_ESF_R(4)
#define _PPC_ESF_R5		_PPC_ESF_R(5)
#define _PPC_ESF_R6		_PPC_ESF_R(6)
#define _PPC_ESF_R7		_PPC_ESF_R(7)
#define _PPC_ESF_R8		_PPC_ESF_R(8)
#define _PPC_ESF_R9		_PPC_ESF_R(9)
#define _PPC_ESF_R10		_PPC_ESF_R(10)
#define _PPC_ESF_R11		_PPC_ESF_R(11)
#define _PPC_ESF_R12		_PPC_ESF_R(12)
#define _PPC_ESF_R13		_PPC_ESF_R(13)
#define _PPC_ESF_R14		_PPC_ESF_R(14)
#define _PPC_ESF_R15		_PPC_ESF_R(15)
#define _PPC_ESF_R16		_PPC_ESF_R(16)
#define _PPC_ESF_R17		_PPC_ESF_R(17)
#define _PPC_ESF_R18		_PPC_ESF_R(18)
#define _PPC_ESF_R19		_PPC_ESF_R(19)
#define _PPC_ESF_R20		_PPC_ESF_R(20)
#define _PPC_ESF_R21		_PPC_ESF_R(21)
#define _PPC_ESF_R22		_PPC_ESF_R(22)
#define _PPC_ESF_R23		_PPC_ESF_R(23)
#define _PPC_ESF_R24		_PPC_ESF_R(24)
#define _PPC_ESF_R25		_PPC_ESF_R(25)
#define _PPC_ESF_R26		_PPC_ESF_R(26)
#define _PPC_ESF_R27		_PPC_ESF_R(27)
#define _PPC_ESF_R28		_PPC_ESF_R(28)
#define _PPC_ESF_R29		_PPC_ESF_R(29)
#define _PPC_ESF_R30		_PPC_ESF_R(30)
#define _PPC_ESF_R31		_PPC_ESF_R(31)

#define	_PPC_ESF_P0		_PPC_ESF_R3
#define	_PPC_ESF_P1		_PPC_ESF_R4
#define	_PPC_ESF_P2		_PPC_ESF_R5
#define	_PPC_ESF_P3		_PPC_ESF_R6
#define	_PPC_ESF_P4		_PPC_ESF_R7
#define	_PPC_ESF_P5		_PPC_ESF_R8
#define	_PPC_ESF_P6		_PPC_ESF_R9
#define	_PPC_ESF_P7		_PPC_ESF_R10

/*fixmeyin: 暂时注释，为工具提供*/
#if 1
/*******************************************************************************
*	  handling using the standard exit path.
*
* This routine is uninstalled from the interrupt handling by the VxWorks debug
* library (VxDBG) using vxdbgCpuCtrlDisable(), when the debugger resumes the
* OS activity on all CPU(s).
*
* void intVxdbgCpuRegsGet (void)
*
* NOMANUAL
*/
.globl taIntVxdbgCpuRegsGet

taIntVxdbgCpuRegsGet:

	/*
	 * Retrieve the pointer to the per-processor register set array for
	 * current CPU: &vxdbgCpuCtrlVars[_WRS_CPU_INDEX_GET()]
	 */

//	_VXDBG_PER_CPU_PREGS_GET(p1)		/* Get per cpu REG_SET addr */
    //_PPC_CPU_INDEX_GET(r5)
    mulli r5,r5,0x1A8
    addis   r5, r5, HIADJ(taSDASmpExpContext);				\
	addi    r5, r5, LO(taSDASmpExpContext);

	/*
	 * First copy the registers that have already been saved by intEnt()
	 * on the interrupt stack.
	 */

	lwz	p0, _PPC_ESF_R0(sp)		/* read R0 from stack */
	stw	p0, REG_SET_GR(0)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_R1(sp)		/* read R1/SP from stack */
	stw	p0, REG_SET_GR(1)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_R2(sp)		/* read R2 from stack */
	stw	p0, REG_SET_GR(2)(r5)		/* save it to REG_SET array */

	lwz	p0, _PPC_ESF_P0(sp)		/* read R3/P0 from stack */
	stw	p0, REG_SET_GR(3)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_P1(sp)		/* read R4/P1 from stack */
	stw	p0, REG_SET_GR(4)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_P2(sp)		/* read R5/P2 from stack */
	stw	p0, REG_SET_GR(5)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_P3(sp)		/* read R6/P3 from stack */
	stw	p0, REG_SET_GR(6)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_P4(sp)		/* read R7/P4 from stack */
	stw	p0, REG_SET_GR(7)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_P5(sp)		/* read R8/P5 from stack */
	stw	p0, REG_SET_GR(8)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_P6(sp)		/* read R9/P6 from stack */
	stw	p0, REG_SET_GR(9)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_P7(sp)		/* read R10/P7 from stack */
	stw	p0, REG_SET_GR(10)(r5)		/* save it to REG_SET array */

	lwz	p0, _PPC_ESF_R11(sp)		/* read R11 from stack */
	stw	p0, REG_SET_GR(11)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_R12(sp)		/* read R12 from stack */
	stw	p0, REG_SET_GR(12)(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_R13(sp)		/* read R13 from stack */
	stw	p0, REG_SET_GR(13)(r5)		/* save it to REG_SET array */

	lwz	p0, _PPC_ESF_XER(sp)		/* read XER from stack */
	stw	p0, REG_SET_XER(r5)		/* save it to REG_SET array */
	lwz	p0, _PPC_ESF_CTR(sp)		/* read CTR from stack */
	stw	p0, REG_SET_CTR(r5)		/* save it to REG_SET array */

#ifdef	_WRS_CONFIG_VM_MULTIPLE_CONTEXT
#ifdef	_WRS_MMU_CLASS_SR_REGS
	lwz	p0, _PPC_ESF_SR_TBL_PTR (sp)	/* read srTbl ptr from stack */
	stw	p0, REG_SET_SR_TBL_PTR(r5)	/* save it to REG_SET array */
#endif	/* _WRS_MMU_CLASS_SR_REGS */
#endif	/* _WRS_CONFIG_VM_MULTIPLE_CONTEXT */

#if	(CPU==PPC85XX)
#ifdef _WRS_SPE_SUPPORT
	lwz	p0, _PPC_ESF_SPEFSCR(sp)	/* read SPEFSCR from stack */
	stw	p0, REG_SET_SPEFSCR(r5)		/* save it to REG_SET array */
#endif  /* _WRS_SPE_SUPPORT */
#endif	/* (CPU==PPC85XX) */

	lwz	p0, _PPC_ESF_LR(sp)		/* read LR from stack */
	stw	p0, REG_SET_LR(r5)		/* save it to REG_SET array */

	lwz	p0, _PPC_ESF_CR(sp)		/* read CR from stack */
	stw	p0, REG_SET_CR(r5)		/* save it to REG_SET array */

	lwz	p0, _PPC_ESF_PC(sp)		/* read PC from stack */
	stw	p0, REG_SET_PC(r5)		/* save it to REG_SET array */

	lwz	p0, _PPC_ESF_MSR(sp)		/* read MSR from stack */
	stw	p0, REG_SET_MSR(r5)		/* save it to REG_SET array */


	/* Save remaining registers: registers not saved on stack by intEnt() */

	stw	r14, REG_SET_GR(14)(r5)
	stw	r15, REG_SET_GR(15)(r5)
	stw	r16, REG_SET_GR(16)(r5)
	stw	r17, REG_SET_GR(17)(r5)
	stw	r18, REG_SET_GR(18)(r5)
	stw	r19, REG_SET_GR(19)(r5)
	stw	r20, REG_SET_GR(20)(r5)
	stw	r21, REG_SET_GR(21)(r5)
	stw	r22, REG_SET_GR(22)(r5)
	stw	r23, REG_SET_GR(23)(r5)
	stw	r24, REG_SET_GR(24)(r5)
	stw	r25, REG_SET_GR(25)(r5)
	stw	r26, REG_SET_GR(26)(r5)
	stw	r27, REG_SET_GR(27)(r5)
	stw	r28, REG_SET_GR(28)(r5)
	stw	r29, REG_SET_GR(29)(r5)
	stw	r30, REG_SET_GR(30)(r5)
	stw	r31, REG_SET_GR(31)(r5)

	/*
	 * Now that all the registers have been saved to the per-processor
	 * register set array, we must enter into the VxDBG CPU control loop.
	 * This control loop will be exited when the debugger will decide to
	 * resume the OS activity on all CPUs.
	 */

	stwu	sp, -FRAMEBASESZ(sp)	/* carve stack frame */
//	bl	FUNC(vxdbgCpuCtrlLoop)	/* Enter VxDBG CPU control loop */
	bl	sdaCpuCtrlHandler	/* Enter VxDBG CPU control loop */
	addi	sp, sp, FRAMEBASESZ	/* release stack */

	/* Exit interrupt handling using the standard interrupt exit routine */

//	b	FUNC(intExit)		/* branch to interrupt exit */
	b	intExit		/* branch to interrupt exit */
.end taIntVxdbgCpuRegsGet
#endif
#endif	/* _WRS_CONFIG_SMP */
#endif
