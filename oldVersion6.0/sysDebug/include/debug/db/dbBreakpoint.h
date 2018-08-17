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
 *@file:dbBreakpoint.h
 *@brief:
 *             <li>���Զϵ������</li>
 */
#ifndef _DB_BREAKPOINT_H
#define _DB_BREAKPOINT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taErrorDefine.h"
#include "taTypes.h"
#include "dbContext.h"
#include "sdaMain.h"

/************************�궨��********************************/

/* ����ϵ���� */
#define	CONFIG_DB_BREAK_POINT (500)

/************************���Ͷ���******************************/

/* �ϵ�ö�����Ͷ���*/
typedef enum
{
    /*��������ϵ�*/
    DB_BP_NORMAL = 0,

    /*Ӳ���ϵ�*/
    DB_BP_HWBP,

    /*Ӳ��д�۲��*/
    DB_BP_HWWP_W,

    /*Ӳ�����۲��*/
    DB_BP_HWWP_R,

    /*Ӳ�����ʹ۲�㣨��д��*/
    DB_BP_HWWP_A,

    /* ���Ϸ�����*/
    DB_BP_INVALID
} T_DB_BpType;

/*�ϵ����*/
typedef enum
{
    BPClass_NORMAL,  /* һ��ϵ���� */
    BPClass_TRACE    /* ���ٵ� */
} T_DB_BpClass;

/* ���ڶϵ㱾�ػ���Ľṹ�嶨�� */
typedef struct T_DB_BreakpointStruct
{
    /*����Ϊ����ʹ��ʱ����ʾ��һ����ͬ��ַ�Ķϵ�*/
    struct T_DB_BreakpointStruct *next_addr;

    /*�͵�ǰ�ϵ��ַ��ͬ����һ���ϵ�*/
    struct T_DB_BreakpointStruct *same_addr;

    /* �ϵ����ڵĻỰID */
    UINT32 sessionID;

    /* �ϵ���Ч����ID */
    UINT32 objID;

    /* �ϵ��߼���ַ */
    UINT32 logicAddr;

    /*�ϵ�����*/
    T_DB_BpType bpType;

    /* �ϵ���� */
    T_DB_BpClass bpClass;

    /* �ϵ��� */
    INT32 breakSize;

    /*�ϵ㴦ԭ���ڴ浥Ԫ������*/
    UINT8  shadow[DB_ARCH_BREAK_INST_LEN];
} T_DB_Breakpoint;

/************************�ӿ�����******************************/

/*
 * @brief:
 *     ��ʼ���ϵ�������ݽṹ
 * @return:
 *     ��
 */
void taDebugBreakPointInit(UINT32 bpNumber);

/*
 * @brief:
 *    ����ƥ��Ķϵ���Ϣ(�ỰID + objID + logicAddrȫ��Ҫƥ��)
 * @param[in]:sessionID: ���ԻỰID
 * @param[in]:objID: �����Զ���ID
 * @param[in]:logicAddr: �ϵ��߼���ַ
 * @param[out]:breakpointList: �洢�鵽�ķ��������Ķϵ��б�,����߼���ַ��ͬ��sessionID,taskID��ͬ���򷵻ص�һ��ƥ��Ķϵ�
 * @return:
 *         TA_OK:�ҵ���ƥ��Ķϵ�
 *         TA_DB_BREAKPOINT_NOTFOUND:û���ҵ��ϵ�,sessionID,taskID,addrȫ����ͬ
 *         TA_DB_BREAKPOINT_DIFFER_ID:û���ҵ���ȫƥ��Ķϵ�,addr��ͬ;sessionID,taskID��ͬ
 */
T_TA_ReturnCode taDebugLookUpBP(UINT32 sessionID, UINT32 objID, UINT32 logicAddr, T_DB_Breakpoint **breakpointList);

/*
 * @brief:
 *        ����һ���ϵ�(����ϵ��Ӳ���ϵ�)������Ѿ������߼���ַ��ͬ�Ķϵ��������ͬ��ַ����������m_breakManageInfo.bp_busy��ͷ
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:breakpoint: �ϵ���Ϣ
 * @return:
 *         TA_OK                   :ִ�гɹ�
 *         TA_DB_BREAKPOINT_FULL   :�ϵ������
 *         TA_INVALID_PARAM        :�Ƿ��Ĳ���
 *         TA_DB_BREAKPOINT_EXIST  :��ͬ�Ự,��ͬ�������ͬ��ַ�Ѵ���һ���ϵ�
 */
T_TA_ReturnCode taDebugInsertBP(T_DB_Info *debugInfo, T_DB_Breakpoint *breakpoint);

/*
 * @brief:
 *        ɾ��һ���ϵ�(����������ȫƥ��)
 * @param[in]:debugInfo :���ԻỰ��Ϣ
 * @param[in]:logicAddr :�ϵ��ַ
 * @param[in]:breakpointType :�ϵ�����
 * @param[in]:breakpointClass :�ϵ����
 * @param[in]:size :�ϵ���
 * @return:
 *         TA_OK                    :ִ�гɹ�
 *         TA_FAIL                    :ɾ���ϵ�ʧ��
 *         TA_DB_BREAKPOINT_EMPTY      :û�д��κζϵ�
 *         TA_INVALID_PARAM         :�Ƿ��Ĳ���
 *         TA_DB_BREAKPOINT_NOTFOUND   :û�ҵ��ϵ�
 */
T_TA_ReturnCode taDebugRemoveBP(T_DB_Info *debugInfo, UINT32 logicAddr, T_DB_BpType breakpointType, T_DB_BpClass breakpointClass, UINT32 size);

/*
 * @brief:
 *      ���ַaddr������һ���ϵ�ָ�ͬʱ��addr��ԭ�е����ݱ��浽shadow��
 * @param[in]:ctxId: ������ID
 * @param[in]:addr: ����ϵ�ָ��ĵ�ַ
 * @param[in]:shadow: ��������ϵ㴦ԭ�����ݵĻ���
 * @return:
 *        TA_OK: �����ɹ�
 *        TA_FAIL: ����ʧ��
 */
T_TA_ReturnCode taDebugInsertSoftBp(UINT32 ctxId,  UINT32 addr, UINT8 *shadow);

/*
 * @brief:
 *        �ӵ�ַaddr��ɾ��һ���ϵ�ָ�ͬʱ�ָ�addr��ԭ�е�����
 * @param[in]:ctxId: ������ID
 * @param[in]:addr: ����ϵ�ָ��ĵ�ַ
 * @param[in]:shadow: ��������ϵ㴦ԭ�����ݵĻ���
 * @return:
 *        TA_OK: �����ɹ�
 *        TA_FAIL:  ����ʧ��
 */
T_TA_ReturnCode taDebugRemoveSoftBp(UINT32 ctxId, UINT32 addr, const UINT8 *shadow);

/*
 * @brief:
 *      ɾ����ǰ���ԻỰ���жϵ�
 * @param[in] debugInfo: ���ԻỰ
 * @return:
 *      ��
 */
void taDebugRemoveAllBp(T_DB_Info *debugInfo);

/*
 * @brief:
 *      ��ֹ��������ϵ�
 * @return:
 *      ��
 */
void taDebugBpAllDisable(void);

/*
 * @brief:
 *      ʹ����������ϵ�
 * @return:
 *      ��
 */
void taDebugBpAllEnable(void);

/*
 * @brief:
 *     ����ڴ��ַ��ŵ��Ƿ��Ƕϵ�ָ��
 * @return:
 *     FALSE: �Ƕϵ�ָ��
 *     TRUE:�ϵ�ָ��
 */
BOOL taDebugBpInstructCheck(UINT8 *addr);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _DB_BREAKPOINT_H */
