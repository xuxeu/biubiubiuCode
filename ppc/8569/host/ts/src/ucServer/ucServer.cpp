/***************************************************************************
 * 
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:ucServerDll.cpp,v $
*/


/**
 * @file: ucServerMain.cpp
 * @brief: 
 * <li>����: ucServer����ں���</li>
 * @author: fangy
 * @date: 2010-10-27
 * <p>����:ϵͳ��
 */




/*****************************���ò���********************************/
#include <windows.h>
#include "stdafx.h"
#include "ucServer.h"

/*****************************ǰ����������**************************/
extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();


/*****************************���岿��********************************/
ucServer *pUcServer = NULL;

/*****************************ʵ�ֲ���********************************/



/*
 * @Funcname:  tsExit
 * @brief   : tsServer�˳�������
 * @param : ctrlType�����յ����ź�
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

    //��ȡ����̨���
    HWND hWnd = GetConsoleWindow();

    //���ؿ���̨����
    ShowWindow(hWnd,SW_HIDE);

    //ע���˳�����
    SetConsoleCtrlHandler(ucServerExit, TRUE);

    if(argc >= 2)
    {
        //������ʼ������
        for(int i = 1; i < argc; ++i)
        {
            idx += sprintf(paramBuf + idx, "%s ", argv[i]);
        }

        //������־����
        sscanf(paramBuf, "-Log %x" ,&logLevel);
    }

    printf("-Log:%x\n", logLevel);

    //������־
    SystemLog::getInstance(logLevel)->openLogFile(string("ucServer.log"));

    //����ucServer
    pUcServer = new ucServer();
    if (NULL == pUcServer)
    {
        return FAIL;
    }

    //���������߳�,�������ݵ�TS
    pUcServer->start();
    
    //��������
    pUcServer->main();

    if (NULL != pUcServer)
    {
        pUcServer->stop();
        delete pUcServer;
        pUcServer = NULL;
    }

    return 0;
}

