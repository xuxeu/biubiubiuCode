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
 *             <li>MIPS��ϵ�ṹӲ���ϵ����ú�ɾ����ʵ��</li>
 */

/************************ͷ �� ��******************************/
#include "taTypes.h"
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
 *         TA_DB_BREAKPOINT_FULL:����ʧ��(�ϵ������)
 */
T_TA_ReturnCode taDebugInsertHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
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
 *         TA_DB_BREAKPOINT_NOTFOUND:�ϵ��Ҳ���
 */
T_TA_ReturnCode taDebugRemoveHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
    /* ���ز����ɹ� */
    return (TA_OK);
}
