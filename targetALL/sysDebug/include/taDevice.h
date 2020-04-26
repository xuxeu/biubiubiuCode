/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:taDevice.h
 *@brief:
 *             <li>�豸��д�ӿڱ���</li>
 */
#ifndef _TADEVICE_H
#define _TADEVICE_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"
#include "taErrorDefine.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/* TA֧�ֵ�ͨ�ŷ�ʽ*/
typedef enum
{
    TA_COMM_UDP  = 0x1,   /* ������UDP���԰�Ϊ���ݷ��ͽ��յ�ͨ������*/
    TA_COMM_UART = 0x2    /* �����ڴ��ڵ��ַ��豸����ͨ��Э��*/
}T_TA_COMMType;

/* ͨ���豸������ṹ�嶨�� */
typedef struct
{
     /* �豸����,Ŀǰ֧��TA_COMM_UDP��TA_COMM_UART */
    T_TA_COMMType commType;

    /* ָ������ݵĺ���ָ�� */
    T_TA_ReturnCode (*read)(UINT8 *buf, UINT32 bufLen, UINT32 *receSize);

    /* ָ��д���ݵĺ���ָ�� */
    T_TA_ReturnCode (*write)(UINT8 *buf, UINT32 bufLen, UINT32 *writeSize);
 }T_TA_COMMIf;

/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TADEVICE_H */
