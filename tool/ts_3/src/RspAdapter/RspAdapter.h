/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * �����ļ�;�����ļ��Ļ����ӿ�
  */

/**
* @file 	RspAdapter.h
* @brief 	
*     <li>���ܣ� ����RSP_Adapter�������</li>
* @author 	������
* @date 	2006-10-11
* <p>���ţ�ϵͳ�� 
*/

#ifndef _RSP_ADAPTER_H
#define _RSP_ADAPTER_H


/************************************���ò���******************************************/
#include "stdafx.h"
#include "../../include/sysTypes.h"
/************************************��������******************************************/

/************************************���岿��******************************************/

/**
* @brief
*	RspAdapter��TMΪ֧�ֱ�׼RSPЭ��ĵ��Է�����BDI�ṩ��RSP������\n
* @author 	������
* @version 1.0
* @date 	2006-10-11
* <p>���ţ�ϵͳ�� 
*/



class RspAdapter
{
public:
	
	/**
	* @brief 
	*    ���캯��
	*/
	RspAdapter();

	/**
	* @brief 
	*    ��������
	*/
	~RspAdapter();

	/**
	* @brief 
	*    ��װ�����������в����������õ���Ҫ��IP��ַ�Ͷ˿ں�
	* @param[in] wId ��Ӧ���������󶨵�AID��
	* @param[in] cpBuf ��������ָ��,���������Դ��н���������Ĳ���
	* @return �����ɹ����ؼ����Ķ˿ںţ�ʧ�ܷ���FAIL
	*/   
	T_WORD Open(T_WORD wId, T_CHAR *cpBuf);

	/**
	* @brief 
	*    �رղ�����ж��������
	*/
	T_VOID Close();

	/**
	* @brief 
	*    �ص��������÷������յ�������ת����BDI
	* @param[in] hwDesSaid �����ỰID�ţ�����������û��ʹ��
	* @param[in] hwSrcAid ����Դ��ID��
	* @param[in] hwSrcSaid ����Դ�Ķ���ID��
	* @param[in] cpBuf Ҫ���͵����ݵĻ�����
	* @param[in] wSize Ҫ�������ݵĴ�С
	* @return SUCC����FAIL
	*/
	T_WORD Putpkt(T_HWORD hwDesSaid,T_HWORD hwSrcAid,T_HWORD hwSrcSaid,T_CHAR* cpBuf,T_WORD wSize);

	/**
	* @brief 
	*    �ص��������÷��������ڷ�����ά���Ļ�����н��հ����յ����ݺ�\n
	*    �����ݿ������������ṩ�Ļ������󷵻�\n
	* @param[in] hwpDesAid  ����Ҫ���͵�Ŀ�ĵص�ID��
	* @param[in] hwpDesSaid ����Ҫ���͵�Ŀ�ĵصĶ���ID��
	* @param[in] hwpSrcSaid Rsp�������Ķ���ID�ţ�Ŀǰ����������û��ʹ��
	* @param     cpBuf    �������ṩ�����ݻ���
	* @param[in] wSize    �������ṩ�Ļ������Ĵ�С
	* @return  ʵ��Ҫ�������ݵĴ�С������FAIL��ʾ�������ӶϿ������߳����������
	*/
	T_WORD Getpkt(T_HWORD *hwpDesAid,T_HWORD *hwpDesSaid,T_HWORD *hwpSrcSaid,T_CHAR* cpBuf,T_WORD wSize);

	/*
	* @brief
	*    ��������BDI�����ݣ��յ����ݺ�\n
	*    �����ݿ������������ṩ�Ļ������󷵻�\n
	* @param     cpBuf    �������ṩ�����ݻ���
	* @param[in] wSize    �������ṩ�Ļ������Ĵ�С
	* @return  ʵ�ʽ��յ����ݵĴ�С������Ͽ�����-1
	*/
	T_WORD GetData(T_CHAR *cpBuf, T_WORD wSize);

	/*
	* @brief
	*    �����ݷ��͸�BDI\n
	* @param     cpBuf    ������Ҫ���͵����ݻ���
	* @param[in] size    ������Ҫ�������ݵĴ�С
	* @return  ʵ�ʷ��͵����ݵĴ�С
	*/
	T_WORD PutData(T_CHAR *cpBuf, T_WORD wSize);

private:
	T_WORD m_port;      //BDI�ļ����˿ں�
	T_CHAR m_ip[16];    //BDI��IP��ַ
	WSocket *m_client;  //�ͻ���SOCKET
	MutexLock m_sktMutex;   //���ڶԿͻ���SOCKET���л��Ᵽ��
	HANDLE m_hBdiSema;	//BDI�ѻظ������¼�����
	T_WORD m_comAid;    //ͨ��AID��
	T_WORD m_comSaid;   //ͨ��SAID��
	HANDLE m_hExitEvent;		//��̬���뿪�¼�����

	T_VBOOL m_txLeave;   //TX�߳��˳���־λ
	T_VBOOL m_rxLeave;   //RX�߳��˳���־λ

	T_BOOL m_txFirstEntryFlag;  //TX��ʼ������putpkt��־

	T_WORD m_lastId;     //�ϴ�ͨ��TX�̷߳������ݵ�ID��
};


#endif