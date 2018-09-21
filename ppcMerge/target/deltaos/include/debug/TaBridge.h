/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                               �������ļ���
*/

/*
 * @file�� taBridge.h
 * @brief��
 *	    <li>�Žṹ�嶨�壬����ϵͳ������ʹ�õ��Ĳ���ϵͳ��ؽӿ�</li>
 */

#ifndef _TA_BRIDGE_H
#define _TA_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "sdaCpuLib.h"
#include "dbContext.h"

/************************�궨��********************************/

#ifdef __MIPS__
/* �ų�ʼ��������ַ����ڴ� */
#define TA_BRIGDE_ADDR 0x81EFFFE0

/* TA�Ƿ�������־��ַ */
#define TA_START_FLAG_ADDR 0x81EFFFF0

#elif defined(__X86__)
/* �ų�ʼ��������ַ����ڴ� */
#define TA_BRIGDE_ADDR 0x14FFFC

/* TA�Ƿ�������־��ַ */
#define TA_START_FLAG_ADDR 0x14FFF0
#endif

#ifdef __PPC__
/* �ų�ʼ��������ַ����ڴ� */
#define TA_BRIGDE_ADDR 0x000EFFE0

/* TA�Ƿ�������־��ַ */
#define TA_START_FLAG_ADDR 0x000EFFF0

#endif

/* TA�Ƿ�������־ֵ */
#define TA_START_FLAG_VALUE 0x55aa55aa
/************************���Ͷ���******************************/

/* ����ϵͳ��ؽӿڶ��� */
typedef struct
{
	T_TA_ReturnCode (*sdaIpiInit)(TA_IPI_INSTALL_HANDLER_FUNC IPI_CONNECT, TA_IPI_EMIT_HANDLER_FUNC IPI_EMIT, TA_IPI_ENABLE_FUNC IPI_ENABLE);
	void  (*taTraceExceptionStub)(void); 
	void  (*taBpExceptionStub)(void); 
	int   (*taskIdSelf)(void); 
	int   (*taskIdVerify)(int tid); 
	char* (*taskName)(int tid);
	int   (*taskIdListGet)(int idList[],int maxTasks);
	int   (*taskContextGet)(int tid, T_DB_ExceptionContext *context);
	int   (*taskContextSet)(int tid, T_DB_ExceptionContext *context);
	cpuset_t (*cpuEnabledGet)(void);
	void (*cpuCtrlHandler)(T_DB_ExceptionContext *context);
	unsigned int (*getCpuID)(void);
	STATUS (*taMemProbeInit)(FUNCPTR *_func_excBaseHook);
	int (*taUdpRead)(UINT8* buf,UINT32 cnt,UINT32* dlen);
	int (*taUdpWrite)(UINT8* buf,UINT32 cnt,UINT32* dlen);
	void (*TaPacketNumGet)(UINT32* pTaTxPacketNum, UINT32* pTaRxPacketNum);
	int (*taGetSaid)();
	unsigned int* (*taGetBreakInfo)();
	BOOL (*taGetMultiThreadInfoTag)();
	void (*printUart)(char *fmt, ...);
}T_TA_BRIDGE;

/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _TA_BRIDGE_H */
