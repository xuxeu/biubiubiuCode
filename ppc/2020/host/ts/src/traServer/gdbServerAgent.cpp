
/*
* 更改历史：
*  北京科银技术有限公司
*                   创建该文件。
*/

/**
*
* @file     gdbServer.cpp
* @brief
*      功能：
*      <li>定义 DLL 应用程序的入口点</li>
*
*/

/************************头文件********************************/
#include "stdafx.h"
#include <map>
using namespace std;

/************************宏定义********************************/
#define FUNC_INT extern "C" __declspec(dllexport) int
#define FUNC_VOID extern "C" __declspec(dllexport) void
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
map<short,GdbServer*> mapAIDToGdb;
typedef map<short,GdbServer*> SHORT2GDB;
MutexLock g_DllLock;    //对动态库操作进行锁操作

/************************外部声明******************************/
/************************前向声明******************************/

/************************实现*********************************/
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}

/**
功能: 动态库向TS注册的回调函数, SA DLL的TX线程会调用此方法对收到数据进行处理
* @param id sa的源said号
* @param des_said 目标said号
* @param src_aid  ca的aid号
* @param src_said ca的said号
* @param pBuf 发送的缓冲区
* @param size 缓冲区大小
* @return0表示失败,非0表示成功
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
功能: 动态库向TS注册的回调函数, SA DLL的RX线程会调用此方法接收动态库的数据进行转发
* @param id sa的源said号
* @param des_aid[out] 目标aid号
* @param des_said[out]  目标said号
* @param src_said[out] ca的said号
* @param pBuf[out] 发送的缓冲区
* @param size 缓冲区大小
* @return 数据的大小
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
功能: 关闭动作,便于动态库内部释放占用资源
* @param id sa的源said号
* @param name 传递的参数
* @return 0表示成功
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
功能: 打开动作,便于动态库内部做初始化工作
* @param id sa的源aid号
* @param pBuf[IN] 传入字符串,通过分割符号','来进行解析
* @param pOutBuf[OUT] 传出给调用者
* @return 0表示成功,
*/
FUNC_INT  open(unsigned short id,char* pBuf,char* pOutBuf)
{
    int result;
    int des_aid;
    int des_said;
    int port = 0;
    char *ptr = NULL;
    SHORT2GDB::iterator theIterator;

    //参数合法性检查
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