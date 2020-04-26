/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
 *�����ļ�;���� DLL Ӧ�ó������ڵ�
 */

/**
* @file 	RspAdapterDLL.cpp
* @brief 	
*     <li>���ܣ� ʵ��RSP��������ѭTS��׼��̬��Ļ����ӿ�</li>
* @author 	������
* @date 	2006-10-11
* <p>���ţ�ϵͳ�� 
*/


/************************************���ò���******************************************/
#include "stdafx.h"
#include <map>
#include "RspAdapter.h"
/**********************************ǰ����������******************************************/
using namespace std;
/************************************���岿��******************************************/	
map<T_WORD, RspAdapter*> vtRSP_Id2Adapter;  //ID����������ӳ���
typedef map<T_WORD,RspAdapter*> SHORT2ADAPTER;        //ӳ������Ͷ���
MutexLock g_DllLock;	                             //�Զ�̬���������������

/************************************ʵ�ֲ���******************************************/	

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

/**
����: ��̬����TSע��Ļص�����, SA DLL��TX�̻߳���ô˷������յ����ݽ��д���
* @param vwServerId sa��Դsaid��
* @param vhwDesSaid Ŀ��said��
* @param vhwSrcAid  ca��aid��
* @param src_said ca��said��
* @param cpBuf ���͵Ļ�����
* @param vwSize ��������С
* @return FAIL��ʾʧ��,SUCC��ʾ�ɹ�
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
����: ��̬����TSע��Ļص�����, SA DLL��RX�̻߳���ô˷������ն�̬������ݽ���ת��
* @param vwServerId[in]  RSP��������ID��
* @param vwpDesAid[out]  RSP������Ҫ�������ݵ�Ŀ��aid��
* @param vwpDesSaid[out] RSP������Ҫ�������ݵ�Ŀ��said��
* @param vwpSrcSaid[out] RSP��������said��
* @param cpBuf[out] ���͵Ļ�����
* @param vwSize ��������С
* @return Ҫ�������ݵĴ�С���������
*/
FUNC_INT getpkt(T_HWORD vwServerId,T_HWORD *vwpDesAid,T_HWORD *vwpDesSaid,T_HWORD *vwpSrcSaid,T_CHAR* cpBuf,T_WORD vwSize)
{
	T_WORD result;
	RspAdapter* adapter = NULL;
	SHORT2ADAPTER::iterator theIterator;

	//����ID�Ŵӹ�ϣ���н��в���
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

	//������ҵ������������
	result = adapter->Getpkt(vwpDesAid,vwpDesSaid,vwpSrcSaid,cpBuf,vwSize);
	return result;	
}

/**
* @brief 
*	����: �رն���,ж��RSP�����������ڶ�̬���ڲ��ͷ�ռ����Դ
* @param hwId Ҫ�رյ�RSP��������ID��
* @return �ɹ�����SUCC,ʧ�ܷ���FAIL
*/
FUNC_INT close(T_HWORD hwId)
{
	RspAdapter* adapter = NULL;
	SHORT2ADAPTER::iterator theIterator;

	//����ID�Ŵӹ�ϣ���н��в���
	g_DllLock.Lock();
	theIterator = vtRSP_Id2Adapter.find(hwId);	
	if (theIterator != vtRSP_Id2Adapter.end()) 
	{
		adapter = (*theIterator).second;
	}
	else
	{
		//���û�У��򷵻�ʧ��
		g_DllLock.Unlock();
		return FAIL;
	}
	//������ҵ����ӹ�ϣ����ɾ��
	vtRSP_Id2Adapter.erase(theIterator);
	g_DllLock.Unlock();

	//֪ͨ�������˳�
	adapter->Close();
	delete adapter;    
	return SUCC;
}

/**
* @brief 
*	����: �򿪶���,���ݽ���cpBuf����RSP�����������ڶ�̬���ڲ�����ʼ������
* @param[in] hwId TSΪ�䴴����ServerAgent��ID��
* @param[in] cpBuf ���ݵĲ���������
* @param[out]cpOutBuf ����Ĳ���������
* @return �ɹ�����SUCC,ʧ�ܷ���FAIL
*/
FUNC_INT  open(T_HWORD hwId,T_CHAR* cpBuf, TCHAR* cpOutBuf)
{
	//�ֲ���������
	SHORT2ADAPTER::iterator theIterator;
	
	//�����Ϸ��Լ��
	if(cpBuf == NULL || hwId < 0 || hwId > 65536)
	{
		return FAIL;
	}

	//����RSP�������Ƿ��Ѿ�����
	g_DllLock.Lock();
	theIterator = vtRSP_Id2Adapter.find(hwId);
	if (theIterator != vtRSP_Id2Adapter.end()) 
	{	
		g_DllLock.Unlock();
		return FAIL;
	}

	//û�У��򴴽���Ӧ��RSP����������
	RspAdapter *adapter = new RspAdapter();	
	vtRSP_Id2Adapter.insert(SHORT2ADAPTER::value_type(hwId, adapter));
	g_DllLock.Unlock();

	//�����������õ�IP���˿ںţ����������ӷ�����
	T_WORD result = adapter->Open(hwId, cpBuf);

	//����������󣬻�������BDIʧ��,�򽫸ղŴ����Ķ���ӹ�ϣ����ɾ��
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

		//�ظ�ʧ�ܱ�־�����������
		cpOutBuf[0] = 'F';
		return FAIL;
	}

	cpOutBuf[0] = 'S';
	return SUCC;
}
