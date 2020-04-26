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
 *@file:taCommand.h
 *@brief:
 *             <li>��������</li>
 */
#ifndef _TACOMMAND_H
#define _TACOMMAND_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "taPacket.h"
#include "taErrorDefine.h"
#include "sdaMain.h"

/************************�궨��********************************/

/* ���������Ƿ���Ҫ�ظ� */
#define TA_NEED_REPLY       1
#define TA_NO_REPLY        	0

/************************���Ͷ���******************************/

/* �������������  */
typedef enum
{
    COMMANDTYPE_QUERY = 0,/* ����ǵ��Բ�ѯ������ */
    COMMANDTYPE_DEBUG,    /* �������������� */
    COMMANDTYPE_NONE      /* δ֪���������� */
} T_TA_CommandType;

/* TA��������������� */
typedef T_TA_ReturnCode (*T_TA_CommandProc)
(
   T_DB_Info *debugInfo,  /* ���ԻỰ������ */
   const UINT8 *inbuf,    /* ����Buffer */
   UINT32 inSize,         /* ����Buffer���� */
   UINT8 *outbuf,         /* ���Buffer */
   UINT32 *outSize        /* ���Buffer���� */
);

/* ����TA�����ṹ */
typedef struct
{
    char *ubCmdStr;           /*������*/
    T_TA_CommandProc cmdProc; /*�������*/
    INT32 ubNeedReply;        /* �����Ƿ���Ҫ�ظ�*/
} T_TA_CommandTable;

/* �����ǰ�ص����� */
typedef void (*TA_ProcessNoReplyCallback)(const T_TA_PacketHead *packetHead);

/* ������ص����� */
typedef void (*TA_ProcessPrepareCallback)(const UINT8* inbuf,const T_TA_PacketHead *packetHead);

/* RSP�������� */
typedef T_TA_CommandProc  T_DB_RspCommandProc;

/* RSP����� */
typedef T_TA_CommandTable T_DB_RspCommandTable;

/************************�ӿ�����******************************/

/*
 * @brief:
 *     ����ִ�е���������
 * @param[in]:   debugInfo: ���Դ���Ļ�����Ϣ
 * @param[in]:   inbuf:����Ĵ�����İ��壬�ް�ͷ
 * @param[in]:   inSize: ���Ĵ�С
 * @param[in]:   commandType: ��������
 * @param[in]:   cmdTable: ������� tpCmdTable �в�ѯ����
 * @param[out]:  outbuf: ������������İ��壬�ް�ͷ
 * @param[out]:  outSize: �������ظ����ݵ��ֽڳ���
 * @param[out]:  isNeedReply: �Ƿ���Ҫ�ظ�
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL: ����ִ��ʧ��
 *     TA_DB_NEED_IRET����Ҫ���쳣�з���
 *     TA_COMMAND_NOTFOUND: ����û�ҵ�
 */
T_TA_ReturnCode taProcessCommand(T_DB_Info *debugInfo,
                                 UINT8 *inbuf,
                                 UINT32 inSize,
                                 UINT8 *outbuf,
                                 UINT32 *outSize,
                                 BOOL *isNeedReply,
                                 T_TA_CommandType commandType,
                                 T_TA_CommandTable *cmdTable,
                                 UINT32 tableLength);
                                    
/*
 * @brief:
 * 	   TA��Ϣ�ӿڴ���ͳһ��ڣ����߼�ͨ��(��������)���Ե���ͳһ������Ϣ�ӿڵ�����
 * @param[in]:packet: Ҫ�������Ϣ��
 * @param[in]:debugInfo ���ԻỰ��Ϣ
 * @param[in]:commandType: �������ͣ�����Ϊ���������ѯ�����������
 * @param[in]:cmdTable: �����
 * @param[in]:tableLength: �������
 * @param[in]:noReplyCallback: �������������ظ�ʱ�Ļص��ӿ�
 * @param[in]:prepareCallback: ��������ǰ�Ļص��ӿ�
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL: ����ִ��ʧ��
 *     TA_DB_NEED_IRET����Ҫ���쳣�з���
 *     TA_COMMAND_NOTFOUND: ����û�ҵ�
 */
T_TA_ReturnCode taAgentEntry(T_TA_Packet *packet,
                             T_DB_Info *debugInfo,
                             T_TA_CommandType commandType,
                             T_TA_CommandTable *cmdTable,
                             UINT32 tableLength,
                             TA_ProcessNoReplyCallback noReplyCallback,
                             TA_ProcessPrepareCallback prepareCallback);

/************************ʵ    ��*******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TACOMMAND_H */
