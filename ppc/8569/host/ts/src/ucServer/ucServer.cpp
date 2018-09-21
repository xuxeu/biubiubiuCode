/***************************************************************************
 * 
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:ucServerDll.cpp,v $
*/


/**
 * @file: ucServerMain.cpp
 * @brief: 
 * <li>功能: ucServer的入口函数</li>
 * @author: fangy
 * @date: 2010-10-27
 * <p>部门:系统部
 */




/*****************************引用部分********************************/
#include <windows.h>
#include "stdafx.h"
#include "ucServer.h"

/*****************************前向声明部分**************************/
extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();


/*****************************定义部分********************************/
ucServer *pUcServer = NULL;

/*****************************实现部分********************************/



/*
 * @Funcname:  tsExit
 * @brief   : tsServer退出处理函数
 * @param : ctrlType窗口收到的信号
 * @return : 
*/
BOOL WINAPI ucServerExit(DWORD ctrlType)
{
    switch(ctrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:        
    case CTRL_SHUTDOWN_EVENT:
        if (NULL != pUcServer)
        {
            pUcServer->stop();
            delete pUcServer;
            pUcServer = NULL;
        }
         break;
    default:
        break;
    }

    return FALSE;
}


int _tmain(int argc, _TCHAR* argv[])
{
    int idx = 0;
    char paramBuf[100] = "\0";
    LogLevel logLevel = LDEBUG;

    //获取控制台句柄
    HWND hWnd = GetConsoleWindow();

    //隐藏控制台窗口
    ShowWindow(hWnd,SW_HIDE);

    //注册退出函数
    SetConsoleCtrlHandler(ucServerExit, TRUE);

    if(argc >= 2)
    {
        //解析初始化参数
        for(int i = 1; i < argc; ++i)
        {
            idx += sprintf(paramBuf + idx, "%s ", argv[i]);
        }

        //解析日志级别
        sscanf(paramBuf, "-Log %x" ,&logLevel);
    }

    printf("-Log:%x\n", logLevel);

    //创建日志
    SystemLog::getInstance(logLevel)->openLogFile(string("ucServer.log"));

    //创建ucServer
    pUcServer = new ucServer();
    if (NULL == pUcServer)
    {
        return FAIL;
    }

    //启动发送线程,发送数据到TS
    pUcServer->start();
    
    //启动运行
    pUcServer->main();

    if (NULL != pUcServer)
    {
        pUcServer->stop();
        delete pUcServer;
        pUcServer = NULL;
    }

    return 0;
}

