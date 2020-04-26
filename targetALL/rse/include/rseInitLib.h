/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-05-26         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  rseInitLib.h
 * @brief
 *       ���ܣ�
 *       <li>RSE��ʼ���������</li>
 */
#ifndef RSE_INITLIB_H_
#define RSE_INITLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************ͷ �� ��******************************/

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ӿ�����*****************************/

/*
 * @brief:
 *     ��װRSE��Ϣ������
 * @return:
 *     ��
 */
void rseCommandInit(void);

/**
  * @brief
  *     ����������Ϣ��ʼ������
  *@return
  *      None
  */
void rseKernelTaskLibInit(void);

/**
  * @brief
  *     CPU��Ϣ��ʼ������
  *@return
  *      None
  */
void rseCpuInfoLibInit(void);

/**
  * @brief
  *     ģ����Ϣ��ʼ������
  *@return
  *      None
  */
void rseModuleLibInit(void);

/**
  * @brief
  *     RTP��Ϣ��������ʼ��
  *@return
  *      None
  */
void rseRtpLibInit(void);

/*
 * @brief
 *    rse���ʼ��
 * @return
 *    ERROR����ʼ��ʧ��
 *    OK����ʼ���ɹ�
 */
STATUS rseLibInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RSE_INITLIB_H_ */
