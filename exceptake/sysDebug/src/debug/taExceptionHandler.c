/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file:taExceptionHandler.c
 * @brief:
 *             <li>�����쳣�������������</li>
 */

/************************ͷ �� ��******************************/
#include "taDebugMode.h"
#include "dbContext.h"

#ifdef EXCEPTAKE
void taSysExceptionTake(UINT32 vector, T_DB_ExceptionContext *context);
void taTaskExceptionTake(UINT32 vector, T_DB_ExceptionContext *context);
#endif

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/* ϵͳ�쳣������ָ�� */
void (*_func_taSysExceptionHandler) (UINT32 vector, T_DB_ExceptionContext *context) = NULL;

/* �����쳣������ָ�� */
void (*_func_taTaskExceptionHandler) (UINT32 vector, T_DB_ExceptionContext *context) = NULL;

/************************����ʵ��*****************************/

/*
 * @brief:
 *      �쳣�����������,���ݵ���ģʽ�Ͱ�װ���쳣�����������쳣����
 * @param[in]: vector: �쳣��
 * @param[in]: context: �쳣������
 * @return:
 *     ��
 */
void taExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context)
{
	/* ϵͳ���쳣���� */
	if((DEBUGMODE_SYSTEM == taDebugModeGet()) && (_func_taSysExceptionHandler != NULL))
	{
		_func_taSysExceptionHandler(vector, context);
	}

	/* �����쳣���� */
	if((DEBUGMODE_TASK == taDebugModeGet()) && (_func_taTaskExceptionHandler != NULL))
	{
		_func_taTaskExceptionHandler(vector, context);
	}
}

#ifdef EXCEPTAKE

void taExceptionTake(UINT32 vector, T_DB_ExceptionContext *context)
{
    UINT32 tid = 0;
    UINT32 sid = 0;

    if(DEBUGMODE_SYSTEM == taDebugModeGet())
    {
        taSysExceptionTake(vector, context);
    }
    
    if(DEBUGMODE_TASK == taDebugModeGet())
    {
        taTaskExceptionTake(vector, context);
    }

}
#endif
