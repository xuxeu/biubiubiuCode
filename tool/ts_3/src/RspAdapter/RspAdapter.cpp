/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	RspAdapter.cpp
 * @brief
 *	<li>���ܣ�ʵ��TCPͨ�ŷ�ʽ�Ļ���RSPЭ���RSP����������Ҫ��������BDI������ </li>
 * @author 	������
 * @date 	2006-10-11
 * <p>���ţ�ϵͳ��
 */

/**************************** ���ò��� *********************************/
#include "stdafx.h"
#include "RspAdapter.h"

#include <stdlib.h>
/*************************** ǰ���������� ******************************/
T_CONST T_CHAR vcaHexchars[] = "0123456789abcdef";
T_CONST T_WORD MAX_SEMAPHORE_COUNT = 1000;	//���������ֵ
/**************************** ʵ�ֲ��� *********************************/

/**
 * @brief ��Ҫ����ʼ������
 */
RspAdapter::RspAdapter()
{
	m_client = NULL;
	m_hBdiSema = NULL;
	m_hExitEvent = NULL;
	m_txLeave = TRUE;
	m_rxLeave = TRUE;
	m_txFirstEntryFlag = TRUE;

	m_lastId = 0;
}

/**
 * @brief ��Ҫ����Դ�������
 */
RspAdapter::~RspAdapter()
{

}


/*
* @brief
*    �����ڷ���������GDB�����ݣ��յ����ݺ�\n
*    �����ݿ������������ṩ�Ļ������󷵻�\n
*/
T_WORD RspAdapter::GetData(T_CHAR *cpBuf, T_WORD wSize)
{

	//��������BDI����
	T_CHAR head;
	T_WORD offset = 0;

	while(TRUE)
	{
		//���SOCKET�Ϸ���
		if (NULL == m_client || NULL == cpBuf)
		{
			return FAIL;
		}


		offset = 0;
        T_WORD ret = m_client->Recv(&head, 1);

		//�������Ͽ����˳�ѭ��
		if(ret < 0)
		{
			return FAIL;
		}

		//������յ�ͷ�ַ�����$�����������
		if('$'!= head)
		{
			continue;
		}

		//��������ֱ��#
		ret = m_client->Recv(&head, 1);
		while((offset < wSize) && (FAIL != ret))
		{
			if('#' == head)
			{
				break;
			}
			cpBuf[offset++] = head;
			ret = m_client->Recv(&head, 1);
		}

		if(ret < 0)
		{
			return FAIL;
		}

		if(offset > wSize)
		{
			continue;
		}


	m_sktMutex.Lock();
	//Ӧ����Ϊ��TCP���գ���û�н���У��
	if(m_client != NULL)
	m_client->Send("+", strlen("+"));

	m_sktMutex.Unlock();
		return offset;
	}
}

/*
* @brief
*    �����ݷ��͸�BDI\n
* @return  ʵ�ʷ��͵����ݵĴ�С
*/
T_WORD RspAdapter::PutData(T_CHAR *cpBuf, T_WORD wSize)
{

	T_WORD i = 0;
	T_UBYTE *sendBuf;
	T_UBYTE checkSum = 0;
	T_WORD	count = wSize;
	T_WORD result = FAIL;

	if(m_client == NULL || cpBuf == NULL)
	{
		return FAIL;
	}

	//���䷢�ͻ�����
	sendBuf = new T_UBYTE[wSize+10];

	if(sendBuf == NULL)
	{
		return FAIL;
	}

	memset(sendBuf, 0, wSize + 10);
	//�����ͷ
	sendBuf[0] = '$';

	//��У���
   	while(count--)
	{
		checkSum += static_cast<int>(cpBuf[i]);
		sendBuf[i+1] = static_cast<T_UBYTE>(cpBuf[i]);
		i++;
	}
	//�����β
	sendBuf[wSize+1] = '#';
	sendBuf[wSize+2] = vcaHexchars[(checkSum >> 4) & 0xf];
	sendBuf[wSize+3] = vcaHexchars[checkSum & 0xf];
	//��������
	//--------------------------------------------------------------------------
	//2006.06.19 �ڷ�������ǰ��SOCKET�ĺϷ��Խ��м��飬����SOCKETɾ��˲�䣬�����߳����ڵ��÷��ͷ�����������
	m_sktMutex.Lock();
	if(m_client != NULL)
	{
		result = m_client->Send(reinterpret_cast<T_CHAR *>(sendBuf),wSize+4);
	}
	m_sktMutex.Unlock();



	delete []sendBuf;
	return result;

}

T_WORD RspAdapter::Getpkt(T_HWORD *hwpDesAid,T_HWORD *hwpDesSaid,T_HWORD *hwpSrcSaid,T_CHAR* cpBuf,T_WORD wSize)
{
	T_WORD recvSize;

	//����RX�߳̽����־λ
	m_rxLeave = FALSE;

	//������BDI��������
	recvSize = GetData(cpBuf, wSize);

	//�������������������
	if(FAIL == recvSize)
	{
		m_rxLeave = TRUE;
		SetEvent(m_hExitEvent);
		return FAIL;
	}

	//�������ݷ���Ŀ�ĵ�
	*hwpDesAid = m_comAid;
	*hwpDesSaid = m_comSaid;

	//�����ź���
	ReleaseSemaphore(m_hBdiSema,1,NULL);

	//����RX�߳��˳���־λ
	m_rxLeave = TRUE;
	SetEvent(m_hExitEvent);

	return recvSize;

}

/**
* @brief
*    �������ݵ�BDI��ȷ���յ�BDI�ظ������ת����һ���ݰ�����˲����ź���
*/
T_WORD RspAdapter::Putpkt(T_HWORD hwDesSaid,T_HWORD hwSrcAid,T_HWORD hwSrcSaid,T_CHAR* cpBuf,T_WORD wSize)
{
	T_WORD ret;

	//����TX�߳̽����־λ
	m_txLeave = FALSE;

	//����ǵ�һ�ν��룬����Ҫ�ȴ��ź���
	if(!m_txFirstEntryFlag)
	{
		//���ԴID��ͬ���ϴη���ID,�����������,����ȴ��ź���
		if(m_lastId != hwSrcAid)
		{
			//�ȴ������¼�����,ͨ�������ź���ʵ��
			if(NULL != m_hBdiSema)
				DWORD dw = WaitForSingleObject(m_hBdiSema,INFINITE);
		}
	}

	m_txFirstEntryFlag = FALSE;

	//��¼ͨ��ID��
	m_comAid = hwSrcAid;
	m_comSaid = hwSrcSaid;

	m_lastId = hwSrcAid;

	//�������ݵ�BDI
	ret = PutData(cpBuf, wSize);

	//����TX�߳��˳���־λ
	m_txLeave = TRUE;
	SetEvent(m_hExitEvent);

	return ret;	

}

/**
* @brief
*    �رղ������ر����ӣ�ȷ���߳��˳�
*/
T_VOID RspAdapter::Close()
{
	//�������е��ź���,������TX�߳�˳���˳�
	if (m_hBdiSema != NULL) {
		ReleaseSemaphore(m_hBdiSema,MAX_SEMAPHORE_COUNT,NULL);	
	}

	//�ر�SOCKET���ӣ����RX�߳�����������˳���˳�
	m_sktMutex.Lock();
	if(m_client != NULL)
	{
		m_client->Close();
		delete m_client;
		m_client = NULL;
	}
	m_sktMutex.Unlock();

	//�ȴ��߳��˳��¼�
	while(m_txLeave == FALSE || m_rxLeave == FALSE)
	{
		DWORD stat = WaitForSingleObject(m_hExitEvent, 50);
		if(stat == WAIT_OBJECT_0)
		{

		}
		else if(stat == WAIT_TIMEOUT)
		{

		}

	}

	CloseHandle( m_hExitEvent );
	m_hExitEvent = NULL;
}

/**
 * @brief ͨ�������õ�BDI��IP��ַ�Ͷ˿ں�����BDI���ɹ�����SUCC��ʧ�ܷ���FAIL
 * ������ʽ��  "PORT:IP"�ַ�����β��0����
 */
T_WORD RspAdapter::Open(T_WORD wId, T_CHAR *cpBuf)
{
	//��cpBuf�н�����Ҫ���ӵķ�������IP��ַ�Ͷ˿ں�
	T_CHAR *ptr = &cpBuf[0];
	T_WORD result;

	//��ȡ�˿ں�
	result = hex2int(&ptr, &m_port);
	
	//���˿ںϷ���
	if(0 == result || m_port < 0 || m_port > 65535)
	{
		return FAIL;	
	}
	
	//ptr += result;
	
	//�Ƿ���IP��ַ
	if(':' == *ptr)
	{
		ptr++;
	}
	else
	{
		return FAIL;
	}

	//��������β�Ƿ���":"��β��
	T_CHAR *pdest = strstr(ptr,":");		//�Ƿ��зָ��
	if(NULL == pdest)
	{
		return FAIL;
	}
	T_WORD size = pdest - ptr;



	//��ȡIP��ַ
	memcpy(m_ip, ptr, size);
	m_ip[size] = 0;
	

	//��������BDI
	m_client = new WSocket;
	m_client->Create(AF_INET, SOCK_STREAM, 0);
	if (m_client->Connect(m_ip, m_port) != TRUE ) {
		delete m_client;
		m_client = NULL;
		return FAIL;
	}

	//����BDI�ظ��ź�������
	m_hBdiSema = CreateSemaphore(NULL,0,MAX_SEMAPHORE_COUNT,NULL);

	if(NULL == m_hBdiSema)
	{
		m_client->Close();
		delete m_client;
		m_client = NULL;
		return FAIL;
	}

	//����һ���¼�����,�����˳���̬��
	m_hExitEvent = ::CreateEvent (NULL, FALSE, FALSE, NULL);

	if(NULL == m_hExitEvent)
	{
		m_client->Close();
		delete m_client;
		m_client = NULL;
		
		CloseHandle(m_hBdiSema);
		m_hBdiSema = NULL;
	}

	return TRUE;
}

