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
 * @file: dbMemory.c
 * @brief:
 *             <li>���Թ�����ڴ�����ӿڹ���ʵ��</li>
 */
/************************ͷ �� ��******************************/
#include "dbMemory.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* �ڴ������ */
static T_DB_MemoryOper taCurrOperTable = {NULL,NULL,NULL};

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *     ��ʼ���ڴ������
 * @param[in]:operTable:�����ڴ��д��ˢ��Cache�ӿڵ��ڴ������
 * @return:
 *        TA_OK: �����ɹ�
 *        TA_INVALID_PARAM: �����Ƿ�
 */
T_TA_ReturnCode taDebugMemoryInit(const T_DB_MemoryOper* operTable)
{
    /* ���õ�ǰ�ڴ������ */
    taCurrOperTable.WriteMemory = operTable->WriteMemory;
    taCurrOperTable.ReadMemory = operTable->ReadMemory;
    taCurrOperTable.FlushCache = operTable->FlushCache;

    return (TA_OK);
}

/*
 * @brief:
 *     ����ȫ���ڴ������ָ���д�ڴ溯������д�ڴ����
 * @param[in]:ctxId: ������ID
 * @param[in]:desAddr: ��д�����ݵ��ڴ���ʼ��ַ
 * @param[in]:srcAddr: ����д�����ݵĻ����ַ
 * @param[in]:size: ��д�����ݴ�С
 * @param[in]:align: �ڴ���뷽ʽ
 * @return:
 *        TA_OK: �����ɹ�
 *        TA_FAIL: д�ڴ����ʧ��
 */
T_TA_ReturnCode taDebugWriteMemory(UINT32 ctxId, UINT32 desAddr, const UINT8 *srcAddr, UINT32 size,T_DB_AccessMemoryAlign align)
{
    T_TA_ReturnCode ret = TA_FAIL;

    /* �ڴ������д�ڴ溯��ָ�벻����NULL */
    if (NULL != taCurrOperTable.WriteMemory)
    {
        /* �����ڴ�������д�ڴ溯�� */
        ret = taCurrOperTable.WriteMemory(ctxId, desAddr, srcAddr, size,align);
		if (ret != TA_OK)
		{
			ret = TA_FAIL;
		}
    }

    return (ret);
}

/*
 * @brief:
 *     ����ȫ���ڴ������ָ��Ķ��ڴ溯�����ж��ڴ����
 * @param[in]:ctxId: ������ID
 * @param[in]:readAddr:����ȡ���ݵ��ڴ���ʼ��ַ
 * @param[out]:outBuf:���ڴ洢��ȡ���ݵĻ����ַ
 * @param[in]:size:����ȡ���ݵĴ�С
 * @param[in]:align: �ڴ���뷽ʽ
 * @return:
 *        TA_OK: �����ɹ�
 *        TA_FAIL: ���ڴ�ʧ��
 */
T_TA_ReturnCode taDebugReadMemory(UINT32 ctxId, UINT32 readAddr, UINT8 *outBuf, UINT32 size, T_DB_AccessMemoryAlign align)
{
    T_TA_ReturnCode ret = TA_FAIL;

    /* �ڴ��������ڴ溯��ָ�벻����NULL */
    if (NULL != taCurrOperTable.ReadMemory)
    {
        /* �����ڴ��������ڴ溯�� */
        ret = taCurrOperTable.ReadMemory(ctxId, readAddr, outBuf, size,align);
		if (ret != TA_OK)
		{
			ret = TA_FAIL;
		}
    }

    return (ret);
}

/*
 * @brief:
 *   ����ȫ���ڴ������ָ���ˢ��Cache��������ˢ��Cache����
 * @param[in]:ctxId: ������ID
 * @param[in]:addr: ������Cache�ڴ��ַ
 * @param[in]:len: ������Cache���ڴ泤��
 * @return:
 *     none
 */
void taDebugFlushCache(UINT32 ctxId, UINT32 addr, UINT32 len)
{
    /* �ڴ������ˢ��Cache����ָ�벻����NULL */
    if (NULL != taCurrOperTable.FlushCache)
    {
        /* �����ڴ�������ˢ��Cache���� */
        taCurrOperTable.FlushCache(ctxId, addr, len);
    }
}
