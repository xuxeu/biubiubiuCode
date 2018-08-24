/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-08-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taskHookLib.c
 * @brief:
 *             <li>���񴴽���ɾ��HOOK��ز���</li>
 */

/************************ͷ �� ��*****************************/
#include "taRtIfLib.h"
#include "taskCtrLib.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

/*
 * @brief:
 *      ���񴴽�HOOK������RTP�����Զ���RTP����������RTP���ԻỰ
 * @param[in]: tid: ����ID
 * @return:
 *     ��
 */
static void taTaskCreateHook(ulong_t tid)
{
	UINT32 sid = 0;

	/* ��ѯ��ǰ�����Ƿ���԰� */
	if (taTaskIsAttach(tid, &sid))
	{
		/* ������ */
		taAttachTask(sid, tid);
	}
}

/*
 * @brief:
 *      ����ɾ��HOOK����������ǵ�������ɾ��ʱ�Զ���������
 * @param[in]: tid: ����ID
 * @return:
 *     ��
 */
static void taTaskDeleteHook(ulong_t tid)
{
	/* ������� */
	taDetachTask(tid);
}

/**
 * @brief:
 * 	   ��ʼ������HOOK���������񴴽�HOOK������ɾ��HOOK
 * @return:
 * 	   ��
 */
void taTaskHookInit(void)
{
    static BOOL	initialized = FALSE;

    /* ��ʼ������HOOK */
    if (!initialized)
	{
    	TA_RT_CREATE_HOOK_ADD((FUNCPTR)taTaskCreateHook);
    	TA_RT_DELETE_HOOK_ADD((FUNCPTR)taTaskDeleteHook);
        initialized = TRUE;
	}
}
