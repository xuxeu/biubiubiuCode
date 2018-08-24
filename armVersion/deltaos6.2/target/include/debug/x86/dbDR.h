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
 *@file:dbDR.h
 *@brief:
 *             <li>����X86Ӳ���ϵ���ص����ݽṹ���궨�弰����ؽӿ�����</li>
 */
#ifndef _DB_DR_H_
#define _DB_DR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"
#include "taErrorDefine.h"

/************************�궨��********************************/
/* X86�ϵ�Ĵ������� */
#define    DR0  0
#define    DR1  1
#define    DR2  2
#define    DR3  3
#define    DR4  4
#define    DR5  5
#define    DR6  6
#define    DR7  7
#define    DR_ERROR 8

/*X86�ϵ�����*/

/* дӲ���۲��(д */
#define  DR_WRITE     0x01
    
/* ��Ӳ���۲��(��) */
#define  DR_READ      0x03
    
/* ����Ӳ���۲��(��д) */
#define  DR_ACCESS    0x03
    
/* ִ��Ӳ���۲�� */
#define  DR_EXECUTE   0x0

/* X86�ϵ������� */
#define DR_BYTE  0x00 /* �ֽ� */
#define DR_WORD  0x01 /* �� */
#define DR_DWORD 0x03 /* ˫�� */

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

/*
 * @brief
 *   ��ȡDR0������
 *@return
 *   DR0������
 */
ulong_t taDebugGetDR0(void);

/*
 * @brief
 *   ��ȡDR1������
 *@return
 *   DR1������
 */
ulong_t taDebugGetDR1(void);

/*
 * @brief
 *   ��ȡDR2������
 *@return
 *   DR2������
 */
ulong_t taDebugGetDR2(void);

/*
* @brief
 *   ��ȡDR3������
 *@return
 *   DR3������
 */
ulong_t taDebugGetDR3(void);

/*
 * @brief
 *   ��ȡDR4������
 *@return
 *   DR4������
 */
ulong_t taDebugGetDR4(void);

/*
 * @brief
 *   ��ȡDR5������
 *@return
 *   DR5������
 */
ulong_t taDebugGetDR5(void);

/*
 * @brief
 *   ��ȡDR6������
 *@return
 *   DR6������
 */
ulong_t taDebugGetDR6(void);

/*
 * @brief
 *   ��ȡDR7������
 *@return
 *   DR7������
 */
ulong_t taDebugGetDR7(void);

/*
 * @brief
 *    ����DR0������
 * @param[in] content: ����
 * @return
 *      none
 */
void taDebugSetDR0(ulong_t content);

/*
 * @brief
 *    ����DR1������
 * @param[in] content: ����
 * @return
 *      none
 */
void taDebugSetDR1(ulong_t content);

/*
 * @brief
 *    ����DR2������
 * @param[in] content: ����
 * @return
 *      none
 */
void taDebugSetDR2(ulong_t content);

/*
 * @brief
 *    ����DR3������
 * @param[in] content: ����
 * @return
 *      none
 */
void taDebugSetDR3(ulong_t content);

/*
 * @brief
 *    ����DR4������
 * @param[in] content: ����
 * @return
 *      none
 */
void taDebugSetDR4(ulong_t content);

/*
 * @brief
 *    ����DR5������
 * @param[in] content: ����
 * @return
 *      none
 */
void taDebugSetDR5(ulong_t content);

/*
 * @brief
 *    ����DR6������
 * @param[in] content: ����
 * @return
 *      none
 */
void taDebugSetDR6(ulong_t content);

/*
 * @brief
 *    ����DR7������
 * @param[in] content: ����
 * @return
 *      none
 */
void taDebugSetDR7(ulong_t content);

/*
 * @brief
 *   ��ȡָ��DR������
 * @param[in] dr: ���ԼĴ����ı��(DR0/DR1/DR2/DR3/DR6/DR7)
 * @return
 *    DR������,���dr��������ʶ�𣬷���0
 */
ulong_t taDebugGetDR(UINT32 dr);

/*
 * @brief
 *     ����ָ��DR������
 * @param[in] wIndex: ���ԼĴ����ı��(DR0/DR1/DR2/DR3/DR6/DR7)
 * @param[in] wContent: ����
 * @return
 *    none
 */
void taDebugSetDR(UINT32 wIndex, ulong_t wContent);

/*
 * @brief
 *     ���ָ���Ķϵ�DR�Ƿ�������/��ʹ��
 * @param[in] wIndex: ���ԼĴ����ı��(DR0/DR1/DR2/DR3)
 * @return
 *     TA_OK: �ϵ�������
 *     TA_INVALID_INPUT:�Ĵ�����ŷǷ�
 *     DB_FAIL: �ϵ�δ����
 */
T_TA_ReturnCode taDebugIsSetDR(UINT32 wIndex);

/*
* @brief
 *    ���ϵ������ת����x86����
 * @param[in]    wType: ��������
 *        DB_BP_HWWP_W ---дӲ���۲��(д)
 *        DB_BP_HWWP_R ---��Ӳ���۲��(��)
 *        DB_BP_HWWP_A ---����Ӳ���۲��(��д)
 *        DB_BP_HWBP ---ִ��Ӳ���۲��
 * @return
 *    x86��Ķϵ�����
 */
UINT32 taDebugTypeConvert(UINT32 wType);

/*
 * @brief
 *   ���ϵ�����ݿ��ת����x86��ʽ�Ŀ��
 * @param[in]    wSize: ���ݿ��
 *                1 ---һ���ֽ�
 *                2 ---�����ֽ�
 *                4 ---�ĸ��ֽ�
 * @return
 *   x86��ʽ�����ݿ��
 */
UINT32 taDebugSizeConvert(UINT32 wSize);

/*
 * @brief
 *      �����Ƿ������ָ���ĵ�ַ���������͡�����ȫ��һ�µĶϵ�
 * @param[in] vpAddr: ���öϵ�ָ��ĵ�ַ
 * @param[in] wType: �ϵ�����
 *                DR_WRITE ---дӲ���۲��(д)
 *                DR_READ  ---��Ӳ���۲��(��)
 *                DR_ACCESS---����Ӳ���۲��(��д)
 *                DR_EXECUTE  ִ��Ӳ���۲��
 * @param[in] wSize: �ϵ�Ŀ��
 *                DR_BYTE  ---�ֽ�
 *                DR_WORD  ---��
 *                DR_DWORD ---˫��
 * @return
 *      ����ɹ�����ƥ��Ķϵ�DR�����򷵻�DR_ERROR
 */
UINT32 taDebugMatchDR(ulong_t vpAddr, UINT32 wType, UINT32 wSize);

/*
 * @brief
 *     ʹ��ָ���Ķϵ�DR
 * @param[in] wIndex: ���ԼĴ����ı��(DR0/DR1/DR2/DR3)
 * @return
 *     none
 */
void taDebugEnableDR(UINT32 wIndex);

/*
 * @brief
 *     ��ָֹ���Ķϵ�DR
 * @param[in] wIndex: ���ԼĴ����ı��(DR0/DR1/DR2/DR3)
 * @return
 *     none
 */
void taDebugDisableDR(UINT32 wIndex);

/*
* @brief
 *    ���ҿ��еĶϵ�DR
 * @return
 *    ����ҵ����еĶϵ�DR�򷵻�idx�����򷵻�DR_ERROR
 */
UINT32 taDebugLookupDR(void);

/*
 * @brief:
 *     ���õ�Ӳ���ϵ��Ƿ��Ѿ��ﵽĿ���֧�ֵ����ޡ�
 * @param[in]:sessionID :���ԻỰID
 * @param[in]:isAddr:�ж�ָ��ϵ�����ݶϵ��ʾ
 *         TRUE:  ���ж�ָ��ϵ�
 *         FALSE: ���ж����ݶϵ�
 * @return:
 *     TRUE:    �Ѿ����õ�Ӳ���ϵ������ﵽ������
 *     FALSE:  �п��е�Ӳ���ϵ�
 */
BOOL taDebugHardBpIsFull(UINT32 sessionID, BOOL isAddr);

/*
 * @brief
 *     �ڵ�ַaddr������һ��Ӳ���ϵ�ָ��
 * @param[in] idx: ���ԼĴ����ı��(DR0/DR1/DR2/DR3)
 * @param[in] vpAddr: ���öϵ�ָ��ĵ�ַ
 * @param[in] wType: �ϵ�����
 *                DR_WRITE  ---дӲ���۲��(д)
 *                DR_READ   ---��Ӳ���۲��(��)
 *                DR_ACCESS ---����Ӳ���۲��(��д)
 *                DR_EXECUTE ִ��Ӳ���۲��
 * @param[in] wSize: �ϵ�Ŀ��
 *                DR_BYTE  ---�ֽ�
 *                DR_WORD  ---��
 *                DR_DWORD ---˫��
 * @return
 *     none
 */
void taDebugInsertDR(UINT32 wIndex, ulong_t vpAddr, UINT32 wType, UINT32 wSize);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_DR_H_ */
