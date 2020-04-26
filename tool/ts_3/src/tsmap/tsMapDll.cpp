/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tsMapDll.cpp
 * @Version        :  1.0.0
 * @brief           :  *   <li> 功能</li>   TSMAP DLL接口层
                                <li>设计思想</li> 
                                <p>注意事项：</p>
 *   
 * @author        :  qingxiaohai
 * @create date:  2007年11月2日 14:46:13
 * @History        : 
 *   
 */



#include <iostream>

#include "tsmapComm.h"
#include "tsMapServer.h"
#include "common.h"

using namespace std;


#define FUNC_INT extern "C" __declspec(dllexport) int   
#define FUNC_VOID extern "C" __declspec(dllexport) void  


static UI tsmapID = 0;

static bool isInit = false;

tsMap *gMapServer = NULL;


/*
*动态库初始化入口
*/
BOOL APIENTRY DllMain( HANDLE hModule, 
                                           DWORD  ul_reason_for_call, 
                                           LPVOID lpReserved)
{
    return TRUE;
}

/**
 * @Funcname:  open
 * @brief        :  向ts提供的激活动态库接口
 * @para[IN]  : hwId, 动态库在TS中的ID号
 * @para[IN]  : cpBuf, 激活参数,这里只有是否记录日志
 * @para[OUT]: cpOutBuf, 暂不使用
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 15:11:43
 *   
**/
FUNC_INT  open(US hwId, char* cpBuf, char* cpOutBuf)
{
    char *pBuf = cpBuf;
    int isLog = LBUTT;
    
    if(NULL == cpBuf)
    {
        return FAIL;
    }
    
    if(isInit)
    {
        return SUCC;
    }
    
    /*记录自身ID号*/
    tsmapID = hwId;

    hex2int(&pBuf, &isLog);

    /*创建系统日志*/
    SystemLog::getInstance((LogLevel)isLog)->openLogFile(string("tsmap.log"));

    gMapServer = new tsMap();
    if(NULL == gMapServer)
    {
        return FAIL;
    }
    isInit = true;
    return SUCC;
    
}

/**
 * @Funcname:  close
 * @brief        :  向ts提供的关闭动态库的接口
 * @para[IN]  : hwId, 动态库在TS中的ID号
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 15:14:43
 *   
**/

FUNC_INT close(US hwId)
{

    if((isInit) && (tsmapID == hwId))
    {
        /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [begin]*/
        /*  Modified brief: 增加停止TSMAP*/
        gMapServer->stop();
        Sleep(10);           //延时10毫秒,确保getpkt执行完毕
        /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [end]*/
        delete gMapServer;
        gMapServer = NULL;
        return SUCC;
    }

    return FAIL;
}

/**
 * @Funcname:  putpkt
 * @brief        :  向ts提供的发包接口,TS调用该接口将数据发送到tsmap
 * @para[IN]  : vwServerId, 动态库在TS中的ID号
 * @para[IN]  : vhwDesSaid, 动态库中的二级代理,暂时无用
 * @para[IN]  : vhwSrcAid, 源ID号
 * @para[IN]  : vwSrcSaid, 源二级ID号
 * @para[IN]  : cpBuf, 数据
 * @para[IN]  : vwSize, 数据大小
 * @return      : 成功返回SUCC,失败返回FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 15:14:43
 *   
**/
FUNC_INT putpkt(US vwServerId, US vhwDesSaid,
                                US vhwSrcAid, US vwSrcSaid,
                                char* cpBuf, int vwSize)
{
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if((vwServerId != tsmapID) || (false == isInit))
    {
        return FAIL;
    }

    return gMapServer->putpkt(vhwSrcAid, vwSrcSaid, cpBuf, vwSize);
}

/**
 * @Funcname:  getpkt
 * @brief        :  向ts提供的收包接口, TS调用该接口从tsmap接收数据
 * @para[IN]  : vwServerId, 动态库在TS中的ID号
 * @para[IN]  : vwpDesAid, 目的ID号
 * @para[IN]  : vwpDesSaid, 目的二级ID号
 * @para[IN]  : vwpSrcSaid, 源二级ID号
 * @para[IN]  : cpBuf, 数据缓冲
 * @para[IN]  : vwSize, 数据缓冲大小
 * @return      : 成功返回接收到的数据的大小,失败返回FAIL
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年11月2日 15:14:43
 *   
**/
FUNC_INT getpkt(US vwServerId, US *vwpDesAid,
                                US *vwpDesSaid, US *vwpSrcSaid,
                                char* cpBuf, int vwSize)
{
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    if((vwServerId != tsmapID) || (false == isInit))
    {
        return FAIL;
    }
    
    *vwpSrcSaid = 0;
    
    return gMapServer->getpkt(vwpDesAid, vwpDesSaid, cpBuf, vwSize);
    
}



