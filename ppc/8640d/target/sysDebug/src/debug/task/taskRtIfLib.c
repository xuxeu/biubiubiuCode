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
 * @file:taskRtIfLib.c
 * @brief:
 *             <li>����ʱ�����ã��ṩ����Ŀ���ֲ��</li>
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

/**
 * @brief:
 * 	  ����ʱ�����ӿڳ�ʼ��
 * @return:
 * 	   ��
 */
void taRtLibInit(TA_RT_IF *pRtIf)
{
	pRtIf->createHookAdd = taTaskCreateHookAdd;
	pRtIf->deleteHookAdd = taTaskDeleteHookAdd;
	pRtIf->memalloc = taMalloc;
	pRtIf->memfree = taFree;
	pRtIf->memRead = taDirectReadMemory;
	pRtIf->memWrite = taDirectWriteMemory;
	pRtIf->memCtxSwitch = (UINT32 (*)(UINT32, int))_func_taskMemCtxSwitch;
	pRtIf->pageSize	= 0;
}

/**
 * @brief:
 * 	  �����������ʱ�ӿڳ�ʼ��
 * @return:
 * 	   ��
 */
void taTaskRtIfInit(void)
{
    (taRtIf.taCtxIf[TA_CTX_TASK]).resume = taTaskAllResume;
    (taRtIf.taCtxIf[TA_CTX_TASK]).suspend = taTaskAllSuspend;
    (taRtIf.taCtxIf[TA_CTX_TASK]).attach = taTaskAttach;
    (taRtIf.taCtxIf[TA_CTX_TASK]).detach = taTaskDetach;
    (taRtIf.taCtxIf[TA_CTX_TASK]).idVerify = taTaskIdVerify;
    (taRtIf.taCtxIf[TA_CTX_TASK]).contextGet = taTaskContextGet;
    (taRtIf.taCtxIf[TA_CTX_TASK]).contextSet = taTaskContextSet;
}
