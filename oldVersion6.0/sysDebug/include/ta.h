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
 *@file:ta.h
 *@brief:
 *        <li>��TA��ģ���ṩ��ͷ�ļ�</li>
 */
#ifndef _TA_H
#define _TA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "db.h"
#include "taPacket.h"
#include "taDevice.h"
#include "taPacketComm.h"
#include "taDirectMemory.h"
#include "taProtocol.h"
#include "taMsgDispatch.h"
#include "taDebugMode.h"
#include "taUtil.h"
#include "taCommand.h"
#include "sdaMain.h"
#include "sdaCpuset.h"
#include "sdaCpuLib.h"
#include "string.h"

/************************�궨��********************************/

#define TA_xprint printk

/*��ֵ�궨��*/
#ifndef MAX
#define MAX(x,y) (((x) > (y))? (x):(y))
#endif

#ifndef MIN
#define MIN(x,y) (((x) < (y))? (x):(y))
#endif

#undef DEBUG
#ifdef DEBUG
	#define DBG(x) \
            do {(x);} while ((0))
#else
	#define DBG(x)
#endif

#define TA_READ		0
#define TA_WRITE	1
/************************���Ͷ���******************************/

/* ��������� */
typedef enum
{
    TA_MANAGER = 0,        /* MANAGER */
    TA_SYS_DEBUGER = 1,    /* ϵͳ���Դ���SDA */
    TA_DYNAMIC_LOADER = 2, /* ��̬���ش��� */
    TA_RSE_AGENT = 3,      /* RSE���� */
    TA_CTRL_COMMAND = 4,   /* ���Կ���������� */
} TA_SUBA_ID;

/* TA�������� */
typedef struct
{
    INT8 *name;
    UINT8 *ip;
    INT16 port;
    INT8 *mac;
    UINT32 minor;
} T_TA_NET_CONFIG;

/* ָ�룬�ص����� */
typedef void (*callbackhandler)(void);

/* �ַ�����ʽ�� */
extern int sprintf(INT8 *buffer,const char *  fmt,...);

/* �����쳣������ָ�� */
extern void (*_func_taTaskExceptionHandler)(UINT32 vector, T_DB_ExceptionContext *context);

/* ϵͳ�쳣������ָ�� */
extern void (*_func_taSysExceptionHandler) (UINT32 vector, T_DB_ExceptionContext *context);

/* �ڴ��л�����ָ�� */
extern FUNCPTR _func_taskMemCtxSwitch;

/* ��ѯ�쳣������ */
extern T_DB_ExceptionContext taGdbGetExpContext;

/* ϵͳ�����ԻỰ  */
extern T_TA_Session taSDASessInfo;

/* wdb����ID */
extern int	wdbTaskId;
/************************�ӿ�����******************************/

/*
 * @brief:
 *     ��װ�쳣��������IDT����
 * @param[in] vector :�쳣��
 * @param[in] vectorHandle :�쳣������
 * @return:
 *     ��
 */
void taInstallVectorHandle(int vector, void *vectorHandle);

/*
 * @brief:
 *     trap�쳣����
 * @return:
 *     ��
 */
void taTraceExceptionStub(void);

/*
 * @brief:
 *     �ϵ��쳣����
 * @return:
 *     ��
 */
void taBpExceptionStub(void);

/*
 * @brief:
 *      �쳣�����������,���ݵ���ģʽ�Ͱ�װ���쳣�����������쳣����
 * @param[in]: vector: �쳣��
 * @param[in]: context: �쳣������
 * @return:
 *     ��
 */
void taExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context);

/*
 * @brief:
 *      �����쳣����
 * @param[in]: vector: �쳣��
 * @param[in]: context: �쳣������
 * @return:
 *     ��
 */
void taTaskExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context);

/*
 * @brief:
 *      ϵͳ�������쳣�������
 * @param[in]:vector: �����쳣������
 * @param[in]: context: �쳣������
 * @return:
 *      ��
 */
void taSDAExceptionHandler(UINT32 vector, T_DB_ExceptionContext *context);

/*
 * @brief:
 *     ˢ��Cache
 * @param[in]: address: Ҫ���µ�cache�ڴ��ַ
 * @param[in]: size: Ҫ���µĳ���
 * @return:
 *     ��
 */
void taCacheTextUpdata(UINT32 address, UINT32 size);

/*
 * @brief:
 *     ��ȡ��ǰ����ID
 * @return:
 *     ��ǰ����ID
 */
int taTaskIdSelf(void);

/*
 * @brief:
 *      �����豸����ַ�
 * @param[in]: ch:�ַ�
 * @return:
 *      ��
 */
void taDisplayDeviceOutputChar(char ch);

/*
 * @brief:
 *    ��printk�豸��ӡ�����ַ�
 * @param[in]: ch: ��Ҫ������ַ�
 * @return: 
 *    ��
 */ 
void taPrintChar(char ch);

/*
 * @brief:
 *      ��ʼ�������豸
 * @return:
 *      ��
 */
void taDebugDeviceInit(void);

/*
 * @brief:
 *      ��ʾ������Ϣ
 * @return:
 *      ��
 */
void taDisplayStartedInfo(void);

/*
* @brief:
*     �Žӿڳ�ʼ��
* @return:
*      ��
*/
void taBrigdeInit(void);

/**
 * @brief:
 *     �ڴ�̽���ʼ��
 * @param[in]:_func_excBaseHook: �쳣HOOKָ�룬�쳣����ʱ����ϵͳ���ø�HOOK
 * @return:
 * 	   OK����ʼ���ɹ�
 * 	   ERROR����ʼ��ʧ��
 */
STATUS taMemProbeInit(FUNCPTR *_func_excBaseHook);

/*
 * @brief:
 *      ���񼶵�����ͣ
 * @param[in]: sid: �ỰID
 * @return:
 *     ERROR: ��ͣʧ��
 *     OK: ��ͣ�ɹ�
 */
STATUS taTaskPause(INT32 sid);

/*
 * @brief:
 *     ���񼶵��Դ�����ں���
 * @param[in]: packet:�������Բ���������Ϣ��
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL: ����ִ��ʧ��
 *     TA_DB_NEED_IRET����Ҫ���쳣�з���
 *     TA_COMMAND_NOTFOUND: ����û�ҵ�
 */
T_TA_ReturnCode taTaskDebugAgent(T_TA_Packet *packet);

/*
 * @brief:
 *     ��װ����ɾ��HOOK����ÿ������ɾ��ʱ���ø�HOOK
 * @param[in]:deleteHook:����ɾ��HOOK����ָ��
 * @return:
 *     ERROR:����ɾ��HOOK������
 *     OK:��װ����ɾ��HOOK�ɹ�
 */
STATUS taTaskDeleteHookAdd(FUNCPTR deleteHook);

/*
 * @brief:
 *     ��װ���񴴽�HOOK����ÿ�����񴴽�ʱ���ø�HOOK
 * @param[in]:createHook:���񴴽�HOOK����ָ��
 * @return:
 *     ERROR:���񴴽�HOOK������
 *     OK:��װ���񴴽�HOOK�ɹ�
 */
STATUS taTaskCreateHookAdd(FUNCPTR createHook);

/*
 * @brief:
 *     У�������Ƿ����
 * @param[in]:taskId:����ID
 * @return:
 *     OK:�������
 *     ERROR:���񲻴���
 */
int taTaskIdVerify(int taskId);

/*
 * @brief:
 *     ��ȡ����������
 * @param[in]:taskId:����ID
 * @param[out]:context:����������
 * @return:
 *     OK����ȡ�����ĳɹ�
 *     ERROR����ȡ������ʧ��
 */
int taTaskContextGet(int taskId, T_DB_ExceptionContext *context);

/*
 * @brief:
 *     ��������������
 * @param[in]:taskId:����ID
 * @param[in]:context:����������
 * @return:
 *     OK�����������ĳɹ�
 *     ERROR������������ʧ��
 */
int taTaskContextSet(int taskId, T_DB_ExceptionContext *context);

/*
 * @brief:
 *     ��ȡ��ǰCPU ID
 * @return:
 *     CPU ID
 */
UINT32 taGetCpuID(void);

/*
 * @brief:
 *     ��ȡOS��ʹ�ܵ�CPU
 * @return:
 *     OS��ʹ�ܵ�CPU
 */
cpuset_t taCpuEnabledGet (void);

/**
* @brief
*    �������շ����ݵ�ģʽ��Ϊ��ѯģʽ
* @param  ��
* @return
*	OK:����ģʽ�л��ɹ�
*	ERROR:����ģʽ�л�����
*/
INT32 taSetEndToPoll(void);

/**
* @brief
*    �������շ����ݵ�ģʽ��Ϊ�ж�ģʽ
* @param  ��
* @return
*	OK: ����ģʽ�л��ɹ�
*	ERROR: ����ģʽ�л�����
*/
INT32 taSetEndToInt(void);

/*
 * @brief:
 *     ��������
 * @param[in]:taskId:����ID
 * @return:
 *     OK:��ͣ����ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskSuspend(int tid);

/*
 * @brief:
 *     �ָ���������
 * @param[in]:tid:����ID
 * @return:
 *     OK:�ָ��������гɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskResume(int tid);

/*
 * @brief:
 *     ��������
 * @param[in]:tid:����ID
 * @return:
 *     OK:��������ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskActivate(int tid);

/*
 * @brief:
 *     ��������
 * @param[in]:tid:����ID
 * @return:
 *     OK:��������ɹ�
 *     ERROR:��Ч����ID
 */
STATUS taTaskCont(int tid);

/*
 * @brief:
 *     ��������ID��ȡ������
 * @param[in]:taskID:����ID
 * @return:
 *     OK:��ȡ�������ɹ�
 *     ERROR:��ȡ������ʧ��
 */
char *taTaskName(int taskID);

/*
 * @brief:
 *     ��ȡϵͳ�������б�
 * @param[out]:idList:����ID�б�
 * @param[in]:maxTasks:�����б��ܹ����ɵ����������
 * @return:
 *     �����б��е��������
 */
int taTaskIdListGet(int idList[],int maxTasks);

/*
 * @brief:
 *     ��ֹ����CPU������ռ
 * @return:
 *     OK:�ɹ�
 *     ERROR:ʧ��
 */
STATUS taTaskCpuLock(void);

/*
 * @brief:
 *     ʹ�ܱ���CPU������ռ������ִ�е��Ⱥ���
 * @return:
 *     OK:�ɹ�
 *     ERROR:ʧ��
 */
STATUS taTaskCpuUnlockNoResched(void);

/*
 * @brief:
 *     ʹ�ܱ���CPU������ռ
 * @return:
 *     OK:�ɹ�
 *     ERROR:ʧ��
 */
STATUS taTaskCpuUnlock(void);

/*
 * @brief:
 *     �����ڴ�
 * @param[in]:nBytes:�����ڴ��С
 * @return:
 *     �ڴ��ַ
 */
void *taMalloc(size_t nBytes);

/*
 * @brief:
 *     �ͷ��ڴ�
 * @param[in]: ptr:�ڴ��ַ
 * @return:
 *     �ڴ��ַ
 */
void taFree(void *ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TA_H */
