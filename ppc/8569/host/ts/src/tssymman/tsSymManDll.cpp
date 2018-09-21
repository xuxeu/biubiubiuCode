/*
* 更改历史：
* 2008-09-23 zhangxu  北京科银技术有限公司
*                       创建该文件。
*/

/**
* @file  tsSymManDll.cpp
* @brief
*       功能：
*       <li>tssymman DLL接口层</li>
*/

/************************头文件********************************/
#include "afx.h"
#include <direct.h>
#include <windows.h>
#include "string.h"
#include "tsSymManComm.h"
#include "tsSymManServer.h"

using namespace std;

/************************宏定义********************************/
#define DLL_FUNC_INT extern "C" __declspec(dllexport) int
#define DLL_FUNC_VOID extern "C" __declspec(dllexport) void

/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
static T_UWORD tsSymManID = 0;

static bool isInit = false;

tsSymMan *gSymManServer = NULL;

/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/

/**动态库初始化入口
BOOL APIENTRY DllMain( HANDLE hModule,
                                           DWORD  ul_reason_for_call,
                                           LPVOID lpReserved)
{
    return TRUE;
}
*/

/**
 * @Funcname:  open
 * @brief        :  向ts提供的激活动态库接口
 * @para[IN]  : hwId, 动态库在TS中的ID号
 * @para[IN]  : cpBuf, 激活参数,这里只有是否记录日志
 * @para[OUT]: cpOutBuf, 暂不使用
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 14:57:55
 *
**/
DLL_FUNC_INT  open(T_UHWORD hwId, char* cpBuf, char* cpOutBuf)
{
    char *pBuf = cpBuf;
    char *pTmp = NULL;
    char pstrPath[MAX_PATH_LEN] = "\0";
    int ret = 0;
    int isLog = LDEBUG;

    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if(isInit)
    {
        return SUCC;
    }

    /*记录自身ID号*/
    tsSymManID = hwId;

    hex2int(&pBuf, &isLog);

    SystemLog::getInstance((LogLevel)isLog)->openLogFile(string("tssymman.log"));

#ifndef SYMDLL_DEBUG
    /*获取当前系统路径*/
    ret = GetModuleFileName(NULL, pstrPath, MAX_PATH_LEN);

    if(0 == ret)
    {
        return FAIL;
    }

#else
    strcpy(pstrPath, "e:\\LambdaAE_1216\\host\\pub\\ts.exe");
#endif
    pTmp = strstr(pstrPath, "ts.exe");

    if(NULL == pTmp)
    {
        return FAIL;
    }

    *pTmp = '\0';
    strcat(pstrPath, "symbolFiles");

    ret = CheckFolderExist(pstrPath);

    if (FALSE == ret)
    {
        _mkdir(pstrPath);
    }

    /*实例化tsSymMan对象*/
    gSymManServer = new tsSymMan(pstrPath);

    if(NULL == gSymManServer)
    {
        sysLoger(LWARNING, "create tsSymMan fail");
        return FAIL;
    }

    isInit = true;

    sysLoger(LINFO, "init tsSymMan succ, aid:%d", tsSymManID);
    return SUCC;
}

/**
 * @Funcname:  close
 * @brief        :  向ts提供的关闭动态库的接口
 * @para[IN]  : hwId, 动态库在TS中的ID号
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:34:30
 *
**/

DLL_FUNC_INT close(T_UHWORD hwId)
{

    if((isInit) && (tsSymManID == hwId))
    {
        gSymManServer->stop();
        Sleep(10);           //延时10毫秒,确保getpkt执行完毕
        delete gSymManServer;
        gSymManServer = NULL;
        sysLoger(LINFO, "close tsSymMan succ");
        return SUCC;
    }

    return FAIL;
}

/**
 * @Funcname:  putpkt
 * @brief       :  向ts提供的发包接口,TS调用该接口将数据发送到tssymman
 * @para[IN]  : vwServerId, 动态库在TS中的ID号
 * @para[IN]  : vhwDesSaid, 动态库中的二级代理,暂时无用
 * @para[IN]  : vhwSrcAid, 源ID号
 * @para[IN]  : vwSrcSaid, 源二级ID号
 * @para[IN]  : cpBuf, 数据
 * @para[IN]  : vwSize, 数据大小
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:38:45
 *
**/
DLL_FUNC_INT putpkt(T_UHWORD vwServerId, T_UHWORD vhwDesSaid,
                    T_UHWORD vhwSrcAid, T_UHWORD vwSrcSaid,
                    char* cpBuf, int vwSize)
{
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if((vwServerId != tsSymManID) || (false == isInit))
    {
        return FAIL;
    }

    return gSymManServer->putpkt(vhwSrcAid, vwSrcSaid, cpBuf, vwSize);
}

/**
 * @Funcname:  getpkt
 * @brief       :  向ts提供的收包接口, TS调用该接口从tssymman接收数据
 * @para[IN]  : vwServerId, 动态库在TS中的ID号
 * @para[IN]  : vwpDesAid, 目的ID号
 * @para[IN]  : vwpDesSaid, 目的二级ID号
 * @para[IN]  : vwpSrcSaid, 源二级ID号
 * @para[IN]  : cpBuf, 数据缓冲
 * @para[IN]  : vwSize, 数据缓冲大小
 * @return    : 成功返回接收到的数据的大小,失败返回FAIL
 * @Author    :  zhangxu
 *
 * @History: 1.  Created this function on 2008年9月23日 15:43:03
 *
**/
DLL_FUNC_INT getpkt(T_UHWORD vwServerId, T_UHWORD *vwpDesAid,
                    T_UHWORD *vwpDesSaid, T_UHWORD *vwpSrcSaid,
                    char* cpBuf, int vwSize)
{
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if((vwServerId != tsSymManID) || (false == isInit))
    {
        return FAIL;
    }

    *vwpSrcSaid = 0;

    return gSymManServer->getpkt(vwpDesAid, vwpDesSaid, cpBuf, vwSize);

}
