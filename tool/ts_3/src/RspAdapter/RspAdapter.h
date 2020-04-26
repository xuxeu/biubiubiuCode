/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * 创建文件;定义文件的基本接口
  */

/**
* @file 	RspAdapter.h
* @brief 	
*     <li>功能： 定义RSP_Adapter对象的类</li>
* @author 	郭建川
* @date 	2006-10-11
* <p>部门：系统部 
*/

#ifndef _RSP_ADAPTER_H
#define _RSP_ADAPTER_H


/************************************引用部分******************************************/
#include "stdafx.h"
#include "../../include/sysTypes.h"
/************************************声明部分******************************************/

/************************************定义部分******************************************/

/**
* @brief
*	RspAdapter是TM为支持标准RSP协议的调试仿真器BDI提供的RSP适配器\n
* @author 	郭建川
* @version 1.0
* @date 	2006-10-11
* <p>部门：系统部 
*/



class RspAdapter
{
public:
	
	/**
	* @brief 
	*    构造函数
	*/
	RspAdapter();

	/**
	* @brief 
	*    析构函数
	*/
	~RspAdapter();

	/**
	* @brief 
	*    安装适配器，进行参数解析，得到需要的IP地址和端口号
	* @param[in] wId 对应服务器所绑定的AID号
	* @param[in] cpBuf 参数缓冲指针,服务器可以从中解析到所需的参数
	* @return 操作成功返回监听的端口号，失败返回FAIL
	*/   
	T_WORD Open(T_WORD wId, T_CHAR *cpBuf);

	/**
	* @brief 
	*    关闭操作，卸载适配器
	*/
	T_VOID Close();

	/**
	* @brief 
	*    回调操作，该方法将收到的数据转发到BDI
	* @param[in] hwDesSaid 二级会话ID号，在适配器中没有使用
	* @param[in] hwSrcAid 数据源的ID号
	* @param[in] hwSrcSaid 数据源的二级ID号
	* @param[in] cpBuf 要发送的数据的缓冲区
	* @param[in] wSize 要发送数据的大小
	* @return SUCC或者FAIL
	*/
	T_WORD Putpkt(T_HWORD hwDesSaid,T_HWORD hwSrcAid,T_HWORD hwSrcSaid,T_CHAR* cpBuf,T_WORD wSize);

	/**
	* @brief 
	*    回调操作，该方法阻塞在服务器维护的缓冲队列接收包，收到数据后\n
	*    将数据拷贝到调用者提供的缓冲区后返回\n
	* @param[in] hwpDesAid  数据要发送的目的地的ID号
	* @param[in] hwpDesSaid 数据要发送的目的地的二级ID号
	* @param[in] hwpSrcSaid Rsp适配器的二级ID号，目前在适配器中没有使用
	* @param     cpBuf    调用者提供的数据缓冲
	* @param[in] wSize    调用者提供的缓冲区的大小
	* @return  实际要发送数据的大小，返回FAIL表示网络连接断开，或者出现网络错误
	*/
	T_WORD Getpkt(T_HWORD *hwpDesAid,T_HWORD *hwpDesSaid,T_HWORD *hwpSrcSaid,T_CHAR* cpBuf,T_WORD wSize);

	/*
	* @brief
	*    阻塞接收BDI的数据，收到数据后\n
	*    将数据拷贝到调用者提供的缓冲区后返回\n
	* @param     cpBuf    调用者提供的数据缓冲
	* @param[in] wSize    调用者提供的缓冲区的大小
	* @return  实际接收的数据的大小，网络断开返回-1
	*/
	T_WORD GetData(T_CHAR *cpBuf, T_WORD wSize);

	/*
	* @brief
	*    将数据发送给BDI\n
	* @param     cpBuf    调用者要发送的数据缓冲
	* @param[in] size    调用者要发送数据的大小
	* @return  实际发送的数据的大小
	*/
	T_WORD PutData(T_CHAR *cpBuf, T_WORD wSize);

private:
	T_WORD m_port;      //BDI的监听端口号
	T_CHAR m_ip[16];    //BDI的IP地址
	WSocket *m_client;  //客户端SOCKET
	MutexLock m_sktMutex;   //用于对客户端SOCKET进行互斥保护
	HANDLE m_hBdiSema;	//BDI已回复数据事件对象
	T_WORD m_comAid;    //通信AID号
	T_WORD m_comSaid;   //通信SAID号
	HANDLE m_hExitEvent;		//动态库离开事件对象

	T_VBOOL m_txLeave;   //TX线程退出标志位
	T_VBOOL m_rxLeave;   //RX线程退出标志位

	T_BOOL m_txFirstEntryFlag;  //TX初始化进入putpkt标志

	T_WORD m_lastId;     //上次通过TX线程发送数据的ID号
};


#endif