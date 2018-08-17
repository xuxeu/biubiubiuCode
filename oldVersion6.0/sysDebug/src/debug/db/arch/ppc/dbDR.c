/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-07-01         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbDR.c
 * @brief:
 *             <li>PPC��ϵ�ṹ��Ӳ���ϵ����</li>
 */
/************************ͷ �� ��******************************/
#include "dbDR.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *     �������ݶϵ�
 * @param[in]:uwAddr:���ݶϵ��ַ
 * @return:
 *    ��
 */
void taDebugSetDataBreakpoint(UINT32 addr)
{
    /* ����DABR�Ĵ�����ֵ */
    taDebugSetDABR(addr);
}

/*
 * @brief:
 *     ����ָ��ϵ�
 * @param[in]:uwAddr:ָ��ϵ��ַ
 * @return:
 *    ��
 */
void taDebugSetAddressBreakpoint(UINT32 addr)
{
    /* ����IABR�Ĵ�����ֵ */
    taDebugSetIABR(addr);
}

/*
 * @brief:
 *     ��ȡ���ݶϵ��ַ
 * @return:
 *    ���ݶϵ��ַ
 */
UINT32 taDebugGetDataBreakpoint(void)
{
    /* ��ȡDABR�Ĵ�����ֵ  */
    return (taDebugGetDABR());
}

/*
 * @brief:
 *    ��ȡָ��ϵ��ַ
 * @return:
 *    ָ��ϵ��ַ
 */
UINT32 taDebugGetAddressBreakpoint(void)
{
    /* ��ȡIABR�Ĵ�����ֵ */
    return (taDebugGetIABR());
}

/*
 * @brief:
 *     ��ȡDAR�Ĵ���ֵ
 * @return:
 *    DAR�Ĵ���ֵ
 */
UINT32 taDebugGetDARValue(void)
{
    /* ��ȡDAR�Ĵ�����ֵ */
    return (taDebugGetDAR());
}

/*
 * @brief:
 *     ���õ�Ӳ���ϵ��Ƿ��Ѿ��ﵽĿ���֧�ֵ�����
 * @param[in]:sessionID :���ԻỰID
 * @param[in]:isAddr:�ж�ָ��ϵ�����ݶϵ��ʾ
 *         TRUE:  ���ж�ָ��ϵ�
 *         FALSE: ���ж����ݶϵ�
 * @return:
 *     TA_OK:    �Ѿ����õ�Ӳ���ϵ������ﵽ������
 *     TA_FAIL:  �п��е�Ӳ���ϵ�
 */
BOOL taDebugHardBpIsFull(UINT32 sessionID, BOOL isAddr)
{
    UINT32 address = 0;

    if (TRUE == isAddr)
    {
        /* ��ȡ��ַ�ϵ�Ĵ���IABR��ֵ */
        address = taDebugGetAddressBreakpoint();
    }
    else
    {
        /* ��ȡ���ݶϵ�Ĵ���DABR��ֵ */
        address = taDebugGetDataBreakpoint();
    }

    return (0 == address) ? FALSE : TRUE;
}
