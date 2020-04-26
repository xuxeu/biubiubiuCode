/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2011-09-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbAtomArch.c
 * @brief:
 *             <li>ta���Թ�����ϵ�ṹ��ص�ԭ�Ӳ���</li>
 */

/************************ͷ �� ��*****************************/
#include "taErrorDefine.h"
#include "dbContext.h"
#include "taUtil.h"
#include "dbAtom.h"
#include "dbSignal.h"
#include "dbVector.h"
#include "ta.h"
#include <taskLibCommon.h>

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/
#if 0
#define _RType unsigned int
#define GREG_NUM	32	/* has 32 32/64-bit data registers */


typedef struct		/* REG_SET */
    {
    _RType gpreg[GREG_NUM];	/* general pourpose registers */
    _RType sr;			/* machine state register */
    _RType lo;			/* link register */
    _RType cause;			/* count register */
    _RType pc;			/* program counter */
    _RType hi;			/* condition register */

    /* the following fields keep this structure compatible with the kernel */

    _RType spare0;		/* MQ/base of page table */
    _RType spare1;		/* SR regs */
    _RType spare2;		/* SR regs */
    _RType spare3;		/* keeps struct size consistent */
    _RType spare4;		/* keeps struct size consistent */

    } REG_SET;

#endif

/************************�ⲿ����*****************************/

/* �ϵ��쳣���� */
extern void taBpExceptionStub(void);

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/
UINT32* gSaveNextPC;
static UINT32  gSaveNextInt;
UINT32 gSoftStepFlag = FALSE;

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
    /* ��װ����ϵ��쳣 */
	taInstallVectorHandle(IV_BP_VEC, taBpExceptionStub);

    return (TRUE);
}

/*
 * @Brief
 *     vector��ת���ɱ�׼�źű�ʾ��ʽ
 * @param[in] vector:������
 * @param[in] context:�쳣������
 * @return
 *     ���������Ŷ�Ӧ���ź�
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context)
{
    UINT32 signal = SIGNAL_TERM;
    
    /* ����vector��ת���ɱ�׼POSIX�ź� */
    switch (vector)
    {
        /* 0 ���жϡ����ж� */
        case IV_LOW_VEC:
            signal = SIGNAL_INT;
            break;      

		/* 6 ���жϡ���ָ������쳣 */
		case IV_IBUS_VEC:
			signal = SIGNAL_BUS;
			break;

		/* 7 ���жϡ������ݴ����쳣 */
		case IV_DBUS_VEC:
			signal = SIGNAL_BUS;
			break;

	 	/* 9 ���жϡ����ϵ��쳣*/
        case IV_BP_VEC:
            signal = SIGNAL_TRAP;
            break;

		/* 13 ���жϡ���TRAP */
		case IV_TRAP_VEC:
			signal = SIGNAL_TRAP;
			break;

		/* 13 ���жϡ��������쳣 */
		case IV_FPE_VEC:
			signal = SIGNAL_FPE;
			break;

        /*  ��������SIGNAL_EMT */
        default:
            signal = SIGNAL_EMT;
            break;
    }

    return (signal); 
} 

/*
 * @brief
 *     д�������û��Ĵ���
 * @param[in] tpContext:�쳣������
 * @param[in] uwRegisterNum:�Ĵ�����
 * @param[in] ubInBuf:����buffer
 * @param[in] uwInLen:�������ݳ���(��λ�ֽ�)
 * @return
 *     TA_OK:�����ɹ�
 *     TA_FAIL:����ʧ��
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
T_TA_ReturnCode taDebugWriteSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, const UINT8 *inBuf)
{
	UINT64 value = 0;

    /* ������Ч�Լ�� */
    if (registerNum >= REGISTER_SUM)
    {
        /* ���������Ч,����TA_INVALID_PARAM */
        return (TA_INVALID_PARAM);
    }

    /* �Ĵ�������ڸ���Ĵ�����Χ��,�����±�Ŵ�38��ʼ����֧�ָ���Ĵ����޸� */
    if(registerNum > GPR_SUM + SPR_SUM)
    {
    	return (TA_INVALID_PARAM);
    }

	#ifndef MIPS32_1B

    /* �Ĵ�����Ų��ڸ���Ĵ�����Χ��,�Ƚ��ַ���ת��Ϊ���� */
    if (0 != hex2int64(&inBuf, &value) )
    {	
		/* ��ת�����ֵ���浽ͨ�üĴ����� */
		context->registers[registerNum] = swap64(value);
		
		return (TA_OK);
    }
	#else
	/* �Ĵ�����Ų��ڸ���Ĵ�����Χ��,�Ƚ��ַ���ת��Ϊ���� */
    if (0 != hex2int(&inBuf, &value) )
    {	
		/* ��ת�����ֵ���浽ͨ�üĴ����� */
		context->registers[registerNum] = swap(value);
		
		return (TA_OK);
    }
	#endif

    /* ת��ʧ��,����TA_FAIL */
    return (TA_FAIL);
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
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext* context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen)
{
    /* ������Ч���ж� */
    if ((registerNum >= REGISTER_SUM))
    {
        return (TA_INVALID_PARAM);
    }
    
    mem2hex((UINT8*)&context->registers[registerNum],outBuf,DB_REGISTER_UNIT_LEN);
    
	*outBufferLen = DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN;

    return (TA_OK);
}

/*
 * @brief:
 *     MIPS����λ����
 * @param[in]:context: �����쳣������
 * @return:
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context)
{
     
    return (TA_OK);
}

/*
 * @brief:
 *     MIPS����λ����
 * @param[in]:context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext* context)
{
    gSoftStepFlag = TRUE;
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
		REG_SET arch_reg ;
		int ret = 0;
		UINT32 NextInt = 0;

   #if 0
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
#endif

             memset(&arch_reg,0,sizeof(REG_SET));
             taDebugContextConvert(context,&arch_reg);
    
		//ɾ�����жϵ�
		taDebugBpAllDisable();

		gSaveNextPC =  wdbDbgGetNpc (&arch_reg);;

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