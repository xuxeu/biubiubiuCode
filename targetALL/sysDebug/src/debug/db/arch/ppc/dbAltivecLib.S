#include "dbAsm.h"

/**********************************************************************/
#ifdef TA_ALTIVEC_SUPPORT_DEBUG

#define	ALTIVEC_NUM_REGS	32

#define	AVX		0x0
#define AVX_OFFSET(n)	(AVX + (n)*16)
#define VSCR_OFFSET     (AVX + (ALTIVEC_NUM_REGS)*16)   /* offset == 0x200 */
#define VRSAVE_OFFSET   (VSCR_OFFSET + 16)              /* offset == 0x210 */
#define AVCONTEXT_SIZE	16*(ALTIVEC_NUM_REGS + 2)


#define _PPC_MSR_VEC        0x0200      /* Bit 6 of MSR                      */
#define _PPC_MSR_BIT_VEC  	06	/* MSR Altivec Available bit - VEC */

#define VRSAVE_REG                      256  /* VRSAVE Register */

/* ALTIVEC Vector register names */

#define v0      0
#define v1      1
#define v2      2
#define v3      3
#define v4      4
#define v5      5
#define v6      6
#define v7      7
#define v8      8
#define v9      9
#define v10     10
#define v11     11
#define v12     12
#define v13     13
#define v14     14
#define v15     15
#define v16     16
#define v17     17
#define v18     18
#define v19     19
#define v20     20
#define v21     21
#define v22     22
#define v23     23
#define v24     24
#define v25     25
#define v26     26
#define v27     27
#define v28     28
#define v29     29
#define v30     30
#define v31     31


.globl taAltiVecSave
.globl taAltiVecRestore
.text
/**************************************************************************
* altivecSave -
* This routine writes the contents of Vector file register to the specified
* memory.
*
* void altivecSave (ALTIVEC_CONTEXT * pAltiCtx)
*/
FUNC_BEGIN(taAltiVecSave)
        mfmsr  p1                       /*  load MSR             */
        mr     p2, p1                   /*  Save the MSR read.   */
        oris   p2, p2, _PPC_MSR_VEC     /*  set ALTIVEC Bit      */
#ifndef	PPC_745x
	sync
#endif	/* PPC_745x */
	mtmsr  p2                       /*  restore MSR          */
#ifdef	PPC_745x
	isync
#else	/* PPC_745x */
	sync
#endif	/* PPC_745x */

        mfspr  p2,  VRSAVE_REG
        stw    p2, VRSAVE_OFFSET(p0)    /*  Save vrsave register.  */
        sync

        stvx    v0, 0, p0
        addi    p3, p0, AVX_OFFSET(1)

        addi  p4,p0,VSCR_OFFSET
        sync
        mfvscr v0
        sync
        stvx    v0,0,p4

        lvx     v0,0,p0                 /* Save v0 */

        stvx    v1, 0,  p3
        addi    p3, p0, AVX_OFFSET(2)
        stvx    v2, 0,  p3
        addi    p3, p0, AVX_OFFSET(3)
        stvx    v3, 0,  p3
        addi    p3, p0, AVX_OFFSET(4)
        stvx    v4, 0,  p3
        addi    p3, p0, AVX_OFFSET(5)
        stvx    v5, 0,  p3
        addi    p3, p0, AVX_OFFSET(6)
        stvx    v6, 0,  p3
        addi    p3, p0, AVX_OFFSET(7)
        stvx    v7, 0,  p3
        addi    p3, p0, AVX_OFFSET(8)
        stvx    v8, 0,  p3
        addi    p3, p0, AVX_OFFSET(9)
        stvx    v9, 0,  p3
        addi    p3, p0, AVX_OFFSET(10)
        stvx    v10, 0,  p3
        addi    p3, p0, AVX_OFFSET(11)
        stvx    v11, 0,  p3
        addi    p3, p0, AVX_OFFSET(12)
        stvx    v12, 0,  p3
        addi    p3, p0, AVX_OFFSET(13)
        stvx    v13, 0,  p3
        addi    p3, p0, AVX_OFFSET(14)
        stvx    v14, 0, p3
        addi    p3, p0, AVX_OFFSET(15)
        stvx    v15, 0, p3
        addi    p3, p0, AVX_OFFSET(16)
        stvx    v16, 0, p3
        addi    p3, p0, AVX_OFFSET(17)
        stvx    v17, 0, p3
        addi    p3, p0, AVX_OFFSET(18)
        stvx    v18, 0, p3
        addi    p3, p0, AVX_OFFSET(19)
        stvx    v19, 0, p3
        addi    p3, p0, AVX_OFFSET(20)
        stvx    v20, 0, p3
        addi    p3, p0, AVX_OFFSET(21)
        stvx    v21, 0, p3
        addi    p3, p0, AVX_OFFSET(22)
        stvx    v22, 0, p3
        addi    p3, p0, AVX_OFFSET(23)
        stvx    v23, 0, p3
        addi    p3, p0, AVX_OFFSET(24)
        stvx    v24, 0, p3
        addi    p3, p0, AVX_OFFSET(25)
        stvx    v25, 0, p3
        addi    p3, p0, AVX_OFFSET(26)
        stvx    v26, 0, p3
        addi    p3, p0, AVX_OFFSET(27)
        stvx    v27, 0, p3
        addi    p3, p0, AVX_OFFSET(28)
        stvx    v28, 0, p3
        addi    p3, p0, AVX_OFFSET(29)
        stvx    v29, 0, p3
        addi    p3, p0, AVX_OFFSET(30)
        stvx    v30, 0, p3
        addi    p3, p0, AVX_OFFSET(31)
        stvx    v31, 0, p3
        mtmsr  p1                        /*  restore MSR from function entry */
#ifdef	PPC_745x
	isync
#else	/* PPC_745x */
	sync
#endif	/* PPC_745x */
        blr
FUNC_END(taAltiVecSave)


/**************************************************************************
* altivecRestore -
* This routine writes to the vectore Registers from the memory specified.
*
* void altivecRestore (ALTIVEC_CONTEXT * pAltiCtx)
*/

FUNC_BEGIN(taAltiVecRestore)
        mfmsr  p1                       /*  load MSR             */
        mr     p2, p1                   /*  Save the MSR read.   */
        oris   p2, p2, _PPC_MSR_VEC     /*  set ALTIVEC Bit      */
#ifndef	PPC_745x
	sync
#endif	/* PPC_745x */
	mtmsr  p2                       /*  restore MSR          */
#ifdef	PPC_745x
	isync
#else	/* PPC_745x */
	sync
#endif	/* PPC_745x */

        lwz    p2, VRSAVE_OFFSET(p0)    /*  Restore vrsave register.  */
        mtspr  VRSAVE_REG, p2
        sync

        addi  p4,p0,VSCR_OFFSET
        lvx    v0, 0, p4
        mtvscr v0                       /*  Restore vscr register.  */
        sync

        lvx     v0, 0, p0
        addi    p3, p0, AVX_OFFSET(1)
        lvx     v1, 0, p3
        addi    p3, p0, AVX_OFFSET(2)
        lvx     v2, 0, p3
        addi    p3, p0, AVX_OFFSET(3)
        lvx     v3, 0, p3
        addi    p3, p0, AVX_OFFSET(4)
        lvx     v4, 0, p3
        addi    p3, p0, AVX_OFFSET(5)
        lvx     v5, 0, p3
        addi    p3, p0, AVX_OFFSET(6)
        lvx     v6, 0, p3
        addi    p3, p0, AVX_OFFSET(7)
        lvx     v7, 0, p3
        addi    p3, p0, AVX_OFFSET(8)
        lvx     v8, 0, p3
        addi    p3, p0, AVX_OFFSET(9)
        lvx     v9, 0, p3
        addi    p3, p0, AVX_OFFSET(10)
        lvx     v10, 0, p3
        addi    p3, p0, AVX_OFFSET(11)
        lvx     v11, 0, p3
        addi    p3, p0, AVX_OFFSET(12)
        lvx     v12, 0, p3
        addi    p3, p0, AVX_OFFSET(13)
        lvx     v13, 0, p3
        addi    p3, p0, AVX_OFFSET(14)
        lvx     v14, 0, p3
        addi    p3, p0, AVX_OFFSET(15)
        lvx     v15, 0, p3
        addi    p3, p0, AVX_OFFSET(16)
        lvx     v16, 0, p3
        addi    p3, p0, AVX_OFFSET(17)
        lvx     v17, 0, p3
        addi    p3, p0, AVX_OFFSET(18)
        lvx     v18, 0, p3
        addi    p3, p0, AVX_OFFSET(19)
        lvx     v19, 0, p3
        addi    p3, p0, AVX_OFFSET(20)
        lvx     v20, 0, p3
        addi    p3, p0, AVX_OFFSET(21)
        lvx     v21, 0, p3
        addi    p3, p0, AVX_OFFSET(22)
        lvx     v22, 0, p3
        addi    p3, p0, AVX_OFFSET(23)
        lvx     v23, 0, p3
        addi    p3, p0, AVX_OFFSET(24)
        lvx     v24, 0, p3
        addi    p3, p0, AVX_OFFSET(25)
        lvx     v25, 0, p3
        addi    p3, p0, AVX_OFFSET(26)
        lvx     v26, 0, p3
        addi    p3, p0, AVX_OFFSET(27)
        lvx     v27, 0, p3
        addi    p3, p0, AVX_OFFSET(28)
        lvx     v28, 0, p3
        addi    p3, p0, AVX_OFFSET(29)
        lvx     v29, 0, p3
        addi    p3, p0, AVX_OFFSET(30)
        lvx     v30, 0, p3
        addi    p3, p0, AVX_OFFSET(31)
        lvx     v31, 0, p3
        mtmsr  p1                        /*  restore MSR from function entry */
#ifdef	PPC_745x
	isync
#else	/* PPC_745x */
	sync
#endif	/* PPC_745x */
        blr
FUNC_END(taAltiVecRestore)

#endif	/* TA_ALTIVEC_SUPPORT_DEBUG */
