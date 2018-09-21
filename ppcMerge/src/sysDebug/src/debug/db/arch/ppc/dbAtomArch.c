/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2011-02-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbAtomArch.c
 * @brief:
 *             <li>ta���Թ�����ϵ�ṹ��ص�ԭ�Ӳ���</li>
 */

/************************ͷ �� ��*****************************/
#include "taErrorDefine.h"
#include "taUtil.h"
#include "dbAtom.h"
#include "dbSignal.h"
#include "dbVector.h"
#include "dbContext.h"
/************************�� �� ��******************************/
/************************���Ͷ���*****************************/
#ifdef __PPC85XX__
#define BRANCH_MASK             0xF0000000
#define OP_BRANCH               0x40000000
#define BCCTR_MASK              0xFC0007FE
#define BCLR_MASK               0xFC0007FE
#define AA_MASK                 0x00000002

#define BO_NB_BIT               5       /* nb bits of BO field    */
#define CR_NB_BIT               32      /* nb bits of CR register */
#define _OP(opcd, xo)           ((opcd << 26) + (xo << 1))
#define INST_BCCTR              _OP(19, 528)
#define INST_BCLR               _OP(19, 16)

/* �쳣����PPC�ļĴ��������Ķ���*/#define _IFIELD_BI(x)           ((0x001f0000 & x) >> 16) /* branch condition */
#define _IFIELD_BO(x)           ((0x03e00000 & x) >> 21) /* branch options */
#define _IFIELD_LI(x)           ((0x02000000 & x) ? ((0xf6000000 | x) & ~3) : ((0x03fffffc & x) & ~3))
#define _IFIELD_BD(x)           ((0x00008000 & x) ? (0xffff0000 | (x & ~3)) : (0x0000fffc & x))
#define _REG32_BIT(reg, no)     (0x00000001 & (reg >> (no)))

#define _RType unsigned int
#define GREG_NUM	32	/* has 32 32/64-bit data registers */

typedef struct		/* REG_SET */
    {
    _RType gpr[GREG_NUM];	/* general pourpose registers */
    _RType msr;			/* machine state register */
    _RType lr;			/* link register */
    _RType ctr;			/* count register */
    _RType pc;			/* program counter */
    _RType cr;			/* condition register */
    _RType xer;			/* fixed-point exception register */

    /* the following fields keep this structure compatible with the kernel */

    _RType spare0;		/* MQ/base of page table */
    _RType spare1;		/* SR regs */
    _RType spare2;		/* SR regs */
    _RType spare3;		/* keeps struct size consistent */
    _RType spare4;		/* keeps struct size consistent */

    } REG_SET;

static UINT32 * daGetNpc(REG_SET * pRegs);

UINT32* gSaveNextPC;
static UINT32  gSaveNextInt;

UINT32 gSoftStepFlag = FALSE;

#endif

/************************�ⲿ����*****************************/
/* �ϵ��쳣���� */
extern void taBpExceptionStub(void);
/************************ǰ������*****************************/

/************************ģ�����*****************************/
#ifdef 	CONFIG_CORE_SMP
static T_UWORD taTasLock =0;
#endif
/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *      ��ϵ�ṹ��ʼ����Ŀǰ��Ҫ�����ϵ�ṹ���RSP�����
 * @return:
 *     TRUE: ��ʼ���ɹ�
 *     FALSE: ��ʼ��ʧ��
 */
BOOL taDebugArchInit(void)
{
    /*��װ�����쳣*/
    taInstallVectorHandle(GENERAL_DB_INT, taBpExceptionStub);
	/* ��װ����ϵ��쳣 */
    taInstallVectorHandle(GENERAL_TRAP_INT, taBpExceptionStub);

    /* ��װӲ��ָ��ϵ��쳣 */
	taInstallVectorHandle(GENERAL_IAB_INT, taBpExceptionStub);

	/* ��װӲ�����ݶϵ��쳣 */
	taInstallVectorHandle(GENERAL_HBP_INT, taBpExceptionStub);


    return (TRUE);
}

/*
 * @brief:
 *     ���쳣������ת���ɱ�׼POSIX�źű�ʾ��ʽ
 * @param[in]:vector:�쳣������
 * @param[in]:context:�����쳣������
 * @return:
 *     ���������Ŷ�Ӧ��POSIX�ź�
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context)
{
    UINT32 signal = SIGNAL_TERM;

    /* �ж����������� */
    switch(vector)
    {
        case GENERAL_MC_INT:
            signal = SIGNAL_SEGV;
            break;

        case GENERAL_DAE_INT:
            signal = SIGNAL_SEGV;
            break;

        case GENERAL_PAGE_INT:
            signal = SIGNAL_SEGV;
            break;

        case GENERAL_AE_INT:
            signal = SIGNAL_BUS;
            break;

        case GENERAL_TRAP_INT:
           signal = SIGNAL_TRAP;
           break;

        case GENERAL_MF_INT:
           signal = SIGNAL_FPE;
           break;

        case GENERAL_ILLINS_INT:
           signal = SIGNAL_ILL;
           break;

        case GENERAL_UF_INT:
            signal = SIGNAL_FPE;
            break;

        case GENERAL_DB_INT:
            signal = SIGNAL_TRAP;
            break;

        case GENERAL_IAB_INT:
            signal = SIGNAL_TRAP;
            break;

        default:
            break;
    }

    return (signal);
}

/*
 * @brief
 *     ���������û��Ĵ���
 * @param[in] context:�쳣������
 * @param[in] registerNum:�Ĵ�����
 * @param[out] outBuf:���buffer
 * @param[out] outBufferLen::�������ĳ���
 * @return
 *     TA_OK:�����ɹ�
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 *     TA_OUT_OF_BUFFER:������峤�Ȳ���
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext* context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen)
{

    /* ��ȡ�ļĴ���Ϊͨ�üĴ��� */
    if ( registerNum < GPR_SUM )
    {
        /* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
        mem2hex( (UINT8*)&(context->registers[registerNum]), outBuf, DB_REGISTER_UNIT_LEN );

        *outBufferLen = DB_REGISTER_SIZE;

        return (TA_OK);
    }

    /* ��ȡ�ļĴ���Ϊ����Ĵ��� */
    if ( (registerNum >= GPR_SUM) && (registerNum < GPR_SUM+FGPR_SUM) )
    {
#ifdef __PPC85XX__
		/* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
		mem2hex( (UINT8*)&(context->registers[registerNum*2-GPR_SUM]), outBuf, DB_REGISTER_SIZE);

		*outBufferLen = DB_REGISTER_SIZE;
#else
		/* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
		mem2hex( (UINT8*)&(context->registers[registerNum*2-GPR_SUM]), outBuf, DB_FLOAT_REGISTER_UNIT_LEN);

		*outBufferLen = DB_FLOAT_REGISTER_SIZE;
#endif


        return (TA_OK);
    }

    /* ���õļĴ���Ϊ����Ĵ���,�����±��64��69 */
    if ((registerNum >= GPR_SUM+FGPR_SUM)&&(registerNum <= GPR_SUM+FGPR_SUM+SPR_SUM) )
    {
    	/* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
         mem2hex( (UINT8*)&(context->registers[registerNum+FGPR_SUM]), outBuf, DB_REGISTER_UNIT_LEN );

        *outBufferLen = DB_REGISTER_SIZE;

        return (TA_OK);
    }

#ifdef TA_ALTIVEC_SUPPORT_DEBUG
    if(TRUE == taAltiVecIsInit())
    {
		/*�ظ�vr0-vr31�Ĵ���*/
		if((registerNum >= ALTIVEC_GDB_VR0_NUM)&&(registerNum <= ALTIVEC_GDB_VR31_NUM))
		{
			T_DB_ExceptionVectorContext *taVectorContext = taSDAExpVectorContextPointerGet(GET_CPU_INDEX());

			VEC_REG *pVr = (VEC_REG*)&(taVectorContext->vr[registerNum-ALTIVEC_GDB_VR0_NUM]);

			/* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
			 mem2hex( (UINT8*)(pVr), outBuf, ALTIVEC_CHAR_COUNT);

			*outBufferLen = ALTIVEC_HEX_COUNT;

			return (TA_OK);
		}

		//�ظ�vscre�Ĵ���
		if(registerNum == ALTIVEC_GDB_VSCR_NUM)
		{
			T_DB_ExceptionVectorContext *taVectorContext = taSDAExpVectorContextPointerGet(GET_CPU_INDEX());

			/* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
			 mem2hex((UINT8*)(taVectorContext->vscr), outBuf, DB_REGISTER_UNIT_LEN );

			*outBufferLen = DB_REGISTER_SIZE;

			return (TA_OK);
		}

		//�ظ�vrsave�Ĵ���
		if(registerNum == ALTIVEC_GDB_VRSAVE_NUM)
		{
			T_DB_ExceptionVectorContext *taVectorContext = taSDAExpVectorContextPointerGet(GET_CPU_INDEX());

			/* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
			 mem2hex((UINT8*)(&(taVectorContext->vrsave)), outBuf, DB_REGISTER_UNIT_LEN );

			*outBufferLen = DB_REGISTER_SIZE;

			return (TA_OK);
		}
    }
#endif

    return (TA_FAIL);
}

/*
 * @brief:
 *     д������Ӧ���������е����Ĵ�����ֵ
 * @param[in]:context: �����쳣������
 * @param[in]:registerNum:�Ĵ����ţ��Ĵ����ŷ�Χ��0~REGISTER_SUM  0~GPR_SUMΪͨ�����ͼĴ��� GPR_SUM~(GPR_SUM+FGPR_SUM)Ϊ����Ĵ��� (GPR_SUM+FGPR_SUM)~REGISTER_SUMΪ����Ĵ���
 * @param[in]:inBuf:�������ݻ��壬��������Ϊд��Ĵ�����ֵ(ʮ����ֵ�ַ�����ʾ)
 * @return:
 *     TA_OK:�����ɹ�
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 *     TA_INVALID_PARAM_LENGTH:���뻺�峤�ȷǷ�
 *     TA_FAIL:ת��ʧ��
 */
T_TA_ReturnCode  taDebugWriteSingleRegister(T_DB_ExceptionContext* context, UINT32 registerNum, const UINT8 *inBuf)
{
    UINT32 value = 0;
    UINT64 doubleValue = 0;

    /* �Ĵ�������ڸ���Ĵ�����Χ��,�����±�Ŵ�32��63 */
    if ( (registerNum >= GPR_SUM) && (registerNum < GPR_SUM+FGPR_SUM) )
    {
        if (0 != hex2int64(&inBuf, &doubleValue) && '\0' == (*inBuf))
        {
#ifdef __PPC85XX__
             //д��ͬ��û����
			 /* �������ַ���ת��Ϊ64λֵ,��д����Ӧ�ĸ���Ĵ���  */
			context->registers[registerNum*2 - GPR_SUM] = ((UINT64)doubleValue);
#else
             /* �������ַ���ת��Ϊ64λֵ,��д����Ӧ�ĸ���Ĵ���  */
            context->registers[registerNum*2 - GPR_SUM] = ((UINT64)doubleValue);
#endif
            return (TA_OK);
        }

        return (TA_FAIL);
    }

#ifdef TA_ALTIVEC_SUPPORT_DEBUG
    if(TRUE == taAltiVecIsInit())
   {
		/*�޸�vr0-vr31��vscr�Ĵ���*/
		if((registerNum >= ALTIVEC_GDB_VR0_NUM)&&(registerNum <= ALTIVEC_GDB_VR31_NUM))
		{
			T_DB_ExceptionVectorContext *taVectorContext = taSDAExpVectorContextPointerGet(GET_CPU_INDEX());

			//��ȡvector����
			VEC_REG *pVr = (VEC_REG*)&(taVectorContext->vr[registerNum-ALTIVEC_GDB_VR0_NUM]);

			hex2mem(inBuf,(UINT8*)(pVr),ALTIVEC_CHAR_COUNT);

			return (TA_OK);
		}
		//�ظ�vscre�Ĵ���
		if(registerNum == ALTIVEC_GDB_VSCR_NUM)
		{
			T_DB_ExceptionVectorContext *taVectorContext = taSDAExpVectorContextPointerGet(GET_CPU_INDEX());

			/* �������ַ���ת��Ϊ64λֵ,��д����Ӧ�ĸ���Ĵ���  */
			 if ( 0 != hex2int(&inBuf, &value) && '\0' == (*inBuf))
			{
				/* �������ַ���ת��Ϊ64λֵ,��д����Ӧ�ĸ���Ĵ���  */
				 taVectorContext->vscr[0] = value;

				return (TA_OK);
			}

			return (TA_OK);
		}

		/*дvscr�Ĵ���*/
		if(registerNum == ALTIVEC_GDB_VRSAVE_NUM)
		{
			T_DB_ExceptionVectorContext *taVectorContext = taSDAExpVectorContextPointerGet(GET_CPU_INDEX());

			if ( 0 != hex2int(&inBuf, &value) && '\0' == (*inBuf))
			{
				/* �������ַ���ת��Ϊ64λֵ,��д����Ӧ�ĸ���Ĵ���  */
				taVectorContext->vrsave = value;

				return (TA_OK);
			}

			return (TA_FAIL);
		}
   }
#endif

    /* �Ĵ�����Ų��ڸ���Ĵ�����Χ��,�Ƚ��ַ���ת��Ϊ���� */
    if ( 0 != hex2int(&inBuf, &value) && '\0' == (*inBuf))
    {
    	/* �Ĵ��������ͨ�üĴ�����Χ��,�����±��0��31  */
        if (registerNum < GPR_SUM)
        {
            context->registers[registerNum] = value;

            return (TA_OK);
        }

        /* �Ĵ������������Ĵ�����Χ��,�����±��64��69 */
        if (registerNum >= GPR_SUM + FGPR_SUM)
        {
        	/* ��ת�����ֵ���浽����Ĵ����� */
            context->registers[registerNum+FGPR_SUM] = value;

            return (TA_OK);
        }
    }

    return (TA_FAIL);
}

/*
 * @brief:
 *     ���������Ӧ����������PS�Ĵ����ĵ�����־λSE��ȷ�����ص�������Ӧ�ú������������
 * @param[in]:context: �����쳣������
 * @return:
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context)
{
    /* ����PS�Ĵ����ĵ�����־λSEΪ0 ������85xx���������������Ҳ���Բ����������Ȼ�����ҲûɶӰ�찡*/
    context->registers[PS] &= (~DB_ARCH_PS_TRAP_BIT);

    return (TA_OK);
}

/*
 * @brief:
 *     ���ñ�����Ӧ����������PS�Ĵ����ĵ�����־λSEΪ1,ȷ�����ص�������Ӧ������ִ��һ��ָ���Ժ󴥷������쳣
 * @param[in]:context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext* context)
{
#ifdef __PPC85XX__

	//�������������־
	gSoftStepFlag = TRUE;

#else
      /* ����PS�Ĵ�����SE��־λΪ1 */
    context->registers[PS] |= DB_ARCH_PS_TRAP_BIT;
#endif
    return (TA_OK);
}

/*
 * @brief:
 *     ���������쳣ָ��
 * @param[in]:context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugJumpOverTrap(T_DB_ExceptionContext* context)
{
    /* �����ϵ㳤�� */
    context->registers[PC] += DB_ARCH_BREAK_INST_LEN;

    return (TA_OK);
}

#ifdef TA_ALTIVEC_SUPPORT_DEBUG
//�����쳣ʸ��������
void taSaveAltiVec()
{

	if(TRUE == taAltiVecIsInit())
    {
		//�����쳣ʸ��������
		taAltiVecSave((void *)taSDAExpVectorContextPointerGet(GET_CPU_INDEX()));
    }

}
#endif

#ifdef TA_ALTIVEC_SUPPORT_DEBUG
/* �ָ��쳣ʸ�������� */
void taRestoreAltiVec()
{

	if(TRUE == taAltiVecIsInit())
	{
		/* �ָ��쳣ʸ�������� */
		taAltiVecRestore((void*)taSDAExpVectorContextPointerGet(GET_CPU_INDEX()));
	}

}
#endif
#ifndef _KERNEL_DEBUG_
#ifdef CONFIG_CORE_SMP

static inline void taSpinLock(unsigned int *lock)
{
        asm volatile(
                    "\n\t mfspr  3,1008\n\t"
                    "andi.  3,3,0xc000\n\t"
                    "beq .2l\n\t"\
                   ".1l:\n\t" \
                  "lwarx    5,0,%0\n\t"\
                   " ori      6, 5, 0x1\n\t"\
                   " stwcx. 6,0,%0\n\t"\
                    "bne-   .1l\n\t"\
                   "isync\n\t"\
                    "sync\n\t"\
                    "cmpwi  5,0x0\n\t"\
                   " bne-   .1l\n\t"\
                   ".2l:\n\t"\
                   "\n\t;"\
                    :
                    : "r" (lock));
}

static inline void taSpinUnlock(unsigned int *lock)
{
	asm volatile(\
		"isync\n\t"\
		"sync\n\t"\
		);
	*lock = 0;

}

BOOL taTas
    (
    void * address 		/* address to test and set */
    )
    {
    int oldLevel;
    oldLevel = intCpuLock ();

#ifdef 	CONFIG_CORE_SMP
    taSpinLock(&taTasLock);
#endif

    if (*(volatile char *) address == 0)
    {
        *(volatile char *) address = 1;
#ifdef 	CONFIG_CORE_SMP
        taSpinUnlock(&taTasLock);
#endif
        intCpuUnlock (oldLevel);
        return (TRUE);
    }

#ifdef 	CONFIG_CORE_SMP
    taSpinUnlock(&taTasLock);
#endif
    intCpuUnlock (oldLevel);

    return (FALSE);

    }

#endif
#endif


#ifdef __PPC85XX__
/**
* @function daGetNpc
*
* @brief Returns the adress of the next instruction to be executed.
*
* @param[in] pRegs: pointer to task registers
*
* @return Adress of the next instruction to be executed.
*/

static UINT32 * daGetNpc(REG_SET * pRegs)
{
	UINT32      machInstr;      /* Machine instruction */
	    UINT32 *    npc;            /* next program counter */
	    UINT32      branchType;
	    UINT32      li;             /* LI field */
	    UINT32      lr;             /* LR field */
	    UINT32      bd;             /* BD field */
	    UINT32      bo;             /* BO field */
	    UINT32      bi;             /* BI field */
	    UINT32      ctr;            /* CTR register  */
	    UINT32 *    cr;             /* CR register   */
	    INT32       cond;
	    UINT32      bo2,bo3,bo0,bo1,crbi;   /* bits values   */

	    npc = (UINT32 *) (pRegs->pc + 4);        /* Default nPC */
	    machInstr = *(UINT32 *)(pRegs->pc);

	    /*
	     * tests for branch instructions:
	     * bits 0-3 are common for all branch
	     */

	    if ((machInstr & BRANCH_MASK) == OP_BRANCH)
	    {
	        /* opcode bits 0 to 5 equal 16,17,18 or 19 */

	        branchType = (machInstr & 0xFC000000) >> 26;

	        ctr = pRegs->ctr;
	        cr = (UINT32 *) pRegs->cr;
	        lr = pRegs->lr;
	        li = _IFIELD_LI (machInstr);
	        bo = _IFIELD_BO (machInstr);
	        bi = _IFIELD_BI (machInstr);
	        bd = _IFIELD_BD (machInstr);

	        bo0 = _REG32_BIT (bo, (BO_NB_BIT-1));   /* bit 0 of BO */
	        bo1 = _REG32_BIT (bo, (BO_NB_BIT-2));   /* bit 1 of BO */
	        bo2 = _REG32_BIT (bo, (BO_NB_BIT-3));   /* bit 2 of BO */
	        bo3 = _REG32_BIT (bo, (BO_NB_BIT-4));   /* bit 3 of BO */
	        crbi = _REG32_BIT ((UINT32) cr, (CR_NB_BIT-1-((UINT32) bi)));
	                                                /* bit bi of CR */

	        /* Switch on the type of branch (Bx, BCx, BCCTRx, BCLRx)   */

	        switch (branchType)
	        {
	            case (16):                          /* BC - Branch Conditional */
	                if (bo2 == 0)                   /* bit 2 of BO == 0   */
	                {
	                    ctr--;                      /* decrement CTR register */
	                }

	            /* test branch condition */

	                cond = 0;
	                if ((bo2 == 1) || ((ctr == 0) && (bo3 == 0)) )
	                {
	                    if ((bo0 == 1) || (bo1 == crbi))
	                    {
	                        cond = 1;
	                    }
	                }
	                if (cond)
	                {
	                    if ((machInstr & AA_MASK) == 1)
	                    {
	                        npc = (UINT32 *) bd;    /* AA = 1 : absolute branch */
	                    }
	                    else
	                    {
	                        /* AA = 0 : relative branch */
	                        npc = (UINT32 *) (pRegs->pc + bd);
	                    }
	                }
	          break;

	        case (18):                          /* B  - Unconditional Branch */
	            if ((machInstr & AA_MASK) == 1)
	            {
	                npc = (UINT32 *) li;        /* AA = 1 : absolute branch */
	            }
	            else
	            {
	                /* AA = 0 : relative branch */
	                npc = (UINT32 *)(pRegs->pc + li);
	            }
	            break;

	        case (19): /* Bcctr or Bclr - Branch Conditional to Register */
	                if ((machInstr & BCCTR_MASK) == INST_BCCTR)
	                {
	                    /* Bcctr - Branch Conditional to Count Register */

	                    if (bo2 == 0)               /* bit 2 of BO == 0   */
	                    {
	                        ctr--;                  /* decrement CTR register */
	                    }

	                    /* test branch condition */

	                    cond = 0;
	                    if ((bo2 == 1) || ((ctr == 0) && (bo3 == 0)))
	                    {
	                        if ((bo0 == 1) || (bo1 == crbi))
	                        {
	                            cond = 1;
	                        }
	                    }
	                    if (cond)
	                    {
	                        /* branch relatif to CTR */
	                        npc = (UINT32 *) (ctr & 0xFFFFFFFC);
	                    }
	                }

	                if ((machInstr & BCLR_MASK) == INST_BCLR)
	                {
	                    /* Bclr - Branch Conditional to Link Register */

	                    if (bo2 == 0)               /* bit 2 of BO == 0   */
	                    {
	                        ctr--;                  /* decrement CTR register */
	                    }

	                    /* test branch condition */

	                    cond = 0;
	                    if ((bo2 == 1) || ((ctr == 0) && (bo3 == 0)))
	                    {
	                        if ((bo0 == 1) || (bo1 == crbi))
	                        {
	                            cond = 1;
	                        }
	                    }
	                    if (cond)
	                    {
	                        /* branch relatif to LR */
	                        npc = (UINT32 *) (lr & 0xFFFFFFFC);
	                    }
	                }
	            break;
	        }
	    }
	    return (npc);
}

/*
 * @brief:
 *     ����������
 * @param[in]:context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
void taDebugClrSoftStep()
{
    if(TRUE == gSoftStepFlag)
    {
    	   //�жϵ���ָ���Ƿ��Ƕϵ�ָ��
    	   if(*gSaveNextPC == DB_ARCH_BREAK_INST)
    	   {
               *gSaveNextPC = gSaveNextInt;
                taCacheTextUpdata(gSaveNextPC, sizeof(DB_ARCH_BREAK_INST));
	    }		
		   
                //������������־
				gSoftStepFlag = FALSE;

				//���ô洢ָ��Ϊ��Чָ��
				gSaveNextInt = 0;
    	   

    	   //�ָ����жϵ�
    	   taDebugBpAllEnable();
    }

}

/*
 * @brief:
 *     �����������
 * @param[in]:context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
void taDebugSetSoftStep(T_DB_ExceptionContext* context)
{
	if(TRUE == gSoftStepFlag)
	{
		REG_SET arch_reg;
		int ret = 0;
		UINT32 NextInt = 0;

		_RType gpr[GREG_NUM];	/* general pourpose registers */
		_RType msr;			/* machine state register */
		_RType lr;			/* link register */
		_RType ctr;			/* count register */
		_RType pc;			/* program counter */
		_RType cr;			/* condition register */
		_RType xer;

		memcpy(arch_reg.gpr, context->registers, GPR_SUM*sizeof(T_UWORD));
		arch_reg.msr = context->registers[PS];
		arch_reg.lr = context->registers[PLR];
		arch_reg.ctr = context->registers[PCTR];
		arch_reg.pc = context->registers[PC];
		arch_reg.cr = context->registers[CR];
		arch_reg.xer = context->registers[PXER];

		//ɾ�����жϵ�
		taDebugBpAllDisable();

		gSaveNextPC =  daGetNpc(&arch_reg);

		NextInt = 	*gSaveNextPC;

		if(NextInt != DB_ARCH_BREAK_INST)
		{
			//�����������
			if((FALSE == gSoftStepFlag)
				||(0 == gSaveNextInt))
			{
				//����ָ��
				gSaveNextInt = NextInt;

				//д��ϵ�ָ��
				*gSaveNextPC = DB_ARCH_BREAK_INST;

				//ˢ��cache
				taCacheTextUpdata(gSaveNextPC, sizeof(DB_ARCH_BREAK_INST));
			}
		}
	}
}

#endif
