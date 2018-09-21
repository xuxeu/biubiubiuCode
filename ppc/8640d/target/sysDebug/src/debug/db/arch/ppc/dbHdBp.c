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
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:addr: �ϵ��ַ
 * @param[in]:type: �ϵ�����
 * @param[in]:size: �ϵ���
 * @return:
 *         TA_OK: ����ɹ�
 */
T_TA_ReturnCode taDebugInsertHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
	T_TA_ReturnCode ret = TA_FAIL;
	PHYS_ADDR * physicalAddr = 0 ;
	UINT32 addrValue = 0;

#ifndef _KERNEL_DEBUG_
	vmTranslate(0,addr,physicalAddr);
	addr = (UINT32 *)(*physicalAddr);
#endif

	/* ���õ�ַ�ϵ� */
	if ( DB_BP_HWBP == type)
	{
		/* ����Ƿ���ڿ��еĵ�ַ�ϵ� */
		ret = taDebugHardBpIsFull(debugInfo, TRUE);
		if (TA_OK == ret)
		{
			/* ��ַ�ϵ��Ѿ����� */
			return (TA_DB_BREAKPOINT_FULL);
		}

		/* ���öϵ��ַ */
		addrValue = addr;

		/* ���MSR�Ĵ�����IRλ */
		if ( 0 != (debugInfo->context->registers[PS] & DB_PPC_MSR_IR ) )
		{
			/* ����IABR�Ĵ�����TEλ */
			addrValue |= 1;
		}

		/* ����IABR�Ĵ�����BEλ */
		addrValue |= 2;

		/* ��дIABR�Ĵ��� */
		taDebugSetAddressBreakpoint(addrValue);

	}
	else /* ���ù۲�� */
	{
		/* ����Ƿ���ڿ��еĵ�ַ�ϵ� */
		ret = taDebugHardBpIsFull(debugInfo, TRUE);
		if (TA_OK == ret)
		{
			/* ��ַ�ϵ��Ѿ����� */
			return (TA_DB_BREAKPOINT_FULL);
		}

		/* ���öϵ��ַ,��յ�3λ */
		addrValue = ( addr  & (~(UINT32)7) );

		/* ���MSR�Ĵ�����DRλ */
		if ( 0 != (debugInfo->context->registers[PS] & DB_PPC_MSR_DR ) )
		{
			/* ����DABR�Ĵ�����BTλ */
			addrValue |= 4;
		}

		/* ������õ���д�۲�� */
		if ( DB_BP_HWWP_W == type)
		{
			/* ����DABR�Ĵ�����DWλ */
			addrValue |= 2;
		}

		/* ������õ��Ƕ��۲�� */
		if ( DB_BP_HWWP_R == type)
		{
			/* ����DABR�Ĵ�����DRλ */
			addrValue |= 1;
		}

		/* ������õ��Ƕ�д�۲�� */
		if ( DB_BP_HWWP_A == type)
		{
			/* ����DABR�Ĵ�����DWλ��DRλ */
			addrValue |= 3;
		}

		/* ��дDABR�Ĵ��� */
		taDebugSetDataBreakpoint(addrValue);
	}

    return (TA_OK);
}

/*
 * @brief:
 *     ɾ��Ӳ���ϵ�
 * @param[in]:sessionID :�ỰID��
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:addr: �ϵ��߼���ַ
 * @param[in]:type: �ϵ�����
 * @param[in]:size: �ϵ���
 * @return:
 *         TA_OK:ɾ���ɹ�
 */
T_TA_ReturnCode taDebugRemoveHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
	UINT32 addrValue = 0;
	UINT32 delAddr = 0;

	/* Ҫɾ�����ǵ�ַ�ϵ� */
	if ( DB_BP_HWBP == type)
	{
		/* ���ڵ�ַ�ϵ�,���ȡ��ǰ���ڵĵ�ַ�ϵ� */
		addrValue = taDebugGetDataBreakpoint();

		/* ���ԭ�е�ַ�ϵ�ĵ�2λ */
		addrValue &= ~(UINT32)0x3;

		/* ��ȡҪɾ���ϵ� */
		delAddr = addr & (~(UINT32)0x3);

		/* �Ƚϵ�ַ,�����ַ��ͬ,�򷵻�DB_BREAKPOINT_NOTFOUND */
		if ( addrValue != delAddr )
		{
			return (TA_DB_BREAKPOINT_NOTFOUND);
		}

		/* Ҫɾ���ϵ���ԭ�ϵ���ͬ,������IABR�Ĵ���Ϊ0 */
		taDebugSetAddressBreakpoint(0);
	}
	else /* Ҫɾ�����ǹ۲�� */
	{
		/* �������ݶϵ�,���ȡ��ǰ���ڵ����ݶϵ� */
		addrValue = taDebugGetDataBreakpoint();

		/* ���ԭ�е�ַ�ϵ�ĵ�3λ */
		addrValue &= ~(UINT32)0x7;

		/* ��ȡҪɾ���ϵ� */
		delAddr = addr & (~(UINT32)0x7);

		/* �Ƚϵ�ַ,�����ַ��ͬ,�򷵻�DB_BREAKPOINT_NOTFOUND */
		if ( addrValue != delAddr )
		{
			return (TA_DB_BREAKPOINT_NOTFOUND);
		}

		/* Ҫɾ���ϵ���ԭ�ϵ���ͬ,������DABR�Ĵ���Ϊ0 */
		taDebugSetDataBreakpoint(0);
	}

    /* ���ز����ɹ� */
    return (TA_OK);
}
