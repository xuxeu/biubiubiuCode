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
 *      验证licence是否能正常解密
 * @return: 
 *      0: 可以正常解密
 *      -1: 无法正常解密
 *      -2: 没有样本无法验证
 *      -3: 缺少解密组件
 *      -4: 解密组件版本不匹配
 */
int ValidateLicence(void)
{
    /* 加载动态库 */
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

    //从配置文件中读取是否已经注册正确的信息
    //存放注册信息的lisence.INI文件存放于IDE目录下的configuration目录中
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

    /* 查询注册表是否已经记录硬盘序列号 */
    ret = regManager.ReadLambadaSerial(driverSN, snLen);
    if (1 == ret)
    {
        /* 验证注册表记录的硬盘序列号 */
        isRegSNOk = CheckoutHardDriveSerialNumber(driverSN, snLen);
        if (!isRegSNOk)
        {
            sysLoger(LINFO, "read invaild reg key");
        }
    }
    else if (-1 == ret)
    {
        /* 没有权限 */
        AfxMessageBox("没有权限访问注册表！" , MB_OK);
        return 0;
    }
    else
    {
        sysLoger(LINFO, "read reg key failed, %d", ret);
    }
    
    if (isRegSNOk)
    {
        sysLoger(LINFO, "read reg key:[%s]", driverSN);
        
        /* 设置硬盘序列号 */
        setHardDriverSerialID(driverSN);
    }
    else
    {      
        /* 没有记录则读取硬盘序列号 */
        
        /* 读取硬盘序列号的字符串值 */ 
        ret = getHardDriveComputerID();
        if (READ_SN_FAIL_NO_RIGHTS == ret)
        {
            /* 没有权限 */
            AfxMessageBox("权限不足，无法注册！" , MB_OK);
            return 0;
        }
        else if (READ_DUMMY_SN == ret)
        {
            CreateDriveSerialNumber(HardDriveSerialNumber, snLen);
        }

        /* 获取硬盘序列号 */
        getHardDriverSerialID(driverSN, &snLen);
        
        /* 将硬盘序列号记录到注册表 */
        ret = regManager.WriteLambadaSerial(driverSN, snLen);
        if (1 != ret)
        {
            if (-1 == ret)
            {
                /* 没有权限FIXME */
                AfxMessageBox("没有权限写注册表！" , MB_OK);
                return 0;
            }
            
            AfxMessageBox("写注册表发生错误！" , MB_OK);
            return 0;
        }
        
    }    
    

    /* 验证licence.ini文件完整性 */
    pLicence = new Licence(LICENCE_FILE_PATH);
    if (NULL == pLicence)
    {
        /* 一般不会出现此情况，仍然继续执行 */
        AfxMessageBox("验证licence.ini文件失败！" , MB_OK);
    }

    /* 检查licence.ini是否缺失 */
    if (!(pLicence->LicenceFileIsExist()))
    {
        /* licence.ini文件缺失 */
        AfxMessageBox("licence.ini文件缺失，请重新输入序列号！" , MB_OK);
        pLicence->CreateLicenceFile();
    }

    /* 检查licence.ini是否完整 */
    if (!(pLicence->LicenceFileIsComplete()))
    {
        /* licence.ini文件不完整，进行修复 */
        pLicence->RepairLicenceFile();
        AfxMessageBox("licence.ini文件损坏，已修复！" , MB_OK);
    }

    /* 检查是否已经licence.ini是否已经记录注册码 */
    //"Register"
    strcpy(section, "Register");

    //"isRegistered"
    strcpy(key, "isRegistered");

    //  "configuration\\licence.ini";
    strcpy(iniFilePath, "configuration\\licence.ini");

    int isRegistered = GetPrivateProfileInt (section, key, 0, iniFilePath); 

    //"SerialNo"
    strcpy(key, "SerialNo");
    
    /* 获取注册码 */
    licenceLen = GetPrivateProfileString(section, key, NULL, licence, sizeof(licence), iniFilePath);
    if (licenceLen < 1)
    {
        isRegistered = false;
    }

    if (isRegistered)
    {
        /* 已经记录，验证是否能解密 */
        ret = ValidateLicence();
        switch (ret)
        {
        case -1:    /* 不能解密，重新生成申请码，要求注册 */
            isRegistered = false;
            AfxMessageBox("序列号不正确，请重新输入！" , MB_OK);
            break;
                        
        case -3:    /* 缺少解密组件 */
            AfxMessageBox("开发环境组件损坏！" , MB_OK);
            return 0;
            break;

        case -2:
        case -4:    /* 解密组件版本不匹配，无法验证序列号，但仍然继续启动 */
            AfxMessageBox("开发环境组件版本不匹配，无法验证序列号！" , MB_OK);
            break;
            
        default:            
            break;
        }
    }
    

    if( !isRegistered )
    {   
        //未注册或注册序列号不正确,弹出注册对话框
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
        // 正确注册则直接启动Lambda主程序
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
            AfxMessageBox("启动主程序失败！" , MB_OK);
         }
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
