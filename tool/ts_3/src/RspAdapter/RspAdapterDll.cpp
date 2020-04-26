/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
 *创建文件;定义 DLL 应用程序的入口点
 */

/**
* @file 	RspAdapterDLL.cpp
* @brief 	
*     <li>功能： 实现RSP适配器遵循TS标准动态库的基本接口</li>
* @author 	郭建川
* @date 	2006-10-11
* <p>部门：系统部 
*/


/************************************引用部分******************************************/
#include "stdafx.h"
#include <map>
#include "RspAdapter.h"
/**********************************前向声明部分******************************************/
using namespace std;
/************************************定义部分******************************************/	
map<T_WORD, RspAdapter*> vtRSP_Id2Adapter;  //ID到适配器的映射表
typedef map<T_WORD,RspAdapter*> SHORT2ADAPTER;        //映射表类型定义
MutexLock g_DllLock;	                             //对动态库操作进行锁操作

/************************************实现部分******************************************/	

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

/**
功能: 动态库向TS注册的回调函数, SA DLL的TX线程会调用此方法对收到数据进行处理
* @param vwServerId sa的源said号
* @param vhwDesSaid 目标said号
* @param vhwSrcAid  ca的aid号
* @param src_said ca的said号
* @param cpBuf 发送的缓冲区
* @param vwSize 缓冲区大小
* @return FAIL表示失败,SUCC表示成功
*/
FUNC_INT putpkt(T_HWORD vwServerId,T_HWORD vhwDesSaid,T_HWORD vhwSrcAid,T_HWORD vwSrcSaid,T_CHAR* cpBuf,T_WORD vwSize)
{
	int result;
	RspAdapter* adapter = NULL;
	SHORT2ADAPTER::iterator theIterator;

	g_DllLock.Lock();
    theIterator = vtRSP_Id2Adapter.find(vwServerId);
    if (theIterator != vtRSP_Id2Adapter.end()) 
	{
		adapter = (*theIterator).second;
	}
	else
	{
		g_DllLock.Unlock();
		return FAIL;
	}
	g_DllLock.Unlock();
	result = adapter->Putpkt(vhwDesSaid,vhwSrcAid,vwSrcSaid,cpBuf,vwSize);
	
	return result;
}

/**
功能: 动态库向TS注册的回调函数, SA DLL的RX线程会调用此方法接收动态库的数据进行转发
* @param vwServerId[in]  RSP适配器的ID号
* @param vwpDesAid[out]  RSP适配器要发送数据的目标aid号
* @param vwpDesSaid[out] RSP适配器要发送数据的目标said号
* @param vwpSrcSaid[out] RSP适配器的said号
* @param cpBuf[out] 发送的缓冲区
* @param vwSize 缓冲区大小
* @return 要发送数据的大小，如果返回
*/
FUNC_INT getpkt(T_HWORD vwServerId,T_HWORD *vwpDesAid,T_HWORD *vwpDesSaid,T_HWORD *vwpSrcSaid,T_CHAR* cpBuf,T_WORD vwSize)
{
	T_WORD result;
	RspAdapter* adapter = NULL;
	SHORT2ADAPTER::iterator theIterator;

	//根据ID号从哈希表中进行查找
	g_DllLock.Lock();
    theIterator = vtRSP_Id2Adapter.find(vwServerId);
    if (theIterator != vtRSP_Id2Adapter.end()) 
	{
		adapter = (*theIterator).second;
	}
	else
	{
		g_DllLock.Unlock();
		return FAIL;
	}
	g_DllLock.Unlock();

	//如果查找到，则接收数据
	result = adapter->Getpkt(vwpDesAid,vwpDesSaid,vwpSrcSaid,cpBuf,vwSize);
	return result;	
}

/**
* @brief 
*	功能: 关闭动作,卸载RSP适配器。便于动态库内部释放占用资源
* @param hwId 要关闭的RSP适配器的ID号
* @return 成功返回SUCC,失败返回FAIL
*/
FUNC_INT close(T_HWORD hwId)
{
	RspAdapter* adapter = NULL;
	SHORT2ADAPTER::iterator theIterator;

	//根据ID号从哈希表中进行查找
	g_DllLock.Lock();
	theIterator = vtRSP_Id2Adapter.find(hwId);	
	if (theIterator != vtRSP_Id2Adapter.end()) 
	{
		adapter = (*theIterator).second;
	}
	else
	{
		//如果没有，则返回失败
		g_DllLock.Unlock();
		return FAIL;
	}
	//如果查找到，从哈希表中删除
	vtRSP_Id2Adapter.erase(theIterator);
	g_DllLock.Unlock();

	//通知适配器退出
	adapter->Close();
	delete adapter;    
	return SUCC;
}

/**
* @brief 
*	功能: 打开动作,根据解析cpBuf创建RSP适配器，便于动态库内部做初始化工作
* @param[in] hwId TS为其创建的ServerAgent的ID号
* @param[in] cpBuf 传递的参数缓冲区
* @param[out]cpOutBuf 输出的参数缓冲区
* @return 成功返回SUCC,失败返回FAIL
*/
FUNC_INT  open(T_HWORD hwId,T_CHAR* cpBuf, TCHAR* cpOutBuf)
{
	//局部变量定义
	SHORT2ADAPTER::iterator theIterator;
	
	//参数合法性检查
	if(cpBuf == NULL || hwId < 0 || hwId > 65536)
	{
		return FAIL;
	}

	//查找RSP适配器是否已经创建
	g_DllLock.Lock();
	theIterator = vtRSP_Id2Adapter.find(hwId);
	if (theIterator != vtRSP_Id2Adapter.end()) 
	{	
		g_DllLock.Unlock();
		return FAIL;
	}

	//没有，则创建对应的RSP适配器对象
	RspAdapter *adapter = new RspAdapter();	
	vtRSP_Id2Adapter.insert(SHORT2ADAPTER::value_type(hwId, adapter));
	g_DllLock.Unlock();

	//解析参数，得到IP、端口号，并尝试连接仿真器
	T_WORD result = adapter->Open(hwId, cpBuf);

	//如果参数错误，或者连接BDI失败,则将刚才创建的对象从哈希表中删除
	if(result == FAIL)
	{	
		g_DllLock.Lock();
		adapter->Close();
		delete adapter;
		theIterator = vtRSP_Id2Adapter.find(hwId);
		if (theIterator != vtRSP_Id2Adapter.end()) 
		{	
		vtRSP_Id2Adapter.erase(theIterator);
		}
		g_DllLock.Unlock();

		//回复失败标志到输出缓冲区
		cpOutBuf[0] = 'F';
		return FAIL;
	}

	cpOutBuf[0] = 'S';
	return SUCC;
}
