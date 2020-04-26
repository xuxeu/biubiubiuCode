/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/

/**
 * @file             :  tsServer.cpp
 * @Version       :  1.0.0
 * @brief           :  *   <li> 功能</li>tsServer的主体
                                <li>设计思想</li> 
                                <p> n     注意事项：n</p> 
 *   
 * @author        :  zhagnxu
 * @create date :  2008年9月23日 15:49:46
 * @History       : 
 *   
 */


/************************引用部分*****************************/

#include "stdafx.h"
#include <direct.h>
#include <string.h>
#include <assert.h>
#include "windows.h"
#include "common.h" 
#include "tssComm.h"
#include "tsServer.h"
#include "tClientAPI.h"
#include "resoure.h"
/************************前向声明部分***********************/

/************************定义部分*****************************/
T_ERR_TBL errTbl[100] = {
                        {SUCC, "成功;"},
                        {FAIL, "失败;"},
                        {ERR_NOT_FOUND_IP, "错误:解析错误，没有找到IP;"},
                        {ERR_NOT_FOUND_EXPRESSION, "错误:解析错误-没有找到表达式;"},
                        {ERR_TOO_MANY_SYMBOL_FILE, "错误:解析错误-符号表文件过多;"},
                        {ERR_TARGET_QUEUE_FULL, "错误:目标机数量已经达到最大;"},
                        {ERR_MESSAGE_QUEUE_FULL, "错误:消息数量已经达到最大;"},
                        {ERR_CREATE_OBJECT_FAIL, "错误:创建对象失败;"},
                        {1, ""}
                    };

T_TSS_Eviroment_Variables *g_envVar;
T_TSS_Target_Queue_head  *g_targetQueueHead;
CRITICAL_SECTION g_cs;
TsServer* TsServer::m_pTsServer = NULL;
                
/************************实现部分****************************/

/**
 * @Funcname:  TsServer
 * @brief        :  TsServer的构造函数，完成路径获取和目标机队
 *                      列头初始化的工作
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
TsServer::TsServer()
{
    T_CHAR *pTmp = NULL;
    T_WORD isLog = 1;
    
     /*获取符号文件和GDB存放路径*/
    g_envVar = (T_TSS_Eviroment_Variables*)malloc(sizeof(T_TSS_Eviroment_Variables));
#ifndef TSSERVER_DEBUG
    GetModuleFileName(NULL, m_lambdaPath, MAX_PATH_LEN); 
	m_lambdaPath[strlen(m_lambdaPath)-strlen("tsServer.exe")] = '\0';	
	strcpy(g_envVar->symbolFilesPath, m_lambdaPath);
	m_lambdaPath[strlen(m_lambdaPath)-strlen("\\pub\\")] = '\0';
#else
    strcpy(m_lambdaPath, "D:\\1.x\\LambdaAE_2.2.8\\LambdaAE\\host");
    strcpy(g_envVar->symbolFilesPath, m_lambdaPath);
    strcat(g_envVar->symbolFilesPath, "\\pub\\");
    isLog = 0;
#endif
	strcat(g_envVar->symbolFilesPath, "symbolFiles");

    /*初始化目标机队列头*/
    g_targetQueueHead = (T_TSS_Target_Queue_head*)malloc(sizeof(T_TSS_Target_Queue_head));
    g_targetQueueHead->pHead = NULL;
    g_targetQueueHead->pTail = NULL;
    g_targetQueueHead->maxTargetNum = MAX_TARGET_NUM;
    g_targetQueueHead->curTargetNum = 0;
    g_targetQueueHead->pMtx = new TMutex();

   /*初始化临界对象*/
    InitializeCriticalSection(&g_cs);
        
    /*打开日志文件*/
    SystemLog::getInstance((LogLevel)isLog)->openLogFile(string("tsServer.log"));

    if (CheckFolderExist(g_envVar->symbolFilesPath) == FALSE)
    {
        _mkdir(g_envVar->symbolFilesPath);
    }

}

/**
 * @Funcname:  TsServer
 * @brief        :  TsServer的析构函数，释放资源
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
TsServer::~TsServer()
{
 //   DeleteDirectory(g_envVar->symbolFilesPath);
    
    if (NULL != g_envVar)
    {
        delete g_envVar;
        g_envVar = NULL;
    }

    if (NULL != g_targetQueueHead->pMtx)
    {
        delete g_targetQueueHead->pMtx;
        g_targetQueueHead->pMtx = NULL;
    }
}

/**
 * @Funcname: main
 * @brief        : TsServer的主处理函数
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 16:34:11
 *   
**/
void TsServer::main()
{
    T_UHWORD srcAid, srcSaid, desAid, desSaid;
    T_CHAR recvBuf[MAX_MESSAGE_LEN] = "\0";
    T_CHAR targetName[MAX_NAME_LEN] = "\0";
    T_CHAR *pTmp = NULL;
    T_CHAR *pAckString = NULL;
    T_WORD len, ret;
    T_TSS_Expression_Message msg;
    T_TSS_Target_Queue_Node *pTarget = NULL;
    
    /*创建用于接收表达式求值消息的连接*/
    m_fd = TClient_open(TS_IP, TS_PORT);
    if(m_fd <= 0)
    {
        sysLoger(LWARNING, "tsServer::main: connetc ts fail");
        return;
    }

    sysLoger(LDEBUG, "tsServer::main: connetc ts succ");

    /*向tsmap 注册*/
    ret = registerToTsmap();
    if(FAIL == ret)
    {
        sysLoger(LWARNING, "tsServer::main: register tsmap fail");
        return;
    }

    sysLoger(LINFO, "tsServer::main: register tsmap succ, aid:%d", m_fd);

    /*启动上报消息处理*/
    RepProcessor::getInstance()->run();

    /*循环接收、处理表达式求值消息*/
    while(1)
    {
        ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
        if(TS_DATACOMING != ret) 
        {
            if (TS_INVALID_LINK_HANDLE == ret)
            {
                sysLoger(LWARNING, "tsServer::main: link closed");
                return;
            }
            continue;
        }

        len = TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
        if(len < 0)
        {
            if (TS_SOCKET_ERROR == len)
            {
                sysLoger(LWARNING, "tsServer::main: link closed");
                return;
            }
            continue;
        }

        sysLoger(LINFO, "tsServer::main: recv cmd:%c", recvBuf[0] );

        switch (recvBuf[0])
        {
        case 's':    //设置日志级别
        {
            int newLevel = 1; 
            char *pBuf = &(recvBuf[1]);

            if (hex2int(&pBuf, &newLevel))
            {
                SystemLog::getInstance()->setLogLevel((LogLevel)newLevel);
                sysLoger(LINFO, "tsServer::main: set system log level as %d", newLevel);
            }
        }
            break;
            
        case 'q':    //退出tsServer
            sysLoger(LINFO, "tsServer::main: exit");
            stop();
            return;
            break;
            
        case 'e':    //解析表达式求值消息
        case 'o':     // 解析其他命令操作
        {
            /*解析表达式求值消息*/
            memset(&msg, 0,  sizeof(T_TSS_Expression_Message));
			memset(targetName, 0,  sizeof(targetName));
            ret = parseExpressionMessage(srcAid, recvBuf, len-2, &msg, targetName);  // 2 for e:
            if(SUCC != ret)
            {
                pAckString = getAckStr(ret);
                TClient_putpkt(m_fd, srcAid, 0, 0, pAckString, strlen(pAckString));
            }
            sysLoger(LDEBUG, "tsServer::main: execute expression, serAid:%d, tgtName:%s", 
                                        srcAid, targetName);

            g_targetQueueHead->pMtx->lock();

            /*查找该消息发向的目标机所对应的处理对象*/
            for(pTarget = g_targetQueueHead->pHead; 
                pTarget != NULL; 
                pTarget = pTarget->next)
            {
                if(pTarget->pExpProcessor->GetSaAid() == msg.saAid)
                {
                    break;
                }
            }
            
            /*如果没有找到对象对象则创建新对象并插入队列中*/
            if(NULL == pTarget)
            {
                if(g_targetQueueHead->curTargetNum >= g_targetQueueHead->maxTargetNum)
                {
                    pAckString = getAckStr(ERR_TARGET_QUEUE_FULL);
                    TClient_putpkt(m_fd, srcAid, 0, 0, pAckString, strlen(pAckString));
                }
                
                pTarget = (T_TSS_Target_Queue_Node *)malloc(sizeof(T_TSS_Target_Queue_Node));
                memset(pTarget, 0, sizeof(T_TSS_Target_Queue_Node));
                pTarget->pExpProcessor = new ExpProcessor(msg.saAid, targetName);
                if (NULL == pTarget->pExpProcessor)
                {
                    pAckString = getAckStr(ERR_CREATE_OBJECT_FAIL);
                    TClient_putpkt(m_fd, srcAid, 0, 0, pAckString, strlen(pAckString));
                    sysLoger(LWARNING, "tsServer::main: create expProcessor fail");
                    continue;
                }
                
                if(g_targetQueueHead->curTargetNum == 0)
                {
                    g_targetQueueHead->pHead = g_targetQueueHead->pTail = pTarget;
                }
                else
                {
                    g_targetQueueHead->pTail->next = pTarget;
                    g_targetQueueHead->pTail = g_targetQueueHead->pTail->next;
                }
                g_targetQueueHead->curTargetNum++;
                pTarget->pExpProcessor->run();
            }

            /*将消息放入该对象的消息队列中*/
            ret = pTarget->pExpProcessor->PushMessage(&msg);
            if(SUCC != ret)
            {
                pAckString = getAckStr(ret);
                TClient_putpkt(m_fd, srcAid, 0, 0, pAckString, strlen(pAckString));
            }
            g_targetQueueHead->pMtx->unlock();
            
        }
            break;
            
        default:
            sysLoger(LWARNING, "tsServer::main: invalid cmd:%c", recvBuf[0]);
            break;
        }        

    }
    
}

/**
 * @Funcname:  stop
 * @brief        :  线程的停止函数
 * @return      : 
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年10月9日 11:42:36
 *   
**/
T_VOID TsServer::stop()
{
    T_TSS_Target_Queue_Node *pTarget = NULL, *pTargetTmp = NULL;

    /*停止上报消息处理对象的运行*/
    RepProcessor::getInstance()->stop();
        
    /*停止所有表达式处理对象的运行*/
    g_targetQueueHead->pMtx->lock();
    pTarget = g_targetQueueHead->pHead;
    while(pTarget != NULL)
    {
        pTargetTmp = pTarget;
        pTarget = pTarget->next;
        delete pTargetTmp->pExpProcessor;
        free(pTargetTmp);
    }
    g_targetQueueHead->pMtx->unlock();

    /*删除临界对象*/
    DeleteCriticalSection(&g_cs);

    /*关闭连接*/
    TClient_close(m_fd);
}

/**
 * @Funcname:  registerToTsmap
 * @brief        :  向tsmap注册
 * @return      :   SUCC(成功)  FAIL(失败)
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_WORD TsServer::registerToTsmap()
{
    T_CHAR regBuf[200] = "\0";
    T_CHAR buf[10] = "\0";
    T_CHAR recvBuf[MAX_MESSAGE_LEN] = "\0";
    T_UHWORD srcAid, srcSaid, desAid, desSaid;
    T_WORD len, ret;
    
    /*注册消息格式：
    r:name;exclusive;regAid;regSaid;queryAid;querySaid;qrlen;queryStr;[datalen;data;]*/
    memset(regBuf, 0, sizeof(regBuf));
    sprintf(regBuf, "r:tsServerMap;1;%x;0;0;0;%x;tsServer;", m_fd, strlen("tsServer"));

    while(1)
    {
        TClient_putpkt(m_fd, TSMAP_AID, 0, 0, regBuf, strlen(regBuf));
        ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
        if(TS_DATACOMING != ret) 
        {
            return FAIL;
        }
        len = TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
        if((len < 0) || (TSMAP_AID != srcAid))
        {
            return FAIL;
        }
        else if ((strncmp(recvBuf, "OK;", 3))
                    && !(strncmp(recvBuf, "E04", 3)))
        {
            //已经注册,注销后重新注册
            ret = unRegisterToTsmap();
            if (FAIL == ret)
            {
                return FAIL;
            }
            continue;
        }   
        return SUCC;
    }

}

/**
 * @Funcname:  unRegisterToTsmap
 * @brief        :  向tsmap注销
 * @return      :   SUCC(成功)  FAIL(失败)
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_WORD TsServer::unRegisterToTsmap()
{
    T_CHAR regBuf[200] = "\0";
    T_CHAR buf[10] = "\0";
    T_CHAR *pBuf = NULL;
    T_CHAR recvBuf[MAX_MESSAGE_LEN] = "\0";
    T_UHWORD srcAid, srcSaid, desAid, desSaid;
    T_WORD len, ret;
    T_WORD regAid, regSaid;

    /*查询注册AID,消息格式:
    q:name;queryAid;querySaid;qrlen;queryStr;*/
    memset(regBuf, 0, sizeof(regBuf));
    sprintf(regBuf, "q:tsServerMap;%x;0;%x;tsServer;", m_fd, strlen("tsServer"));
    TClient_putpkt(m_fd, TSMAP_AID, 0, 0, regBuf, strlen(regBuf));
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(TS_DATACOMING != ret) 
    {
        return FAIL;
    }
    len = TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
    if((len < 0) ||(strncmp(recvBuf, "OK;", 3))||(TSMAP_AID != srcAid))
    {
        return FAIL;
    }
    
    //解析注册AID和注册SAID
    pBuf = recvBuf + strlen("OK;");
    hex2int(&pBuf, &regAid);
    pBuf += strlen(";");
    
    hex2int(&pBuf, &regSaid);
    

    /*注销消息格式：
    u:name;regAid;regSaid;queryAid;querySaid;qrlen;queryStr;*/
    memset(regBuf, 0, sizeof(regBuf));
    sprintf(regBuf, "u:tsServerMap;%x;%x;0;0;%x;tsServer;", regAid, regSaid, strlen("tsServer"));
    
    TClient_putpkt(m_fd, TSMAP_AID, 0, 0, regBuf, strlen(regBuf));
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(TS_DATACOMING != ret) 
    {
        return FAIL;
    }
    len = TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
    if((len < 0) ||(strncmp(recvBuf, "OK;", 3))||(TSMAP_AID != srcAid))
    {
        return FAIL;
    }

    //关闭已启动的tsServer
    TClient_putpkt(m_fd, regAid, 0, 0, "q", strlen("q"));

    return SUCC;
}

/**
 * @Funcname:  getLambdaPath
 * @brief        :  获取Lambda的路径
 * @return      :  m_lambdaPath :Lambda的路径
 * @Author     :  qingxh
 *  
 * @History: 1.  Created this function on 2009年11月3日
 *   
**/
T_CHAR* TsServer::getLambdaPath()
{
	return m_lambdaPath;
}

/**
 * @Funcname:  parseExpressionMessage
 * @brief        :  解析表达式求值消息的函数
 * @param[IN]: srcAid, 消息包源AID
 * @param[IN]: pMsgStr, 消息字符串
 * @param[IN]: len, 消息字符串长度
 * @param[OUT]: pMsg, 消息结构变量
 * @param[OUT]: targetName, 目标机名字
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_WORD TsServer::parseExpressionMessage
(
    T_UHWORD srcAid, 
    T_CHAR *pMsgStr, 
    T_WORD len, 
    T_TSS_Expression_Message *pMsg,
    T_CHAR *targetName
)
{
    T_CHAR *pBuf = pMsgStr, *pTmp = NULL;
    T_WORD i = 0;
    T_WORD saAid;

    /* 表达式求值操作 */
    if(pBuf[0] == 'e')
    {
        pMsg->cmdType = COMMAND_TYPE_EXPRESSION;
    }
    else if(pBuf[0] == 'o')
    {
        pMsg->cmdType = COMMAND_TYPE_OTHER;
    }

    /* 跳过命令字"e:"或者"o:" */
    pBuf++;
    pBuf++;
    
    /*获取发送端AID*/
    pMsg->caAid = srcAid;
    
    /*获取目标机saAid*/
    hex2int(&pBuf, &saAid);
    pMsg->saAid = saAid;

    /*获取目标机名字*/
    pBuf++;
    pTmp = strstr(pBuf, TSS_CONTROL_FLAG);
    if(NULL == pTmp)
    {
        return ERR_NOT_FOUND_IP;
    }
    strncpy(targetName, pBuf, pTmp-pBuf);
    pBuf = pTmp;
    pBuf++;
    
    if(*pBuf == '<')
    {
        //命令含有符号文件名
        pBuf++;
        pTmp = strstr(pBuf, ">");
        if(NULL == pTmp)
        {
            return ERR_NOT_FOUND_EXPRESSION;
        }
        /*获取表达式*/
        strcpy(pMsg->expression, pTmp+1);
        if (' ' != *(pTmp-1))
        {
            *pTmp = ' ';
        }        

        /*获取符号文件名,以空格作为符号文件间隔*/
        while(pTmp = strstr(pBuf, " "))
        {
            if(i >= MAX_SYSMBOL_FILE_NUM)
            {
                return ERR_TOO_MANY_SYMBOL_FILE;
            }
            *pTmp = '\0';
            sprintf(pMsg->symbolFiles[i++], "%s\\%s\\%s", g_envVar->symbolFilesPath,
                            targetName, pBuf);
            pBuf = pTmp + 1;      
        }
    }
    else
    {
        strcpy(pMsg->expression, pBuf);
    }
    
    return SUCC;    
}

/**
 * @Funcname:  getAckStr
 * @brief        :  通过错误号获取错误字符串
 * @param[IN]: eno 错误号
 * @return      : 错误字符串
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
char* getAckStr(int eno)
{
    int i = 0;
    
    for(i = 0; (errTbl[i].erNo) != 1; i++)
    {
        if((errTbl[i].erNo) == eno)
        {
            return (errTbl[i].erStr);
        }
    }
    
    return NULL;
}


