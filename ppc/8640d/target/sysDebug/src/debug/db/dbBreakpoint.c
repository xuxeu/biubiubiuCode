/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-07-2         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbBreakpoint.c
 * @brief:
 *             <li>���Թ���ģ��Ķϵ����</li>
 */

/************************ͷ �� ��******************************/
#include <string.h>
#include "dbMemory.h"
#include "dbBreakpoint.h"
#include "dbHdBp.h"
#include "dbAtom.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/*ȫ�ֵĶϵ������Ϣ��*/
typedef struct
{
    T_DB_Breakpoint   *bp_busy;  /* �������˶ϵ������ */
    T_DB_Breakpoint   *bp_free;  /* ���жϵ����� */
}T_DB_BreakpointManageInfo;

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* �ϵ������Ϣ�� */
static T_DB_BreakpointManageInfo taBreakManageInfo={NULL,NULL};

/************************ȫ�ֱ���*****************************/

/* �ϵ�� */
static T_DB_Breakpoint dbBpTable[CONFIG_DB_BREAK_POINT];

/************************����ʵ��*****************************/

/*
 * @brief:
 *     ��ʼ���ϵ�������ݽṹ
 * @param[in] bpNumber :���õĶϵ����
 * @return:
 *     ��
 */
void taDebugBreakPointInit(UINT32 bpNumber)
{
    UINT32 idx = 0;
    
    /* ��նϵ��б�  */
    ZeroMemory(&dbBpTable, sizeof(dbBpTable));

    /* ��ʼ��ȫ�ֶϵ������Ϣ�������Ϊ���õĶϵ��ռ� */
    taBreakManageInfo.bp_free = dbBpTable;

    /* ��ʼ��ȫ�ֶϵ������Ϣ��������öϵ��ΪNULL */
    taBreakManageInfo.bp_busy = NULL;

    /* �����жϵ�Ŀռ䴮�����γɵ������� */
    for (idx = 0; (INT32)idx < (bpNumber - 1); idx++)
    {        
        (taBreakManageInfo.bp_free[idx]).next_addr = &(taBreakManageInfo.bp_free[idx + 1]);
    }
}

/*
 * @brief:
 *    ����ƥ��Ķϵ���Ϣ
 * @param[in]: sessionID :���ԻỰID
 * @param[in]: objID: �����Զ���ID
 * @param[in]: logicAddr: �ϵ��߼���ַ
 * @param[out]: breakpointList: �洢�鵽�ķ��������Ķϵ��б�,����߼���ַ��ͬ��sessionID,taskID��ͬ���򷵻ص�һ��ƥ��Ķϵ�
 * @return:
 *         TA_OK:�ҵ���ƥ��Ķϵ�
 *         TA_DB_BREAKPOINT_NOTFOUND:û���ҵ��ϵ�
 *         TA_DB_BREAKPOINT_DIFFER_ID:û���ҵ���ȫƥ��Ķϵ�,��ַ��ͬ����sessionID,objID��ͬ
 */
T_TA_ReturnCode taDebugLookUpBP(UINT32 sessionID, UINT32 objID, UINT32 logicAddr, T_DB_Breakpoint **breakpointList)
{
    T_DB_Breakpoint *pBreakpoint = taBreakManageInfo.bp_busy;

    /* ����breakpointListΪNULL */
    *breakpointList = NULL;

    /* �������öϵ�����pBreakpoint�б��������߼���ַ����<logicAddr>�Ķϵ� */
    while ((NULL != pBreakpoint) && (pBreakpoint->logicAddr != logicAddr))
    {
        /* δ�ҵ�������������е���һ���ڵ� */
        pBreakpoint = pBreakpoint->next_addr;
    }

    /* ���û���ҵ��߼���ַ��ͬ�Ķϵ� */
    if (NULL == pBreakpoint)
    {
        return (TA_DB_BREAKPOINT_NOTFOUND);
    }

    /* ����tpBpListָ����ͬ��ַ�ϵ�ͷ�ڵ� */
    *breakpointList = pBreakpoint;

    /* ����������ͬ�ϵ��ַ�Ķϵ�ڵ� */
    while(NULL != pBreakpoint)
    {
         /* ���һỰ��ȫƥ��Ķϵ� */
        if ((sessionID == pBreakpoint->sessionID) && (objID == pBreakpoint->objID))
        {
            *breakpointList = pBreakpoint;

            return (TA_OK);
        }

        /* δ�ҵ�������������е���һ���ڵ� */
        pBreakpoint = pBreakpoint->same_addr;
    }

    /* ֻ�ҵ��߼���ַƥ��Ķϵ� */
    return (TA_DB_BREAKPOINT_DIFFER_ID);
}

/*
 * @brief:
 *        ����һ���ϵ�(����ϵ��Ӳ���ϵ�)������Ѿ������߼���ַ��ͬ�Ķϵ��������ͬ��ַ����������taBreakManageInfo.bp_busy��ͷ
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:breakpoint: �ϵ���Ϣ
 * @return:
 *         TA_OK                   :ִ�гɹ�
 *         TA_DB_BREAKPOINT_FULL   :�ϵ������
 *         TA_INVALID_PARAM        :�Ƿ��Ĳ���
 *         TA_DB_BREAKPOINT_EXIST  :��ͬ�Ự,��ͬ�������ͬ��ַ�Ѵ���һ���ϵ�
 */
T_TA_ReturnCode taDebugInsertBP(T_DB_Info *debugInfo, T_DB_Breakpoint *breakpoint)
{
    T_DB_Breakpoint* pNewBP = NULL;
    T_DB_Breakpoint* pSameAddrBP = NULL;
    T_TA_ReturnCode ret = TA_OK;

    /* �Ӷϵ����в鿴�Ƿ������ȫƥ��Ķϵ� */
    ret = taDebugLookUpBP(debugInfo->sessionID, debugInfo->objID, breakpoint->logicAddr, &pSameAddrBP);
    if (TA_OK == ret)
    {
        return (TA_OK);
    }

    /* ����Ѿ��ﵽ����������ϵ��������� */
    if (NULL == taBreakManageInfo.bp_free)
    {
        return (TA_DB_BREAKPOINT_FULL);
    }

    /* �ӿնϵ����ϻ�ȡһ���ڵ� */
    pNewBP = taBreakManageInfo.bp_free;

    /* �նϵ�����ͷ�ڵ�ָ����һ���ڵ� */
    taBreakManageInfo.bp_free = taBreakManageInfo.bp_free->next_addr;

    /* ��������������²���Ķϵ�ڵ㸳ֵ */
    pNewBP->sessionID = debugInfo->sessionID;
    pNewBP->objID = debugInfo->objID;
    pNewBP->logicAddr = breakpoint->logicAddr;
    pNewBP->bpType = breakpoint->bpType;
    pNewBP->bpClass = breakpoint->bpClass;
    pNewBP->breakSize = breakpoint->breakSize;
    pNewBP->next_addr = NULL;
    pNewBP->same_addr = NULL;

    /* ����ǲ�������ϵ� */
    if (DB_BP_NORMAL == breakpoint->bpType || DB_BP_SOFT_STEP == breakpoint->bpType)
    {
        /* ��������ϵ� */
        ret = taDebugInsertSoftBp(debugInfo->objID, breakpoint->logicAddr, (UINT8*)(&(pNewBP->shadow)));
    }
    else
    {
        /* ����Ӳ���ϵ� */
        ret = taDebugInsertHardBp(debugInfo, breakpoint->logicAddr, breakpoint->bpType, breakpoint->breakSize);
    }

    /* �ϵ����ʧ�� */
    if (TA_OK != ret)
    {
        /* ����ϵ�ʧ�ܣ���ȡ�µĶϵ�д��free����  */
        pNewBP->next_addr = taBreakManageInfo.bp_free;
        taBreakManageInfo.bp_free = pNewBP;
        return ret;
    }

    /* ����������Ѵ�����ͬ�߼���ַ�Ķϵ㣬������ͬһ�Ự�Ķϵ� */
    if (NULL != pSameAddrBP)
    {
        /* ����ͬ��ַ����ͷ�ڵ�����¼���Ķϵ� */
        pNewBP->same_addr = pSameAddrBP->same_addr;
        pSameAddrBP->same_addr = pNewBP;
    }
    else
    {
        /* ��������ͬ��ַ�Ķϵ㣬ֱ�ӽ��¶ϵ�ڵ����taBreakManageInfo.bp_busy��ͷ */
        pNewBP->next_addr = taBreakManageInfo.bp_busy;
        taBreakManageInfo.bp_busy = pNewBP;
    }

    return (TA_OK);
}

/*
 * @brief:
 *        ɾ��һ���ϵ�
 * @param[in]:debugInfo :���ԻỰ��Ϣ
 * @param[in]:logicAddr :�ϵ��ַ
 * @param[in]:breakpointType :�ϵ�����
 * @param[in]:breakpointClass :�ϵ����
 * @param[in]:size :�ϵ���
 * @return:
 *         TA_OK :ִ�гɹ�
 *         TA_FAIL :ɾ���ϵ�ʧ��
 *         TA_DB_BREAKPOINT_EMPTY :û�д��κζϵ�
 *         TA_INVALID_PARAM :�Ƿ��Ĳ���
 *         TA_DB_BREAKPOINT_NOTFOUND :û�ҵ��ϵ�
 */
T_TA_ReturnCode taDebugRemoveBP(T_DB_Info *debugInfo, UINT32 logicAddr, T_DB_BpType breakpointType, T_DB_BpClass breakpointClass, UINT32 size)
{
    T_DB_Breakpoint* pMatchBP = taBreakManageInfo.bp_busy;
    T_DB_Breakpoint* pPreBusyBP = NULL;
    T_DB_Breakpoint* pPreSameBP = NULL;

    /* ���Ҿ�����ͬ��ַ�Ķϵ���������¼ǰһ���ϵ��� */
    while ((NULL != pMatchBP) && (logicAddr != pMatchBP->logicAddr))
    {
    	/* ��¼ǰһ���ڵ�,����ɾ�� */
        pPreBusyBP = pMatchBP;

        /* ָ����һ���ڵ�Ԫ�� */
        pMatchBP = pMatchBP->next_addr;
    }

    /* ����ȫƥ��ϵ㣬����¼ǰһ���ϵ�ڵ� */
    while ((NULL != pMatchBP)
            && ((debugInfo->sessionID != pMatchBP->sessionID)
            || (debugInfo->objID != pMatchBP->objID)
            || ((pMatchBP->bpType == DB_BP_SOFT_STEP)?0:(breakpointType != pMatchBP->bpType))
            || (breakpointClass != pMatchBP->bpClass)))
    {
        /* ����pPreSameBPΪpMatchBP,��¼ǰһ���ڵ����ɾ�� */
        pPreSameBP = pMatchBP;

        /* ����pMatchBP����ָ��ǰpMatchBP�����ӵ���ͬ�ϵ��ַ�� */
        pMatchBP = pMatchBP->same_addr;
    }


    /* û�ҵ���ƥ��ϵ� */
    if (NULL == pMatchBP)
    {
        //return (TA_DB_BREAKPOINT_NOTFOUND);
         /*���Ҳ����ϵ㷵��OK*/
        return TA_OK;
    }

    /* ����ϵ�����Ϊ����ϵ� */
    if (DB_BP_NORMAL == breakpointType || DB_BP_SOFT_STEP == breakpointType)
    {
        /* ɾ������ϵ� */
        taDebugRemoveSoftBp(debugInfo->objID, logicAddr, (const UINT8*)(pMatchBP->shadow));
    }
    else
    {
        /* ɾ��Ӳ���ϵ� */
        taDebugRemoveHardBp(debugInfo, logicAddr, breakpointType, size);
    }

    /* ����ҵ��Ķϵ�Ϊ��ͬ��ַ�������һ���ڵ�(��ֻ��һ���ڵ�) */
    if (NULL == pMatchBP->same_addr)
    {
    	/* �ҵ��ڵ��ǰһ���ڵ㲻Ϊ�ձ�ʾ��ͬ��ַ���ж���ϵ㣬ֻ��򵥵Ľ�ǰһ���ڵ��nextָ����Ϊ�ռ���������ȥ�����ҵ��Ľڵ� */
        if (NULL != pPreSameBP)
        {
            /* ��ǰɾ���Ķϵ�ڵ�Ϊsame���е����һ���ڵ㣬����pPreSameBP��same_addrΪNULL */
            pPreSameBP->same_addr = NULL;
        }
        else
        {
        	/* ǰһ���ϵ�Ϊ�գ�˵��same����ֻ��һ���ϵ�,ֱ�ӴӲ�ͬ��ַ����ժ�� */
            if (NULL == pPreBusyBP)
            {
                /* ��ǰɾ���Ķϵ�ڵ���taBreakManageInfo.bp_busy��ͷ�ڵ㣬����taBreakManageInfo.bp_busyָ����һ���ڵ� */
                taBreakManageInfo.bp_busy = taBreakManageInfo.bp_busy->next_addr;
            }
            else
            {
                /* ����ͷ�ӵ㣬����һ���ڵ�ָ��ָ���ҵ�������һ����㣬����busy������ժ���� */
                pPreBusyBP->next_addr = pMatchBP->next_addr;
            }
        }
    }
    else
    {
    	/* ����ҵ��Ķϵ㲻����ͬ��ַ�������һ���ڵ� */
        if (NULL == pPreSameBP)
        {
        	/* ǰһ���ڵ�Ϊ�գ����ҵ��Ľڵ�Ϊ��ͬ��ַ����ͷ�ڵ㣬����ҵ��Ľڵ�Ϊ����busy���ĵ�һ���ڵ� */
        	if (NULL == pPreBusyBP)
        	{
        		/* busy����ͷ�ӵ�ָ���ҵ��Ľڵ��same������һ���ڵ� */
                taBreakManageInfo.bp_busy = pMatchBP->same_addr;
        	}
        	else
        	{
            	/* ���ҵ��ڵ����ͬ��ַ���ҵ���ͬ��ַ�ڵ����� */
            	pPreBusyBP->next_addr = pMatchBP->same_addr;
        	}
        	
        	/* �µ���ͬ��ַ����ͷ�ڵ��ڲ�ͬ��ַ���ϣ�ָ��ԭͷ������һ���ڵ�*/
        	pMatchBP->same_addr->next_addr = pMatchBP->next_addr;
        }
        else
        {
            /* pPreSameBP��same_addr�ڵ㸳ֵΪpMatchBP��same_addr�ڵ� */
            pPreSameBP->same_addr = pMatchBP->same_addr;
        }
    }

    /* ��ȡ�µĿ���������뵽bp_free�� */
    pMatchBP->next_addr = taBreakManageInfo.bp_free;
    taBreakManageInfo.bp_free = pMatchBP;

    return (TA_OK);
}

/*
 * @brief:
 *      ���ַaddr������һ���ϵ�ָ�ͬʱ��addr��ԭ�е����ݱ��浽shadow��
 * @param[in]: ctxId: ������ID
 * @param[in]: addr: ����ϵ�ָ��ĵ�ַ
 * @param[in]: shadow: ��������ϵ㴦ԭ�����ݵĻ���
 * @return:
 *        TA_OK: �����ɹ�
 *        TA_FAIL: ����ʧ��
 */
T_TA_ReturnCode taDebugInsertSoftBp(UINT32 ctxId,  UINT32 addr, UINT8 *shadow)
{
	UINT32 inst = DB_ARCH_BREAK_INST;
    T_TA_ReturnCode ret = TA_OK;

    /* ��addr��ַ��ȡDB_ARCH_BREAK_INST_LEN���ȵ����ݱ��浽shadow�� */
    ret = taDebugReadMemory(ctxId, addr, shadow, DB_ARCH_BREAK_INST_LEN, Align_None);
    if (TA_OK != ret)
    {
        return (TA_FAIL);
    }

    /* ��ǰ�ڴ��Ƿ��Ƕϵ�ָ�� */
    if (taDebugBpInstructCheck((UINT8 *)shadow) != FALSE)
    {
    	return TA_OK;
    }

    /* ���ϵ�ָ��д�뵽addr�� */
    ret = taDebugWriteMemory(ctxId, addr, (const UINT8*)&inst, DB_ARCH_BREAK_INST_LEN, Align_None);
    if (TA_OK != ret)
    {
        return (TA_FAIL);
    }

    /* ˢ��Cache */
    taDebugFlushCache(ctxId, addr, DB_ARCH_BREAK_INST_LEN);

    return (TA_OK);
}

/*
 * @brief:
 *        �ӵ�ַaddr��ɾ��һ���ϵ�ָ�ͬʱ�ָ�addr��ԭ�е�����
 * @param[in]: ctxId: ������ID
 * @param[in]: addr: ����ϵ�ָ��ĵ�ַ
 * @param[in]: shadow: ��������ϵ㴦ԭ�����ݵĻ���
 * @return:
 *        TA_OK: �����ɹ�
 *        TA_FAIL:  ����ʧ��
 */
T_TA_ReturnCode taDebugRemoveSoftBp(UINT32 ctxId, UINT32 addr, const UINT8 *shadow)
{
    T_TA_ReturnCode ret = TA_OK;
    UINT8 buffer[DB_ARCH_BREAK_INST_LEN];

    /* ��addr��ַ��ȡDB_ARCH_BREAK_INST_LEN���ȵ����ݱ��浽shadow�� */
    ret = taDebugReadMemory(ctxId, addr, buffer, DB_ARCH_BREAK_INST_LEN, Align_None);
    if (TA_OK != ret)
    {
        return (TA_FAIL);
    }

     /* ��ǰ�ڴ��Ƿ��Ƕϵ�ָ�� */
    if (taDebugBpInstructCheck((UINT8 *)buffer) != TRUE)
    {

    	return TA_OK;
    }

    /* ��shadow������д�뵽addr�� */
    ret = taDebugWriteMemory(ctxId, addr, shadow, DB_ARCH_BREAK_INST_LEN, Align_None);
    if(TA_OK != ret)
    {
        return (TA_FAIL);
    }

    /* ˢ��Cache */
    taDebugFlushCache(ctxId, addr, DB_ARCH_BREAK_INST_LEN);

    return (TA_OK);
}

/*
 * @brief:
 *      ɾ����ǰ���ԻỰ���жϵ�
 * @param[in] debugInfo: ���ԻỰ
 * @return:
 *      ��
 */
void taDebugRemoveAllBp(T_DB_Info *debugInfo)
{
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;
	T_DB_Breakpoint* pSameBreakpoint = NULL;
	T_DB_Breakpoint* pPreSameBreakpoint = NULL;

    /* �������жϵ� */
    while (NULL != pBreakpoint)
    {
        /* ����same�ڵ� */
        pSameBreakpoint = pBreakpoint;

        /* ָ����һ���ϵ�Ԫ�� */
        pBreakpoint = pBreakpoint->next_addr;

        /* ����������ͬ�ϵ��ַ�Ķϵ�ڵ� */
        while(NULL != pSameBreakpoint)
        {
            /* ��¼ǰһ��same�ڵ�*/
            pPreSameBreakpoint = pSameBreakpoint;

            /* ָ����һ��������ͬ�ϵ��ַ�Ľڵ㣬������ָ����һ���ϵ㣬
             * ����ɾ����ϵ㱻���գ��޷��ҵ���һ���ϵ� */
            pSameBreakpoint = pSameBreakpoint->same_addr;

            /* ɾ���ϵ� */
            taDebugRemoveBP(debugInfo, pPreSameBreakpoint->logicAddr, pPreSameBreakpoint->bpType, BPClass_NORMAL, pPreSameBreakpoint->breakSize);
        }
    }
}

/*
 * @brief:
 *      ��ֹ��������ϵ�
 * @return:
 *      ��
 */
void taDebugBpAllDisable(void)
{
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;
	T_DB_Breakpoint* pSameBreakpoint = NULL;
	T_DB_Breakpoint* pPreSameBreakpoint = NULL;

    /* �������жϵ� */
    while (NULL != pBreakpoint)
    {
        /* ����same�ڵ� */
        pSameBreakpoint = pBreakpoint;

        /* ָ����һ���ϵ�Ԫ�� */
        pBreakpoint = pBreakpoint->next_addr;

        /* ����������ͬ�ϵ��ַ�Ķϵ�ڵ� */
        while(NULL != pSameBreakpoint)
        {
            /* ��¼ǰһ��same�ڵ�*/
            pPreSameBreakpoint = pSameBreakpoint;

            /* ָ����һ��������ͬ�ϵ��ַ�Ľڵ㣬������ָ����һ���ϵ㣬
             * ����ɾ����ϵ㱻���գ��޷��ҵ���һ���ϵ� */
            pSameBreakpoint = pSameBreakpoint->same_addr;

            /* ɾ������ϵ� */
            taDebugRemoveSoftBp(pPreSameBreakpoint->objID, pPreSameBreakpoint->logicAddr, (const UINT8*)(pPreSameBreakpoint->shadow));
        }
    }
}

/*
 * @brief:
 *      ʹ����������ϵ�
 * @return:
 *      ��
 */
void taDebugBpAllEnable(void)
{
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;
	T_DB_Breakpoint* pSameBreakpoint = NULL;
	T_DB_Breakpoint* pPreSameBreakpoint = NULL;

    /* �������жϵ� */
    while (NULL != pBreakpoint)
    {
        /* ����same�ڵ� */
        pSameBreakpoint = pBreakpoint;

        /* ָ����һ���ϵ�Ԫ�� */
        pBreakpoint = pBreakpoint->next_addr;

        /* ����������ͬ�ϵ��ַ�Ķϵ�ڵ� */
        while(NULL != pSameBreakpoint)
        {
            /* ��¼ǰһ��same�ڵ�*/
            pPreSameBreakpoint = pSameBreakpoint;

            /* ָ����һ��������ͬ�ϵ��ַ�Ľڵ㣬������ָ����һ���ϵ㣬
             * ����ɾ����ϵ㱻���գ��޷��ҵ���һ���ϵ� */
            pSameBreakpoint = pSameBreakpoint->same_addr;

            /* ����ϵ� */
            taDebugInsertSoftBp(pPreSameBreakpoint->objID, pPreSameBreakpoint->logicAddr, (UINT8*)(&(pPreSameBreakpoint->shadow)));
        }
    }
}

/*
 * @brief:
 *      �鿴�Ƿ�Ϊ����������õĶϵ�
 * @return:
 *      ��
 */
BOOL taIsSoftStepBreakPoint(UINT32 addr)
{
		
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;


    /* �������жϵ� */
    while (NULL != pBreakpoint)
    {
     

	if(pBreakpoint->logicAddr ==addr &&  pBreakpoint->bpType == DB_BP_SOFT_STEP)
	{
		return TRUE;
	}

        /* ָ����һ���ϵ�Ԫ�� */
        pBreakpoint = pBreakpoint->next_addr;

     
    }

	return FALSE;
}
/*
 * @brief:
 *     ����ڴ��ַ��ŵ��Ƿ��Ƕϵ�ָ��
 * @return:
 *     FALSE: �Ƕϵ�ָ��
 *     TRUE:�ϵ�ָ��
 */
BOOL taDebugBpInstructCheck(UINT8 *addr)
{
	UINT32 bpInstr = DB_ARCH_BREAK_INST;
	UINT8 *bpAddr = (UINT8 *)&bpInstr;
    UINT32 index = 0;

    for (index = 0; index < DB_ARCH_BREAK_INST_LEN; index++)
	{
    	if (*addr++ != *bpAddr++)
    	{
    		return (FALSE);
    	}
	}

    return (TRUE);
}

 /**********************�����������ͬ���ϵ��б�*****************************/
#ifdef _KERNEL_DEBUG_
/*
 * @brief:
 *    ��ȡ�ϵ���Ϣ
 * @return:
 *     FALSE: �Ƕϵ�ָ��
 *     TRUE:�ϵ�ָ��
 */
unsigned int* taGetBreakInfo()
{
    return (unsigned int*)(&taBreakManageInfo);
}
#endif

/*
 * @brief:
 *     ͬ���ϵ���Ϣ
 * @return:
 *     FALSE: �Ƕϵ�ָ��
 *     TRUE:�ϵ�ָ��
 */
BOOL taSyncBreakInfo(unsigned int *BreakManageInfo)
{
    T_DB_BreakpointManageInfo *taBreakInfo = (T_DB_BreakpointManageInfo*)BreakManageInfo;
    T_DB_Breakpoint* pBreakpoint = taBreakInfo->bp_busy;
    T_DB_Breakpoint* pNewBP = NULL;

    /* �������жϵ� */
    while (NULL != pBreakpoint)
    {	
	     /* �ӿնϵ����ϻ�ȡһ���ڵ� */
	    pNewBP = taBreakManageInfo.bp_free;
	    /* �նϵ�����ͷ�ڵ�ָ����һ���ڵ� */
	    taBreakManageInfo.bp_free = taBreakManageInfo.bp_free->next_addr;

           memset(pNewBP,0,sizeof(T_DB_Breakpoint));
	    /* ��������������²���Ķϵ�ڵ㸳ֵ */
	    pNewBP->sessionID = pBreakpoint->sessionID;
        pNewBP->objID = pBreakpoint->objID;
	    pNewBP->logicAddr = pBreakpoint->logicAddr;
	    pNewBP->bpType = pBreakpoint->bpType;
	    pNewBP->bpClass = pBreakpoint->bpClass;
	    pNewBP->breakSize = pBreakpoint->breakSize;
           memcpy(pNewBP->shadow,pBreakpoint->shadow,DB_ARCH_BREAK_INST_LEN);
	    pNewBP->next_addr = NULL;
	    pNewBP->same_addr = NULL;

	    /* ֱ�ӽ��¶ϵ�ڵ����taBreakManageInfo.bp_busy��ͷ */
          pNewBP->next_addr = taBreakManageInfo.bp_busy;
          taBreakManageInfo.bp_busy = pNewBP;

	    /* ָ����һ���ϵ�Ԫ�� */
        pBreakpoint = pBreakpoint->next_addr;

	}
}

#ifdef __PPC__
//ˢ��
void taUpdateCacheBreakList()
{
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;

	/* �������жϵ� */
	while (NULL != pBreakpoint)
	{
		taCacheTextUpdata(pBreakpoint->logicAddr,sizeof(T_DB_Breakpoint));

		/* ָ����һ���ϵ�Ԫ�� */
		pBreakpoint = pBreakpoint->next_addr;
	}
}
#endif

