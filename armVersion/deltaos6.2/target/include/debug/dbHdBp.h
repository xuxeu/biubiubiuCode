/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2011-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ��� 
 */

/*
 *@file:dbHdBp.h
 *@brief:
 *             <li>Ӳ���ϵ�ɾ�������õĽӿڶ���</li>
 */

#ifndef _DB_HDBREAKPOINT_H
#define _DB_HDBREAKPOINT_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ�ļ�********************************/
#include "taErrorDefine.h"
#include "dbBreakpoint.h"
#include "ta.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

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
T_TA_ReturnCode taDebugInsertHardBp(T_DB_Info *debugInfo, ulong_t addr, T_DB_BpType type, UINT32 size);

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
T_TA_ReturnCode taDebugRemoveHardBp(T_DB_Info *debugInfo, ulong_t addr, T_DB_BpType type, UINT32 size);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_DB_HDBREAKPOINT_H */
