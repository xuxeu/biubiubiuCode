/*
* 更改历史：
* 2004-12-6  董洪涛  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 唐兴培  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  expressionProcess.cpp
* @brief  
*       功能：
*       <li>表达式函数执行类实现</li>
*/

/************************头文件********************************/
#include "expressionProcess.h"

#include <stdio.h>

#include "shellMain.h"

#include "consoleManager.h"

#include "coreCmdProcess.h"

#include "CheckSubArgv.h"
using namespace std;

/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/

/************************模块变量******************************/
/* tsServer代理号 */
int tsServerAid = -1;

static const basic_string <char>::size_type npos = -1;


/************************外部声明******************************/
/* 与ts建立的连接ID */
extern int tsLinkId;

/* 当前目标机连接的AID */
extern int curTargetAID;

/* 当前目标机连接的超时时间 */
extern int timeOut;

/* 当前连接的目标机名*/
extern char curTargetName [NAME_SIZE + 1];


extern consoleManager *pConsoleManager;

extern coreCmdProcess *pCoreCmdProcess;


/************************前向声明******************************/

/************************实现*********************************/

/*
    查询shellAgent代理是否空闲
*/

static bool checkTargetShellAgentIsIdle(){

    bool ret = true;
    
    if(pCoreCmdProcess != NULL && pCoreCmdProcess->isCheckShellAgentIsIdle()){
    
        ret = pCoreCmdProcess->checkTargetShellAgentIsIdle();
    }
    return ret;

}

expressionProcess::expressionProcess()
{
    runFlag = true;    
}


expressionProcess::~expressionProcess()
{
    runFlag = false;
}


void expressionProcess::stop()
{
    runFlag = false;
    return;
}



/* 插入待发送命令到核心操作系统解析器 */
int expressionProcess::insertData(string cmd)
{

    /* 将获取到的命令放入容器中 */
    expCmdListMtx.lock();
    expCmdList.push_back(cmd);
    expCmdListMtx.unlock();

    return SUCC;
}



/* 发送数据包到目标机shell代理 */
int expressionProcess::sendDataToTsServer(char* pBuf, int size)
{

    int ret = 0;

    if(NULL == pBuf)
    {
        return FAIL;
    }

    if(tsServerAid <= 0)
    {
        return FAIL;
    }

    /* 发送数据包到目标机 */
    ret = TClient_putpkt(tsLinkId, tsServerAid, 0, 0, pBuf, size);
    if( ret < 0 )
    {
        return ret;
    }

    return SUCC;

}


/* 发送数据包到目标机的shellCallFun*/
int expressionProcess::sendDataToShellCallFun(char* pBuf, int size)
{

    int ret = 0;

    if(NULL == pBuf)
    {
        return FAIL;
    }

    if(tsServerAid <= 0)
    {
        return FAIL;
    }

    /* 发送数据包到目标机 */
    ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TASK_SHELLAGENT_CALL_FUN, 0, pBuf, size);
    if( ret < 0 )
    {
        return ret;
    }

    return SUCC;

}



/* 从ts收包 */
int expressionProcess::recvDataFromTs(char* pBuf, int size)
{

    int ret_wait = 0,ret_getpkt = 0;
    unsigned short src_aid, des_said, src_said;
    extern bool ctrl_c_flag;

    if(NULL == pBuf)
    {
        return FAIL;
    }

    /*等待数据到来,当按下ctrl+c键时结束操作*/
    ctrl_c_flag = false;
    while( !ctrl_c_flag  )
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
    }

    /*ctrl+c键被按下*/
    return STOP_CMD;

}



/* 处理命令 */
int expressionProcess::processData()
{

    int ret = 0;
    int len = 0;
    int scanfret = 0;
    int nlines = 0;
    string cmdtemp;
    char recvBuf[2*MAX_MSG_SIZE] = {'\0'};
    char expBuf[MAX_MSG_SIZE] = {'\0'};
    char pbuf[MAX_MSG_SIZE] = {'\0'};
    string::size_type noblankpos = 0;


    if (!expCmdList.empty())
    {
        /* 从命令队列中取出未处理的命令 */
        expCmdListMtx.lock();
        cmdtemp = expCmdList.front();
        expCmdList.pop_front();
        expCmdListMtx.unlock();

        /*查询目标机shell代理是否启动*/
        ret = pCoreCmdProcess->queryTargetAgent();
        if(ret != SUCC)
        {
            return FAIL;
        }

        memset(expBuf, 0, sizeof(expBuf));

		if(strncmp(cmdtemp.c_str(), ": ", strlen(": ")) == 0)
        {
            if ( coreCmdSetDomainCheckSubArgv(cmdtemp) == false){
            
                return FAIL;
            }

		//查询OS版本
		if( SUCC != pCoreCmdProcess->queryTargetAgentOsVersion() )
		{
			return FAIL;
		}
	//如果当前目标机的操作系统版本与当前名称不匹配，则直接不做任何处理
		if(pCoreCmdProcess->getOSVersion() != OS_2X_VERSION ){
			cout << "该命令只支持ACoreOS653操作系统"<< endl;
			return FAIL;
		}

		if(pCoreCmdProcess->queryTargetAgentOsStauts() != SUCC){
				return FAIL;
		}
            /* 去掉命令字setD*/
            cmdtemp.erase( 0, strlen(SET_USING_DOMAIN_STR) );

            /* 查找第一个非空格的字符,并删除空格之前内容 */
            noblankpos = cmdtemp.find_first_not_of(BLANK_STR);
        	if( (string::npos  == noblankpos) || (0  == noblankpos) )
            {
                cout << "输入的查询指定任务命令格式错误" << endl;
                return FAIL;
            }
            cmdtemp.erase(0, noblankpos);
    
            /* 组装查询指定任务命令包格式ti:xxxx */
            if(strcmp(cmdtemp.c_str() ,"coreOS") == 0)
            {
                strcpy(pbuf,"setD:__kdomain__");
                len = strlen(pbuf);
            }
            else 
            {
                len = sprintf(pbuf, "setD:%s", cmdtemp.c_str());
            }
                
            /*如果是setD命令就发送到shellAgent*/
            sendDataToShellCallFun(pbuf, len);

            /* 接收执行结果 */
            ret = recvDataFromTs( recvBuf, sizeof(recvBuf) );
            if(STOP_CMD == ret )
            {
                cout << endl;
            }
            else if(ret < 0)
            {
                cout << "接收数据出错!" << endl;
            }

            /* 回复的信息为错误代码,则直接打印操作失败 */
            if('E' == recvBuf[0])
            {
                cout << "分区" <<cmdtemp<<"不存在"<<endl;
                return FAIL;
            }

            cout<< "完成" << endl;
            return SUCC;
            
        }
        else if(strncmp(cmdtemp.c_str(),"l ",strlen("l ")) == 0)
        {
            /* 如果命令为反汇编操作 */
            if(expressionCmdDisasCheck(cmdtemp)){
                ret = m_processDisas(cmdtemp);
                if(ret != SUCC)
                {
                    return FAIL;
                }
            }else{
                return FAIL;
            }
        }
        else
        {
            /* 组装查询数据包,格式e:TargetAID;symbolFile;expression */
            //修改BUG0006449中命令不完整的情况
            if(cmdtemp[cmdtemp.size() -1 ] == '\\'){
                printf("命令不完整\n");
                return FAIL;
            }
            
            if(checkTargetShellAgentIsIdle() == false){
                std::cout << "目标机正在执行函数调用，请稍后再试" <<  std::endl;
                return FAIL;
            }
			if(!checkChineseCh(cmdtemp)){
				std::cout << "不支持中文"<< std::endl;
				return FAIL;
			}	
            len = sprintf( expBuf, "e:%x;%s;%s", curTargetAID, curTargetName, cmdtemp.c_str() );
        
            /* 将数据发送到tsServer */
            sendDataToTsServer(expBuf, len);
        }

        /* 接收执行结果 */
        ret = recvDataFromTs( recvBuf, sizeof(recvBuf) );
        if(STOP_CMD == ret )
        {
            cout << endl;
        }
        else if(ret < 0)
        {
            cout << "接收数据出错!" << endl;
        }
        else
        {
            cout <<  recvBuf << endl;
        }
    }

    return SUCC;
}



/* 从tsmap 获取tsServer的Aid号 */
int expressionProcess::getTsServerAidFromTsMap()
{
    int len = 0;
    int ret = 0;
    int tsserverid = 0;
    unsigned short src_aid, des_said, src_said;
    char quryBuf[100] = {'\0'};
    char* pStr = NULL;

    /* 组装查询tsServerAid 的数据包 */
    len = sprintf(quryBuf, "q:tsServerMap;%x;%x;%x;%s;",tsLinkId, 0, strlen("tsServer"), "tsServer");

    /* 发包 */
    ret = TClient_putpkt(tsLinkId, TS_MAP_ID, 0, 0, quryBuf, len);
    if( ret < 0 )
    {
        return TSMAP_ERROR;
    }

    ret = TClient_wait(tsLinkId, timeOut);
    if(TS_DATACOMING != ret) 
    {
        return TSMAP_ERROR;
    }

    /* 收包 */
    memset(quryBuf, 0, sizeof(quryBuf));
    ret = TClient_getpkt(tsLinkId, &src_aid, &des_said, &src_said, quryBuf, sizeof(quryBuf));
    if ( ret < 0 )
    {
        return (TSMAP_ERROR);    
    }

    /* 查询失败 */
    pStr = strstr(quryBuf, "E05");
    if (NULL != pStr)
    {   
        return(TSMAP_ERROR);
    }

    /* 解析回复数据,格式OK; regAid;regSaid; 找到对应的项 */
    pStr = strstr(quryBuf, "OK;");
    if (NULL == pStr)
    {
        return(FAIL);
    }
    
    pStr += strlen("OK;");
    hex2int(&pStr, &tsserverid);
    
    if(tsserverid <= 0)
    {
        return (FAIL);
    }

    tsServerAid = tsserverid;

    return SUCC;
}

    
