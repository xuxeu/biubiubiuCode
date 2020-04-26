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
 *@file:dbDR.h
 *@brief:
 *             <li>PPC��ϵ�ṹ��Ӳ���ϵ��������</li>
 */
#ifndef _DB_DR_H
#define _DB_DR_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ�ļ�********************************/
#include "taErrorDefine.h"
#include "taTypes.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

/*
 * @brief:
 *      ����DABR�Ĵ���
 * @param[in]:addr: �����ַ
 * @return:
 *      ��
 */
void taDebugSetDABR(UINT32 addr);

/*
 * @brief:
 *      ����IABR�Ĵ���
 * @param[in]:addr: �����ַ
 * @return:
 *      ��
 */
void taDebugSetIABR(UINT32 addr);

/*
 * @brief:
 *      ��ȡDABR�Ĵ�����ֵ
 * @return:
 *      DABR�Ĵ�����ֵ
 */
UINT32 taDebugGetDABR(void);

/*
 * @brief:
 *      ��ȡIABR�Ĵ�����ֵ
 * @return:
 *      IABR�Ĵ�����ֵ
 */
UINT32 taDebugGetIABR(void);

/*
 * @brief:
 *      ��ȡDAR�Ĵ�����ֵ
 * @return:
 *      DAR�Ĵ���ֵ
 */
UINT32 taDebugGetDAR(void);

/*
 * @brief:
 *     ���õ�Ӳ���ϵ��Ƿ��Ѿ��ﵽĿ���֧�ֵ����ޡ�
 * @param[in]:sessionID :���ԻỰID
 * @param[in]:isAddr:�ж�ָ��ϵ�����ݶϵ��ʾ
 *         TRUE:  ���ж�ָ��ϵ�
 *         FALSE: ���ж����ݶϵ�
 * @return:
 *     TA_OK:    �Ѿ����õ�Ӳ���ϵ������ﵽ������
 *     TA_FAIL:  �п��е�Ӳ���ϵ�
 */
T_TA_ReturnCode taDebugHardBpIsFull(UINT32 sessionID, BOOL isAddr);

/*
 * @brief:
 *     �������ݶϵ�
 * @param[in]:addr:���ݶϵ��ַ
 * @return:
 *    ��
 */
void taDebugSetDataBreakpoint(UINT32 addr);

/*
 * @brief:
 *     ����ָ��ϵ�
 * @param[in]:addr:ָ��ϵ��ַ
 * @return:
 *    ��
 */
void taDebugSetAddressBreakpoint(UINT32 addr);

/*
 * @brief:
 *    ��ȡ���ݶϵ��ַ
 * @return:
 *    ���ݶϵ��ַ
 */
UINT32 taDebugGetDataBreakpoint(void);

/*
 * @brief:
 *    ��ȡָ��ϵ��ַ
 * @return:
 *    ָ��ϵ��ַ
 */
UINT32 taDebugGetAddressBreakpoint(void);

/*
 * @brief:
 *     ��ȡDAR�Ĵ���ֵ
 * @return:
 *    DAR�Ĵ���ֵ
 */
UINT32 taDebugGetDARValue(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _DB_DR_H */
