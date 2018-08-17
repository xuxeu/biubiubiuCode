/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * �޸���ʷ:
 * 2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbDR.c
 * @brief:
 *             <li>����Ӳ���ϵ���صĽӿ�</li>
 */

/************************ͷ �� ��*****************************/
#include "dbDR.h"
#include "dbBreakpoint.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief
 *    ��ȡָ��DR������
 * @param[in] dr: ���ԼĴ����ı��(DR0/DR1/DR2/DR3/DR6/DR7)
 * @return        
 *    DR������,���dr��������ʶ�𣬷���0
 */
ulong_t taDebugGetDR(UINT32 dr)
{
    ulong_t drContent = 0;

    /* ���ݼĴ�����Ż�ȡֵ */
    switch (dr)
    {
        case DR0:   
        { 
            drContent = taDebugGetDR0();
            break;
        }
        case DR1:    
        {
            drContent = taDebugGetDR1();
            break;
        }
        case DR2:    
        {
            drContent = taDebugGetDR2();
            break;
        }
        case DR3:    
        {
            drContent = taDebugGetDR3();
            break;
        }
        case DR6:    
        {
            drContent = taDebugGetDR6();
            break;
        }
        case DR7:   
        {
            drContent = taDebugGetDR7();
            break;
        }
        default:
        {
            drContent = 0;
            break;
        }
    }

    return (drContent);
}

/*
 * @brief
 *     ����ָ��DR������
 * @param[in] wIndex: ���ԼĴ����ı��(DR0/DR1/DR2/DR3/DR6/DR7)
 * @param[in] wContent: ����
 * @return        
 *    none
 */
void taDebugSetDR(UINT32 wIndex, ulong_t wContent)
{
    /* ���ݼĴ���������üĴ���ֵ */
    switch (wIndex)
    {
        case DR0:    
        {    
            taDebugSetDR0(wContent);    
            break;
        }
        case DR1:
        {
            taDebugSetDR1(wContent);    
            break;
        }
        case DR2:
        {
            taDebugSetDR2(wContent);    
            break;
        }
        case DR3:
        {
            taDebugSetDR3(wContent);    
            break;
        }
        case DR6:
        {
            taDebugSetDR6(wContent);
            break;
        }
        case DR7:
        {
            taDebugSetDR7(wContent);
            break;
        }
        default:
            break;
    }
}

/*
 * @brief
 *    ���ָ���Ķϵ�DR�Ƿ�������/��ʹ��
 * @param[in] wIndex: ���ԼĴ����ı��(DR0/DR1/DR2/DR3)
 * @return        
 *    TA_OK: �ϵ�������
 *    TA_INVALID_INPUT:�Ĵ�����ŷǷ�
 *    DB_FAIL: �ϵ�δ����
 */
T_TA_ReturnCode taDebugIsSetDR(UINT32 wIndex)
{
	BOOL isEnable = FALSE;
	UINT32 dr7 = 0;
	
    /* ���Ĵ�������Ƿ�Ϸ� */
    if (wIndex > DR3)
    {
        return (TA_INVALID_INPUT);
    }

    /* ���Ȼ��DR7������ */
    dr7 = taDebugGetDR(DR7);

    /* ����öϵ㱻ʹ���򷵻ؼ� */
    isEnable = dr7 & ((UINT32)0x3 << (wIndex * 2));
    if (isEnable != FALSE)
    {
        /* �öϵ��ѱ�ʹ�� */
        return (TA_OK);
    }
    
    return (TA_FAIL);
}

/*
* @brief
 *    ���ϵ������ת����x86����
 * @param[in] wType: ��������
 *        DB_BP_HWWP_W ---дӲ���۲��(д)
 *        DB_BP_HWWP_R ---��Ӳ���۲��(��)
 *        DB_BP_HWWP_A ---����Ӳ���۲��(��д)
 *        DB_BP_HWBP ---ִ��Ӳ���۲��
 * @return        
 *    x86��Ķϵ�����
 */
UINT32 taDebugTypeConvert(UINT32 wType)
{
    UINT32 ret = DR_WRITE;

    /* ���ݶϵ��ȣ����ϵ�����ת����ΪX86��ʾ��ʽ */
    switch (wType)
    {
        case DB_BP_HWWP_W:
        {
            ret =  DR_WRITE;
            break;
        }
        case DB_BP_HWWP_R:
        {
            ret =  DR_READ;
            break;
        }
        case DB_BP_HWWP_A:
        {
            ret =  DR_ACCESS;
            break;
        }
        case DB_BP_HWBP:
        {
            ret =  DR_EXECUTE;
            break;
        }
        default:
            ret =  DR_WRITE;
            break;
    }

    return (ret);
}

/*
 * @brief
 *   ���ϵ�����ݿ��ת����x86��ʽ�Ŀ��
 * @param[in] wSize: ���ݿ��
 *                1 ---һ���ֽ�
 *                2 ---�����ֽ�
 *                4 ---�ĸ��ֽ�
 * @return
 *   x86��ʽ�����ݿ��
 */
UINT32 taDebugSizeConvert(UINT32 wSize)
{
    UINT32 width = 1;

    /* ���ݶϵ��ȣ����ϵ���ת����ΪX86��ʾ��ʽ */
    switch (wSize)
    {
        case 1:
        {
            width = DR_BYTE;
            break;
        }
        case 2:
        {
            width = DR_WORD;
            break;
        }
        case 4:
        {
            width = DR_DWORD;
            break;
        }
        default:
        {
            width = DR_BYTE;
            break;
        }
    }

    return (width);
}

/*
 * @brief
 *      �����Ƿ������ָ���ĵ�ַ���������͡�����ȫ��һ�µĶϵ�
 * @param[in] vpAddr: ���öϵ�ָ��ĵ�ַ
 * @param[in] wType: �ϵ�����
 *                DR_WRITE ---дӲ���۲��(д)
 *                DR_READ  ---��Ӳ���۲��(��)
 *                DR_ACCESS---����Ӳ���۲��(��д)
 *                DR_EXECUTE  ִ��Ӳ���۲��
 * @param[in] wSize: �ϵ�Ŀ��
 *                DR_BYTE  ---�ֽ�
 *                DR_WORD  ---��
 *                DR_DWORD ---˫��
 * @return        
 *      ����ɹ�����ƥ��Ķϵ�DR�����򷵻�DR_ERROR
 */
UINT32 taDebugMatchDR(ulong_t vpAddr, UINT32 wType, UINT32 wSize)
{
    UINT32 idx = 0;
    ulong_t dr7 = 0;
    UINT32 flag = 0;
    UINT32 dummy = 0;
    T_TA_ReturnCode ret = TA_OK;
    ulong_t address = 0;

    /* �����ϵ�Ĵ��� */
    for (idx = DR0; idx < DR4; idx++)
    {
        /* ���ȼ��ϵ�DR�Ƿ�æ�Լ���ַ�Ƿ�ƥ�� */
    	ret = taDebugIsSetDR(idx);
        address = taDebugGetDR(idx);
        if( (TA_OK == ret) && (address == vpAddr) )
        {
            /* ���DR7������ */
            dr7 = taDebugGetDR(DR7);

            /* ���ָ��DR�ı���ֶ� */
            flag = ((dr7 >> (idx * 4 + 16)) & 0x0f);

            /* ����һ���ٵı���ֶ� */
            dummy = 0;
            dummy |= wType << 0;
            dummy |= wSize << 2;

            /* �����������Լ����ݿ���Ƿ�һ�� */
            if(dummy == flag)
            {
                return (idx);
            }
        }
    }
    
    return (DR_ERROR);
}

/*
 * @brief
 *     ʹ��ָ���Ķϵ�DR
 * @param[in] wIndex: ���ԼĴ����ı��(DR0/DR1/DR2/DR3)
 * @return        
 *     none
 */
void taDebugEnableDR(UINT32 wIndex)
{
    UINT32 dr7 = 0;

	/* ���Ĵ�������Ƿ�Ϸ�*/
	if (wIndex > DR3)
	{
	   return;
	}

    /* ���Ȼ��DR7������ */
    dr7 = taDebugGetDR(DR7);

    /* ����ָ��DR�Ķϵ� */
    dr7 = dr7 | (0x3U << (wIndex * 2));

    /* ��������DR�Ķϵ� */
    dr7 |= 0x300;

    /* ��������DR7������ */
    taDebugSetDR(DR7,dr7);
    taDebugSetDR(DR6,0);
}

/*
 * @brief
 *     ��ָֹ���Ķϵ�DR
 * @param[in] wIndex: ���ԼĴ����ı��(DR0/DR1/DR2/DR3)
 * @return        
 *     none
 */
void taDebugDisableDR(UINT32 wIndex)
{
	UINT32 dr7 = 0;

    /* ���Ĵ�������Ƿ�Ϸ�*/
    if (wIndex > DR3)
    {
        return;
    }

    /* ���Ȼ��DR7������ */
    dr7 = taDebugGetDR(DR7);

    /* ��ָֹ��DR�ľֲ���ȫ�ֶϵ� */
    dr7 = dr7 & (~(0x3U << (wIndex * 2)));

    /* ��������DR7������ */
    taDebugSetDR(DR7,dr7);
}

/*
* @brief
 *    ���ҿ��еĶϵ�DR
 * @return        
 *    ����ҵ����еĶϵ�DR�򷵻�idx�����򷵻�DR_ERROR
 */
UINT32 taDebugLookupDR(void)
{
    UINT32 idx = DR0;

    for (idx = DR0; idx < DR4; idx++)
    {
        if (taDebugIsSetDR(idx) == TA_FAIL)
        {
            return (idx);
        }
    }
    
    return (DR_ERROR);
}

/*
 * @brief:
 *     ���õ�Ӳ���ϵ��Ƿ��Ѿ��ﵽĿ���֧�ֵ�����
 * @param[in] sessionID :���ԻỰID
 * @param[in] isAddr:�ж�ָ��ϵ�����ݶϵ��ʶ
 *         TRUE:  ���ж�ָ��ϵ�
 *         FALSE: ���ж����ݶϵ�
 * @return:
 *     TRUE:�Ѿ����õ�Ӳ���ϵ������ﵽ������
 *     FALSE:�п��е�Ӳ���ϵ�
 */
BOOL taDebugHardBpIsFull(UINT32 sessionID, BOOL isAddr)
{
	UINT32 idx = DR0;

	idx = taDebugLookupDR();
	if(DR_ERROR != idx)
	{
		return (FALSE);
	}
	
	return (TRUE);
}

/*
 * @brief
 *     �ڵ�ַaddr������һ��Ӳ���ϵ�ָ��
 * @param[in] wIndex: ���ԼĴ����ı��(DR0/DR1/DR2/DR3)
 * @param[in] vpAddr: ���öϵ�ָ��ĵ�ַ
 * @param[in] wType: �ϵ�����
 *                DR_WRITE  ---дӲ���۲��(д)
 *                DR_READ   ---��Ӳ���۲��(��)
 *                DR_ACCESS ---����Ӳ���۲��(��д)
 *                DR_EXECUTE ִ��Ӳ���۲��
 * @param[in] wSize: �ϵ�Ŀ��
 *                DR_BYTE  ---�ֽ�
 *                DR_WORD  ---��
 *                DR_DWORD ---˫��
 * @return        
 *     none
 */
void taDebugInsertDR(UINT32 wIndex, ulong_t vpAddr, UINT32 wType, UINT32 wSize)
{
    /* ���Ȼ��DR7������ */
    UINT32 dr7 = taDebugGetDR(DR7);

    /* ���Ĵ�������Ƿ�Ϸ�*/
    if (wIndex > DR3)
    {
        return;
    }

    /* ���DR7�еĶϵ����ͺͳ��� */
    dr7 = (dr7 & (~(0xfU << (wIndex * 4 + 16))));

    /* ���öϵ������д���� */
    dr7 = (dr7 | (wType  << (wIndex * 4 + 16 + 0)));

    /* ���öϵ�Ŀ�� */
    dr7 = (dr7 | (wSize  << (wIndex * 4 + 16 + 2)));

    /* ��������DR7������ */
    taDebugSetDR(DR7,dr7);

    /* ���öϵ��ַ��ָ����DR */
    taDebugSetDR(wIndex,vpAddr);
}
