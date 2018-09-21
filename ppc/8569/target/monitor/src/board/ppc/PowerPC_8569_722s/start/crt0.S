#include "dbAsm.h"

/*
 * Some releases of h/arch/ppc/toolPpc.h had bad definitions of
 * LOADPTR and LOADVAR. So we will define it correctly.
 * [REMOVE THESE FOR NEXT MAJOR RELEASE].
 *
 * LOADPTR initializes a register with a 32 bit constant, presumably the
 * address of something.
 */
#define	_PPC_HID0_TBEN_U	0x0400		/* Enable timebase & decr */
#undef LOADPTR
#define	LOADPTR(reg, const32) \
	addis reg, r0, HIADJ(const32); addi reg, reg, LO(const32)

/*
 * LOADVAR initializes a register with the contents of a specified memory
 * address. The difference being that the value loaded is the contents of
 * the memory location and not just the address of it.
 */

#undef LOADVAR
#define	LOADVAR(reg, addr32) \
	addis reg, r0, HIADJ(addr32); lwz reg, LO(addr32)(reg)

#undef WRITEADR
#define WRITEADR(reg1, reg2, addr32, val) \
		lis	reg1, HI(addr32); \
		ori	reg1, reg1, LO(addr32); \
		lis	reg2, HI(val); \
		ori	reg2, reg2, LO(val); \
		stw	reg2, 0(reg1)

/* globals */
#define  FUNC_EXPORT(a) .global a
#define FUNC(func)          func
#define FUNC_LABEL(func)    func:
#define FUNC_BEGIN(func)        FUNC_LABEL(func)
#define FUNC_END(func)          .size   FUNC(func), . - FUNC(func)

	FUNC_EXPORT(_sysInit)		/* start of system code */
	FUNC_EXPORT(sysInByte)
	FUNC_EXPORT(sysOutByte)
	FUNC_EXPORT(sysPciRead32)
	FUNC_EXPORT(sysPciWrite32)
	FUNC_EXPORT(sysPciInByte)
	FUNC_EXPORT(sysPciOutByte)
	FUNC_EXPORT(sysPciInWord)
	FUNC_EXPORT(sysPciOutWord)
	FUNC_EXPORT(sysPciInLong)
	FUNC_EXPORT(sysPciOutLong)
	FUNC_EXPORT(sysInWord)
	FUNC_EXPORT(sysOutWord)
	FUNC_EXPORT(sysInLong)
	FUNC_EXPORT(sysOutLong)
	FUNC_EXPORT(sysMemProbeSup)
	FUNC_EXPORT(sysSioWrite)
	FUNC_EXPORT(sysSioRead)
	FUNC_EXPORT(sysTas)
	FUNC_EXPORT(sysMsscr0Get)
#if defined(_WRS_CONFIG_SMP)
	FUNC_EXPORT(sysSmpTimeBaseSet)
#endif	/* _WRS_CONFIG_SMP */

	FUNC_EXPORT(sysCpu1Start)
	FUNC_EXPORT(sysCpu1Stop)
	/* externals */

/*******************************************************************************
*
* sysInit - start after boot
*
* This is the system start-up entry point for VxWorks in RAM, the
* first code executed after booting.  It disables interrupts, sets up
* the stack, and jumps to the C routine usrInit() in usrConfig.c.
*
* The initial stack is set to grow down from the address of sysInit().  This
* stack is used only by usrInit() and is never used again.  Memory for the
* stack must be accounted for when determining the system load address.
*
* NOTE: This routine should not be called by the user.
*
* RETURNS: N/A

* sysInit (int startType)		/@ THIS IS NOT A CALLABLE ROUTINE @/

*/
.text
.global _start
_start:
FUNC_BEGIN(_sysInit)

    mr      r8, r3
    xor     r3, r3,r3
    mtspr   340, r3
    xor     r6, r6, r6
    msync
    isync

    mtspr   1010, r6          /* Disable the Data cache           */
    li      r6, 0x00000002
    msync
    isync
#if 0
    mtspr   1010, r6          /* Invalidate the Data cache        */
    li      r6, 0x0000
    msync
    isync
#endif

    mtspr   1011, r6          /* Disable the Instruction cache    */
    li      r6, 0x00000002
    msync
    isync
#if 0
    mtspr   1011, r6          /* Invalidate the Instruction cache */
    isync

    li      r6, 0x00000000
    msync
    isync
#endif

#ifndef __PPC__
    mtspr   1011, r6          /* Enable the Instruction cache     */
    msync
    isync
#endif

    /* initialize the stack pointer */
	lwi     sp, _init_stack
    addi    sp, sp, SYS_INIT_STACK_SIZE 

    mr      r3, r8
	b	lmain	

FUNC_END(_sysInit)
.global _endsysInit
_endsysInit:
        b       .

