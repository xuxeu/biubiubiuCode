/************************************************************************
*				北京科银京成技术有限公司 版权所有
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	RspAdapter.cpp
 * @brief
 *	<li>功能：实现TCP通信方式的基于RSP协议的RSP适配器，主要用于适配BDI仿真器 </li>
 * @author 	郭建川
 * @date 	2006-10-11
 * <p>部门：系统部
 */

/**************************** 引用部分 *********************************/
//#include "stdafx.h"
#include "RspAdapter.h"

#include <stdlib.h>
/*************************** 前向声明部分 ******************************/
T_CONST T_CHAR vcaHexchars[] = "0123456789abcdef";
T_CONST T_WORD MAX_SEMAPHORE_COUNT = 1000;	//计数器最大值
/**************************** 实现部分 *********************************/

/**
 * @brief 主要做初始化操作
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
 * @brief 主要做资源清理操作
 */
RspAdapter::~RspAdapter()
{

}


/*
* @brief
*    阻塞在服务器接收GDB的数据，收到数据后\n
*    将数据拷贝到调用者提供的缓冲区后返回\n
*/
T_WORD RspAdapter::GetData(T_CHAR *cpBuf, T_WORD wSize)
{

	//阻塞接收BDI数据
	T_CHAR head;
	T_WORD offset = 0;

	while(TRUE)
	{
		//检查SOCKET合法性
		if (NULL == m_client || NULL == cpBuf)
		{
			return FAIL;
		}


		offset = 0;
        /*Modified by tangxp on 2007.10.23 [begin]*/
        /*Modified brief:改为以通信协议对象收发数据*/
        T_WORD ret = m_client->getpkt(&head, 1);
        /*Modified by tangxp on 2007.10.23 [end]*/

		//如果网络断开，退出循环
		if(ret < 0)
		{
			return FAIL;
		}

		//如果接收的头字符不是$，则继续接收
		if('$'!= head)
		{
			continue;
		}

		//接收数据直到#
        /*Modified by tangxp on 2007.10.23 [begin]*/
        /*Modified brief:改为以通信协议对象收发数据*/
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
            /*Modified brief:改为以通信协议对象收发数据*/
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
	    //发送确认应答
	    if(m_client != NULL)
        {
            /*Modified by tangxp on 2007.10.23 [begin]*/
            /*Modified brief:改为以通信协议对象收发数据*/
            m_client->putpkt("+", strlen("+"));
            /*Modified by tangxp on 2007.10.23 [end]*/
        }
	    m_sktMutex.Unlock();

        return offset;
	}
}

/*
* @brief
*    将数据发送给BDI\n
* @return  实际发送的数据的大小
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

	//分配发送缓冲区
	sendBuf = new T_UBYTE[wSize+10];

	if(sendBuf == NULL)
	{
		return FAIL;
	}

	memset(sendBuf, 0, wSize + 10);
	//构造包头
	sendBuf[0] = '$';

	//求校验和
   	while(count--)
	{
		checkSum += static_cast<int>(cpBuf[i]);
		sendBuf[i+1] = static_cast<T_UBYTE>(cpBuf[i]);
		i++;
	}
	//构造包尾
	sendBuf[wSize+1] = '#';
	sendBuf[wSize+2] = vcaHexchars[(checkSum >> 4) & 0xf];
	sendBuf[wSize+3] = vcaHexchars[checkSum & 0xf];
	//发送数据
	//--------------------------------------------------------------------------
	//2006.06.19 在发送数据前对SOCKET的合法性进行检验，避免SOCKET删除瞬间，发送线程仍在调用发送方法发送数据
	m_sktMutex.Lock();
	if(m_client != NULL)
	{
        /*Modified by tangxp on 2007.10.23 [begin]*/
        /*Modified brief:改为以通信协议对象收发数据*/
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

	//设置RX线程进入标志位
	m_rxLeave = FALSE;

	//阻塞在BDI接收数据
	recvSize = GetData(cpBuf, wSize);

	//发生网络错误，立即返回
	if(FAIL == recvSize)
	{
		m_rxLeave = TRUE;
		SetEvent(m_hExitEvent);
		return FAIL;
	}

	//设置数据发送目的地
	*hwpDesAid = m_comAid;
	*hwpDesSaid = m_comSaid;

	//唤醒信号量
	ReleaseSemaphore(m_hBdiSema,1,NULL);

	//设置RX线程退出标志位
	m_rxLeave = TRUE;
	SetEvent(m_hExitEvent);

	return recvSize;

}

/**
* @brief
*    发送数据到BDI，确保收到BDI回复后才能转发下一数据包，因此采用信号量
*/
T_WORD RspAdapter::Putpkt(T_HWORD hwDesSaid,T_HWORD hwSrcAid,T_HWORD hwSrcSaid,T_CHAR* cpBuf,T_WORD wSize)
{
	T_WORD ret;

	//设置TX线程进入标志位
	m_txLeave = FALSE;

	//如果是第一次进入，则不需要等待信号量
	if(!m_txFirstEntryFlag)
	{
		//如果源ID等同于上次发包ID,允许继续发送,无须等待信号量
		if(m_lastId != hwSrcAid)
		{
			//等待发送事件到来,通过申请信号量实现
			if(NULL != m_hBdiSema)
				DWORD dw = WaitForSingleObject(m_hBdiSema,INFINITE);
		}
	}

	m_txFirstEntryFlag = FALSE;

	//记录通信ID号
	m_comAid = hwSrcAid;
	m_comSaid = hwSrcSaid;

	m_lastId = hwSrcAid;

	//发送数据到BDI
	ret = PutData(cpBuf, wSize);

	//设置TX线程退出标志位
	m_txLeave = TRUE;
	SetEvent(m_hExitEvent);

	return ret;	

}

/**
* @brief
*    关闭操作，关闭连接，确保线程退出
*/
T_VOID RspAdapter::Close()
{
	//唤醒所有的信号量,让阻塞TX线程顺利退出
	if (m_hBdiSema != NULL) {
		ReleaseSemaphore(m_hBdiSema,MAX_SEMAPHORE_COUNT,NULL);	
	}

	//关闭连接，如果RX线程阻塞，让其顺利退出
	m_sktMutex.Lock();
	if(m_client != NULL)
	{
        /*Modified by tangxp on 2007.10.23 [begin]*/
        /*Modified brief:改为以通信协议对象收发数据*/
        m_device->close();
        delete m_device;
        m_device = NULL;
        /*Modified by tangxp on 2007.10.23 [end]*/
		m_client->close();
		delete m_client;
		m_client = NULL;
	}
	m_sktMutex.Unlock();

	//等待线程退出事件
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
 * @brief 通过参数得到BDI的IP地址和端口号连接BDI，成功返回SUCC，失败返回FAIL
 * cpBuf格式：  "0:IP:PORT:TIMEOUT:RETRY:"字符串结尾以0结束
 */
T_WORD RspAdapter::Open(T_WORD wId, T_CHAR *cpBuf)
{   
    /*Modified by tangxp on 2007.10.23 [begin]*/
    /*Modified brief:改为以通信协议对象收发数据*/
    if(NULL == cpBuf)
    {   
        return FAIL;
    }

	//从cpBuf中解析出要连接的仿真器的通信类型、IP地址、端口号、超时时间、重试次数
	T_CHAR *ptr = &cpBuf[0];
    int ret;

    //获取通信类型
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

    //检查是否有IP地址
	if(':' == *ptr)
	{
		ptr++;
	}
	else
	{
		return FAIL;
	}

    //获取IP地址
    pdest = strstr(ptr,":");
    int ipLen = pdest-ptr;
    if( !pdest || (IP_LENGTH-1)<ipLen )
    {
        return FAIL;
    }
    memcpy(m_ip,ptr,ipLen);
    m_ip[ipLen] = 0;

    //是否有端口号
	if(':' == *ptr)
	{
		ptr++;
	}
	else
	{
		return FAIL;
	}
    
	//获取端口号
    pdest = strstr(ptr,":");
    if(!pdest)
    {
        return FAIL;
    }
	ret = hex2int(&ptr, &m_port);
	
	//检查端口号合法性
	if(0 == ret || m_port < 0 || m_port > 65535)
	{
		return FAIL;	
	}
    
    //如果是UDP通信方式，则还需要获取超时时间和重试次数
    if(UDP_PROTOCOL == m_type)
    {
        //是否有超时时间
	    if(':' == *ptr)
	    {
		    ptr++;
	    }
	    else
	    {
		    return FAIL;
	    }

        //获取超时时间
        pdest = strstr(ptr,":");
        if(!pdest)
        {
            return FAIL;
        }
        ret = hex2int(&ptr, &m_timeout);
	
	    //检查超时时间合法性
	    if(0 == ret || m_timeout < 0 || m_timeout > 65535)
	    {
		    return FAIL;	
	    }

        //是否有重试次数
	    if(':' == *ptr)
	    {
		    ptr++;
	    }
	    else
	    {
		    return FAIL;
	    }

        //获取重试次数
        pdest = strstr(ptr,":");
        if(!pdest)
        {
            return FAIL;
        }
        ret = hex2int(&ptr, &m_retry);
	
	    //检查重试次数合法性
	    if(0 == ret || m_retry < 0 || m_retry > 65535)
	    {
		    return FAIL;	
	    }
    }

    //根据通信方式建立对应的通信协议
    if(UDP_PROTOCOL == m_type)
    {
        //建立device对象
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

        //建立协议对象
        m_client = new PacketProtocol(m_device);
        if(!m_client)
        {
             return FAIL;
        }
    }
    else if(TCP_PROTOCOL == m_type)
    {
        //建立device对象
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

        //建立协议对象
        m_client = new RapidProtocol(m_device);
        if(!m_client)
        {
             return FAIL;
        }
    }
    /*Modified by tangxp on 2007.10.23 [end]*/


	//创建BDI回复信号量对象
	m_hBdiSema = CreateSemaphore(NULL,0,MAX_SEMAPHORE_COUNT,NULL);

	if(NULL == m_hBdiSema)
	{
		m_client->close();
		delete m_client;
		m_client = NULL;
		return FAIL;
	}

	//创建一个事件对象,用来退出动态库
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

