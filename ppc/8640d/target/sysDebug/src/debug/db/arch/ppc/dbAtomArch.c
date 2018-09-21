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
        /* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
        mem2hex( (UINT8*)&(context->registers[registerNum*2-GPR_SUM]), outBuf, DB_FLOAT_REGISTER_UNIT_LEN);

        *outBufferLen = DB_FLOAT_REGISTER_SIZE;

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
            /* �������ַ���ת��Ϊ64λֵ,��д����Ӧ�ĸ���Ĵ���  */
            context->registers[registerNum*2 - GPR_SUM] = ((UINT64)doubleValue);

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
    /* ����PS�Ĵ����ĵ�����־λSEΪ0 */
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
    /* ����PS�Ĵ�����SE��־λΪ1 */
    context->registers[PS] |= DB_ARCH_PS_TRAP_BIT;
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
