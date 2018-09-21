
/*
* ������ʷ��
*  ���������������޹�˾
*                   �������ļ���
*/

/**
*
* @file     gdbServer.cpp
* @brief
*      ���ܣ�
*      <li>���� DLL Ӧ�ó������ڵ�</li>
*
*/

/************************ͷ�ļ�********************************/
#include "stdafx.h"
#include <map>
using namespace std;

/************************�궨��********************************/
#define FUNC_INT extern "C" __declspec(dllexport) int
#define FUNC_VOID extern "C" __declspec(dllexport) void
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
map<short,GdbServer*> mapAIDToGdb;
typedef map<short,GdbServer*> SHORT2GDB;
MutexLock g_DllLock;    //�Զ�̬���������������

/************************�ⲿ����******************************/
/************************ǰ������******************************/

/************************ʵ��*********************************/
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}

/**
����: ��̬����TSע��Ļص�����, SA DLL��TX�̻߳���ô˷������յ����ݽ��д���
* @param id sa��Դsaid��
* @param des_said Ŀ��said��
* @param src_aid  ca��aid��
* @param src_said ca��said��
* @param pBuf ���͵Ļ�����
* @param size ��������С
* @return0��ʾʧ��,��0��ʾ�ɹ�
*/
FUNC_INT putpkt(unsigned short id,short des_said,short src_aid,short src_said,char* pBuf,int size)
{
    int result;
    GdbServer* gdb = NULL;
    SHORT2GDB::iterator theIterator;

    g_DllLock.Lock();
    theIterator = mapAIDToGdb.find(id);

    if (theIterator != mapAIDToGdb.end())
    {
        gdb = (*theIterator).second;
    }

    else
    {
        g_DllLock.Unlock();
        return FAIL;
    }

    g_DllLock.Unlock();
    result = gdb->putpkt(des_said,src_aid,src_said,pBuf,size);

    return result;
}

/**
����: ��̬����TSע��Ļص�����, SA DLL��RX�̻߳���ô˷������ն�̬������ݽ���ת��
* @param id sa��Դsaid��
* @param des_aid[out] Ŀ��aid��
* @param des_said[out]  Ŀ��said��
* @param src_said[out] ca��said��
* @param pBuf[out] ���͵Ļ�����
* @param size ��������С
* @return ���ݵĴ�С
*/
FUNC_INT getpkt(unsigned short id,short *des_aid,short *des_said,short *src_said,char* pBuf,int size)
{
    int result;
    GdbServer* gdb = NULL;
    SHORT2GDB::iterator theIterator;

    g_DllLock.Lock();
    theIterator = mapAIDToGdb.find(id);

    if (theIterator != mapAIDToGdb.end())
    {
        gdb = (*theIterator).second;
    }

    else
    {
        g_DllLock.Unlock();
        return FAIL;
    }

    g_DllLock.Unlock();
    result = gdb->getpkt(des_aid,des_said,src_said,pBuf,size);
    return result;
}

/**
����: �رն���,���ڶ�̬���ڲ��ͷ�ռ����Դ
* @param id sa��Դsaid��
* @param name ���ݵĲ���
* @return 0��ʾ�ɹ�
*/
FUNC_INT close(unsigned short id)
{
    GdbServer* gdb = NULL;
    SHORT2GDB::iterator theIterator;

    g_DllLock.Lock();
    theIterator = mapAIDToGdb.find(id);

    if (theIterator != mapAIDToGdb.end())
    {
        gdb = (*theIterator).second;
    }

    else
    {
        g_DllLock.Unlock();
        return FAIL;
    }

    mapAIDToGdb.erase(theIterator);
    g_DllLock.Unlock();

    if(gdb != NULL)
    {
        gdb->close();
        delete gdb;
        gdb = NULL;
    }

    return SUCC;
}

/**
����: �򿪶���,���ڶ�̬���ڲ�����ʼ������
* @param id sa��Դaid��
* @param pBuf[IN] �����ַ���,ͨ���ָ����','�����н���
* @param pOutBuf[OUT] ������������
* @return 0��ʾ�ɹ�,
*/
FUNC_INT  open(unsigned short id,char* pBuf,char* pOutBuf)
{
    int result;
    int des_aid;
    int des_said;
    int port = 0;
    char *ptr = NULL;
    SHORT2GDB::iterator theIterator;

    //�����Ϸ��Լ��
    if(pBuf == NULL || id < 0 || id > 65536)
    {
        return FAIL;
    }

    result = hex2int(&pBuf,&des_aid);

    if(result ==0 || des_aid <= 0 || des_aid > 65535)
    {
        return FAIL;
    }

    ptr = strstr(pBuf,",");

    if(ptr == NULL)
    {
        return FAIL;
    }

    ptr += 1;
    result = hex2int(&ptr,&des_said);

    if(result ==0 || des_said < 0 || des_said > 65535)
    {
        return FAIL;
    }

    g_DllLock.Lock();
    theIterator = mapAIDToGdb.find(id);

    if (theIterator != mapAIDToGdb.end())
    {
        g_DllLock.Unlock();
        return FAIL;
    }

    GdbServer *gdb = new GdbServer(id);

    if(NULL == gdb)
    {
        return FAIL;
    }

    mapAIDToGdb.insert(SHORT2GDB::value_type(id, gdb));
    g_DllLock.Unlock();

    gdb->des_aid = des_aid;
    gdb->des_said = des_said;
    gdb->setOutBuf(pOutBuf);
    result = gdb->open(port);

    if(result == FAIL)
    {
        g_DllLock.Lock();
        gdb->close();
        delete gdb;
        gdb = NULL;
        theIterator = mapAIDToGdb.find(id);

        if (theIterator != mapAIDToGdb.end())
        {
            mapAIDToGdb.erase(theIterator);
        }

        g_DllLock.Unlock();
    }

    return result;

}