/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ������  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  main.cpp
* @brief  
*       ���ܣ�
*       <li>����������ʵ��</li>
*/
/************************ͷ�ļ�********************************/
#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
using namespace std;
/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/

/************************ģ�����******************************/
/* ����� */
extern  coreCmdTableStruct coreCmdTable[] ; 

static const basic_string <char>::size_type npos = -1;

/************************�ⲿ����******************************/
/* ��ts����������ID */
extern int tsLinkId;

/* ��ǰĿ������ӵ�AID */
extern int curTargetAID;

/* ��ǰ����Ŀ��������� */
extern int curTargetName;

/* ��ǰ����Ŀ����ĳ�ʱʱ�� */
extern int timeOut;

/************************ǰ������******************************/

/************************ʵ��*********************************/

coreCmdProcess::coreCmdProcess()
{
    runFlag = true;

    m_readMemorySend = 0;

    /* Ĭ�Ϸ���50���ڴ浥Ԫ */
    m_defaultControlMemUnits = DEFAULT_DISPLAY_MEM_UNITS;

    /* Ĭ�Ϸ����ڴ�������4�ֽ� */
    m_defaultControlMemWidth = Width_16Bit;

}


coreCmdProcess::~coreCmdProcess()
{
    runFlag = false;
}


void coreCmdProcess::stop()
{
    runFlag = false;
    return;
}



/* ���������������Ĳ���ϵͳ������ */
int coreCmdProcess::insertData(string cmd)
{

    /* ����ȡ����������������� */
    coreCmdListMtx.lock();
    coreCmdList.push_back(cmd);
    coreCmdListMtx.unlock();

    return SUCC;
}


/* �������ݰ���Ŀ���shell���� */
int coreCmdProcess::sendDataToShellAgent(const char* pBuf, int size)
{

    int ret = 0;

    if(NULL == pBuf)
    {
        return FAIL;
    }

    /* �������ݰ���Ŀ��� */
    ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TASK_SHELLAGENT_RESEARCH, 0, pBuf, size);
    if( ret < 0 )
    {
        return ret;
    }

    return SUCC;

}




/* ��������� */
coreCmdLableEnum coreCmdProcess::getCmdLable(string& cmd)
{
    int i = 0;
    string::size_type blankpos = 0;
	//���˵������ж���Ŀո���Ʊ��
	skipInvalidChFormCmd(cmd);
	if(cmd.size() > 0 ){
		string backupCmd = cmd; //�Ե�ǰ������cmd������б���
		blankpos = backupCmd.find_first_of(' ');
		if(npos != blankpos )
		{
			backupCmd[blankpos] = '\0';
		}

		for(i = 0; coreCmdTable[i].cmdStr; i++)
		{
			if( 0 ==  strcmp(backupCmd.c_str(), coreCmdTable[i].cmdStr))
			{
				if(coreCmdTable[i].checkSubArgv(cmd)){
					return coreCmdTable[i].cmdLable;
				}else{
					return CORE_ERROR_ARGV ;
				}
			}
		}
	}

    return NOT_DEFINE_CMD;
}




/* ���ݴ����� */
int coreCmdProcess::processData()
{
    string cmdtemp;
    coreCmdLableEnum cmdLable;

    if (!coreCmdList.empty())
    {
        coreCmdListMtx.lock();
        cmdtemp = coreCmdList.front();
        coreCmdList.pop_front();
        coreCmdListMtx.unlock();
         
		/* ���������ֲ��Ҷ�Ӧ��������ǩ  */
        cmdLable = getCmdLable(cmdtemp);
       
        switch(cmdLable)
        {
            case READ_MEMORY:
                m_readMemory(cmdtemp);
                break;

            case WRITE_MEMORY:
                m_writeMemory(cmdtemp);
                break;

            case SET_USING_DOMAIN:
                m_setDomain(cmdtemp);
                break;

            case LIST_ALL_TASK:
                m_listAllTaskInfo(cmdtemp);
                break;

            case LIST_APPOINT_TASK:
                m_listAppointTaskInfo(cmdtemp);
                break;

            case STOP_TASK:
                m_suspendTask(cmdtemp);
                break;

            case RESUME_TASK:
                m_resumeTask(cmdtemp);
                break;

            case GET_TASK_ID:
                m_getTaskID(cmdtemp);
                break;

            case SHOW_PD_INFO:
                m_showPdInfo(cmdtemp);
                break;

            case SETUP_DEBUG_EXP:
                m_setupDbExp(cmdtemp);
                break;
			case DETACHTASK:
				detachTask();
				break;
			case DETACHPARTITION:
				detachPartition(cmdtemp);
				break;
			case DETACHSYSTEM:
				detachSystem(cmdtemp);
				break;
			/*��ǰ������ҵ������������������*/
			case CORE_ERROR_ARGV:
				break;
            default:
                cout << "δ�ҵ�����:" << cmdtemp << endl;
                break;
        }
    }


    return SUCC;
}


/* ��ts�հ� */
int coreCmdProcess::recvDataFromTs(char* pBuf, int size ,int pTimeOut)
{

    int ret_wait = 0,ret_getpkt = 0;
    int timeCount = 0;
    unsigned short src_aid = 0, des_said = 0, src_said = 0;
    extern bool ctrl_c_flag;

    if(NULL == pBuf)
    {
        return FAIL;
    }

    switch(pTimeOut)
    {
        case -1:
        case 0:
            timeCount = 1;
            break;
        default:
            timeCount = pTimeOut;
    }
    
    /*�ȴ����ݵ���,������ctrl+c��ʱ��������*/
    ctrl_c_flag = false;
    while( !ctrl_c_flag )
    {
        ret_wait = TClient_wait(tsLinkId, 1);
        if( TS_DATACOMING == ret_wait )
        {
            /* �����ݵ��������հ� */
            ret_getpkt = TClient_getpkt(tsLinkId, &src_aid, &des_said, &src_said, pBuf, size);
            if( ret_getpkt < 0 )
            {
                return FAIL;    
            }
            return ret_getpkt;
        }
		
        /*����ȴ����ݵ�������*/
        else if( ret_wait != TS_TIME_OUT )
        {
            return FAIL;
        }

        /*����������õȴ���ʱ�����һ��*/
        if(pTimeOut != -1) 
        {
            /*����ȴ�ʱ���ѵ�*/
            if((--timeCount) <= 0)
            {
                return FAIL;
            }
        }
    }

    /*ctrl+c��������*/
    return STOP_CMD;

}
/* ����Ĭ���ڴ�������� */
int coreCmdProcess::setDefaultControlMemWidth(unsigned int width)
{

    switch(width)
    {
        case 0:
            break;

        case 1:
            m_defaultControlMemWidth = Width_8Bit;
            break;

        case 2:
            m_defaultControlMemWidth = Width_16Bit;
            break;

        case 4:
            m_defaultControlMemWidth = Width_32Bit;
            break;

        default:
            m_defaultControlMemWidth = Width_16Bit;
            break;
    }

    return SUCC;
}




/*��һ���ڴ�����ʮ�������ַ���ת���ɶ�Ӧ�����ݱ�����ָ�����ڴ�λ���С�*/
void coreCmdProcess::hex2memEx(char *buf, int count)
{
    char ch;
    char *mem = buf;
    int transcunt = count;

    while (transcunt-- > 0)
    {
        /* ��buf������ת���ɶ�Ӧ�����ִ���ch�ĸ�4λ */
        ch = hex2num(*buf++) << 4;

        /* ��buf������ת���ɶ�Ӧ�����ִ���ch�ĵ�4λ */
        ch |= hex2num(*buf++);

        *mem++ = ch;
    }
}



/* ����������Ϣ */
int coreCmdProcess::analysisTaskInfo(char* buf, ACoreOs_task_information* pinfo)
{

    char* psource = NULL;
    char* pstr = NULL;
    int scanfret = 0;


    if( (NULL == buf) || (NULL == pinfo) )
    {
        return FAIL;
    }

    psource = buf;

    /* ������ʼ��Ϣ */
    if(psource[0] != '<')
    {
        return FAIL;
    }
    psource++;

    /* ���ұ�ʾ�� */
    pstr = strstr(psource, ">");
    if(NULL == pstr)
    {
        return FAIL;
    }
    
    /* ������������ */
    strncpy(pinfo->name, psource, (int)(pstr-psource));

    /* ����> */
    pstr++;

    scanfret = sscanf(pstr, "%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x", 
                                &(pinfo->task_id),
                                &pinfo->isr_level,
                                &pinfo->preemptible,
                                &pinfo->timeslice,
                                &pinfo->floating_point,
                                &pinfo->time_budget,
                                &pinfo->initial_time_budget,
                                &pinfo->entry,
                                &pinfo->initial_priority,
                                &pinfo->real_priority,
                                &pinfo->resource_count,
                                &pinfo->state,
                                &pinfo->stackSize,
                                &pinfo->stackBase,
                                &pinfo->stackUseSize,
                                &pinfo->cpuTime);


    if(16 != scanfret)
    {
        return FAIL;
    }

    return SUCC;
}


/* ����������Ϣ */
int coreCmdProcess::analysisParttitionInfo(char* buf, int *id, UINT8 *name, int *type, UINT32 *startAddr, UINT32 *size, int *lowPriority, int *highPriority)
{

    char *psource = NULL;
    char *pstr = NULL;
    int scanfret = 0;


    if(NULL == buf)
    {
        return FAIL;
    }

    psource = buf;

    /* ������ʼ��Ϣ */
    if(psource[0] != '<')
    {
        return FAIL;
    }
    psource++;

    /* ���ұ�ʾ�� */
    pstr = strstr(psource, ">");
    if(NULL == pstr)
    {
        return FAIL;
    }
    
    /* ������������ */
    strncpy((char *)name, psource, (int)(pstr-psource));

    /* ����">" */
    pstr++;

    scanfret = sscanf(pstr, "%x,%x,%x,%x,%x,%x",
                                id,
                                type,
                                startAddr,
                                size,
                                lowPriority,
                                highPriority);


    if(6 != scanfret)
    {
        return FAIL;
    }

    return SUCC;
}
/*
	��ѯOS�汾
*/
int coreCmdProcess::queryTargetAgentOsVersion(){
	
	char pBuf[8 + 1] = {0};
    int size = 8;
    US src_said = 0;
    int ret = 0;

    /*��ѯOS�汾*/
    TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "MV", 2);
    ret = recvDataFromTs( pBuf, size ,timeOut);

    if( ret == STOP_CMD)
    {
        return FAIL;
	}else if(ret < 0){
		printf("��������ʧ��\n");
		return FAIL;
	}

	/*�����C1*/
    if(strcmp(pBuf,"OK2"))
    {
		//OS1�²���Ҫ���shellagent�Ƿ����
		m_bCheckShellAgentIsIdle = false;
		//���õ�ǰĿ���OS�汾
		m_osVersion = OS_1X_VERSION ;
	}else{
		//OS2����Ҫ���shellagent�Ƿ����
		m_bCheckShellAgentIsIdle = true;
		m_osVersion = OS_2X_VERSION;
	}
	return SUCC;

}

/*
	��ѯOS״̬�Ƿ�����
*/
int coreCmdProcess::queryTargetAgentOsStauts(){
	
	int ret;
	char pBuf[8 + 1] = {0};
    int size = 8;
	US src_said = 0;
	
	TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "o", 1);
    ret = recvDataFromTs( pBuf, size ,timeOut);
    if( ret == STOP_CMD )
    {
        return FAIL;
	}else if(ret < 0){
		printf("��������ʧ��\n");
		return FAIL;
	}
        
	if(0 != strcmp(pBuf,"OK"))
    {
        printf("Ŀ���OSû������!\n");
        return FAIL;
    }
	
	return SUCC;
}
/*
	��ѯshell�����Ƿ�����
*/
int  coreCmdProcess::queryTargetShellAgent(){

	int ret = SUCC;
	char pBuf[8 + 1] = {0};
    int size = 8;
    US src_said = 0;
	if(m_osVersion == OS_2X_VERSION){
		/*��ѯ������û������*/
        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QA9", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		}else if(ret < 0){
			printf("��������ʧ��\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("Ŀ���δ����Ŀ���SHELL����!\n");
            return FAIL;
        }

        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QAB", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
        }else if(ret < 0){
			printf("��������ʧ��\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("Ŀ���δ����Ŀ���SHELL����!\n");
            return FAIL;
        }    
	
	}else if(m_osVersion == OS_1X_VERSION){
		/*��ѯ������û������*/
		TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QA1", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		
		}else if(ret < 0 ){
			printf("��������ʧ��\n");
			return FAIL;
		}

        if(0 == strcmp(pBuf,"OK"))
        {
            return SUCC;
        }
        else
        {
            printf("Ŀ���δ���õ��Դ���\n");
            return FAIL;
        }
	}
	return SUCC;
}

/*��ѯOS��shell�����Ƿ�����*/
int  coreCmdProcess::queryTargetAgent()
{
    char pBuf[128 + 1] = {0};
    int size = 128;
    US src_said = 0;
    int ret = 0;
	char *pRecvBuf = NULL;
    /*��ѯOS�汾*/
    TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "MV", 2);
    ret = recvDataFromTs( pBuf, size ,timeOut);
    if( ret == STOP_CMD )
    {
        return FAIL;
	}else if(ret < 0){
		printf("��������ʧ��\n");
		return FAIL;
	}

    /*�����C1*/
    if(strcmp(pBuf,"OK2"))
    {
		//OS1�²���Ҫ���shellagent�Ƿ����
		m_bCheckShellAgentIsIdle = false;
		//���õ�ǰĿ���OS�汾
		m_osVersion = OS_1X_VERSION ;

        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QA1", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		}else if(ret < 0 ){
			printf("��������ʧ��\n");
			return FAIL;
		
		}

        if(0 == strcmp(pBuf,"OK"))
        {
            return SUCC;
        }
        else
        {
            printf("Ŀ���δ���õ��Դ���\n");
            return FAIL;
        }
    }
    else 
    {
		
		/*C2 ��ȡĿ�����ǰģʽ*/
		pRecvBuf = pBuf;
        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "MD", strlen("MD"));
		ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		}else if(ret < 0){
			printf("��������ʧ��\n");
			return FAIL;
		}
        
        if(strncmp(pBuf, "OK;", 3))
        {
            printf("��ȡĿ�����ǰģʽʧ��!\n");
            return FAIL;
        }
		 pRecvBuf += 3;

		 if(!strncmp(pRecvBuf, "system", strlen("system")))
		 {
			 /*��ȡĿ�������״̬*/
			 TClient_putpkt(tsLinkId, curTargetAID, 1, src_said, "QSS", strlen("QSS"));
			 ret = recvDataFromTs( pBuf, size ,timeOut);
			 if( ret == STOP_CMD )
			 {
			    return FAIL;
			 }else if(ret < 0){
				  printf("��������ʧ��\n");
				  return FAIL;
			 }
			  if(!strncmp(pBuf, "pause", strlen("pause")))
            {
				  printf("ACoreOS653ϵͳ��������ͣ״̬,��֧��shell����!\n");
				  return FAIL;
			}
		 }
		//OS2����Ҫ���shellagent�Ƿ����
		m_bCheckShellAgentIsIdle = true;
		m_osVersion = OS_2X_VERSION;
        /*�����C2*/
        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "o", 1);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		}else if(ret < 0){
			printf("��������ʧ��\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("Ŀ���OSû������!\n");
            return FAIL;
        }

        /*��ѯ������û������*/
        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QA9", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
        }else if(ret < 0){
			printf("��������ʧ��\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("Ŀ���δ����Ŀ���SHELL����!\n");
            return FAIL;
        }

        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QAB", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
        }else if(ret < 0){
			printf("��������ʧ��\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("Ŀ���δ����Ŀ���SHELL����!\n");
            return FAIL;
        }    

        return SUCC;
    }

}


/*
	��ѯshell������SHELL_Aide�����Ƿ����
	BUG:0006625
	shellͬʱ����ͬһ��C2Ŀ���ִ�к���ʱû�м��
*/
bool coreCmdProcess::checkTargetShellAgentIsIdle()
{
	bool bRet = true;
	int len = 0,ret = 0;
	char pbuf[MAX_MSG_SIZE] = {'\0'};
    ACoreOs_task_information taskinfo;
	
	memset(pbuf, 0, sizeof(pbuf));
	memset(&taskinfo,0,sizeof(ACoreOs_task_information));
	
	 /* ��װ��ѯָ�������������ʽti:SHELL_Aide */
    len = sprintf(pbuf, "ti:%s","SHELL_Aide");
	
	/* ���Ͳ�ѯָ��������Ϣ�������shell agent */
    ret = sendDataToShellAgent(pbuf, len);
    if(ret < 0)
    {
        return false;
	}
    memset(pbuf, 0, sizeof(pbuf));
	/* ���ն�ȡ��� */
    ret = recvDataFromTs( pbuf, sizeof(pbuf) );
    if( ret == STOP_CMD )
    {
        return false;
    }
    else if(ret < 0)
    {
        cout << "��������ʧ��" << endl;
        return false;
    }

    memset(&taskinfo, 0, sizeof(taskinfo));

    /* ����������Ϣ */
    ret = analysisTaskInfo(pbuf, &taskinfo);
    if(ret < 0)
    {
        return false;
    }
	return ( STATES_READY != taskinfo.state);
	
}
/*
		��ȡ��ǰ�Ƿ���Ҫ���shellAgent�Ƿ����
		����ֵ:
			ture:��Ҫ
			false:����Ҫ
*/

bool coreCmdProcess::isCheckShellAgentIsIdle() const
{
		return m_bCheckShellAgentIsIdle;
}

	