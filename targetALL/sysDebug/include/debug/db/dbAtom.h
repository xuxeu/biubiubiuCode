/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ��� 
 */

/*
 *@file:dbAtom.h
 *@brief:
 *             <li>����ԭ�Ӳ���ͷ�ļ�</li>
 */
#ifndef _DBATOM_H
#define _DBATOM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"
#include "taErrorDefine.h"
#include "dbContext.h"
#include "sdaMain.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

/*
 * @brief:
 *     ���쳣������ת���ɱ�׼POSIX�źű�ʾ��ʽ
 * @param[in]:vector:�쳣������
 * @param[in]:context:�����쳣������
 * @return:
 *     ���������Ŷ�Ӧ��POSIX�ź�
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context);

/*
 * @brief:
 *     ��ȡ������Ӧ�������������мĴ�����ֵ
 * @param[in]:context: �����쳣������
 * @param[out]: outBuf:������ݻ��壬��ʮ�������ַ��������ȡ�������мĴ���ֵ
 * @param[out]:outBufferLen:�������ĳ���,������峤�������DB_PPC_REGISTER_SUM_SIZE(���Ĵ�����ֵת����16�����ַ����󣬳�����չΪԭ����2��)
 * @return:
 *     TA_OK:�����ɹ�
 *     TA_OUT_OF_BUFFER:������峤�Ȳ���
 */
T_TA_ReturnCode taDebugReadAllRegisters(T_DB_ExceptionContext *context, UINT8 *outBuf, UINT32 *outBufferLen);

/*
 * @brief
 *     ���������û��Ĵ���
 * @param[in] context:�쳣������
 * @param[in] registerNum:�Ĵ�����
 * @param[out] outBuf:���buffer
 * @param[out] outBufferLen::�������ĳ���
 * @return
 *     TA_OK:�����ɹ�
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen);

/*
 * @brief
 *     д�������û��Ĵ���
 * @param[in]: context:�쳣������
 * @param[in]: registerNum:�Ĵ�����
 * @param[in]: inBuf:����buffer
 * @return
 *     TA_OK:�����ɹ�
 *     TA_FAIL:����ʧ��
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
T_TA_ReturnCode taDebugWriteSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, const UINT8 *inBuf);

/*
 * @brief:
 *     д������Ӧ�������������мĴ�����ֵ
 * @param[in]:context: �����쳣������
 * @param[in]:inBuf:�������ݻ��壬��������Ϊд�����мĴ�����ֵ(ʮ����ֵ�ַ�����ʾ)
 * @param[in]:inLen:�������ݳ��ȣ��������ݵĳ��ȱ���ΪDB_PPC_REGISTER_SUM_SIZE(�����ʮ�������ַ���Ϊ���мĴ��������ܺ͵�2��)
 * @return:
 *     TA_OK:�����ɹ�
 *     TA_OUT_OF_BUFFER:���뻺�峤�ȷǷ�
 */
T_TA_ReturnCode taDebugWriteAllRegisters(T_DB_ExceptionContext *context, const UINT8 *inBuf, UINT32 inLen);

/*
 * @brief:
 *     ���������Ӧ����������PS�Ĵ����ĵ�����־λ��ȷ�����ص�������Ӧ�ú������������
 * @param[in]:context: �����쳣������
 * @return:
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context);

/*
 * @brief:
 *     ���ñ�����Ӧ����������PS�Ĵ����ĵ�����־λ,ȷ�����ص�������Ӧ������ִ��һ��ָ���Ժ󴥷������쳣
 * @param[in]:context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext *context);

/*
 * @brief:
 *    ��ȡ�����Գ���ǰ�ϵ㴥���ĵ�ַ���������Գ��򴥷��ϵ��쳣��,PC�Ĵ�����ֵ��ָ��ǰ�ϵ�ĵ�ַ
 * @param[in]:context: �����쳣������
 * @param[out]:address:�洢�����ϵ�ָ��ĵ�ַ
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugGetCurrentBpAddress(T_DB_ExceptionContext *context, UINT32 *address);

/*
 * @brief:
 *     ���������쳣ָ��
 * @param[in]: context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugJumpOverTrap(T_DB_ExceptionContext *context);

/*
 * @brief:
 *      ��ϵ�ṹ��ʼ��
 * @return:
 *     TRUE: ��ʼ���ɹ�
 */
BOOL taDebugArchInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _DBATOM_H */
