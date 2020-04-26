// RegisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Register.h"
#include "RegisterDlg.h"
#include "diskid.h"
#include "md5.h"
#include "lambdaProc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAP_TABLE_LEN   10
#define MD5CODE_LEN     16
#define APPLY_CODE_LEN  16
/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CRegisterDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CRegisterDlg)
    m_strApply1 = _T("");
    m_strApply2 = _T("");
    m_strApply3 = _T("");
    m_strApply4 = _T("");
    m_strSerial1 = _T("");
    m_strSerial2 = _T("");
    m_strSerial3 = _T("");
    m_strSerial4 = _T("");
    m_strInfo = _T("");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRegisterDlg)
    DDX_Control(pDX, IDC_INFO, m_ctrlInfo);
    DDX_Control(pDX, IDC_EDIT_SERIAL4, m_ctrlSerial4);
    DDX_Control(pDX, IDC_EDIT_SERIAL3, m_ctrlSerial3);
    DDX_Control(pDX, IDC_EDIT_SERIAL1, m_ctrlSerial1);
    DDX_Control(pDX, IDC_EDIT_SERIAL2, m_ctrlSerial2);
    DDX_Control(pDX, IDOK, m_ctrlOK);
    DDX_Text(pDX, IDC_EDIT_APPLY1, m_strApply1);
    DDV_MaxChars(pDX, m_strApply1, 4);
    DDX_Text(pDX, IDC_EDIT_APPLY2, m_strApply2);
    DDV_MaxChars(pDX, m_strApply2, 4);
    DDX_Text(pDX, IDC_EDIT_APPLY3, m_strApply3);
    DDV_MaxChars(pDX, m_strApply3, 4);
    DDX_Text(pDX, IDC_EDIT_APPLY4, m_strApply4);
    DDV_MaxChars(pDX, m_strApply4, 4);
    DDX_Text(pDX, IDC_EDIT_SERIAL1, m_strSerial1);
    DDV_MaxChars(pDX, m_strSerial1, 6);
    DDX_Text(pDX, IDC_EDIT_SERIAL2, m_strSerial2);
    DDV_MaxChars(pDX, m_strSerial2, 6);
    DDX_Text(pDX, IDC_EDIT_SERIAL3, m_strSerial3);
    DDV_MaxChars(pDX, m_strSerial3, 6);
    DDX_Text(pDX, IDC_EDIT_SERIAL4, m_strSerial4);
    DDV_MaxChars(pDX, m_strSerial4, 6);
    DDX_Text(pDX, IDC_INFO, m_strInfo);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
    //{{AFX_MSG_MAP(CRegisterDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, OnOk)
    ON_EN_CHANGE(IDC_EDIT_SERIAL1, OnChangeEditSerial1)
    ON_EN_CHANGE(IDC_EDIT_SERIAL2, OnChangeEditSerial2)
    ON_EN_CHANGE(IDC_EDIT_SERIAL3, OnChangeEditSerial3)
    ON_EN_CHANGE(IDC_EDIT_SERIAL4, OnChangeEditSerial4)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg message handlers

BOOL CRegisterDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon
    
    // TODO: Add extra initialization here
    // ��ȡָ�����ע����ļ�register.info��Ϣ��IDC_INFO���ı�������ʾ
    char *regInfo = NULL;
    char infoFile[256];
    char mode[10];

    //��ʼ��
    memset((void *)infoFile, '\0', 256);
    memset((void *) mode, '\0', 10);

    //"register.info"
    infoFile[0] = 'r';
    infoFile[1] = 'e';
    infoFile[2] = 'g';
    infoFile[3] = 'i';
    infoFile[4] = 's';
    infoFile[5] = 't';
    infoFile[6] = 'e';
    infoFile[7] = 'r';
    infoFile[8] = '.';
    infoFile[9] = 'i';
    infoFile[10] = 'n';
    infoFile[11] = 'f';
    infoFile[12] = 'o';
    infoFile[13] = '\0';

    // "r"Ϊֻ����ʽ
    mode[0] = 'r';
    mode[1] = 'b';
    mode[2] = '\0';

    FILE *info = fopen(infoFile, mode);
    struct _stat  infoStat;
    int result = _stat(infoFile, &infoStat);
    if(info != NULL){
        if(result ==0){
            regInfo = (char *)malloc(infoStat.st_size * sizeof(char));
            assert(regInfo != NULL);
            if(fread((void*)regInfo, sizeof(char), 2*1024, info) >0){
                m_strInfo = _T(regInfo);
                free(regInfo);
            }
        }
        fclose(info);
    }else{
        m_strInfo = _T("");
    }

    //�жϱ�������
    //�������ļ��ж�ȡ����������Ϣ

    char section[256];
    char key[256];
    char iniFilePath[256];

    memset((void *)section, sizeof(unsigned short), 256);
    memset((void *)key, sizeof(unsigned short), 256);
    memset((void *)iniFilePath, sizeof(unsigned short), 256);

    //"Register"
    section[0] = 'R';
    section[1] = 'e';
    section[2] = 'g';
    section[3] = 'i';
    section[4] = 's';
    section[5] = 't';
    section[6] = 'e';
    section[7] = 'r';
    section[8] = '\0';

    //"isNeedApply"
    key[0] = 'i';
    key[1] = 's';
    key[2] = 'N';
    key[3] = 'e';
    key[4] = 'e';
    key[5] = 'd';
    key[6] = 'A';
    key[7] = 'p';
    key[8] = 'p';
    key[9] = 'l';
    key[10] = 'y';
    key[11] = '\0';

    //  "configuration\\licence.ini";
    iniFilePath[0] = 'c';
    iniFilePath[1] = 'o';
    iniFilePath[2] = 'n';
    iniFilePath[3] = 'f';
    iniFilePath[4] = 'i';
    iniFilePath[5] = 'g';
    iniFilePath[6] = 'u';
    iniFilePath[7] = 'r';
    iniFilePath[8] = 'a';
    iniFilePath[9] = 't';
    iniFilePath[10] = 'i';
    iniFilePath[11] = 'o';
    iniFilePath[12] = 'n';
    iniFilePath[13] = '\\';
    iniFilePath[14] = 'l';
    iniFilePath[15] = 'i';
    iniFilePath[16] = 'c';
    iniFilePath[17] = 'e';
    iniFilePath[18] = 'n';
    iniFilePath[19] = 'c';
    iniFilePath[20] = 'e';
    iniFilePath[21] = '.';
    iniFilePath[22] = 'i';
    iniFilePath[23] = 'n';
    iniFilePath[24] = 'i';
    iniFilePath[25] = '\0';

    int isCopyrightProtect = GetPrivateProfileInt (section, key, 0, iniFilePath); 
    
    //��Ϊ��Ȩ����,���ֳ�����ע��������
    if( isCopyrightProtect ){       //����Ϊ���뱣������ֵΪ��Ȩ����
        // ����0~9����ĸ��ӳ���
        unsigned char mapTable[MAP_TABLE_LEN] = { 'Q', 'T', 'H', 'P', 'S', 'X', 'M', 'W', 'K', 'Y' };
        char *hdFeature = NULL;
        int len = 0;
        unsigned char hdMd5Digest[MD5CODE_LEN];
        unsigned char hdMd5DigestXor[MD5CODE_LEN/2];
        unsigned int i = 0;
        unsigned int k = 0;
        char buf[APPLY_CODE_LEN + 1];   // ���һ���ֽ����ڴ���ַ���������'\0'

        // ��ʼ��
        ZeroMemory(hdMd5Digest, sizeof(hdMd5Digest)); 
        ZeroMemory(hdMd5DigestXor, sizeof(hdMd5DigestXor));
        ZeroMemory(buf, sizeof(buf));
    
        if((hdFeature = (char *)malloc(sizeof(char) * 1024)) == NULL)
        {
#ifdef _DEBUG
            printf("Memory allocate faild!");
#endif
            exit(1);
        }

        // ���Ӳ�����кŵ��ַ���ֵ
        getHardDriverSerialID(hdFeature, &len);

        // ����Ӳ�����кŵ�MD5ժҪ
        calMD5Code(hdMd5Digest, (unsigned char*)hdFeature);

        free(hdFeature);
        hdFeature = NULL;
        // ��Ӳ�����кŵ�MD5ժҪ�������ֽ���ż���ֽڽ������
        for(i=0; i<MD5CODE_LEN/2; i++)
        {
            hdMd5DigestXor[i] = hdMd5Digest[2*i] ^ hdMd5Digest[2*i+1];
        }

        // ���ֽ�����ת��Ϊʮ�������ַ���
        for(i=0; i<MD5CODE_LEN/2; i++)
        {
            sprintf(buf+2*i, "%2X", hdMd5DigestXor[i]);
        }

        // ��buf�ַ��������ְ�ӳ���ӳ�䵽��ĸ,���пո���'0'����
        for(i=0; i<APPLY_CODE_LEN; i++){
            if(isspace(buf[i]))
                buf[i]=mapTable[0];
            else if(isdigit(buf[i]))
                buf[i] = mapTable[(int)buf[i] - 48];
        }

        //��ע��������д���ļ�apply.txt
        char applyFile[256];
        
        //"apply.txt"
        applyFile[0] = 'a';
        applyFile[1] = 'p';
        applyFile[2] = 'p';
        applyFile[3] = 'l';
        applyFile[4] = 'y';
        applyFile[5] = '.';
        applyFile[6] = 't';
        applyFile[7] = 'x';
        applyFile[8] = 't';
        applyFile[9] = '\0';

        mode[0] = 'w';
        mode[1] = '+';
        mode[2] = '\0';

        FILE *apply = fopen(applyFile, mode);
        if(apply != NULL)
        {
            fprintf(apply, "%s", buf);
            fclose(apply);
        }

        //��ע�������루16���ַ����ֳ��Ķ�,ÿ��4���ַ�
        for( i=0; i<4; i++)
        {
            memcpy((void *)(m_applyCode[i]), (const void*)(4*i + buf), (size_t)4);
            m_applyCode[i][4] = '\0';   //����ַ���β0
        }
        //�ֱ�ֵ���ĸ��༭���Ա����
        m_strApply1 = _T(m_applyCode[0]);
        m_strApply2 = _T(m_applyCode[1]);
        m_strApply3 = _T(m_applyCode[2]);
        m_strApply4 = _T(m_applyCode[3]);
    }
    
        
    m_ctrlOK.EnableWindow(false);   // �����кſ�Ϊ��ʱ����ע�ᰴť
    UpdateData(false);
    m_ctrlSerial1.SetFocus();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRegisterDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRegisterDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CRegisterDlg::OnOk() 
{
    // TODO: Add your control notification handler code here
        // TODO: Add your control notification handler code here
    //��ȡ�û���������к�
    UpdateData(true);
    CString serail = m_strSerial1+ m_strSerial2 + m_strSerial3 + m_strSerial4;
    serail.MakeUpper();

    //���û���������к�д��ָ���������ļ�configuration/win32.ini��
    char section[256];
    char key[256];
    char iniFilePath[256];

    int ret = 0;

    memset((void *)section, sizeof(unsigned short), 256);
    memset((void *)key, sizeof(unsigned short), 256);
    memset((void *)iniFilePath, sizeof(unsigned short), 256);

    //"Register"
    section[0] = 'R';
    section[1] = 'e';
    section[2] = 'g';
    section[3] = 'i';
    section[4] = 's';
    section[5] = 't';
    section[6] = 'e';
    section[7] = 'r';
    section[8] = '\0';

    //"SerialNo"
    key[0] = 'S';
    key[1] = 'e';
    key[2] = 'r';
    key[3] = 'i';
    key[4] = 'a';
    key[5] = 'l';
    key[6] = 'N';
    key[7] = 'o';
    key[8] = '\0';
    
    //  "configuration\\licence.ini";
    iniFilePath[0] = 'c';
    iniFilePath[1] = 'o';
    iniFilePath[2] = 'n';
    iniFilePath[3] = 'f';
    iniFilePath[4] = 'i';
    iniFilePath[5] = 'g';
    iniFilePath[6] = 'u';
    iniFilePath[7] = 'r';
    iniFilePath[8] = 'a';
    iniFilePath[9] = 't';
    iniFilePath[10] = 'i';
    iniFilePath[11] = 'o';
    iniFilePath[12] = 'n';
    iniFilePath[13] = '\\';
    iniFilePath[14] = 'l';
    iniFilePath[15] = 'i';
    iniFilePath[16] = 'c';
    iniFilePath[17] = 'e';
    iniFilePath[18] = 'n';
    iniFilePath[19] = 'c';
    iniFilePath[20] = 'e';
    iniFilePath[21] = '.';
    iniFilePath[22] = 'i';
    iniFilePath[23] = 'n';
    iniFilePath[24] = 'i';
    iniFilePath[25] = '\0';
    WritePrivateProfileString(section,key, serail, iniFilePath); 

    /* ��֤�Ƿ��ܽ��� */
    ret = ValidateLicence();
    switch (ret)
    {
    case -1:    /* ���ܽ��ܣ�Ҫ���������� */
        AfxMessageBox("���кŲ���ȷ�����������룡" , MB_OK);

        /* ���licence.ini�ļ��м�¼��ע���� */
        WritePrivateProfileString(section,key, "", iniFilePath);
        
        return;
        break;
                
    case -3:    /* ȱ�ٽ������ */
        AfxMessageBox("������������𻵣�" , MB_OK);
        return;
        break;

    case -2:
    case -4:    /* ��������汾��ƥ�䣬����Ȼ�������� */
        AfxMessageBox("������������汾��ƥ�䣬�޷���֤���кţ�" , MB_OK);
        break;
        
    default:            
        break;
    }

    //����LambdaIDE.exe������
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );


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
        MessageBox("����������ʧ�ܣ�" , NULL, MB_OK);
    }

    // Wait until child process exits.
 //   WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
 //   CloseHandle( pi.hProcess );
 //   CloseHandle( pi.hThread );

    
    CDialog::OnOK();
}

void CRegisterDlg::OnChangeEditSerial1() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    UpdateData(true);
    if(m_strSerial1.GetLength() == 6)   //��һ���ı�������6���ַ���,�Զ��л����ڶ����ı���
    {
        m_ctrlSerial2.SetFocus();
    }
    if( (m_strSerial1.GetLength() == 6) && (m_strSerial2.GetLength() == 6)
        && (m_strSerial3.GetLength() ==6) && (m_strSerial4.GetLength() == 6))
    {   //���к�ȫ�������ʹ��ע�ᰴť
        m_ctrlOK.EnableWindow(true);
        m_ctrlOK.SetFocus();
    }
    else
    {
        m_ctrlOK.EnableWindow(false);
    }
}

void CRegisterDlg::OnChangeEditSerial2() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    UpdateData(true);
    if(m_strSerial2.GetLength() == 6)   //�ڶ����ı�������6���ַ���,�Զ��л����������ı���
    {
        m_ctrlSerial3.SetFocus();
    }
    if( (m_strSerial1.GetLength() == 6) && (m_strSerial2.GetLength() == 6)
        && (m_strSerial3.GetLength() ==6) && (m_strSerial4.GetLength() == 6))
    {   //���к�ȫ�������ʹ��ע�ᰴť
        m_ctrlOK.EnableWindow(true);
        m_ctrlOK.SetFocus();
    }
    else
    {
        m_ctrlOK.EnableWindow(false);
    }
}

void CRegisterDlg::OnChangeEditSerial3() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    UpdateData(true);
    if(m_strSerial3.GetLength() == 6)   //�������ı�������6���ַ���,�Զ��л������ĸ��ı���
    {
        m_ctrlSerial4.SetFocus();
    }
    if( (m_strSerial1.GetLength() == 6) && (m_strSerial2.GetLength() == 6)
        && (m_strSerial3.GetLength() ==6) && (m_strSerial4.GetLength() == 6))
    {   //���к�ȫ�������ʹ��ע�ᰴť
        m_ctrlOK.EnableWindow(true);
        m_ctrlOK.SetFocus();
    }
    else
    {
        m_ctrlOK.EnableWindow(false);
    }
}

void CRegisterDlg::OnChangeEditSerial4() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    UpdateData(true);
    if(m_strSerial3.GetLength() == 6)   //���ĸ��ı�������6���ַ���,�Զ��л���ע�ᰴ��
    {
        m_ctrlOK.SetFocus();
    }
    if( (m_strSerial1.GetLength() == 6) && (m_strSerial2.GetLength() == 6)
        && (m_strSerial3.GetLength() ==6) && (m_strSerial4.GetLength() == 6))
    {   //���к�ȫ�������ʹ��ע�ᰴť
        m_ctrlOK.EnableWindow(true);
        m_ctrlOK.SetFocus();
    }
    else
    {
        m_ctrlOK.EnableWindow(false);
    }
}
