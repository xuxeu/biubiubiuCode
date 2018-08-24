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
#ifdef CONFIG_TA_LP64
/* �ų�ʼ��������ַ����ڴ� */

#define TA_BRIGDE_ADDR 0xffffffff8014FFF8

/* TA�Ƿ�������־��ַ */
#define TA_START_FLAG_ADDR 0xffffffff8014FFF0

/* TA�Ƿ�������־ֵ */
#define TA_START_FLAG_VALUE 0x55aa55aa
#else
/* �ų�ʼ��������ַ����ڴ� */

#define TA_BRIGDE_ADDR 0x14FFF8

/* TA�Ƿ�������־��ַ */
#define TA_START_FLAG_ADDR 0x14FFF0

/* TA�Ƿ�������־ֵ */
#define TA_START_FLAG_VALUE 0x55aa55aa
#endif

/************************���Ͷ���******************************/

/* ����ϵͳ��ؽӿڶ��� */
typedef struct
{
	//T_TA_ReturnCode (*sdaIpiInit)(IPI_INSTALL_HANDLER_FUNC IPI_CONNECT, IPI_EMIT_HANDLER_FUNC IPI_EMIT);
	void  (*taTraceExceptionStub)(void); 
	void  (*taBpExceptionStub)(void); 
	ulong_t   (*taskIdSelf)(void); 
	int   (*taskIdVerify)(ulong_t tid); 
	char* (*taskName)(ulong_t tid);
	int   (*taskIdListGet)(ulong_t idList[],int maxTasks);
	int   (*taskContextGet)(ulong_t tid, T_DB_ExceptionContext *context);
	int   (*taskContextSet)(ulong_t tid, T_DB_ExceptionContext *context);
	cpuset_t (*cpuEnabledGet)(void);
	void (*cpuCtrlHandler)(T_DB_ExceptionContext *context);
	unsigned int (*getCpuID)(void);
	STATUS (*taMemProbeInit)(FUNCPTR *_func_excBaseHook);
	STATUS (*vxMemArchProbe)(void *adrs,int mode,int length,void *pVal);
}T_TA_BRIDGE;

/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _TA_BRIDGE_H */
