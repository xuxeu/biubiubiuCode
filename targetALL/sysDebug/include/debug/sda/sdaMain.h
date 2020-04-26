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
 * @file  sdaMain.h
 * @brief
 *       ����:
 *       <li>CPU������غ�������</li>
 */
#ifndef _SDA_MAIN_H_
#define _SDA_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "dbContext.h"
#include "taPacket.h"
#include "taskSllLib.h"

/************************�궨��********************************/

/* ϵͳ�����ԻỰID */
#define TA_SYS_DEBUGER_SESSION 1

/* ���õ�ǰ�����쳣���߳�ID */
#define TA_SET_CUR_THREAD_ID(debugInfo, threadId) \
do\
{\
	if(ERROR != (threadId))\
    {\
	    ((debugInfo)->curThreadId = (threadId));\
	}\
}\
while(0)

/* ��ȡ��ǰ�����쳣���߳�ID */
#define TA_GET_CUR_THREAD_ID(debugInfo) ((debugInfo)->curThreadId)

/* ���õ���״̬ */
#define TA_SET_DEBUG_STATUS(debugInfo, debugStatus) ((debugInfo)->status = (debugStatus))

/* ���õ��Զ���ID */
#define TA_SET_DEBUG_OBJ_ID(debugInfo, id) ((debugInfo)->objID = (id))

/* ���ԻỰ�Ƿ�������̬ */
#define TA_DEBUG_IS_CONTINUE_STATUS(debugInfo) ((debugInfo)->status == DEBUG_STATUS_CONTINUE)

/* ���ԻỰ�Ƿ�����̬ͣ */
#define TA_DEBUG_IS_PAUSE_STATUS(debugInfo) ((debugInfo)->status == DEBUG_STATUS_PAUSE)

/************************���Ͷ���******************************/

/* ����״̬ */
typedef enum
{
    DEBUG_STATUS_NONE = 0,
    DEBUG_STATUS_CONTINUE = 1,
    DEBUG_STATUS_EXCEPTION = 2,
    DEBUG_STATUS_PAUSE = 4
}T_DB_DebugStatus;

/* ���������״̬ */
typedef enum
{
	DEBUG_STATUS_DETACH = 0,
	DEBUG_STATUS_ATTACH = 1
}T_TA_AttachStatus;

/* ����ڵ� */
typedef struct
{
    SLL_NODE slNode;
    INT32    taskId;
} T_TASK_NODE;

/* ���������� */
typedef enum
{
    TA_CTX_SYSTEM	  = 0,	/* ϵͳģʽ */
    TA_CTX_TASK	      = 1,	/* �������� */
    TA_CTX_TASK_RTP	  = 2,	/* ����ʵʱ���� */
    TA_CTX_SYSTEM_RTP = 3,  /* ϵͳ��ʵʱ���� */
    TA_CTX_TYPE_NUM
} TA_CTX_TYPE;

/* ������Ϣ */
typedef struct
{
    /* �쳣������,�����ָ�뷽���ڻ���д����쳣������ */
    T_DB_ExceptionContext *context;

    /* �������� */
    SLL_LIST taskList;

    /* �����Ƿ��Զ��� */
    BOOL taskIsAutoAttach;

    /* �ỰID */
    UINT32 sessionID;

    /* �����Զ��� */
    UINT32 objID;

    /* ��ǰ�����쳣���߳�ID */
    INT32 curThreadId;

    /* ���������� */
    TA_CTX_TYPE ctxType;

    /* ����״̬ */
    T_DB_DebugStatus status;

    /* ��¼�����״̬ */
    T_TA_AttachStatus attachStatus;
} T_DB_Info;

/* ������ԻỰ�ṹ */
typedef struct
{
    T_DB_Info sessInfo;              /* �����Զ��������ĺͻỰID */
    T_TA_Packet packet;              /* �����˹��߽�������Ϣ�� */
    T_TA_PacketHead gdbPacketHead;   /* ���������������ʱ��ID */
} T_TA_Session;

/************************�ӿ�����******************************/

/*
 * @brief:
 *         ��ȡ������Ϣ
 * @return:
 *       ϵͳ���ĵ�����Ϣ
 */
T_DB_Info* taSDADebugInfoGet(void);

/*
 * @brief:
 *     ��ȡ�쳣������ָ��
 * @return:
 * 		�쳣������ָ��
 */
T_DB_ExceptionContext* taSDAExpContextPointerGet(UINT32 cpuIndex);
#ifdef __PPC__
#ifdef TA_ALTIVEC_SUPPORT_DEBUG
/*
 * @brief:
 *     ��ȡ�쳣ʸ��������ָ��
 * @return:
 * 		�쳣ʸ��������ָ��
 */
T_DB_ExceptionVectorContext* taSDAExpVectorContextPointerGet(UINT32 cpuIndex);
#endif
#endif

/*
 * @brief:
 *      SDA��ʼ��,�����ʼ��ϵͳ�����ԻỰ��������Ϣ�������İ�װ
 * @return:
 *        TA_OK: ��ʼ���ɹ�
 */
T_TA_ReturnCode taSDAInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SDA_MAIN_H_ */
