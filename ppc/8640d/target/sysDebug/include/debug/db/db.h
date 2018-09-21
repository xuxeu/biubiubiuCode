/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ��� 
 */

/*
 *@file:db.h
 *@brief:
 *             <li>��������ģ�������ṩ��ͷ�ļ�</li>
 */
#ifndef _DB_H
#define _DB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"
#include "dbStandardRsp.h"
#include "dbBreakpoint.h"
#include "dbSignal.h"
#include "dbContext.h"
#include "dbVector.h"
#include "dbAtom.h"
#include "dbAtomic.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

/*
 * @brief:
 *     �������Գ�ʼ��
 * @return:
 *     TRUE: ��ʼ���ɹ�
 *     FALSE: ��ʼ��ʧ��
 */
BOOL taDBInit(void);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif  /*_DB_H*/
