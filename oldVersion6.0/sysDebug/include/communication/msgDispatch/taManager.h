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
 *@file:taManager.h
 *@brief:
 *             <li>��Ϣ����ģ��ͷ�ļ�</li>
 */

#ifndef _TA_MANAGER_H
#define _TA_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"

/************************�궨��********************************/

/* Manager֧�ֵ��������*/
#define TA_MANAGER_COMMANDNUM  5

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

/*
 * @brief:
 *     ��ʼ��TA��ѯģ��
 * @return:
 *     ��
 */
void taManagerInit(void);

/*
 * @brief:
 *     ��ȡ��ϵ�ṹ��Ϣ
 * @param[out]:outbuf: ������ݻ���
 * @param[out]:outsize: ������ݳ���
 * @return:
 *      ��
 */
void taArchInfoGet(UINT8 *outbuf, UINT32 *outSize);

/*
 * @brief:
 *     ��ѯ��ϵ�ṹ�Ĵ�С��
 * @param[out]:outbuf: ������ݻ���
 * @param[out]:outsize: ������ݳ���
 * @return:
 *      ��
 */
void taEndianInfoGet(UINT8 *outbuf, UINT32 *outSize);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TA_MANAGER_H */
