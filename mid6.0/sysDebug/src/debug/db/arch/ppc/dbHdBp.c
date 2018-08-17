/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-07-020        ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbHdBp.c
 * @brief:
 *             <li>PPC��ϵ�ṹӲ���ϵ����ú�ɾ����ʵ��</li>
 */

/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "dbDR.h"
#include "dbBreakpoint.h"
#include "dbContext.h"
#include "sdaMain.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *     ����Ӳ���ϵ㣬Ӳ���ϵ��Ϊ���ݶϵ��ָ��ϵ�
 * @param[in]: debugInfo: ���ԻỰ��Ϣ
 * @param[in]: addr: �ϵ��ַ
 * @param[in]: type: �ϵ�����
 * @param[in]: size: �ϵ���
 * @return:
 *    TA_OK: ����ɹ�
 *    TA_DB_BREAKPOINT_FULL:����ʧ��(�ϵ������)
 */
T_TA_ReturnCode taDebugInsertHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
    UINT32 addrValue = 0;
    T_TA_ReturnCode ret = TA_FAIL;

    /* �ϵ�����Ϊָ��ϵ� */
    if (DB_BP_HWBP == type)
    {
        /* ����Ƿ���ڿ��е�ָ��ϵ� */
        ret = taDebugHardBpIsFull(debugInfo->sessionID, TRUE);
        if (TA_OK == ret)
        {
            return (TA_DB_BREAKPOINT_FULL);
        }

        /* ���öϵ��ַ */
        addrValue = addr;

        /* ���MSR�Ĵ�����IRλ */
        if ( 0 != (debugInfo->context->registers[PS] & DB_PPC_MSR_IR))
        {
        	/* ����IABR�Ĵ�����TEλ */
            addrValue |= 1;
        }

        /* ����IABR�Ĵ�����BEλ */
        addrValue |= 2;

        /* ��addrValueֵд��IABR�Ĵ��� */
        taDebugSetAddressBreakpoint(addrValue);
    }
    else
    {
        /* ����Ƿ���ڿ��е����ݶϵ� */
        ret = taDebugHardBpIsFull(debugInfo->sessionID, FALSE);
        if (TA_OK == ret)
        {
            return (TA_DB_BREAKPOINT_FULL);
        }

        /* ���öϵ��ַ,��յ�3λ */
        addrValue = ( addr  & (~7) );

        /* ���MSR�Ĵ�����DRλ */
        if ( 0 != (debugInfo->context->registers[PS] & DB_PPC_MSR_DR))
        {
        	 /* ����DABR�Ĵ�����BTλ */
            addrValue |= 4;
        }

        /* �ϵ�����Ϊд�۲�� */
        if (DB_BP_HWWP_W == type)
        {
        	/* ����DABR�Ĵ�����DWλ */
            addrValue |= 2;
        }

        /* �ϵ�����Ϊ���۲�� */
        if (DB_BP_HWWP_R == type)
        {
        	/* ����DABR�Ĵ�����DRλ */
            addrValue |= 1;
        }

        /* �ϵ�����Ϊ��д�۲�� */
        if (DB_BP_HWWP_A == type)
        {
        	/* ����DABR�Ĵ�����DWλ��DRλ */
            addrValue |= 3;
        }

        /* ��addrValueֵд��DABR�Ĵ���������DABR�Ĵ��� */
	    taDebugSetDataBreakpoint(addrValue);
    }

    return (TA_OK);
}

/*
 * @brief:
 *     ɾ��Ӳ���ϵ�
 * @param[in]: debugInfo: ���ԻỰ��Ϣ
 * @param[in]: addr: �ϵ��߼���ַ
 * @param[in]: type: �ϵ�����
 * @param[in]: size: �ϵ���
 * @return:
 *    TA_OK:ɾ���ɹ�
 *    TA_DB_BREAKPOINT_NOTFOUND:�ϵ��Ҳ���
 */
T_TA_ReturnCode taDebugRemoveHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
    UINT32 addrValue = 0;
    UINT32 delAddr = 0;

    /* �ϵ�����Ϊָ��ϵ� */
    if (DB_BP_HWBP == type)
    {
        /* ��ȡָ��ϵ��ַ */
        addrValue = taDebugGetAddressBreakpoint();

        /* ���ԭ��ָ��ϵ�addrValue�ĵ�2λ */
        addrValue &= ~0x3;

        /* ��ȡҪɾ���ϵ� */
        delAddr = addr & (~0x3);

        /* �Ƚϵ�ַ,�����ַ��ͬ,�򷵻�TA_DB_BREAKPOINT_NOTFOUND */
        if ( addrValue != delAddr )
        {
            return (TA_DB_BREAKPOINT_NOTFOUND);
        }

        /* Ҫɾ���ϵ���ԭ�ϵ���ͬ,������IABR�Ĵ���Ϊ0 */
	    taDebugSetAddressBreakpoint(0);
    }
    else
    {
        /* ��ȡ��ǰ���ڵ����ݶϵ� */
        addrValue = dbGetDataBreakpoint();

        /* ���ԭ�����ݶϵ�addrValue�ĵ�3λ */
        addrValue &= ~0x7;

        /* ��ȡҪɾ���ϵ��ַ */
        delAddr = addr & (~0x7);
        if (addrValue != delAddr )
        {
            return (TA_DB_BREAKPOINT_NOTFOUND);
        }

        /* Ҫɾ���ϵ���ԭ�ϵ���ͬ,������DABR�Ĵ���Ϊ0 */
	    taDebugSetDataBreakpoint(0);
    }

    return (TA_OK);
}
