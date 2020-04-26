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
 * @file  sdaCpuLib.h
 * @brief
 *       ����:
 *       <li>CPU������غ�������</li>
 */
#ifdef CONFIG_CORE_SMP

#ifndef _SDA_CPU_LIB_H_
#define _SDA_CPU_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "sdaCpuset.h"
#include "ta.h"

/************************�궨��********************************/

/* ��ʱ�����궨�� */
#define DEBUG_CPU_DELAY()

/* ����IPI�жϺ� */
#define TA_IPI_INTR_ID_DEBUG  (1)

/* IPI�ж�Ͷ�ݵȴ�ʱ�� */
#define TA_NO_WAIT	      (0)
#define TA_WAIT_FOREVER   (-1)

/************************���Ͷ���******************************/

/* IPI��غ���ָ�� */
typedef void (*IPI_HANDLER_FUNC)(void);
typedef INT32 (*IPI_INSTALL_HANDLER_FUNC)(INT32 ipiId, IPI_HANDLER_FUNC ipiHandler,void *ipiArg);
typedef INT32 (*IPI_EMIT_HANDLER_FUNC)(INT32 ipiId, UINT32 cpus);
typedef INT32 (*IPI_ENABLE_FUNC)(INT32 ipiId);

/************************�ӿ�����******************************/

/*
 *@brief
 *     ����:IPI��ʼ�����ҽӵ��Դ���˼��ж�
 *@return
 *    TA_OK:ִ�гɹ�
 *    DA_CONNECT_IPI_FAILED:�ҽӺ˼��жϴ�����ʧ��
 *	  DA_ENABLE_IPI_FAILED:ʹ�ܺ˼��ж�ʧ��
 */

T_TA_ReturnCode sdaIpiInit(IPI_INSTALL_HANDLER_FUNC IPI_CONNECT, IPI_EMIT_HANDLER_FUNC IPI_EMIT, IPI_ENABLE_FUNC IPI_ENABLE);

/*
 *@brief
 *     ����:��������CPU
 *@return
 *    TA_OK:ִ�гɹ�
 *    TA_DA_IPI_NO_INIT:IPIδ��ʼ����
 *	  TA_DA_IPI_EMIT_FAILED:Ͷ�����ж�ʧ��
 */
T_TA_ReturnCode sdaCpuAllSuspend(void);

/*
 *@brief
 *     ����:�ָ�����CPU
 *@return
 *    TA_OK:ִ�гɹ�
 *    TA_DA_IPI_NO_INIT:IPIδ��ʼ����
 *	  TA_DA_ILLEGAL_OPERATION:�Ƿ�����
 */
T_TA_ReturnCode sdaCpuAllResume(void);

/*
 *@brief
 *     ����:ͨ���߳�ID��ȡCPU ID
 *@return
 *    ����ɹ�����CPU ID�����򷵻�0xffffffff
 */
UINT32 sdaGetCpuIdByThreadId(UINT32 threadId);

/*
 *@brief
 *     ����:��ȡ������ID
 *@return
 *    �ɹ���������ID
 */
UINT32 GET_CPU_INDEX();

/*
 *@brief
 *     ����:IPI��ʼ�����ҽӵ��Դ���˼��ж�
 * @param[in]:func:�˼��жϵ��øú���
 * @param[in]:timeout:��ʱʱ��
 *@return
 *    TA_OK:ִ�гɹ�
 *    TA_IPI_EMIT_FAILED:Ͷ�ݺ˼��ж�ʧ��
 */
T_TA_ReturnCode sdaIpiEmit(IPI_HANDLER_FUNC	func, UINT32 timeout);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SDA_CPU_LIB_H_ */
#endif /* CONFIG_CORE_SMP */
