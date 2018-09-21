/*
* 更改历史：
* 2004-12-6  董洪涛  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 唐兴培  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  stdafx.cpp
* @brief  
*       功能：
*       <li>shell控制台应用程序的入口点</li>
*/

/************************头文件********************************/
#include "stdafx.h"
#include <windows.h>
#include "utils.h"
#include <Winsock2.h>
#include "consoleManager.h"
#include "shellMain.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/
/* 类指针 */
consoleManager* pConsoleManager = NULL;

/*ctrl+c键是否有按下*/
bool ctrl_c_flag = false;
/************************模块变量******************************/

/************************外部声明******************************/
/* 当前目标机连接AID */
extern int curTargetAID;

extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();

/************************前向声明******************************/

/************************实现*********************************/
/*
 * @Funcname:  hostShellExit
 * @brief   : hostShell退出处理函数
 * @param : ctrlType窗口收到的信号
 * @return : 
 *          true:执行函数后不退出程序
 *          false:执行函数后退出程序
*/
BOOL WINAPI hostShellExit(DWORD ctrlType)
{
    switch(ctrlType)
    {

    /*设置标志,并返回true不让操作系统再处理该事件
	如果操作系统继续处理该事件会导致程序直接退出*/
    case CTRL_C_EVENT:
        ctrl_c_flag = true;
        return true;
        
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:        
    case CTRL_SHUTDOWN_EVENT:
        if (NULL != pConsoleManager)
        {
            pConsoleManager->stop();
            delete pConsoleManager;
            pConsoleManager = NULL;
        }
        break;
		
    default:
        break;
    }

    return false;
}

void setWindownTitle(const char* titleName){

	HWND hWindowsHandler;
		//获取控制台句柄
	hWindowsHandler = GetConsoleWindow();
	char hostName[260] = "\0";
	string sTitleName = "";
	gethostname(hostName,260);
	if(titleName != NULL && titleName[0] != '\0'){
		sTitleName = sTitleName + titleName + "@" + hostName + " - Host Shell";
	}else{
		sTitleName = "Host Shell";
	}
	//设置当前窗口标题
	SetWindowText(hWindowsHandler,sTitleName.c_str());
}


int _tmain(int argc, char* argv[])
{
    int ret = 0;
    HWND hWindowsHandler;
    char *targetName = NULL;
	int timeout = -1;
	char *tmpTimeOut = NULL;

    if(argc > 1)
    {
        targetName = ArgGetStr(argc,argv,"-name");
        if(targetName == NULL)
        {
            cout << "获取目标机名称失败" << endl;
            getchar();
            return -1;
        }
			 tmpTimeOut= ArgGetStr(argc,argv,"-timeout");
        if(tmpTimeOut == NULL)
        {
            cout << "获取目标机超时失败" << endl;
            getchar();
            return -1;
        }
		timeout = atoi(tmpTimeOut)/1000;
    }

    //注册退出函数
    SetConsoleCtrlHandler(hostShellExit, true);

    //获取控制台句柄
    hWindowsHandler = GetConsoleWindow();
        
    /* 显示窗口 */
    ShowWindow( hWindowsHandler, SW_SHOW );
    
	/*设置缺省解析器为核心操作系统解析器 */
    pConsoleManager = new consoleManager(COREOS_INTERPRETER);
    if(NULL == pConsoleManager)
    {
        cout << "启动shell失败!" << endl;
        getchar();
        return -1;
    }

    /*保存shell.exe的路径*/
    pConsoleManager->savePath(argv[0]);

    /* 启动控制台管理器 */
    pConsoleManager->run(targetName,timeout);

    /* 退出 shell*/
    if (NULL != pConsoleManager)
    {
        pConsoleManager->stop();
		
        delete pConsoleManager;
        pConsoleManager = NULL;
    }

    return 0;
}


