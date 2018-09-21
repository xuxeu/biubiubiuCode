/*
* 更改历史：
* 2004-12-6  董洪涛  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 唐兴培  北京科银技术有限公司
*                    评审该文件。
*2004-12-6  去除CTA命令
*/

/**
* @file  main.cpp
* @brief  
*       功能：
*       <li>控制台管理类实现</li>
*/
/************************头文件********************************/
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
/************************宏定义********************************/

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


/************************类型定义******************************/

/************************全局变量******************************/
coreCmdProcess* pCoreCmdProcess = NULL;

expressionProcess* pExpressionProcess = NULL;

const char ANSWER_SUCEESS[] = "OK";

const char  SEARCH_SERVER_COUNT = 'S';

const char ERRHEAD[] = "E";

const char SERVER_EXTINFO_TA[] = "0*";

/* 与ts建立的连接ID */
int tsLinkId = -1;

/* 当前目标机连接的AID */
int curTargetAID = -1;

/*当前目标机的超时时间*/

int timeOut = -1;

/*当前连接目标机的名字*/
char curTargetName [NAME_SIZE + 1] = {0};

static const basic_string <char>::size_type npos = -1;

extern sysCmdTableStruct sysCmdTable [] ; 

/************************外部声明******************************/
extern void setWindownTitle(const char* titleName);
/************************前向声明******************************/

/************************实现*********************************/
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


/* 启动可执行程序*/
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
        /* 存在连接的情况下关闭连接 */
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
    /* 连接TS*/
    tsLinkId = TClient_open(TS_DEFAULT_IP, TS_DEFAULT_PORT);
    if( tsLinkId < 0 )
    {
        if(TS_INVALID_IP == tsLinkId)
        {
            /* 无效IP地址 */
            return FAIL;
        }

        if(SUCC != ret)
        {
            return FAIL;
        }
    }

    return tsLinkId;
}


/* 获取当前所有目标机的AID */
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
    
    //填充dialog ID
    util::__store(sendDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    
    //填充控制命令
    memcpy(pBuf, &SEARCH_SERVER_COUNT, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    //向manager发送命令包
    ret = TClient_putpkt(tsLinkId, 0, 0, 0, m_tmBuf, (UI)(pBuf-m_tmBuf));
    if(ret < 0)
    {
        return ret;
    }

    //等待回复
    ret = TClient_wait(tsLinkId, timeOut);
    if(TS_DATACOMING != ret) 
    {
        return ret;
    }

    //接收回复
    ret = TClient_getpkt(tsLinkId, &des_aid, &des_said, &src_said, m_tmBuf, m_bufSize);
    if(ret < 0)
    {
        return ret;
    }
    
    //将pbuf指定包内容开始部分, 即跳过DID部分
    pBuf = m_tmBuf;
    pBuf += DIDLEN + CONTROL_FLAG_LEN;
    if(strlen(pBuf) == 0)
    {
        return TS_RECV_DATA_ERROR;
    }

    //判定包类型,是错误包还是正确回复包
    if(strncmp(pBuf, ERRHEAD, ANSWER_ERRHEAD_LEN) == 0)
    {
        //在strBuf里查找字母E,找到就表示是错误消息
        return TS_UNKNOW_ERROR;
    }
    else if(strncmp(pBuf, ANSWER_SUCEESS, ANSWER_SUCEESS_LEN) == 0)
    {
        //找字母OK，如果找以就表示是正确回复消息
        pBuf += ANSWER_SUCEESS_LEN;
    }
    else
    {
        //即不是Exx开头的错误回复消息,也不是OK开头的正确回复消息
        return TS_UNKNOW_DATA;
    }
    
    //正确回复的消息包的格式为
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
    
    //每个SA的AID    
    while(strstr(pBuf, CONTROL_FLAG) 
                && (saNum < saCount) 
                && (saNum < (arrayLen - 1)))
    {
        util::hex2int(&pBuf, &pAidArray[saNum++]);
        pBuf += CONTROL_FLAG_LEN;
    }

    return saNum;
    
}



/* 提示用户输入目标机连接的输入信息 */
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

    printf("直接回车退出创建\n\n");
    
    //SAType:默认设置为TA方式
    strncpy_s(pSerCfg->extInfo, sizeof(pSerCfg->extInfo), SERVER_EXTINFO_TA, _TRUNCATE);
 
    //name
    while(1)
    {
        printf("目标机名字: ");
        gets_s(pSerCfg->name, sizeof(pSerCfg->name));
        if(strcmp(pSerCfg->name, "") == 0)
        {
            return FAIL;
        }

        //查讯目标机是否存在
        ret = TClient_getServerID(tsLinkId, pSerCfg->name);               
        if( ret >= 0 )
        {
            printf("警告: 目标机名字重复\n");
            continue;          
        }

        break;
    }    

    //logflag
    while(1)
    {
        printf("是否记录日志,(1:是, 0:否): ");
        consoleInput(temp, sizeof(temp));
        pSerCfg->isLog = atoi(temp);
        if((1 == pSerCfg->isLog) || (0 == pSerCfg->isLog))
        {
            break;
        }
    }

    //channelcount:默认设置为1
    pSerCfg->channelCount = 1;
    
    pComCfg = pSerCfg->pComConfig;

    //way
    while(1)
    {
        printf("通道类型, (1:UDP, 2:UART): ");
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
        printf("通道IP地址: ");
        consoleInput(pComCfg->NET.ip, sizeof(pComCfg->NET.ip));
        
        //port
        printf("通道端口: ");
        consoleInput(temp, sizeof(temp));
        pComCfg->NET.port = atoi(temp);
        
        //timeout
        printf("通道超时时间(ms): ");
        consoleInput(temp, sizeof(temp));
        pComCfg->NET.timeout = atoi(temp);

        //retry
        printf("通道重试次数: ");
        consoleInput(temp, sizeof(temp));
        pComCfg->NET.retry = atoi(temp);

        break;
        
    case UART:
        //获取主机串口号
        ret = Tclient_queryHostSerialPort(tsLinkId, temp, sizeof(temp));
        tempBool = true;
        while(tempBool)
        {                
            printf("通道串口(");
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

            //输入串口
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
                printf("主机所支持串口: ");                
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
        printf("通道波特率: ");
        consoleInput(temp, sizeof(temp));
        pComCfg->UART.baud = atoi(temp);

        //timeout
        printf("通道超时时间(ms): ");
        consoleInput(temp, sizeof(temp));
        pComCfg->UART.timeout = atoi(temp);

        //retry
        printf("通道重试次数: ");
        consoleInput(temp, sizeof(temp));
        pComCfg->UART.retry = atoi(temp);
        
        break;
        
    default:
        printf("未知通道类型\n");
        return FAIL;
        break;
    } 
    
    return SUCC;
}

 
 


/* 初始化解析器 */
int consoleManager::installInterpreter()
{
    int ret = 0;
    
    if(runFlag)
    {
        /* 创建coreos命令解析器 */
        pCoreCmdProcess = new coreCmdProcess();
        if( NULL == pCoreCmdProcess )
        {
            return FAIL;
        }

        /* 创建表达式解析器 */
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

    /* 清除屏幕 */
    system("cls");

    /* 查找ts.exe*/
    strcpy(programName,"ts.exe");
    ret = findProcess(programName);
    if(FAIL == ret)
    {
        /*ts没有启动*/
        cout << "启动TS ......" ;
        ret = launchProgram("ts.exe");
        if(TS_SUCCESS != ret)
        {
            /*启动ts失败*/
            cout << "失败!" << endl;
            getchar();
            return ;
        }
        else 
        {
            Sleep(2000);
            cout << "成功!" << endl;
        }
    }

    cout << "连接TS ......" ;
    ret = connectToTs(Timeout);
    if(ret == FAIL)
    {
        /*连接ts失败*/
        cout << "失败!" << endl;
        getchar();
        return ;
    }
    else
    {
        cout << "成功!" << endl;
    }

    /*查找tsServer*/
    strcpy(programName,"tsServer.exe");
    ret = findProcess(programName);
    if(FAIL == ret)
    {
        /*tsServer没有启动*/
        cout << "启动tsServer ......" ;
        ret = launchProgram("tsServer.exe");
        if(TS_SUCCESS != ret)
        {
            /*启动tsServer失败*/
            cout << "失败!" << endl;
            getchar();
            return ;
        }
        else 
        {
            cout << "成功!" << endl;
        }
    }    

    /* 初始化解析器 */
    cout << "初始化解析器......" ;
    ret = installInterpreter();
    if(ret != SUCC )
    {
        cout << "失败!" << endl;
        getchar();
        return ;
    }
    else
    {
        cout << "成功!" << endl;
    }

    /*如果已经由-name指定了目标机,激活该目标机*/
    if( targetName != NULL )
    {
		if( m_activeTargetConnect(targetName,true) == FAIL){
			setWindownTitle("");
		}
	}else{
		setWindownTitle("");
	}

    /*打印图标*/
    printWelcome();
	
	/* 同步符号表 */
	syncSymbolFiles();

	while(runFlag)
    {
        /* 循环从控制台获取输入命令 */
        ret = msgGet();
        if(SUCC != ret)
        {
            continue;
        }

        /* 解析获取到的输入命令 */
        processCmd();
    }
    
    return;
}



/* 获取控制台输入 */
int consoleManager::msgGet()
{
    string tempstring;
    string::size_type noblankpos = 0;


    /* 判断当前解析器，并输出提示符 */
    if(C_INTERPRETER ==  currentInterpreter )
    {
        cout << EXPRESSION_PROMPT;
    }
    else if( COREOS_INTERPRETER == currentInterpreter )
    {
        cout << COREOS_PROMPT;
    }
    
    /* 从命令行读入一行命令 */
    /*如果是ctrl+c就再次读取*/
    do
    {
        cin.clear();
    }
    while(getline(cin, tempstring) == 0);

    if(  "" == tempstring )
    {
        /*当直接输入回车时直接返回*/
        return FAIL;
    }

    /*找到第一个不是空格的字符*/
    noblankpos = tempstring.find_first_not_of(BLANK_STR);
    if( (npos  == noblankpos) )
    {
        /*输入的全是空格*/
        return FAIL;
    }

    /*删除第一个非空格字符之前的空格*/
    tempstring.erase(0, noblankpos);

    /* 将获取到的命令放入容器中 */
    busyCmdListMtx.lock();

    /* 如果命令队列个数超过记录数目,则删除末尾队列元素 */
    if( cmdDeque.size() >= m_storeCmdHistoryNum)
    {
        cmdDeque.pop_back();
    }

    cmdDeque.push_front(tempstring);
    m_totalExecuteCmdNum++;
    busyCmdListMtx.unlock();

    return SUCC;
}


/* 查找命令表 */
sysCmdLableEnum consoleManager::getCmdLable(string& cmd)
{
    int i = 0;
    int cmpret = 0;
    string::size_type blankpos = 0;
	//过滤掉命令中多余的空格和制表符
	skipInvalidChFormCmd(cmd);
	//如果当前处理字符串不为空
	if(cmd.size() > 0 ){
		string backupCmd = cmd; //对当前处理后的cmd命令进行备份
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
				//对当前命令参数进行检查
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

        /* 根据命令字查找命令表 */
        cmdlable = getCmdLable(cmdtemp);

        switch(cmdlable)
        {
            /* 激活目标机连接 */
        case ACTIVE_TARGET_CONNECT:
            m_activeTargetConnect( &(cmdtemp[4]) );
            break;
 
            /* 打印系统中的目标机连接 */
        case LIST_TARGET_CONNECT:
            m_listTargetInfo();
            break;

            /* 切换到C解析器 */    
        case SWITCH_TO_EXP_LABLE:
            m_switchToExpInterpreter();
            break;

            /* 切换到coreos解析器 */
        case SWITCH_TO_COREOS_LABLE:
            m_switchToCoreOSInterpreter();
            break;

            /* 退出shell ,不关闭TS */
        case QUIT_LABLE:
            m_quit();
            break;

            /* 帮助信息 */
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
		/*当前命令已经查找到，但是参数解析出错,直接退出当前命令执行*/
		case SYS_ERROR_ARGV:
			break;
        /* 默认情况下将信息发送到当前命令解析器 */
        default:
			if(cmdtemp.size() > 0){
            m_sendCmdToInterpreter(cmdtemp);
			}
            break;
        }
    }

    return SUCC;
}


/*  发送命令到解析器 */
int consoleManager::m_sendCmdToInterpreter(string cmd)
{
    extern int tsServerAid;
    char programName[20] = {0};

    /* 如果不存在目标机连接,则不将命令包发送到命令执行体 */
    if(curTargetAID < 0)
    {
        printf("没有绑定目标机\n");
        return FAIL;
    }

    /* 判断当前解析器，插入命令到不同的解析器 */
    if( C_INTERPRETER ==  currentInterpreter )
    {
        if(syncSymbolFiles() != SUCC )
        {
            return FAIL;
        }

        /*tsServer没有启动*/
        strcpy(programName,"tsServer.exe");
        if( SUCC != findProcess(programName))
        {
            /*启动失败*/
            if( TS_SUCCESS != launchProgram("tsServer.exe"))
            {
                cout << "tsServer 出错!" << endl;
                return FAIL;
            }
            else
            {
                Sleep(1000);
                if(SUCC != pExpressionProcess->getTsServerAidFromTsMap())
                {
                    cout << "tsServer 出错!" << endl;
                    return FAIL;
                }
            }
        }

        /*tsServer在shell启动时已经启动了,但是没有获得它的ID*/
        if(tsServerAid < 0)
        {
            if(SUCC != pExpressionProcess->getTsServerAidFromTsMap())
            {
                cout << "tsServer 出错!" << endl;
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
        /* 关闭与ts的连接 */
        TClient_close(tsLinkId);
        tsLinkId = -1;
    }
    
    /*释放解析器*/
    uninstallInterpreter();

    return;
}

/**
 * @brief: 
 *    获取目录文件列表
 * @param dirPath: 目录路径
 * @return：
 *    无
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
			/* 排除子目录和父目录 */
			if((!string(".").compare(FindFileData.cFileName)) || (!string("..").compare(FindFileData.cFileName)))
			{
				continue;
			}
			
			/* 递归目录 */
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

	/* 关闭句柄 */
	FindClose(hFind);
}

/*保存当前路径*/
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
    /*根据符号表文件记录判断符号表是否存在*/
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
    
	/* 获取目录文件列表 */
	DirectoryFileList(fullSymbolFilesFolder, fileList);

	strncpy(fullSymbolListName, fullSymbolFilesFolder, MAX_PATH_LEN);   //fullSymbolListName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name"
    strcat(fullSymbolFilesFolder, "\\");    //fullSymbolFilesFolder="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\"
    strcat(fullSymbolListName, ".txt");           //fullSymbolListName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name.txt"
	
    fp_txt = fopen(fullSymbolListName, "a+");
    if(fp_txt == NULL)
    {
        printf("打开符号表列表文件失败\n");
        printErrorInfo();
        return FAIL;
    }

    /*读取一行数据*/
    while(fgets(readSymbolListBuff,MAX_PATH_LEN, fp_txt) != NULL)
    {
        /*将文件中的提示信息跳过*/
        if('#' == readSymbolListBuff[0])
        {
            continue;
        }
		/*不允许空换行，也不允许前面有空格、制表符*/
        if(' ' == readSymbolListBuff[0] || '\n' == readSymbolListBuff[0] || '\r' == readSymbolListBuff[0] ||  '\t' == readSymbolListBuff[0])
        {
            printf("符号表列表文件中信息格式错误\n");
            fclose(fp_txt);
            return FAIL;
        }
        /*将最后的换行符删除*/
        length = strlen(readSymbolListBuff);
        if('\n' == readSymbolListBuff[length - 1])
        {
            readSymbolListBuff[length - 1] = 0;
        }
    
        /*得到文件名*/
        elfName = strrchr(readSymbolListBuff, '\\');
        if( elfName == NULL )
        {
            printf("符号表列表文件中无信息或信息有误\n");
            fclose(fp_txt);
            return FAIL;
        }

        elfName++;
        strcpy(fullSymbolFileName,fullSymbolFilesFolder);   //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\"
        strcat(fullSymbolFileName, elfName);        //fullSymbolFileName="D:\\LambdaAE_0.5.3.6\\host\\pub\\symbolFiles\\name\\os.elf"
		
	 	/* 剔除不需要删除的符号文件 */
		for(iter = fileList.begin(); iter != fileList.end(); iter++)
		{
			if(!(*iter).compare(fullSymbolFileName))
			{
				fileList.erase(iter);
				break;
			}
		}

        /*如果没有符号表则添加*/
        if((fp_elf = fopen(fullSymbolFileName,"rb") ) == NULL)
        {
            if(CopyFileA(readSymbolListBuff,fullSymbolFileName,false) == 0)
            {
                printf("符号表列表文件中记录的信息有误\n");
                printErrorInfo();
                fclose(fp_txt);
                return FAIL;
            }
        }
        else
        {
            fclose(fp_elf);
        }

        /*根据符号表文件的修改时间和大小,判断是否需要重新拷贝符号表文件*/
        _stat(fullSymbolFileName, &ssbuf);
        _stat(readSymbolListBuff, &ssbuf1);

        if((ssbuf.st_size != ssbuf1.st_size) || (ssbuf.st_mtime != ssbuf1.st_mtime))
        {
            if(CopyFileA(readSymbolListBuff,fullSymbolFileName,false) == 0)
            {
                printf("符号表列表文件中记录的信息有误\n");
                printErrorInfo();
                fclose(fp_txt);
                return FAIL;
            }
        }
    }

	/* 删除符号表文件未记录的符号表 */
	for(iter = fileList.begin(); iter != fileList.end(); iter++)
	{
		DeleteFile((*iter).c_str());
	}

    fclose(fp_txt);
    return SUCC;
}


/*打印错误信息*/
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
    
    // 创建快照句柄
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    // 先搜索系统中第一个进程的信息
    Process32First(hSnapshot, &pe);
    
    // 下面对系统中的所有进程进行枚举
    do
    {
        if(strlen(pe.szExeFile)>0)
        {
            /*不区分大小写*/
            if (strnicmp((char*)pe.szExeFile,strlwr(ProcessName),strlen(pe.szExeFile)) == 0)
            {
                /*找到后设置SUCC*/
                ret = SUCC;
                break;
            }
        }  
    }while (Process32Next(hSnapshot, &pe));
    
    // 关闭快照句柄
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


