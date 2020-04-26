/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * �޸���ʷ:
 * 2013-11-08         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbMemProbeLib.c
 * @brief:
 *             <li>��ģ���ṩ��ϵ�ṹ��ص��ڴ�̽����ʽӿ�</li>
 */

/************************ͷ �� ��*****************************/
#include "dbVector.h"
#include "ta.h"
#ifdef CONFIG_DELTAOS
#include <taskLib.h>
#endif

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/
extern void taMemProbeByteRead  (void);
extern void taMemProbeByteWrite (void);
extern void taMemProbeWordRead  (void);
extern void taMemProbeWordWrite (void);
extern void taMemProbeLongRead  (void);
extern void taMemProbeLongWrite (void);
extern void taMemProbeErrorRtn  (void);

/************************ǰ������*****************************/

#ifdef CONFIG_DELTAOS
/* �ڴ�̽���쳣������ */
static BOOL taMemProbeTrapHandle(int vecNum, ESF1 *pEsf, REG_SET *pRegSet,
		EXC_INFO *pExcInfo);
#endif

/************************ģ�����*****************************/

/* ����old�쳣����HOOK */
static FUNCPTR oldExcBaseHook;

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/**
 * @brief:
 *     �ڴ�̽����ϵ�ṹ��س�ʼ�����ҽ��ڴ���ʷǷ��쳣������
 * @param[in]:_func_excBaseHook: �쳣HOOKָ�룬�쳣����ʱ����ϵͳ���ø�HOOK
 * @return:
 * 	   OK����ʼ���ɹ�
 */
STATUS taMemArchProbeInit(FUNCPTR *_func_excBaseHook)
{
	oldExcBaseHook = *_func_excBaseHook;
#ifdef CONFIG_DELTAOS
    *_func_excBaseHook = &taMemProbeTrapHandle;
#endif
    return (OK);
}

#ifdef CONFIG_DELTAOS
/*
 * @brief:
 *      �ڴ�̽���쳣������ں���
 * @param[in]: vecNum: �쳣��
 * @param[in]: pEsf: �쳣����ʱCPUѹջ�������ģ�����EFLAGS, CS, EIP��
 * @param[in]: pRegSet: ջ�ϱ�����쳣������
 * @param[in]: pExcInfo: ������Ϣ(δʹ��)
 * @return:
 *     TRUE:ϵͳ���������쳣
 *     FALSE:ϵͳ�����������쳣
 */
static BOOL taMemProbeTrapHandle(int vecNum, ESF1 *pEsf, REG_SET *pRegSet, EXC_INFO *pExcInfo)
{
    /* ����쳣�Ƿ������ڴ�̽����� */
    if (((pRegSet->pc == (INSTR *) taMemProbeByteRead)    ||
         (pRegSet->pc == (INSTR *) taMemProbeByteWrite)   ||
         (pRegSet->pc == (INSTR *) taMemProbeWordRead)    ||
         (pRegSet->pc == (INSTR *) taMemProbeWordWrite)   ||
         (pRegSet->pc == (INSTR *) taMemProbeLongRead)    ||
         (pRegSet->pc == (INSTR *) taMemProbeLongWrite))  &&
         ((vecNum == GENERAL_PAGE_INT)||(vecNum == GENERAL_GP_INT)))
    {
       /* �쳣��taMemProbe()����������ǿ��ʹ��PC�ص�taMemProbeErrorRtn����ʹtaMemProbe()�������� ERROR */
       pRegSet->pc = (INSTR *)taMemProbeErrorRtn;
       pEsf->pc = (INSTR *)taMemProbeErrorRtn;
                                                                       
       return (TRUE);
    }

    /* �쳣�����ڴ�̽�����������쳣���������������װ���쳣HOOK */
    if (oldExcBaseHook != NULL)
    {
       return (oldExcBaseHook(vecNum, pEsf, pRegSet, pExcInfo));
    }

    return (FALSE);
}
#endif
