/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-10-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:taMemProbeLib.h
 *@brief:
 *             <li>�ڴ�̽����غ�������</li>
 */
#ifndef _TA_MEM_PROBE_LIB_H
#define _TA_MEM_PROBE_LIB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ �� ��******************************/
#include "ta.h"

/************************�궨��*******************************/

/************************���Ͷ���*****************************/

/************************�ӿ�����*****************************/

/**
 * @brief:
 *     �ڴ�̽���ʼ��
 * @param[in]:_func_excBaseHook: �쳣HOOKָ�룬�쳣����ʱ����ϵͳ���ø�HOOK
 * @return:
 * 	   OK����ʼ���ɹ�
 * 	   ERROR����ʼ��ʧ��
 */
STATUS taMemProbeInit(FUNCPTR *_func_excBaseHook);

/**
 * @brief:
 *     �ڴ�̽����ϵ�ṹ��س�ʼ�����ҽ��ڴ���ʷǷ��쳣������
 * @param[in]:_func_excBaseHook: �쳣HOOKָ�룬�쳣����ʱ����ϵͳ���ø�HOOK
 * @return:
 * 	   OK����ʼ���ɹ�
 */
STATUS taMemArchProbeInit(FUNCPTR *_func_excBaseHook);

/*
 * @brief:
 *     �ڴ�̽�⣬̽���ڴ��Ƿ�ɶ�д��������ɶ�д���������ߴ����������ģʽΪTA_READ�����ݽ�
 *     ������pVal��ַ�����������ģʽΪTA_WRITE������pValָ���ֵд��pAdrs��ַ��
 * @param[in]:pAdrs: �ڴ��ַ
 * @param[in]:mode:����ģʽ��TA_READ���� TA_WRITE
 * @param[out]:length: �ڴ���ʳ���
 * @param[in]:pVal: TA_WRITEģʽpValΪԴ��ַ��TA_READģʽΪ pValΪĿ�ĵ�ַ
 *     OK����д�ڴ�ɹ�
 *     ERROR�������Ƿ���ַ�����쳣
 */
STATUS taMemCommonProbe(char *pAdrs, int  mode, int length, char *pVal);

/*
 * @brief:
 *     �ڴ�̽�⣬̽���ڴ��Ƿ�ɶ�д��������ɶ�д���������ߴ����������ģʽΪTA_READ�����ݽ�
 *     ������pVal��ַ�����������ģʽΪTA_WRITE������pValָ���ֵд��pAdrs��ַ��
 * @param[in]:pAdrs: ���ʵ��ڴ��ַ
 * @param[in]:mode: �ڴ����ģʽ��TA_READ���� TA_WRITE
 * @param[out]:length: �ڴ���ʳ���
 * @param[in]:pVal: TA_WRITEģʽpValΪԴ��ַ��TA_READģʽΪ pValΪĿ�ĵ�ַ
 *     OK���ڴ�ɷ���
 *     ERROR���ڴ治�ɷ���
 */
STATUS taMemProbe(char *pAdrs, int  mode, int length, char *pVal);

/*
 * @brief:
 *      �ڴ�̽�⣬̽���ڴ��Ƿ�ɶ�д��������ɶ�д���������ߴ���
 * @param[out]:length: �ڴ���ʳ���
 * @param[in]:src: Դ��ַ
 * @param[in]:dest: Ŀ�ĵ�ַ
 *      OK����д�ڴ�ɹ�
 *      ERROR�������Ƿ���ַ�����쳣
 */
STATUS taMemProbeSup(int length, char *src, char* dest);

/*
 * @brief:
 *     ����һ���ڴ淶Χ��ַ�Ƿ�ɷ���
 * @param[in]:pAdrs: ���ʵ��ڴ��ַ
 * @param[out]:size: �ڴ���ʳ���
 * @param[in]:mode: �ڴ����ģʽ��TA_READ���� TA_WRITE
 * @param[in]:width: ���ʿ��
 *     TA_OK���ڴ�ɷ���
 *     TA_INVALID_INPUT���ڴ���ʷ�Χ���
 *     TA_ERR_MEM_ACCESS���ڴ����ʧ��
 */
UINT32 taMemTest(char *addr, UINT32 size, int mode, UINT32 width);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TA_MEM_PROBE_LIB_H */
