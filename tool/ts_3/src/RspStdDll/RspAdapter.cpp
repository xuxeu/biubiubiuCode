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
//#include "stdafx.h"
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
    m_device = NULL;
    m_client = NULL;
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
        /*Modified by tangxp on 2007.10.23 [begin]*/
        /*Modified brief:��Ϊ��ͨ��Э������շ�����*/
        T_WORD ret = m_client->getpkt(&head, 1);
        /*Modified by tangxp on 2007.10.23 [end]*/

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
        /*Modified by tangxp on 2007.10.23 [begin]*/
        /*Modified brief:��Ϊ��ͨ��Э������շ�����*/
        ret = m_client->getpkt(&head, 1);
        /*Modified by tangxp on 2007.10.23 [end]*/
		while((offset < wSize) && (FAIL != ret))
		{
			if('#' == head)
			{
				break;
			}
			cpBuf[offset++] = head;
            /*Modified by tangxp on 2007.10.23 [begin]*/
            /*Modified brief:��Ϊ��ͨ��Э������շ�����*/
            ret = m_client->getpkt(&head, 1);
            /*Modified by tangxp on 2007.10.23 [end]*/
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
	    //����ȷ��Ӧ��
	    if(m_client != NULL)
        {
            /*Modified by tangxp on 2007.10.23 [begin]*/
            /*Modified brief:��Ϊ��ͨ��Э������շ�����*/
            m_client->putpkt("+", strlen("+"));
            /*Modified by tangxp on 2007.10.23 [end]*/
        }
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
        /*Modified by tangxp on 2007.10.23 [begin]*/
        /*Modified brief:��Ϊ��ͨ��Э������շ�����*/
        result = m_client->putpkt(reinterpret_cast<T_CHAR *>(sendBuf),wSize+4);
        /*Modified by tangxp on 2007.10.23 [end]*/
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

	//�ر����ӣ����RX�߳�����������˳���˳�
	m_sktMutex.Lock();
	if(m_client != NULL)
	{
        /*Modified by tangxp on 2007.10.23 [begin]*/
        /*Modified brief:��Ϊ��ͨ��Э������շ�����*/
        m_device->close();
        delete m_device;
        m_device = NULL;
        /*Modified by tangxp on 2007.10.23 [end]*/
		m_client->close();
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
 * cpBuf��ʽ��  "0:IP:PORT:TIMEOUT:RETRY:"�ַ�����β��0����
 */
T_WORD RspAdapter::Open(T_WORD wId, T_CHAR *cpBuf)
{   
    /*Modified by tangxp on 2007.10.23 [begin]*/
    /*Modified brief:��Ϊ��ͨ��Э������շ�����*/
    if(NULL == cpBuf)
    {   
        return FAIL;
    }

	//��cpBuf�н�����Ҫ���ӵķ�������ͨ�����͡�IP��ַ���˿ںš���ʱʱ�䡢���Դ���
	T_CHAR *ptr = &cpBuf[0];
    int ret;

    //��ȡͨ������
    char *pdest = strstr(ptr,":");
    if(!pdest)
    {
        return FAIL;
    }
    ret = hex2int(&ptr,&m_type);
    if(0==ret || TCP_PROTOCOL != m_type || UDP_PROTOCOL != m_type )
    {
        return FAIL;
    }

    //����Ƿ���IP��ַ
	if(':' == *ptr)
	{
		ptr++;
	}
	else
	{
		return FAIL;
	}

    //��ȡIP��ַ
    pdest = strstr(ptr,":");
    int ipLen = pdest-ptr;
    if( !pdest || (IP_LENGTH-1)<ipLen )
    {
        return FAIL;
    }
    memcpy(m_ip,ptr,ipLen);
    m_ip[ipLen] = 0;

    //�Ƿ��ж˿ں�
	if(':' == *ptr)
	{
		ptr++;
	}
	else
	{
		return FAIL;
	}
    
	//��ȡ�˿ں�
    pdest = strstr(ptr,":");
    if(!pdest)
    {
        return FAIL;
    }
	ret = hex2int(&ptr, &m_port);
	
	//���˿ںźϷ���
	if(0 == ret || m_port < 0 || m_port > 65535)
	{
		return FAIL;	
	}
    
    //�����UDPͨ�ŷ�ʽ������Ҫ��ȡ��ʱʱ������Դ���
    if(UDP_PROTOCOL == m_type)
    {
        //�Ƿ��г�ʱʱ��
	    if(':' == *ptr)
	    {
		    ptr++;
	    }
	    else
	    {
		    return FAIL;
	    }

        //��ȡ��ʱʱ��
        pdest = strstr(ptr,":");
        if(!pdest)
        {
            return FAIL;
        }
        ret = hex2int(&ptr, &m_timeout);
	
	    //��鳬ʱʱ��Ϸ���
	    if(0 == ret || m_timeout < 0 || m_timeout > 65535)
	    {
		    return FAIL;	
	    }

        //�Ƿ������Դ���
	    if(':' == *ptr)
	    {
		    ptr++;
	    }
	    else
	    {
		    return FAIL;
	    }

        //��ȡ���Դ���
        pdest = strstr(ptr,":");
        if(!pdest)
        {
            return FAIL;
        }
        ret = hex2int(&ptr, &m_retry);
	
	    //������Դ����Ϸ���
	    if(0 == ret || m_retry < 0 || m_retry > 65535)
	    {
		    return FAIL;	
	    }
    }

    //����ͨ�ŷ�ʽ������Ӧ��ͨ��Э��
    if(UDP_PROTOCOL == m_type)
    {
        //����device����
        m_device = new DeviceUdp;
        if(!m_device)
        {
             return FAIL;
        }
        if( false == m_device->open(m_ip,m_port,m_timeout,m_retry) )
        {
            m_device->close();
            delete m_device;
            m_device = NULL;
            return FAIL;
        }

        //����Э�����
        m_client = new PacketProtocol(m_device);
        if(!m_client)
        {
             return FAIL;
        }
    }
    else if(TCP_PROTOCOL == m_type)
    {
        //����device����
        m_device = new DeviceTcp;
        if(!m_device)
        {
             return FAIL;
        }
        if( false == m_device->open(m_ip,m_port,0,0) )
        {
            m_device->close();
            delete m_device;
            m_device = NULL;
            return FAIL;
        }

        //����Э�����
        m_client = new RapidProtocol(m_device);
        if(!m_client)
        {
             return FAIL;
        }
    }
    /*Modified by tangxp on 2007.10.23 [end]*/


	//����BDI�ظ��ź�������
	m_hBdiSema = CreateSemaphore(NULL,0,MAX_SEMAPHORE_COUNT,NULL);

	if(NULL == m_hBdiSema)
	{
		m_client->close();
		delete m_client;
		m_client = NULL;
		return FAIL;
	}

	//����һ���¼�����,�����˳���̬��
	m_hExitEvent = ::CreateEvent (NULL, FALSE, FALSE, NULL);

	if(NULL == m_hExitEvent)
	{
		m_client->close();
		delete m_client;
		m_client = NULL;
		
		CloseHandle(m_hBdiSema);
		m_hBdiSema = NULL;
	}

	return TRUE;
}

