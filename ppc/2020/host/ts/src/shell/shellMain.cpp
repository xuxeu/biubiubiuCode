/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ������  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  stdafx.cpp
* @brief  
*       ���ܣ�
*       <li>shell����̨Ӧ�ó������ڵ�</li>
*/

/************************ͷ�ļ�********************************/
#include "stdafx.h"
#include <windows.h>
#include "utils.h"
#include <Winsock2.h>
#include "consoleManager.h"
#include "shellMain.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/
/* ��ָ�� */
consoleManager* pConsoleManager = NULL;

/*ctrl+c���Ƿ��а���*/
bool ctrl_c_flag = false;
/************************ģ�����******************************/

/************************�ⲿ����******************************/
/* ��ǰĿ�������AID */
extern int curTargetAID;

extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();

/************************ǰ������******************************/

/************************ʵ��*********************************/
/*
 * @Funcname:  hostShellExit
 * @brief   : hostShell�˳�������
 * @param : ctrlType�����յ����ź�
 * @return : 
 *          true:ִ�к������˳�����
 *          false:ִ�к������˳�����
*/
BOOL WINAPI hostShellExit(DWORD ctrlType)
{
    switch(ctrlType)
    {

    /*���ñ�־,������true���ò���ϵͳ�ٴ�����¼�
	�������ϵͳ����������¼��ᵼ�³���ֱ���˳�*/
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
		//��ȡ����̨���
	hWindowsHandler = GetConsoleWindow();
	char hostName[260] = "\0";
	string sTitleName = "";
	gethostname(hostName,260);
	if(titleName != NULL && titleName[0] != '\0'){
		sTitleName = sTitleName + titleName + "@" + hostName + " - Host Shell";
	}else{
		sTitleName = "Host Shell";
	}
	//���õ�ǰ���ڱ���
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
            cout << "��ȡĿ�������ʧ��" << endl;
            getchar();
            return -1;
        }
			 tmpTimeOut= ArgGetStr(argc,argv,"-timeout");
        if(tmpTimeOut == NULL)
        {
            cout << "��ȡĿ�����ʱʧ��" << endl;
            getchar();
            return -1;
        }
		timeout = atoi(tmpTimeOut)/1000;
    }

    //ע���˳�����
    SetConsoleCtrlHandler(hostShellExit, true);

    //��ȡ����̨���
    hWindowsHandler = GetConsoleWindow();
        
    /* ��ʾ���� */
    ShowWindow( hWindowsHandler, SW_SHOW );
    
	/*����ȱʡ������Ϊ���Ĳ���ϵͳ������ */
    pConsoleManager = new consoleManager(COREOS_INTERPRETER);
    if(NULL == pConsoleManager)
    {
        cout << "����shellʧ��!" << endl;
        getchar();
        return -1;
    }

    /*����shell.exe��·��*/
    pConsoleManager->savePath(argv[0]);

    /* ��������̨������ */
    pConsoleManager->run(targetName,timeout);

    /* �˳� shell*/
    if (NULL != pConsoleManager)
    {
        pConsoleManager->stop();
		
        delete pConsoleManager;
        pConsoleManager = NULL;
    }

    return 0;
}


