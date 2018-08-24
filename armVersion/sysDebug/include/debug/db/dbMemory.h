/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:dbMemory.h
 *@brief:
 *             <li>���Թ�����ڴ������</li>
 */
#ifndef _DB_MEMORY_H
#define _DB_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"
#include "taErrorDefine.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/* �ڴ���ʶ��䷽ʽ */
typedef enum
{
    Align_8Bit = 1,
    Align_16Bit = 2,
    Align_32Bit = 4,
    Align_64Bit = 8,
    Align_Error = 0xFFFFFFFF,
    Align_None = Align_8Bit
} T_DB_AccessMemoryAlign;

/* �ڴ������ */
typedef struct
{
    /* д�ڴ棬�ýӿھ߱�����ַ��Ч�ԣ����ܷ��ز���ʧ�ܵ���ϸԭ��  */
    ulong_t (*WriteMemory)(ulong_t ctxId, ulong_t desAddr, const UINT8  *srcAddr, UINT32 size, T_DB_AccessMemoryAlign align);

    /* ���ڴ棬�ýӿھ߱�����ַ��Ч�ԣ����ܷ��ز���ʧ�ܵ���ϸԭ�� */
    ulong_t (*ReadMemory)(ulong_t ctxId, ulong_t readAddr, UINT8  *outBuf, UINT32  size, T_DB_AccessMemoryAlign align);

    /* ˢCache */
    void (*FlushCache)(ulong_t ctxId, ulong_t addr, UINT32 len);
} T_DB_MemoryOper;

/************************�ӿ�����******************************/

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
T_TA_ReturnCode taDebugReadMemory(ulong_t ctxId, ulong_t readAddr, UINT8 *outBuf, UINT32 size, T_DB_AccessMemoryAlign align);

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
T_TA_ReturnCode taDebugWriteMemory(ulong_t ctxId, ulong_t desAddr, const UINT8 *srcAddr, UINT32 size,T_DB_AccessMemoryAlign align);

/*
 * @brief:
 *   ����ȫ���ڴ������ָ���ˢ��Cache��������ˢ��Cache����
 * @param[in]:ctxId: ������ID
 * @param[in]:addr: ������Cache�ڴ��ַ
 * @param[in]:len: ������Cache���ڴ泤��
 * @return:
 *     none
 */
void taDebugFlushCache(ulong_t ctxId, ulong_t addr, UINT32 len);

/*
 * @brief:
 *     ��ʼ���ڴ������
 * @param[in]:operTable:�����ڴ��д��ˢ��Cache�ӿڵ��ڴ������
 * @return:
 *        TA_OK: �����ɹ�
 *        TA_INVALID_PARAM: �����Ƿ�
 */
T_TA_ReturnCode taDebugMemoryInit(const T_DB_MemoryOper* operTable);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_DB_MEMORY_H */
