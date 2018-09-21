/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/

/**
 * @file             :  main.cpp
 * @Version       :  1.0.0
 * @brief           :  *   <li> ����</li>tsServer������ں���
                                <li>���˼��</li> 
                                <p> n     ע�����n</p> 
 *   
 * @author        :  zhagnxu
 * @create date :  2008��10��9�� 15:49:46
 * @History       : 
 *   
 */

 
/************************���ò���*****************************/

#include <windows.h>
#include "tssComm.h"
#include "tsServer.h"

extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();


/************************ǰ����������***********************/

/************************���岿��*****************************/

/************************ʵ�ֲ���****************************/


/*
 * @Funcname:  tsExit
 * @brief   : tsServer�˳�������
 * @param : ctrlType�����յ����ź�
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
        /*ֹͣTsServer*/
        TsServer::getInstance()->stop();
         break;
    default:
        break;
    }

    return FALSE;
}

T_WORD main(T_WORD argc, T_CHAR *argv)
{
    //��ȡ����̨���
    HWND hWnd = GetConsoleWindow();

    //���ؿ���̨����
    ShowWindow(hWnd,SW_HIDE);

    //ע���˳�����
    SetConsoleCtrlHandler(tsServerExit, TRUE);
   
    /*����TsServer�����������ʼ
    ������Ϣ����ѭ��*/
    TsServer::getInstance()->main();

    return 0;
}

