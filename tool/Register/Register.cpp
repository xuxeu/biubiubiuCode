// Register.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Register.h"
#include "RegisterDlg.h"
#include "Diskid.h"
#include "lambdaProc.h"
#include "Licence.h"
#include "RegX.h"
#include "log.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterApp

BEGIN_MESSAGE_MAP(CRegisterApp, CWinApp)
    //{{AFX_MSG_MAP(CRegisterApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegisterApp construction

CRegisterApp::CRegisterApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRegisterApp object

CRegisterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRegisterApp initialization
typedef int (*VALIDLIC)(void);

/*
 * @brief: 
 *      ��֤licence�Ƿ�����������
 * @return: 
 *      0: ������������
 *      -1: �޷���������
 *      -2: û�������޷���֤
 *      -3: ȱ�ٽ������
 *      -4: ��������汾��ƥ��
 */
int ValidateLicence(void)
{
    /* ���ض�̬�� */
    HMODULE hDll;

    VALIDLIC validFun = NULL;

    hDll = LoadLibrary("jre\\bin\\win32.dll");
    if (NULL == hDll)
    {
        sysLoger(LERROR, "not find win32.dll");
        return -3;
    }

    validFun = (VALIDLIC)GetProcAddress(hDll, "validateLicence");
    if (NULL == validFun)
    {
        sysLoger(LERROR, "get validate fun failed");
        return -4;
    }

    return validFun();
}

BOOL CRegisterApp::InitInstance()
{
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

#ifdef _AFXDLL
    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

    //�������ļ��ж�ȡ�Ƿ��Ѿ�ע����ȷ����Ϣ
    //���ע����Ϣ��lisence.INI�ļ������IDEĿ¼�µ�configurationĿ¼��
    char section[256];
    char key[256];
    char iniFilePath[256];
    int ret = 0;
    char driverSN[256];
    int snLen = 0;
    Licence *pLicence = NULL;
    bool isLicenceFileOk = false;
    RegX regManager;
    bool isRegSNOk = false;
    char licence[128];
    DWORD licenceLen = 0;


    memset((void *)section, sizeof(unsigned short), 256);
    memset((void *)key, sizeof(unsigned short), 256);
    memset((void *)iniFilePath, sizeof(unsigned short), 256);
    memset(driverSN, 0, sizeof(driverSN));
    memset(licence, 0, sizeof(licence));

    SystemLog::getInstance()->openLogFile(string("LambdaPRO.log"));

    /* ��ѯע����Ƿ��Ѿ���¼Ӳ�����к� */
    ret = regManager.ReadLambadaSerial(driverSN, snLen);
    if (1 == ret)
    {
        /* ��֤ע����¼��Ӳ�����к� */
        isRegSNOk = CheckoutHardDriveSerialNumber(driverSN, snLen);
        if (!isRegSNOk)
        {
            sysLoger(LINFO, "read invaild reg key");
        }
    }
    else if (-1 == ret)
    {
        /* û��Ȩ�� */
        AfxMessageBox("û��Ȩ�޷���ע���" , MB_OK);
        return 0;
    }
    else
    {
        sysLoger(LINFO, "read reg key failed, %d", ret);
    }
    
    if (isRegSNOk)
    {
        sysLoger(LINFO, "read reg key:[%s]", driverSN);
        
        /* ����Ӳ�����к� */
        setHardDriverSerialID(driverSN);
    }
    else
    {      
        /* û�м�¼���ȡӲ�����к� */
        
        /* ��ȡӲ�����кŵ��ַ���ֵ */ 
        ret = getHardDriveComputerID();
        if (READ_SN_FAIL_NO_RIGHTS == ret)
        {
            /* û��Ȩ�� */
            AfxMessageBox("Ȩ�޲��㣬�޷�ע�ᣡ" , MB_OK);
            return 0;
        }
        else if (READ_DUMMY_SN == ret)
        {
            CreateDriveSerialNumber(HardDriveSerialNumber, snLen);
        }

        /* ��ȡӲ�����к� */
        getHardDriverSerialID(driverSN, &snLen);
        
        /* ��Ӳ�����кż�¼��ע��� */
        ret = regManager.WriteLambadaSerial(driverSN, snLen);
        if (1 != ret)
        {
            if (-1 == ret)
            {
                /* û��Ȩ��FIXME */
                AfxMessageBox("û��Ȩ��дע���" , MB_OK);
                return 0;
            }
            
            AfxMessageBox("дע���������" , MB_OK);
            return 0;
        }
        
    }    
    

    /* ��֤licence.ini�ļ������� */
    pLicence = new Licence(LICENCE_FILE_PATH);
    if (NULL == pLicence)
    {
        /* һ�㲻����ִ��������Ȼ����ִ�� */
        AfxMessageBox("��֤licence.ini�ļ�ʧ�ܣ�" , MB_OK);
    }

    /* ���licence.ini�Ƿ�ȱʧ */
    if (!(pLicence->LicenceFileIsExist()))
    {
        /* licence.ini�ļ�ȱʧ */
        AfxMessageBox("licence.ini�ļ�ȱʧ���������������кţ�" , MB_OK);
        pLicence->CreateLicenceFile();
    }

    /* ���licence.ini�Ƿ����� */
    if (!(pLicence->LicenceFileIsComplete()))
    {
        /* licence.ini�ļ��������������޸� */
        pLicence->RepairLicenceFile();
        AfxMessageBox("licence.ini�ļ��𻵣����޸���" , MB_OK);
    }

    /* ����Ƿ��Ѿ�licence.ini�Ƿ��Ѿ���¼ע���� */
    //"Register"
    strcpy(section, "Register");

    //"isRegistered"
    strcpy(key, "isRegistered");

    //  "configuration\\licence.ini";
    strcpy(iniFilePath, "configuration\\licence.ini");

    int isRegistered = GetPrivateProfileInt (section, key, 0, iniFilePath); 

    //"SerialNo"
    strcpy(key, "SerialNo");
    
    /* ��ȡע���� */
    licenceLen = GetPrivateProfileString(section, key, NULL, licence, sizeof(licence), iniFilePath);
    if (licenceLen < 1)
    {
        isRegistered = false;
    }

    if (isRegistered)
    {
        /* �Ѿ���¼����֤�Ƿ��ܽ��� */
        ret = ValidateLicence();
        switch (ret)
        {
        case -1:    /* ���ܽ��ܣ��������������룬Ҫ��ע�� */
            isRegistered = false;
            AfxMessageBox("���кŲ���ȷ�����������룡" , MB_OK);
            break;
                        
        case -3:    /* ȱ�ٽ������ */
            AfxMessageBox("������������𻵣�" , MB_OK);
            return 0;
            break;

        case -2:
        case -4:    /* ��������汾��ƥ�䣬�޷���֤���кţ�����Ȼ�������� */
            AfxMessageBox("������������汾��ƥ�䣬�޷���֤���кţ�" , MB_OK);
            break;
            
        default:            
            break;
        }
    }
    

    if( !isRegistered )
    {   
        //δע���ע�����кŲ���ȷ,����ע��Ի���
        CRegisterDlg dlg;
        m_pMainWnd = &dlg;
        int nResponse = dlg.DoModal();
        if (nResponse == IDOK)
        {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with OK
        }
        else if (nResponse == IDCANCEL)
        {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with Cancel
        }
    }
    else
    {
        // ��ȷע����ֱ������Lambda������
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &pi, sizeof(pi) );

        //"LambdaIDE"
        // Start the child process. 
         if( !CreateProcess( NULL, // No module name (use command line). 
            LAMBDA_PROC_NAME, // Command line. 
            NULL,             // Process handle not inheritable. 
            NULL,             // Thread handle not inheritable. 
            FALSE,            // Set handle inheritance to FALSE. 
            0,                // No creation flags. 
            NULL,             // Use parent's environment block. 
            NULL,             // Use parent's starting directory. 
            &si,              // Pointer to STARTUPINFO structure.
            &pi )             // Pointer to PROCESS_INFORMATION structure.
         ) 
         {
            AfxMessageBox("����������ʧ�ܣ�" , MB_OK);
         }
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
