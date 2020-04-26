/*
* 更改历史：
* 2004-12-6  董洪涛  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 唐兴培  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  main.cpp
* @brief  
*       功能：
*       <li>核心命令类实现</li>
*/
/************************头文件********************************/
#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
using namespace std;
/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/

/************************模块变量******************************/
/* 命令表 */
extern  coreCmdTableStruct coreCmdTable[] ; 

static const basic_string <char>::size_type npos = -1;

/************************外部声明******************************/
/* 与ts建立的连接ID */
extern int tsLinkId;

/* 当前目标机连接的AID */
extern int curTargetAID;

/* 当前连接目标机的名字 */
extern int curTargetName;

/* 当前连接目标机的超时时间 */
extern int timeOut;

/************************前向声明******************************/

/************************实现*********************************/

coreCmdProcess::coreCmdProcess()
{
    runFlag = true;

    m_readMemorySend = 0;

    /* 默认访问50个内存单元 */
    m_defaultControlMemUnits = DEFAULT_DISPLAY_MEM_UNITS;

    /* 默认访问内存操作宽度4字节 */
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



/* 插入待发送命令到核心操作系统解析器 */
int coreCmdProcess::insertData(string cmd)
{

    /* 将获取到的命令放入容器中 */
    coreCmdListMtx.lock();
    coreCmdList.push_back(cmd);
    coreCmdListMtx.unlock();

    return SUCC;
}


/* 发送数据包到目标机shell代理 */
int coreCmdProcess::sendDataToShellAgent(const char* pBuf, int size)
{

    int ret = 0;

    if(NULL == pBuf)
    {
        return FAIL;
    }

    /* 发送数据包到目标机 */
    ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TASK_SHELLAGENT_RESEARCH, 0, pBuf, size);
    if( ret < 0 )
    {
        return ret;
    }

    return SUCC;

}




/* 查找命令表 */
coreCmdLableEnum coreCmdProcess::getCmdLable(string& cmd)
{
    int i = 0;
    string::size_type blankpos = 0;
	//过滤掉命令中多余的空格和制表符
	skipInvalidChFormCmd(cmd);
	if(cmd.size() > 0 ){
		string backupCmd = cmd; //对当前处理后的cmd命令进行备份
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




/* 数据处理函数 */
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
         
		/* 根据命令字查找对应的命令处理标签  */
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
			/*当前命令查找到，但是命令参数出错*/
			case CORE_ERROR_ARGV:
				break;
            default:
                cout << "未找到命令:" << cmdtemp << endl;
                break;
        }
    }


    return SUCC;
}


/* 从ts收包 */
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
    
    /*等待数据到来,当按下ctrl+c键时结束操作*/
    ctrl_c_flag = false;
    while( !ctrl_c_flag )
    {
        ret_wait = TClient_wait(tsLinkId, 1);
        if( TS_DATACOMING == ret_wait )
        {
            /* 有数据到来进行收包 */
            ret_getpkt = TClient_getpkt(tsLinkId, &src_aid, &des_said, &src_said, pBuf, size);
            if( ret_getpkt < 0 )
            {
                return FAIL;    
            }
            return ret_getpkt;
        }
		
        /*如果等待数据到来出错*/
        else if( ret_wait != TS_TIME_OUT )
        {
            return FAIL;
        }

        /*如果不是永久等待则时间减少一秒*/
        if(pTimeOut != -1) 
        {
            /*如果等待时间已到*/
            if((--timeCount) <= 0)
            {
                return FAIL;
            }
        }
    }

    /*ctrl+c键被按下*/
    return STOP_CMD;

}
/* 设置默认内存操作长度 */
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




/*将一块内存区中十六进制字符串转换成对应的内容保存在指定的内存位置中。*/
void coreCmdProcess::hex2memEx(char *buf, int count)
{
    char ch;
    char *mem = buf;
    int transcunt = count;

    while (transcunt-- > 0)
    {
        /* 将buf的内容转换成对应的数字存入ch的高4位 */
        ch = hex2num(*buf++) << 4;

        /* 将buf的内容转换成对应的数字存入ch的低4位 */
        ch |= hex2num(*buf++);

        *mem++ = ch;
    }
}



/* 解析任务信息 */
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

    /* 解析开始信息 */
    if(psource[0] != '<')
    {
        return FAIL;
    }
    psource++;

    /* 查找标示符 */
    pstr = strstr(psource, ">");
    if(NULL == pstr)
    {
        return FAIL;
    }
    
    /* 复制任务名称 */
    strncpy(pinfo->name, psource, (int)(pstr-psource));

    /* 跳过> */
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


/* 解析任务信息 */
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

    /* 解析开始信息 */
    if(psource[0] != '<')
    {
        return FAIL;
    }
    psource++;

    /* 查找标示符 */
    pstr = strstr(psource, ">");
    if(NULL == pstr)
    {
        return FAIL;
    }
    
    /* 复制任务名称 */
    strncpy((char *)name, psource, (int)(pstr-psource));

    /* 跳过">" */
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
	查询OS版本
*/
int coreCmdProcess::queryTargetAgentOsVersion(){
	
	char pBuf[8 + 1] = {0};
    int size = 8;
    US src_said = 0;
    int ret = 0;

    /*查询OS版本*/
    TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "MV", 2);
    ret = recvDataFromTs( pBuf, size ,timeOut);

    if( ret == STOP_CMD)
    {
        return FAIL;
	}else if(ret < 0){
		printf("接收数据失败\n");
		return FAIL;
	}

	/*如果是C1*/
    if(strcmp(pBuf,"OK2"))
    {
		//OS1下不需要检查shellagent是否空闲
		m_bCheckShellAgentIsIdle = false;
		//设置当前目标机OS版本
		m_osVersion = OS_1X_VERSION ;
	}else{
		//OS2下需要检查shellagent是否空闲
		m_bCheckShellAgentIsIdle = true;
		m_osVersion = OS_2X_VERSION;
	}
	return SUCC;

}

/*
	查询OS状态是否启动
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
		printf("接收数据失败\n");
		return FAIL;
	}
        
	if(0 != strcmp(pBuf,"OK"))
    {
        printf("目标机OS没有启动!\n");
        return FAIL;
    }
	
	return SUCC;
}
/*
	查询shell代理是否启动
*/
int  coreCmdProcess::queryTargetShellAgent(){

	int ret = SUCC;
	char pBuf[8 + 1] = {0};
    int size = 8;
    US src_said = 0;
	if(m_osVersion == OS_2X_VERSION){
		/*查询代理有没有启动*/
        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QA9", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		}else if(ret < 0){
			printf("接收数据失败\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("目标机未配置目标机SHELL代理!\n");
            return FAIL;
        }

        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QAB", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
        }else if(ret < 0){
			printf("接收数据失败\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("目标机未配置目标机SHELL代理!\n");
            return FAIL;
        }    
	
	}else if(m_osVersion == OS_1X_VERSION){
		/*查询代理有没有启动*/
		TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QA1", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		
		}else if(ret < 0 ){
			printf("接收数据失败\n");
			return FAIL;
		}

        if(0 == strcmp(pBuf,"OK"))
        {
            return SUCC;
        }
        else
        {
            printf("目标机未配置调试代理\n");
            return FAIL;
        }
	}
	return SUCC;
}

/*查询OS和shell代理是否启动*/
int  coreCmdProcess::queryTargetAgent()
{
    char pBuf[128 + 1] = {0};
    int size = 128;
    US src_said = 0;
    int ret = 0;
	char *pRecvBuf = NULL;
    /*查询OS版本*/
    TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "MV", 2);
    ret = recvDataFromTs( pBuf, size ,timeOut);
    if( ret == STOP_CMD )
    {
        return FAIL;
	}else if(ret < 0){
		printf("接收数据失败\n");
		return FAIL;
	}

    /*如果是C1*/
    if(strcmp(pBuf,"OK2"))
    {
		//OS1下不需要检查shellagent是否空闲
		m_bCheckShellAgentIsIdle = false;
		//设置当前目标机OS版本
		m_osVersion = OS_1X_VERSION ;

        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QA1", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		}else if(ret < 0 ){
			printf("接收数据失败\n");
			return FAIL;
		
		}

        if(0 == strcmp(pBuf,"OK"))
        {
            return SUCC;
        }
        else
        {
            printf("目标机未配置调试代理\n");
            return FAIL;
        }
    }
    else 
    {
		
		/*C2 获取目标机当前模式*/
		pRecvBuf = pBuf;
        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "MD", strlen("MD"));
		ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		}else if(ret < 0){
			printf("接收数据失败\n");
			return FAIL;
		}
        
        if(strncmp(pBuf, "OK;", 3))
        {
            printf("获取目标机当前模式失败!\n");
            return FAIL;
        }
		 pRecvBuf += 3;

		 if(!strncmp(pRecvBuf, "system", strlen("system")))
		 {
			 /*获取目标机运行状态*/
			 TClient_putpkt(tsLinkId, curTargetAID, 1, src_said, "QSS", strlen("QSS"));
			 ret = recvDataFromTs( pBuf, size ,timeOut);
			 if( ret == STOP_CMD )
			 {
			    return FAIL;
			 }else if(ret < 0){
				  printf("接收数据失败\n");
				  return FAIL;
			 }
			  if(!strncmp(pBuf, "pause", strlen("pause")))
            {
				  printf("ACoreOS653系统级处于暂停状态,不支持shell功能!\n");
				  return FAIL;
			}
		 }
		//OS2下需要检查shellagent是否空闲
		m_bCheckShellAgentIsIdle = true;
		m_osVersion = OS_2X_VERSION;
        /*如果是C2*/
        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "o", 1);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
		}else if(ret < 0){
			printf("接收数据失败\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("目标机OS没有启动!\n");
            return FAIL;
        }

        /*查询代理有没有启动*/
        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QA9", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
        }else if(ret < 0){
			printf("接收数据失败\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("目标机未配置目标机SHELL代理!\n");
            return FAIL;
        }

        TClient_putpkt(tsLinkId, curTargetAID, TA_SUBA_MANAGER, src_said, "QAB", 3);
        ret = recvDataFromTs( pBuf, size ,timeOut);
        if( ret == STOP_CMD )
        {
            return FAIL;
        }else if(ret < 0){
			printf("接收数据失败\n");
			return FAIL;
		}
        
        if(0 != strcmp(pBuf,"OK"))
        {
            printf("目标机未配置目标机SHELL代理!\n");
            return FAIL;
        }    

        return SUCC;
    }

}


/*
	查询shell代理中SHELL_Aide任务是否空闲
	BUG:0006625
	shell同时请求同一个C2目标机执行函数时没有检查
*/
bool coreCmdProcess::checkTargetShellAgentIsIdle()
{
	bool bRet = true;
	int len = 0,ret = 0;
	char pbuf[MAX_MSG_SIZE] = {'\0'};
    ACoreOs_task_information taskinfo;
	
	memset(pbuf, 0, sizeof(pbuf));
	memset(&taskinfo,0,sizeof(ACoreOs_task_information));
	
	 /* 组装查询指定任务命令包格式ti:SHELL_Aide */
    len = sprintf(pbuf, "ti:%s","SHELL_Aide");
	
	/* 发送查询指定任务信息命令包到shell agent */
    ret = sendDataToShellAgent(pbuf, len);
    if(ret < 0)
    {
        return false;
	}
    memset(pbuf, 0, sizeof(pbuf));
	/* 接收读取结果 */
    ret = recvDataFromTs( pbuf, sizeof(pbuf) );
    if( ret == STOP_CMD )
    {
        return false;
    }
    else if(ret < 0)
    {
        cout << "接收数据失败" << endl;
        return false;
    }

    memset(&taskinfo, 0, sizeof(taskinfo));

    /* 解析任务信息 */
    ret = analysisTaskInfo(pbuf, &taskinfo);
    if(ret < 0)
    {
        return false;
    }
	return ( STATES_READY != taskinfo.state);
	
}
/*
		获取当前是否需要检查shellAgent是否空闲
		返回值:
			ture:需要
			false:不需要
*/

bool coreCmdProcess::isCheckShellAgentIsIdle() const
{
		return m_bCheckShellAgentIsIdle;
}

	