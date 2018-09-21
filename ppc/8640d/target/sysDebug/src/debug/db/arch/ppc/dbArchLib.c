/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  dbArchLib.c
 * @brief:
 *             <li>ARCH��ص��쳣����</li>
 */
/************************ͷ�ļ�******************************/
#include "dbAtom.h"
#include "taUtil.h"
#include "ta.h"
#include "dbVector.h"
#include "Esf.h"
/************************�궨��******************************/

/************************���Ͷ���****************************/

/************************ȫ�ֱ���****************************/

/************************ǰ������****************************/
/* �쳣������ */
extern T_DB_ExceptionContext taSDASmpExpContext[MAX_CPU_NUM];

/************************ģ�����****************************/
/************************����ʵ��*****************************/

#ifdef _KERNEL_DEBUG_

/*
 * @brief:
 *      ��ϵ�ṹ��ص��쳣������ں���
 * @param[in]: vector: �쳣��
 * @return:
 *     ��
 */
void taAachExceptionHandler(UINT32 vector)
{
	T_DB_ExceptionContext *context = taSDAExpContextPointerGet(GET_CPU_INDEX());

    /* ����ͨ���쳣������  */
	taExceptionHandler(vector, context);
}

#else

/*
 * @brief:
 *      ��ϵ�ṹ��ص��쳣������ں���
 * @param[in]: vector: �쳣��
 * @param[in]: pInfo: �쳣����ʱCPUѹջ��������
 * @param[in]: pRegs: ջ�ϱ�����쳣������
 * @return:
 *     ��
 */
void taBpExceptionStub(ESFPPC *pInfo)
{
	unsigned int Debuglevel = 0;

	//��CPU�ж�
	TA_INT_CPU_LOCK(Debuglevel);

	if(pInfo->vecOffset == EXC_PROGRAM)
	{
		taBpExceptionStub2(GENERAL_TRAP_INT,pInfo,&(pInfo->regSet));
	}
	else if(pInfo->vecOffset == EXC_TRACE)
	{
		taBpExceptionStub2(GENERAL_DB_INT,pInfo,&(pInfo->regSet));
	}
	else if(pInfo->vecOffset == EXC_DSI)
	{
		taBpExceptionStub2(GENERAL_HBP_INT,pInfo,&(pInfo->regSet));
	}
	else if(pInfo->vecOffset == EXC_INSTRUCTION_ADDRESS_BREAKPOINT)
	{
		taBpExceptionStub2(GENERAL_IAB_INT,pInfo,&(pInfo->regSet));
	}

	TA_INT_CPU_UNLOCK(Debuglevel);
}

void taBpExceptionStub2(int vector, ESFPPC *pInfo, REG_SET *pRegs)
{
	T_DB_ExceptionContext context;

    /* ����쳣������  */
    ZeroMemory((void *)&context, sizeof(T_DB_ExceptionContext));

    /* ��ȡ�쳣������ */
	taDebugRegsConvert(pRegs, &context);

    /* ����ͨ���쳣������  */
	taExceptionHandler(vector, &context);

    /* �����쳣������ */
	taDebugContextConvert(&context, pRegs);
}
#endif
