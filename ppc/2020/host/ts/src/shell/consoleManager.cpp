/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ������  ���������������޹�˾
*                    ������ļ���
*2004-12-6  ȥ��CTA����
*/

/**
* @file  main.cpp
* @brief  
*       ���ܣ�
*       <li>����̨������ʵ��</li>
*/
/************************ͷ�ļ�********************************/
#include "windows.h"
#include "consoleManager.h"
#include "coreCmdProcess.h"
#include "expressionProcess.h"
#include "shellMain.h"
#include <stdio.h>
#include <fstream>
#include <direct.h>
#include <TLHELP32.H>
#include "CheckSubArgv.h"

using namespace std;
/************************�궨��********************************/

#define ANSWER_SUCEESS_LEN strlen(ANSWER_SUCEESS)

#define CONTROL_COMMAND_LEN      1 

#define ANSWER_ERRHEAD_LEN strlen(ERRHEAD)

#define consoleInput(element, length)\
{\
    memset(element, 0, length);\
    gets_s(element, length);\
    if(strcmp(element, "") == 0)\
    {\
        return FAIL;\
    }\
 }


/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/
coreCmdProcess* pCoreCmdProcess = NULL;

expressionProcess* pExpressionProcess = NULL;

const char ANSWER_SUCEESS[] = "OK";

const char  SEARCH_SERVER_COUNT = 'S';

const char ERRHEAD[] = "E";

const char SERVER_EXTINFO_TA[] = "0*";

/* ��ts����������ID */
int tsLinkId = -1;

/* ��ǰĿ������ӵ�AID */
int curTargetAID = -1;

/*��ǰĿ����ĳ�ʱʱ��*/

int timeOut = -1;

/*��ǰ����Ŀ���������*/
char curTargetName [NAME_SIZE + 1] = {0};

static const basic_string <char>::size_type npos = -1;

extern sysCmdTableStruct sysCmdTable [] ; 

/************************�ⲿ����******************************/
extern void setWindownTitle(const char* titleName);
/************************ǰ������******************************/

/************************ʵ��*********************************/
consoleManager::consoleManager(int interpreter)
{
    runFlag = true;
    currentInterpreter = interpreter;
    ifInterpreterRun = false;
    m_bufSize = BUF_SIZE;
    m_totalExecuteCmdNum = 0;
    m_storeCmdHistoryNum = DEFAULT_CMD_HISTORY_NUM;
    cmdDeque.clear();
}

consoleManager::~consoleManager()
{
    runFlag = false;
    cmdDeque.clear();
}


/* ������ִ�г���*/
int consoleManager::launchProgram(char *fileName)
{
    WIN32_FIND_DATA d;
    char fullName[MAX_PATH_LEN];
	
    strncpy(fullName,cur_path,MAX_PATH_LEN);
    strcat(fullName, fileName);

    HANDLE hHandle = FindFirstFile(fullName,  &d);
    if ( hHandle == INVALID_HANDLE_VALUE )
    {
        return TS_FILE_NOT_EXIST;
    }
    else 
    {
        FindClose( hHandle );
        STARTUPINFO startInfo={0};
        startInfo.cb = sizeof(startInfo);
        PROCESS_INFORMATION ProcInfo;
        BOOL bCreatSucces = CreateProcess(fullName, fullName, NULL, NULL, false,  CREATE_NEW_CONSOLE,  NULL,  NULL,  &startInfo,  &ProcInfo);
        if (bCreatSucces)
        {
            return TS_SUCCESS;
        }
        return FAIL;
    }

}

int consoleManager::connectToTs(int TimeOut)
{
    int ret = 0;

    if( tsLinkId > 0 )
    {
        /* �������ӵ�����¹ر����� */
        TClient_close(tsLinkId);
        tsLinkId = -1;
    }
	if( TimeOut > 0)
	{
		timeOut = TimeOut;
	}else
	{
		timeOut = DEFAULT_WAIT_TIME;
	}
    /* ����TS*/
    tsLinkId = TClient_open(TS_DEFAULT_IP, TS_DEFAULT_PORT);
    if( tsLinkId < 0 )
    {
        if(TS_INVALID_IP == tsLinkId)
        {
            /* ��ЧIP��ַ */
            return FAIL;
        }

        if(SUCC != ret)
        {
            return FAIL;
        }
    }

    return tsLinkId;
}


/* ��ȡ��ǰ����Ŀ�����AID */
int consoleManager::getAllServerAid(int pAidArray[], int arrayLen)
{
    if(NULL == pAidArray)
    {
        return FAIL;
    }

    int saCount = 0;
    int sendDID = 122;
    int recvDid = 0;
    int saNum = 0;
    int ret = 0;
    US des_aid, des_said, src_said;
    char *pBuf = m_tmBuf;
    char *pStr = NULL;
    
    memset(m_tmBuf, 0, BUF_SIZE);
    
    //���dialog ID
    util::__store(sendDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    
    //����������
    memcpy(pBuf, &SEARCH_SERVER_COUNT, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    //��manager���������
    ret = TClient_putpkt(tsLinkId, 0, 0, 0, m_tmBuf, (UI)(pBuf-m_tmBuf));
    if(ret < 0)
    {
        return ret;
    }

    //�ȴ��ظ�
    ret = TClient_wait(tsLinkId, timeOut);
    if(TS_DATACOMING != ret) 
    {
        return ret;
    }

    //���ջظ�
    ret = TClient_getpkt(tsLinkId, &des_aid, &des_said, &src_said, m_tmBuf, m_bufSize);
    if(ret < 0)
    {
        return ret;
    }
    
    //��pbufָ�������ݿ�ʼ����, ������DID����
    pBuf = m_tmBuf;
    pBuf += DIDLEN + CONTROL_FLAG_LEN;
    if(strlen(pBuf) == 0)
    {
        return TS_RECV_DATA_ERROR;
    }

    //�ж�������,�Ǵ����������ȷ�ظ���
    if(strncmp(pBuf, ERRHEAD, ANSWER_ERRHEAD_LEN) == 0)
    {
        //��strBuf�������ĸE,�ҵ��ͱ�ʾ�Ǵ�����Ϣ
        return TS_UNKNOW_ERROR;
    }
    else if(strncmp(pBuf, ANSWER_SUCEESS, ANSWER_SUCEESS_LEN) == 0)
    {
        //����ĸOK��������Ծͱ�ʾ����ȷ�ظ���Ϣ
        pBuf += ANSWER_SUCEESS_LEN;
    }
    else
    {
        //������Exx��ͷ�Ĵ���ظ���Ϣ,Ҳ����OK��ͷ����ȷ�ظ���Ϣ
        return TS_UNKNOW_DATA;
    }
    
    //��ȷ�ظ�����Ϣ���ĸ�ʽΪ
    /*OK, SACount, SA1aid, ...*/
    //OK
    pStr = strstr(pBuf, CONTROL_FLAG);
    if (NULL == pStr)
    {
        return TS_UNKNOW_DATA_FORMAT;
    }
    pBuf += CONTROL_FLAG_LEN;

    //SACount
    pStr = strstr(pBuf, CONTROL_FLAG);
    if (NULL == pStr)
    {
        return TS_UNKNOW_DATA_FORMAT;
    }
    util::hex2int(&pBuf, &saCount);
    pBuf += CONTROL_FLAG_LEN;
    if(0 == saCount)
    {
        return SUCC;
    }
    
    //ÿ��SA��AID    
    while(strstr(pBuf, CONTROL_FLAG) 
                && (saNum < saCount) 
                && (saNum < (arrayLen - 1)))
    {
        util::hex2int(&pBuf, &pAidArray[saNum++]);
        pBuf += CONTROL_FLAG_LEN;
    }

    return saNum;
    
}



/* ��ʾ�û�����Ŀ������ӵ�������Ϣ */
int consoleManager::inputSerCfg(ServerCfg *pSerCfg)
{

    char saType[10] = "\0";
    char temp[MAX_PATH] = "\0";
    char *pStr = NULL;
    char *pDer = NULL;
    bool tempBool = false;
    int ret = 0;
    comConfig *pComCfg;

    if(NULL == pSerCfg)
    {
        return FAIL;
    }

    printf("ֱ�ӻس��˳�����\n\n");
    
    //SAType:Ĭ������ΪTA��ʽ
    strncpy_s(pSerCfg->extInfo, sizeof(pSerCfg->extInfo), SERVER_EXTINFO_TA, _TRUNCATE);
 
    //name
    while(1)
    {
        printf("Ŀ�������: ");
        gets_s(pSerCfg->name, sizeof(pSerCfg->name));
        if(strcmp(pSerCfg->name, "") == 0)
        {
            return FAIL;
        }

        //��ѶĿ����Ƿ����
        ret = TClient_getServerID(tsLinkId, pSerCfg->name);               
        if( ret >= 0 )
        {
            printf("����: Ŀ��������ظ�\n");
            continue;          
        }

        break;
    }    

    //logflag
    while(1)
    {
        printf("�Ƿ��¼��־,(1:��, 0:��): ");
        consoleInput(temp, sizeof(temp));
        pSerCfg->isLog = atoi(temp);
        if((1 == pSerCfg->isLog) || (0 == pSerCfg->isLog))
        {
            break;
        }
    }

    //channelcount:Ĭ������Ϊ1
    pSerCfg->channelCount = 1;
    
    pComCfg = pSerCfg->pComConfig;

    //way
    while(1)
    {
        printf("ͨ������, (1:UDP, 2:UART): ");
        consoleInput(temp, sizeof(temp));
        pStr = temp;
        if(util::hex2int(&pStr, &ret))
        {
            switch(ret)
            {    
            case UDP:
                pComCfg->way = UDP;
                pComCfg->protocolType = UDP;
                break;
                
            case UART:
                pComCfg->way = UART;
                pComCfg->protocolType = UART;
                break;
  
            default:
                continue;
            }
            break;
        }
        
    }

    //comCfg
    switch(pComCfg->way)
    {
    case UDP:
        //IP
        printf("ͨ��IP��ַ: ");
        consoleInput(pComCfg->NET.ip, sizeof(pComCfg->NET.ip));
        
        //port
        printf("ͨ���˿�: ");
        consoleInput(temp, sizeof(temp));
        pComCfg->NET.port = atoi(temp);
        
        //timeout
        printf("ͨ����ʱʱ��(ms): ");
        consoleInput(temp, sizeof(temp));
        pComCfg->NET.timeout = atoi(temp);

        //retry
        printf("ͨ�����Դ���: ");
        consoleInput(temp, sizeof(temp));
        pComCfg->NET.retry = atoi(temp);

        break;
        
    case UART:
        //��ȡ�������ں�
        ret = Tclient_queryHostSerialPort(tsLinkId, temp, sizeof(temp));
        tempBool = true;
        while(tempBool)
        {                
            printf("ͨ������(");
            pStr = temp;
            for(int i = 0; i < 2; ++i)
            {
                if(pDer = strstr(pStr, CONTROL_FLAG))
                {
                    *pDer = 0;
                    printf("%s, ", pStr);
                    *pDer = ',';
                    pStr = ++pDer;
                }                    
            }
            if(strstr(pStr, CONTROL_FLAG))
            {
                printf("...");
            }
            printf("): ");

            //���봮��
            consoleInput(m_tmBuf, m_bufSize);
            memcpy(pComCfg->UART.channel, 
                        m_tmBuf,
                        sizeof(pComCfg->UART.channel));
            pStr = pComCfg->UART.channel;
            while(*(pStr))
            {
                *(pStr++) = toupper(*pStr);
            }
            
            pDer = temp;
            while(pDer && (pStr = strstr(pDer, CONTROL_FLAG)))
            {
                if(strncmp(pDer, pComCfg->UART.channel, 
                        strlen(pComCfg->UART.channel)) == 0)
                {
                    tempBool = false;
                    break;
                }
                pDer = pStr + CONTROL_COMMAND_LEN;
            }

            if(tempBool)
            {
                printf("������֧�ִ���: ");                
                pStr = temp;
                while(pDer = strstr(pStr, CONTROL_FLAG))
                {
                    *pDer = 0;
                    printf("%s, ", pStr);
                    *pDer = ',';
                    pStr = ++pDer;
                }
                printf("\n");  
            }
            
        }

        //baud
        printf("ͨ��������: ");
        consoleInput(temp, sizeof(temp));
        pComCfg->UART.baud = atoi(temp);

        //timeout
        printf("ͨ����ʱʱ��(ms): ");
        consoleInput(temp, sizeof(temp));
        pComCfg->UART.timeout = atoi(temp);

        //retry
        printf("ͨ�����Դ���: ");
        consoleInput(temp, sizeof(temp));
        pComCfg->UART.retry = atoi(temp);
        
        break;
        
    default:
        printf("δ֪ͨ������\n");
        return FAIL;
        break;
    } 
    
    return SUCC;
}

 
 


/* ��ʼ�������� */
int consoleManager::installInterpreter()
{
    int ret = 0;
    
    if(runFlag)
    {
        /* ����coreos��������� */
        pCoreCmdProcess = new coreCmdProcess();
        if( NULL == pCoreCmdProcess )
        {
            return FAIL;
        }

        /* �������ʽ������ */
        pExpressionProcess = new expressionProcess();
        if( NULL == pExpressionProcess )
        {
            return FAIL;
        }

    }


    return SUCC;
}


void consoleManager::run(char *targetName,int Timeout)
{
    int ret = 0;
    char programName[20]= {0};

    /* �����Ļ */
    system("cls");

    /* ����ts.exe*/
    strcpy(programName,"ts.exe");
    ret = findProcess(programName);
    if(FAIL == ret)
    {
        /*tsû������*/
        cout << "����TS ......" ;
        ret = launchProgram("ts.exe");
        if(TS_SUCCESS != ret)
        {
            /*����tsʧ��*/
            cout << "ʧ��!" << endl;
            getchar();
            return ;
        }
        else 
        {
            Sleep(2000);
            cout << "�ɹ�!" << endl;
        }
    }

    cout << "����TS ......" ;
    ret = connectToTs(Timeout);
    if(ret == FAIL)
    {
        /*����tsʧ��*/
        cout << "ʧ��!" << endl;
        getchar();
        return ;
    }
    else
    {
        cout << "�ɹ�!" << endl;
    }

    /*����tsServer*/
    strcpy(programName,"tsServer.exe");
    ret = findProcess(programName);
    if(FAIL == ret)
    {
        /*tsServerû������*/
        cout << "����tsServer ......" ;
        ret = launchProgram("tsServer.exe");
        if(TS_SUCCESS != ret)
        {
            /*����tsServerʧ��*/
            cout << "ʧ��!" << endl;
            getchar();
            return ;
        }
        else 
        {
            cout << "�ɹ�!" << endl;
        }
    }    

    /* ��ʼ�������� */
    cout << "��ʼ��������......" ;
    ret = installInterpreter();
    if(ret != SUCC )
    {
        cout << "ʧ��!" << endl;
        getchar();
        return ;
    }
    else
    {
        cout << "�ɹ�!" << endl;
    }

    /*����Ѿ���-nameָ����Ŀ���,�����Ŀ���*/
    if( targetName != NULL )
    {
		if( m_activeTargetConnect(targetName,true) == FAIL){
			setWindownTitle("");
		}
	}else{
		setWindownTitle("");
	}

    /*��ӡͼ��*/
    printWelcome();
	
	/* ͬ�����ű� */
	syncSymbolFiles();

	while(runFlag)
    {
        /* ѭ���ӿ���̨��ȡ�������� */
        ret = msgGet();
        if(SUCC != ret)
        {
            continue;
        }

        /* ������ȡ������������ */
        processCmd();
    }
    
    return;
}



/* ��ȡ����̨���� */
int consoleManager::msgGet()
{
    string tempstring;
    string::size_type noblankpos = 0;


    /* �жϵ�ǰ���������������ʾ�� */
    if(C_INTERPRETER ==  currentInterpreter )
    {
        cout << EXPRESSION_PROMPT;
    }
    else if( COREOS_INTERPRETER == currentInterpreter )
    {
        cout << COREOS_PROMPT;
    }
    
    /* �������ж���һ������ */
    /*�����ctrl+c���ٴζ�ȡ*/
    do
    {
        cin.clear();
    }
    while(getline(cin, tempstring) == 0);

    if(  "" == tempstring )
    {
        /*��ֱ������س�ʱֱ�ӷ���*/
        return FAIL;
    }

    /*�ҵ���һ�����ǿո���ַ�*/
    noblankpos = tempstring.find_first_not_of(BLANK_STR);
    if( (npos  == noblankpos) )
    {
        /*�����ȫ�ǿո�*/
        return FAIL;
    }

    /*ɾ����һ���ǿո��ַ�֮ǰ�Ŀո�*/
    tempstring.erase(0, noblankpos);

    /* ����ȡ����������������� */
    busyCmdListMtx.lock();

    /* ���������и���������¼��Ŀ,��ɾ��ĩβ����Ԫ�� */
    if( cmdDeque.size() >= m_storeCmdHistoryNum)
    {
        cmdDeque.pop_back();
    }

    cmdDeque.push_front(tempstring);
    m_totalExecuteCmdNum++;
    busyCmdListMtx.unlock();

    return SUCC;
}


/* ��������� */
sysCmdLableEnum consoleManager::getCmdLable(string& cmd)
{
    int i = 0;
    int cmpret = 0;
    string::size_type blankpos = 0;
	//���˵������ж���Ŀո���Ʊ��
	skipInvalidChFormCmd(cmd);
	//�����ǰ�����ַ�����Ϊ��
	if(cmd.size() > 0 ){
		string backupCmd = cmd; //�Ե�ǰ������cmd������б���
		blankpos = backupCmd.find_first_of(' ');
		if(npos != blankpos )
		{
			backupCmd[blankpos] = '\0';
		}

		for(i = 0; sysCmdTable[i].cmdStr; i++)
		{
			cmpret = strcmp(backupCmd.c_str(), sysCmdTable[i].cmdStr);
			
			if(0 == cmpret)
			{
				//�Ե�ǰ����������м��
				if(sysCmdTable[i].checkSubArgv(cmd)){
					return sysCmdTable[i].cmdLable;
				}else{
					return SYS_ERROR_ARGV;
					break;
				}
			}
		}
	}

    return SEND_TO_INTERPRETER_LABLE;
}



int consoleManager::processCmd()
{
    string cmdtemp;
    sysCmdLableEnum cmdlable;

    if (!cmdDeque.empty())
    {
        busyCmdListMtx.lock();
        cmdtemp = cmdDeque.front();
        busyCmdListMtx.unlock();

        /* ���������ֲ�������� */
        cmdlable = getCmdLable(cmdtemp);

        switch(cmdlable)
        {
            /* ����Ŀ������� */
        case ACTIVE_TARGET_CONNECT:
            m_activeTargetConnect( &(cmdtemp[4]) );
            break;
 
            /* ��ӡϵͳ�е�Ŀ������� */
        case LIST_TARGET_CONNECT:
            m_listTargetInfo();
            break;

            /* �л���C������ */    
        case SWITCH_TO_EXP_LABLE:
            m_switchToExpInterpreter();
            break;

            /* �л���coreos������ */
        case SWITCH_TO_COREOS_LABLE:
            m_switchToCoreOSInterpreter();
            break;

            /* �˳�shell ,���ر�TS */
        case QUIT_LABLE:
            m_quit();
            break;

            /* ������Ϣ */
        case HELP_LABLE:
            m_getHelp();
            break;

        case CLEAR_SCREEN_LABLE:
            m_clearScreen();
            break;

        case PRINT_HISTROY:
            m_printHistory(cmdtemp);
            break;

        case ADD_SYMBOL:
            m_addSymbol(&(cmdtemp[10]));
            break;

        case SUB_SYMBOL:
            m_subSymbol(&(cmdtemp[10]));
            break;
		/*��ǰ�����Ѿ����ҵ������ǲ�����������,ֱ���˳���ǰ����ִ��*/
		case SYS_ERROR_ARGV:
			break;
        /* Ĭ������½���Ϣ���͵���ǰ��������� */
        default:
			if(cmdtemp.size() > 0){
            m_sendCmdToInterpreter(cmdtemp);
			}
            break;
        }
    }

    return SUCC;
}


/*  ������������� */
int consoleManager::m_sendCmdToInterpreter(string cmd)
{
    extern int tsServerAid;
    char programName[20] = {0};

    /* ���������Ŀ�������,�򲻽���������͵�����ִ���� */
    if(curTargetAID < 0)
    {
        printf("û�а�Ŀ���\n");
        return FAIL;
    }

    /* �жϵ�ǰ�����������������ͬ�Ľ����� */
    if( C_INTERPRETER ==  currentInterpreter )
    {
        if(syncSymbolFiles() != SUCC )
        {
            return FAIL;
        }

        /*tsServerû������*/
        strcpy(programName,"tsServer.exe");
        if( SUCC != findProcess(programName))
        {
            /*����ʧ��*/
            if( TS_SUCCESS != launchProgram("tsServer.exe"))
            {
                cout << "tsServer ����!" << endl;
                return FAIL;
            }
            else
            {
                Sleep(1000);
                if(SUCC != pExpressionProcess->getTsServerAidFromTsMap())
                {
                    cout << "tsServer ����!" << endl;
                    return FAIL;
                }
            }
        }

        /*tsServer��shell����ʱ�Ѿ�������,����û�л������ID*/
        if(tsServerAid < 0)
        {
            if(SUCC != pExpressionProcess->getTsServerAidFromTsMap())
            {
                cout << "tsServer ����!" << endl;
                return FAIL;
            }
        }

        pExpressionProcess->insertData(cmd);
        pExpressionProcess->processData();

    }
    else if(COREOS_INTERPRETER == currentInterpreter )
    {
        pCoreCmdProcess->insertData(cmd);
        pCoreCmdProcess->processData();
    }

    return SUCC;

}



int consoleManager::uninstallInterpreter()
{
    delete pCoreCmdProcess;
    pCoreCmdProcess = NULL;

    delete pExpressionProcess;
    pExpressionProcess = NULL;

    return SUCC;
}

void consoleManager::stop()
{

    runFlag = false;

    if( tsLinkId != -1 )
    {
        /* �ر���ts������ */
        TClient_close(tsLinkId);
        tsLinkId = -1;
    }
    
    /*�ͷŽ�����*/
    uninstallInterpreter();

    return;
}

/**
 * @brief: 
 *    ��ȡĿ¼�ļ��б�
 * @param dirPath: Ŀ¼·��
 * @return��
 *    ��
 */
void consoleManager::DirectoryFileList(string dirPath, vector<string> &fileList)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	string DirSpec("");
	string DirName("");
	string FileName("");

	DirSpec = dirPath + "\\*";

	hFind = FindFirstFile(DirSpec.c_str(), &FindFileData);
	if ((hFind != INVALID_HANDLE_VALUE) && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		while(FindNextFile(hFind, &FindFileData) != 0) 
		{
			/* �ų���Ŀ¼�͸�Ŀ¼ */
			if((!string(".").compare(FindFileData.cFileName)) || (!string("..").compare(FindFileData.cFileName)))
			{
				continue;
			}
			
			/* �ݹ�Ŀ¼ */
			if ((hFind != INVALID_HANDLE_VALUE) && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				DirName.clear();
				DirName = dirPath +  "\\" + FindFileData.cFileName;
				DirectoryFileList(DirName,fileList);
			}
			else
			{
				FileName = dirPath + "\\" + FindFileData.cFileName;
				fileList.push_back(FileName);
			}
		}
	}

	/* �رվ�� */
	FindClose(hFind);
}

/*���浱ǰ·��*/
void consoleManager::savePath(char * path)
{
    char *temp;
    strncpy( cur_path ,path, MAX_PATH_LEN);
    
    temp = strchr(cur_path, '/');
    while ( NULL != temp )
    {
        *temp = '\\';
        temp = strchr(cur_path, '/');
    }
    
    temp = strrchr(cur_path,'\\');
    if(temp == NULL)
    {
        return ;
    }
    *(++temp) = 0;
}

int consoleManager::syncSymbolFiles()
{
    /*���ݷ��ű��ļ���¼�жϷ��ű��Ƿ����*/
    int length;
    FILE *fp_txt,*fp_elf;
    char *elfName;
    char fullSymbolFilesFolder[MAX_PATH_LEN] = {0};
    char fullSymbolListName[MAX_PATH_LEN] = {0};
    char fullSymbolFileName[MAX_PATH_LEN] = {0};
    char readSymbolListBuff[MAX_PATH_LEN] = {0};
    struct _stat ssbuf;
    struct _stat ssbuf1;
	vector<string> fileList;
	vector<string>::iterator iter; 

    strncpy(fullSymbolFilesFolder, cur_path, MAX_PATH_LEN); //fullSymbolFilesFolder="D:\\LambdaAE_0.5.3.6\\host\\pub\\"
    strcat(fullSymbolFilesFolder, "symbolFiles\\");     //fullSymbolFilesFolder="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\"
    strcat(fullSymbolFilesFolder, curTargetName);   //fullSymbolFilesFolder="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name"
    
	/* ��ȡĿ¼�ļ��б� */
	DirectoryFileList(fullSymbolFilesFolder, fileList);

	strncpy(fullSymbolListName, fullSymbolFilesFolder, MAX_PATH_LEN);   //fullSymbolListName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name"
    strcat(fullSymbolFilesFolder, "\\");    //fullSymbolFilesFolder="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\"
    strcat(fullSymbolListName, ".txt");           //fullSymbolListName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name.txt"
	
    fp_txt = fopen(fullSymbolListName, "a+");
    if(fp_txt == NULL)
    {
        printf("�򿪷��ű��б��ļ�ʧ��\n");
        printErrorInfo();
        return FAIL;
    }

    /*��ȡһ������*/
    while(fgets(readSymbolListBuff,MAX_PATH_LEN, fp_txt) != NULL)
    {
        /*���ļ��е���ʾ��Ϣ����*/
        if('#' == readSymbolListBuff[0])
        {
            continue;
        }
		/*������ջ��У�Ҳ������ǰ���пո��Ʊ��*/
        if(' ' == readSymbolListBuff[0] || '\n' == readSymbolListBuff[0] || '\r' == readSymbolListBuff[0] ||  '\t' == readSymbolListBuff[0])
        {
            printf("���ű��б��ļ�����Ϣ��ʽ����\n");
            fclose(fp_txt);
            return FAIL;
        }
        /*�����Ļ��з�ɾ��*/
        length = strlen(readSymbolListBuff);
        if('\n' == readSymbolListBuff[length - 1])
        {
            readSymbolListBuff[length - 1] = 0;
        }
    
        /*�õ��ļ���*/
        elfName = strrchr(readSymbolListBuff, '\\');
        if( elfName == NULL )
        {
            printf("���ű��б��ļ�������Ϣ����Ϣ����\n");
            fclose(fp_txt);
            return FAIL;
        }

        elfName++;
        strcpy(fullSymbolFileName,fullSymbolFilesFolder);   //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\"
        strcat(fullSymbolFileName, elfName);        //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\os.elf"
		
	 	/* �޳�����Ҫɾ���ķ����ļ� */
		for(iter = fileList.begin(); iter != fileList.end(); iter++)
		{
			if(!(*iter).compare(fullSymbolFileName))
			{
				fileList.erase(iter);
				break;
			}
		}

        /*���û�з��ű������*/
        if((fp_elf = fopen(fullSymbolFileName,"rb") ) == NULL)
        {
            if(CopyFileA(readSymbolListBuff,fullSymbolFileName,false) == 0)
            {
                printf("���ű��б��ļ��м�¼����Ϣ����\n");
                printErrorInfo();
                fclose(fp_txt);
                return FAIL;
            }
        }
        else
        {
            fclose(fp_elf);
        }

        /*���ݷ��ű��ļ����޸�ʱ��ʹ�С,�ж��Ƿ���Ҫ���¿������ű��ļ�*/
        _stat(fullSymbolFileName, &ssbuf);
        _stat(readSymbolListBuff, &ssbuf1);

        if((ssbuf.st_size != ssbuf1.st_size) || (ssbuf.st_mtime != ssbuf1.st_mtime))
        {
            if(CopyFileA(readSymbolListBuff,fullSymbolFileName,false) == 0)
            {
                printf("���ű��б��ļ��м�¼����Ϣ����\n");
                printErrorInfo();
                fclose(fp_txt);
                return FAIL;
            }
        }
    }

	/* ɾ�����ű��ļ�δ��¼�ķ��ű� */
	for(iter = fileList.begin(); iter != fileList.end(); iter++)
	{
		DeleteFile((*iter).c_str());
	}

    fclose(fp_txt);
    return SUCC;
}


/*��ӡ������Ϣ*/
void consoleManager::printErrorInfo()
{
    WCHAR szBuf[ERROR_BUF_SIZE]; 
    char muBuf[ERROR_BUF_SIZE] = "\0";
    DWORD error = 0;
    
    memset(szBuf, 0, _countof(szBuf));
    memset(muBuf, 0, _countof(muBuf));

    error = GetLastError();
     
#ifdef _UNICODE
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        error,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPTSTR)szBuf,
                        sizeof(szBuf), 
                        NULL );

        WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,
                            szBuf, _countof(szBuf), muBuf, sizeof(muBuf),
                            NULL,NULL);
#else
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)muBuf,
                sizeof(muBuf), 
                NULL );
#endif
        printf("%s\n", muBuf);

}


int consoleManager::findProcess(char *ProcessName)
{
    int ret = FAIL;
    
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32) ;
    
    // �������վ��
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    // ������ϵͳ�е�һ�����̵���Ϣ
    Process32First(hSnapshot, &pe);
    
    // �����ϵͳ�е����н��̽���ö��
    do
    {
        if(strlen(pe.szExeFile)>0)
        {
            /*�����ִ�Сд*/
            if (strnicmp((char*)pe.szExeFile,strlwr(ProcessName),strlen(pe.szExeFile)) == 0)
            {
                /*�ҵ�������SUCC*/
                ret = SUCC;
                break;
            }
        }  
    }while (Process32Next(hSnapshot, &pe));
    
    // �رտ��վ��
    CloseHandle(hSnapshot); 

    return ret;
}

void consoleManager::printWelcome()
{
	cout << "\n" << "\
                      /---                         |                     \n\
                    //////---                      |                     \n\
                  //////////---                    |                     \n\
                //////////////---                  |                     \n\
              //////////////////---                |                     \n\
             /////////////////////---              |                     \n\
          /--- /////////////////////---            |  CoreTek  LambdaAE  \n\
        /////--- /////////////////////---          |                     \n\
      /////////--- /////////////////////---        |                     \n\
    /////////////--- /////////////////////---      |  Development System \n\
  /////////////////--- /////////////////////---    |                     \n\
   //////////////////--- /////////////////////---  |                     \n\
     /////////////--- /--- /////////////////---    |  Host  Based  Shell \n\
       /////////--- /////--- /////////////---      |                     \n\
         /////--  /////////--- /////////---        |                     \n\
           /--  /////////////--- /////---          |  Version  2.2       \n\
              /////////////////--- /--             |                     \n\
               //////////////////---               |                     \n\
                 //////////////---                 |                     \n\
                   //////////---                   |                     \n\
                     //////---                     |                     \n\
                       //---                       |" << "\n" << endl;
	
	
	return;
}


