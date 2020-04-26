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
 *@file:taDirectMemory.h
 *@brief:
 *             <li>ֱ�Ӷ�д�ڴ�����ӿ�����</li>
 */
#ifndef _TA_DIRECT_MEMORY_H
#define _TA_DIRECT_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ�ļ�********************************/
#include "dbMemory.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

/**
 * @brief
 * 		TAֱ���ڴ���ʳ�ʼ��
 * @return:
 *      ��
 */
void taDebugInitDirectMemory(void);

/*
 * @brief:
 *      ˢ������Cache
 * @param[in]:sessionID: �ỰID
 * @param[in]:addr: Ҫ���µ�cache�ڴ��ַ
 * @param[in]:len: Ҫ���µĳ���
 * @return:
 *      ��
 */
void taDirectFlushCache(UINT32 sessionID, UINT32 addr, UINT32 len);

/*
 * @brief:
 *      ֱ�Ӷ��ڴ�
 * @param[in]:sessionID:�ỰID
 * @param[in]:readAddr:�ڴ��ַ
 * @param[out]:outBuf: ���ڴ滺��
 * @param[in]:size: Ҫ�����ڴ��С
 * @param[in]:align: �����С
 * @return:
 *      TA_OK:�����ɹ�
 *      TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
UINT32 taDirectReadMemory(UINT32 sessionID, UINT32 readAddr, UINT8 *outBuf, UINT32 size,T_DB_AccessMemoryAlign align);

/*
 * @brief:
 *      ֱ��д�ڴ�
 * @param[in]:sessionID: �ỰID
 * @param[in]:src: Դ�ڴ��ַ
 * @param[in]:size: ���ݴ�С
 * @param[in]:align: �����С
 * @param[out]:dest: Ŀ���ڴ��ַ
 * @return:
 *      TA_OK:�����ɹ�
 *      TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
T_TA_ReturnCode taDirectWriteMemory(UINT32 sessionID, UINT32 dest, const UINT8 *src, UINT32 size,T_DB_AccessMemoryAlign align);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TA_DIRECT_MEMORY_H */
