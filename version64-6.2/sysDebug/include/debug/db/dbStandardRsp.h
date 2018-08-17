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
 *@file:dbStandardRsp.h
 *@brief:
 *             <li>��׼RSP������غ�������</li>
 */
#ifndef _DB_STANDARD_RSP_H
#define _DB_STANDARD_RSP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "taCommand.h"
#include "taErrorDefine.h"
#include "sdaMain.h"

/************************�궨��*******************************/

/* ��׼RSP������� */
#define TA_RSP_TABLE_LEN  22

/* �����������֮ǰ��1000��luosy�ĳ���200 */
#define MAX_TASKS_NUM 200

/* д�ڴ���������ݳ��� */
#define TA_WRITE_M_MAX_DATA_SIZE 590

/************************���Ͷ���*****************************/

/************************�ӿ�����*****************************/

/* RSP����� */
extern T_TA_CommandTable taRSPCmdTable[TA_RSP_TABLE_LEN];

/*
 * @brief:
 *      �����Ƿ�����ͣ�����־
 * @return:
 *     ��
 */
void taDebugSetStopCommandTag(BOOL stopTag);

/*
 * @brief:
 *      ��ȡ��ͣ�����־
 * @return:
 *     TRUE: ����ͣ����
 *     FALSE: ������ͣ����
 */
BOOL taDebugGetStopCommandTag(void);

/*
 * @brief:
 *      �����ѯ��ǰ�����쳣���¼�
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���,��ʽΪ?
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 */
T_TA_ReturnCode RspQueryEvent(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����M����д�ڴ�
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���,������ʽΪaddress,size:xxxxxx
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:bpOutbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspWriteMemory(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *     ������ڴ��������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspReadMemory(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����д��������ʽ�ڴ����GDB����0x23(ASCII #),0x24(ASCII $)����չ�ַ��ĸ�ʽΪ(0x7d ԭʼ�ַ����0x20),
 *      ���� 0x7d�������ֽ�(0x7d 0x5d)���͡�����Ŀ����ν���ʱ����н����������ǰ�ַ�Ϊ0x7d,���¸��ַ�Ϊ(*src^ 0x20)
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���,������ʽΪaddr,size:xxxx
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspWriteBinaryMemory(T_DB_Info *debugInfo,
                                            const UINT8 *inbuf,
                                            UINT32 inSize,
                                            UINT8 *outbuf,
                                            UINT32 *outSize);

/*
 * @brief:
 *      �����������ʽ���ڴ�����
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���,������ʽΪaddr,size
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspReadBinaryMemory(T_DB_Info *debugInfo,
                                           const UINT8 *inbuf,
                                           UINT32 inSize,
                                           UINT8 *outbuf,
                                           UINT32 *outSize);

/*
 * @brief:
 *      ����д�����Ĵ�����������
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���,������ʽΪregNum=value
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspWriteSingleRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����д���мĴ�����������
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspWriteAllRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ��������мĴ�����������
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspReadAllRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����������Ĵ�����������
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK:ִ�гɹ�
 *    TA_FAIL: ִ��ʧ��
 */
T_TA_ReturnCode RspReadSingleRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      �������öϵ�����,��ָ����ַ�����öϵ�(���/Ӳ���ϵ�)
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���,���������ʽΪtype,address,size
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *     TA_OK��ִ�гɹ�
 *     TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspSetBreakpoint(T_DB_Info *debugInfo,
                                        const UINT8 *inbuf,
                                        UINT32 inSize,
                                        UINT8 *outbuf,
                                        UINT32 *outSize);

/*
 * @brief:
 *      ����ɾ���ϵ�������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���,������ʽΪtype,address,size
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspRemoveBreakpoint(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ���������ͣ����
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_DB_NEED_IRET:ִ�гɹ�
 */
T_TA_ReturnCode RspBreakDebugProc(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *    ����������в�������,���ص������Գ����������
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_DB_NEED_IRET: ִ�гɹ�
 */
T_TA_ReturnCode RspContinue(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ��������������,��λ��־�Ĵ����ĵ�����־��ʹ��������ʱ���������쳣
 * @param[in]:tpInfo:���ԻỰ��Ϣ
 * @param[in]:bpInbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]: bpOutbuf:�洢������������ݻ���
 * @param[out]: outSize:�洢����������ݳ���
 * @return:
 *     TA_DB_NEED_IRET: ��Ҫ���ж��쳣����
 */
T_TA_ReturnCode RspStep(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����"qC"����,��ȡ��ǰ�߳�ID
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspGetCurThreadID(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����"qfThreadInfo"��"qsThreadInfo"����,��ȡ�����߳���Ϣ�����������ȷ���qfThreadInfo�����ȡ��һ���߳���Ϣ
 *      Ȼ����qsThreadInfo�����ȡʣ�µ��߳���Ϣ����Ŀ����˻ظ�'l'��ʾ�߳���Ϣ�ظ����
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspGetThreadIdList(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����"Hg"����,����GDBҪ���̺߳����Ĳ���,Hg��ʾ����������Ҫ����g�����ȡ���мĴ���
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspSetThreadSubsequentOptHg(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����"T"����,�鿴�߳��Ƿ��ڼ���״̬
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspThreadIsActive(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ����"qThreadExtraInfo,tid"����,��ȡ�̶߳�����Ϣ,������Ϣ�����߳�ID,�߳����ͺ���Ϣ
 * @param[in]:debugInfo: ���ԻỰ��Ϣ
 * @param[in]:inbuf: ����������������ݻ���
 * @param[in]:inSize: �����������ݳ���
 * @param[out]:outbuf: �洢������������ݻ���
 * @param[out]:outSize: �洢����������ݳ���
 * @return:
 *    TA_OK��ִ�гɹ�
 *    TA_FAIL:ִ��ʧ��
 */
T_TA_ReturnCode RspThreadExtraInfo(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

/*
 * @brief:
 *      ���ö��߳���Ϣ��������̱߳�־ΪTRUE�ظ����߳���Ϣ������ֻ�ظ���ǰ�����쳣���߳���Ϣ
 * @param[in]:tpInfo:���ԻỰ��Ϣ
 * @param[in]:bpInbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:bpOutbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *     TA_FAIL: ����ʧ��
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode RspSetMultiThreadInfo(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _DB_STANDARD_RSP_H */
