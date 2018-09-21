/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/

/**
 * @file             :  main.cpp
 * @Version       :  1.0.0
 * @brief           :  *   <li> 功能</li>tsServer的主入口函数
                                <li>设计思想</li> 
                                <p> n     注意事项：n</p> 
 *   
 * @author        :  zhagnxu
 * @create date :  2008年10月9日 15:49:46
 * @History       : 
 *   
 */

 
/************************引用部分*****************************/

#include <windows.h>
#include "tssComm.h"
#include "tsServer.h"

extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();


/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/


/*
 * @Funcname:  tsExit
 * @brief   : tsServer退出处理函数
 * @param : ctrlType窗口收到的信号
 * @return : 
*/
BOOL WINAPI tsServerExit(DWORD ctrlType)
{
    switch(ctrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:		
    case CTRL_SHUTDOWN_EVENT:
        /*停止TsServer*/
        TsServer::getInstance()->stop();
         break;
    default:
        break;
    }

    return FALSE;
}

T_WORD main(T_WORD argc, T_CHAR *argv)
{
    //获取控制台句柄
    HWND hWnd = GetConsoleWindow();

    //隐藏控制台窗口
    ShowWindow(hWnd,SW_HIDE);

    //注册退出函数
    SetConsoleCtrlHandler(tsServerExit, TRUE);
   
    /*调用TsServer类的主函数开始
    进入消息处理循环*/
    TsServer::getInstance()->main();

    return 0;
}

