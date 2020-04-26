/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/

/**
 * @file           :  tsServer.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> 功能</li> 表达式处理主线程
                                 <li>设计思想</li> 
                                 <p> n     注意事项：n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008年10月6日 16:02:35
 * @History        : 
 *   
 */

/************************引用部分*****************************/
#include <afx.h>
#include "windows.h"
#include <process.h>
#include "Shlwapi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "tssConfig.h"
#include "tssComm.h"
#include "ExpProcessor.h"
#include "tClientAPI.h"
#include "tsServer.h"
#include "resoure.h"
#include "elf.h"
#include "translateGdbErrorInfo.h"
/************************前向声明部分***********************/
int readGdbResult
(
  HANDLE hFile,
  char* pBuffer,
  DWORD bufSize,
  DWORD* pReadSize,
  LPOVERLAPPED lpOverlapped
 );

/************************定义部分*****************************/
#define SDA_SAID 1
#define GDB_READ_FAIL -1

T_TSS_Version g_version;
/************************实现部分****************************/



/**
 * @Funcname:  ExpProcessor
 * @brief        : 构造函数
 * @param[IN]: saAid, 消息发向的目标机对应的saAid
 * @param[IN]: targetName, 目标机名字
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
ExpProcessor::ExpProcessor(T_UHWORD saAid, T_CHAR* targetSymbolFolderName){
    m_saAid = saAid;
    strcpy(m_targetSymbolFolderName, targetSymbolFolderName);
    m_maxMsgNum = MAX_MESSAGE_NUM;
    m_curMsgNum = 0; 
    m_hThread = NULL;
    m_threadID = 0;
    m_pSem = new waitCondition();
    m_pMtx = new TMutex(); 
    m_isRunning = 0;
    m_pMsgQueueHead = NULL;
    m_pMsgQueueTail = NULL;
    m_toolChainVersion = 0xFFFFFFFF;
    m_targetArchType = 0xFFFFFFFF;
}

/**
 * @Funcname:  ~ExpProcessor
 * @brief        :  析构函数，释放消息对象以及信号量、互斥量等对象
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
ExpProcessor::~ExpProcessor()
{
    T_TSS_Expression_Message *pMsg = NULL, *pMsgTmp = NULL;
    
    if(m_isRunning != 0)
    {
        TerminateThread(m_hThread, 0);  
    }
    
    TClient_close(m_fd);
    delete m_pSem;
    delete m_pMtx;
    pMsg = m_pMsgQueueHead;
    while(pMsg != NULL)
    {
        pMsgTmp = pMsg;
        pMsg = pMsg->next;
        free(pMsgTmp);
    }

    
}

/**
 * @Funcname:  PushMessage
 * @brief        :  向消息队列放入消息
 * @param[IN]: pMsg, 消息体
 * @return     : SUCC 成功
                     ERR_MESSAGE_QUEUE_FULL-消息队列满
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_WORD ExpProcessor::PushMessage(T_TSS_Expression_Message* pMsg)
{
    T_TSS_Expression_Message *pSrcMsg = pMsg, *pDestMsg = NULL;

    m_pMtx->lock();
    if(m_curMsgNum >= m_maxMsgNum)
    {
        return ERR_MESSAGE_QUEUE_FULL;
    }
    
    pDestMsg = (T_TSS_Expression_Message*)malloc(sizeof(T_TSS_Expression_Message));
    memset(pDestMsg, 0, sizeof(T_TSS_Expression_Message));
    *pDestMsg = *pSrcMsg;
    
    if(0 == m_curMsgNum)
    {
        m_pMsgQueueHead = m_pMsgQueueTail = pDestMsg;
    }
    else
    {
        m_pMsgQueueTail->next = pDestMsg;
        m_pMsgQueueTail = m_pMsgQueueTail->next;
    }
    m_curMsgNum++;
    m_pMtx->unlock();
    m_pSem->wakeup();

    return SUCC;
}

/**
 * @Funcname:  PopMessage
 * @brief        :  从消息队列取消息
 * @param[IN]: pMsg, 消息体
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_VOID ExpProcessor::PopMessage(T_TSS_Expression_Message** pMsg)
{
    m_pSem->wait();
    m_pMtx->lock();
    if(1 == m_curMsgNum)
    {
        *pMsg = m_pMsgQueueHead;
        m_pMsgQueueHead = m_pMsgQueueTail = NULL;
    }
    else
    {
        *pMsg = m_pMsgQueueHead;
        m_pMsgQueueHead = m_pMsgQueueHead->next;
    }       
    m_curMsgNum--;
    m_pMtx->unlock();
    
}

/**
 * @Funcname:  GetFd
 * @brief        :  获取连接句柄
 * @return      : 连接句柄
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_UHWORD ExpProcessor::GetFd()
{
    return m_fd;
}

/**
 * @Funcname:  GetCurMsgNum
 * @brief        :  获取当前消息数量
 * @return      : 当前消息数量
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_UWORD ExpProcessor::GetCurMsgNum()
{
    return m_curMsgNum;
}

/**
 * @Funcname:  GetSaAid
 * @brief        :  获取目标机saAid
 * @return      : 目标机saAid
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_UHWORD ExpProcessor::GetSaAid()
{
    return m_saAid;
}

/**
 * @Funcname:  GetTargetSymbolFolderName
 * @brief        :  获取目标机名字
 * @return      : 目标机名字
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_CHAR* ExpProcessor::GetTargetSymbolFolderName()
{
    return m_targetSymbolFolderName;
}

/**
 * @Funcname:  GetState
 * @brief        :  获取主线程当前状态
 * @return      : 主线程当前状态
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_UHWORD ExpProcessor::GetState()
{
    return m_isRunning;
}

/**
 * @Funcname:  run
 * @brief        :  主处理函数，创建连接并启动主线程
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_VOID ExpProcessor::run()
{
    if(!m_hThread)
    {
        m_fd = TClient_open(TS_IP, TS_PORT);
        if(m_fd <= 0)
        {
            sysLoger(LDEBUG, "ExpProcessor::run: connect ts fail, ret:%d", m_fd);
            return;
        }

        /*创建线程用于接收、处理上报消息*/
        m_hThread = (HANDLE)_beginthreadex(NULL,
                                    1024*10, 
                                    ExpProcessorThreadEntry, 
                                    (T_VOID*)this, 
                                    CREATE_SUSPENDED, 
                                    &m_threadID);
        if(!m_hThread)
        {
           sysLoger(LWARNING, "ExpProcessor::run: create process thread fail");
        }
    }

    if(!m_isRunning)
    {
        m_isRunning = 1;
        ResumeThread(m_hThread); 
    }
    
}

/**
 * @Funcname:  stop
 * @brief        :  停止主线程
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_VOID ExpProcessor::stop()
{
    if(m_isRunning)
    {
        SuspendThread(m_hThread);
    }
}

/**
 * @Funcname:  GetTargetArchType
 * @brief        :  获取目标机的体系结构
 * @Author     :  qingxh
 *  
 * @History: 1.  Created this function on 2009年11月3日
 *   
**/
T_WORD ExpProcessor::GetTargetArchType()
{
    ServerCfg targetServerConfig;
    COMCONFIG comConfig;
    T_WORD ret = 0;

    if(m_targetArchType == 0xFFFFFFFF)
    {
        /*查询目标机体系结构*/
        memset(&targetServerConfig, 0, sizeof(targetServerConfig));
        targetServerConfig.aid = m_saAid;
        targetServerConfig.pComConfig = &comConfig;
        ret = TClient_queryServer(m_fd, &targetServerConfig);
        if (!ret)
        {
            switch(targetServerConfig.extInfo[0])
            {
            case '0':
                m_targetArchType = TARGET_ARCH_X86;
                break;
            case '1':
                m_targetArchType = TARGET_ARCH_ARM;
                break;
            case '2':
                m_targetArchType = TARGET_ARCH_PPC;
                break;
            default:
                sysLoger(LWARNING, "GetTargetArchType: query target architecture fail");
                break;
            }
        }
        else
        {
            sysLoger(LWARNING, "GetTargetArchType: query target architecture fail");
            return 0xFFFFFFFF;
        }
    }

    return m_targetArchType;
}

/**
 * @Funcname:  GetTargetToolChainVersion
 * @brief        :  获取目标机的工具链版本
 * @Author     :  qingxh
 *  
 * @History: 1.  Created this function on 2009年11月3日
 *   
**/
T_WORD ExpProcessor::GetTargetToolChainVersion()
{
    T_UHWORD des_aid = 0;
    T_UHWORD src_aid = 0;
    T_UHWORD des_said = 0;
    T_UHWORD src_said = 0;
    T_WORD  cnt = 0;
    T_CHAR  cmdBuf[30]= {0};
    T_WORD  rc = 0;
    
    if(m_toolChainVersion == 0xFFFFFFFF)
    {
        cnt = sprintf_s(cmdBuf, sizeof(cmdBuf), "MT"); 
        
         //发送命令
        rc = TClient_putpkt(m_fd, m_saAid, DES_TA_MANAGER_ID, src_said, cmdBuf, cnt) ;
        if (rc < 0)
        {    
            return 0xFFFFFFFF;     
        }    
        
        //等待结果
        rc = TClient_wait(m_fd,TS_DEFAULT_TIMEOUT);
        if (rc < 0)
        {
            return 0xFFFFFFFF;
        }
    
        //接收结果
        rc = TClient_getpkt(m_fd, &src_aid, &des_said, &src_said, cmdBuf, sizeof(cmdBuf)) ;
        if(rc < 0)
        {
            return 0xFFFFFFFF;
        }
    
        //设置路径
        if ( (cmdBuf[0] == 'O') && (cmdBuf[1] == 'K') )
        {
             /*目标机工具链为2.96*/
            if ( cmdBuf[2] == '0')
            {
                m_toolChainVersion = TOOL_CHAIN_VER_296;
    
            }
               
            /*目标机工具链为3.4.4*/
            if ( cmdBuf[2]== '1' )
            {
                m_toolChainVersion = TOOL_CHAIN_VER_344;
            }
       }
    }

    return m_toolChainVersion;
}

bool ExpProcessor::handleTarget_1X(T_TSS_Expression_Message *pMsg )
{
	T_CHAR recvBuf[MAX_MESSAGE_LEN] = "\0";
	T_CHAR *pRecvBuf = NULL;
	T_UHWORD srcAid, srcSaid, desAid, desSaid;
    T_WORD ret, len;
	T_UHWORD fd;
	T_CHAR *pAckString = NULL;
	HANDLE hThread;
    T_UWORD threadID;
    T_TSS_EXP_PROC_ARGV param;
	fd = GetFd();
    pRecvBuf = recvBuf;
	 /*获取目标机当前状态*/
	TClient_putpkt(fd, pMsg->saAid, QUERY_TARGET_STATUS_AGENT_1X, 0, 
                        TARGET_STATUS_QUERY_STRING, strlen(TARGET_STATUS_QUERY_STRING));    
    ret = TClient_wait(fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
         sysLoger(LWARNING, "handleTarget_1X: wait target status fail, ret:%d", ret);
		 pAckString = "错误:获取目标机当前状态失败";
         TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
         return false;
     }
        
     len = TClient_getpkt(fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
     if(len < 0) 
      {
          sysLoger(LWARNING, "handleTarget_1X: getpkt target status fail, ret:%d", ret);
          pAckString = "错误:获取目标机当前状态失败";
          TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
          return false;
      }
      if(strncmp(recvBuf, "OK;", 3))
      {
          sysLoger(LWARNING, "handleTarget_1X: target status error, status:%s", recvBuf);
          pAckString = "错误:获取目标机当前状态失败";
          TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
          return false;
      }
      pRecvBuf += 3;

      if(!strncmp(pRecvBuf, "system", strlen("system")))
      {
          /*目标机处于系统级调试模式*/
          pRecvBuf = strstr(pRecvBuf, ";");
          pRecvBuf++;
          /*目标机处于暂停状态*/
          if(!strncmp(pRecvBuf, "suspend", strlen("suspend")))
          {
				 sysLoger(LDEBUG, "handleTarget_1X: system debug mode and suspend");
				 param.pExpProcessor = this;
				 param.pMsg = pMsg;
				 param.mode = TARGET_DEBUG_MODE_SYSTEM;
				 handleExpMessage((void*)&param);

		  }
            /*目标机处于运行状态*/
            else
            {
				sysLoger(LDEBUG, "handleTarget_1X: system debug mode and continue");

				/*先暂停目标机，执行表达式求值（函数调用），再恢复目标机运行*/
			   TClient_putpkt(fd, pMsg->saAid, QUERY_TARGET_STATUS_AGENT_1X, 0, 
					TARGET_TASK_STOP, strlen(TARGET_TASK_STOP)); 

				ret = TClient_wait(fd, DEFAULT_WAIT_TIME);
				if(TS_DATACOMING != ret) 
			    {
					sysLoger(LWARNING, "handleTarget_1X: wait stop target fail, ret:%d", ret);
					pAckString = "错误:暂停目标机失败";
					TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
					 return false;
				}

				len = TClient_getpkt(fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
			    if(len < 0) 
				{
					sysLoger(LWARNING, "handleTarget_1X: getpkt stop target fail, ret:%d", ret);
					pAckString = "错误:暂停目标机失败";
					TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
					 return false;
				 }
					
				if(strncmp(recvBuf, "S05", strlen("S05")))
				{
					sysLoger(LWARNING, "handleTarget_1X: getpkt stop target fail recv:%s", recvBuf);
					pAckString = "错误:暂停目标机失败";
					TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
					 return false;
				 }

				  T_UHWORD tmp_saAid = pMsg->saAid;
				  /*执行表达式求值（函数调用）*/
				  param.pExpProcessor = this;
				  param.pMsg = pMsg;
				  param.mode = TARGET_DEBUG_MODE_SYSTEM;
				  handleExpMessage((void*)&param);

				  /*执行表达式求值（函数调用）完之后，恢复运行*/

				  TClient_putpkt(fd, tmp_saAid, QUERY_TARGET_STATUS_AGENT_1X, 0, 
					TARGET_TASK_CONTINUE, strlen(TARGET_TASK_CONTINUE)); 
				}
        }
        else
        { 
            /*目标机处于任务级调试模式*/
            sysLoger(LDEBUG, "handleTarget_1X: task debug mode");
            
            param.pExpProcessor = this;
            param.pMsg = pMsg;
            param.mode = TARGET_DEBUG_MODE_TASK;
            hThread = (HANDLE)_beginthreadex(NULL,
                                1024*10, 
                                handleExpMessage, 
                                &param, 
                                CREATE_SUSPENDED, 
                                &threadID);
            if(NULL == hThread)
            {
                sysLoger(LWARNING, "handleTarget_1X: create proccess expression thread fail");
                pAckString = "错误:创建任务级状态执行表达式线程失败";
                TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
                 return false;
            }

            //设置为高优先级
            SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);

            //恢复任务
            ResumeThread(hThread);
            
            CloseHandle(hThread);

		}
	return true;
}

bool ExpProcessor::handleTarget_2X(T_TSS_Expression_Message *pMsg )
{
   T_TSS_EXP_PROC_ARGV param;

	/*在shell中已经判断了C2系统级是否是暂停状态，所以在这里不用再判断*/
	param.pExpProcessor = this;
	param.pMsg = pMsg;
	param.mode = TARGET_DEBUG_MODE_SYSTEM;
	handleExpMessage((void*)&param);

	return true;
}
/**
 * @Funcname: ExpProcessorThreadEntry
 * @brief        : ExpProcessor类的主线程入口函数
 * @param[IN] : 线程参数
 * @return      : 线程退出码
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 16:34:11
 *   
**/
T_UWORD __stdcall ExpProcessorThreadEntry(void *pParam)
{
    ExpProcessor* pExpProcessor = (ExpProcessor*)pParam;
    T_TSS_Expression_Message *pMsg = NULL;
    T_CHAR recvBuf[MAX_MESSAGE_LEN] = "\0", *pRecvBuf = NULL;
    T_CHAR *pAckString = NULL;
    T_UHWORD fd;
    T_UHWORD srcAid, srcSaid, desAid, desSaid;
    T_WORD ret, len;
    T_TSS_EXP_PROC_ARGV param;
    HANDLE hThread;
    T_UWORD threadID;
	T_CHAR recvPbuf[MAX_MESSAGE_LEN] = "\0";
    fd = pExpProcessor->GetFd();
    int temp;
	int qAid = 0;

    while(1)
    {
        pExpProcessor->PopMessage(&pMsg);

		/*获取目标机OS版本*/
        TClient_putpkt(fd, pMsg->saAid, 0, 0, 
                        TARGET_MODE_QUERY_STRING, strlen(TARGET_MODE_QUERY_STRING));    
        TClient_wait(fd, DEFAULT_WAIT_TIME);

		TClient_getpkt(fd, &srcAid, &desSaid, &srcSaid, recvPbuf, MAX_MESSAGE_LEN);

		/*比较回复的字符串,判断是1X还是2X*/
		temp = strcmp(recvPbuf, "OK2");
		if(0 == temp)
		{
			g_version = SYS_VERSION_2X;
			pExpProcessor->handleTarget_2X(pMsg);
			}
		else
		{
			g_version = SYS_VERSION_1X;
			pExpProcessor->handleTarget_1X(pMsg);
		}

    }   
    return SUCC;
}

/**
 * @Funcname:  handleExpMessage
 * @brief        : 表达式求值消息处理函数，同时也作为任务级
                       模式时子线程的入口函数
 * @param[IN]: pParam, 包含对象指针、消息体和目标机状态的参数
 * @return      : SUCC/FAIL
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
T_UWORD __stdcall handleExpMessage(void *pParam)
{
    ExpProcessor* pExpProcessor = ((T_TSS_EXP_PROC_ARGV*)pParam)->pExpProcessor;
    T_TSS_Expression_Message *pMsg = (((T_TSS_EXP_PROC_ARGV*)pParam)->pMsg);
    T_TSS_Target_Debug_Mode mode = ((T_TSS_EXP_PROC_ARGV*)pParam)->mode;  
    HANDLE hChildStdinWrDup = NULL;
    HANDLE hChildStdoutRdDup = NULL;
    T_CHAR rwBuf[TSS_GDB_RW_BUF_LEN] = "\0";
    T_CHAR *pRwBuf = rwBuf;
    T_CHAR *pTargetSymbolFolderName = NULL;
    T_CHAR *pAckString = NULL;
    T_CHAR *pResult = NULL;
    T_WORD fd;
    T_WORD dllAid = FAIL;
    T_WORD dllPort, daSaid;
    T_WORD ret, i, isLoadFail;
    DWORD writtenLen, readLen;
    DWORD err = 0;
    T_TSS_Tda *pTdaItem = NULL;
    T_TSS_Gdb* pGdbItem = NULL;

    pTargetSymbolFolderName = pExpProcessor->GetTargetSymbolFolderName();
    do
    {
        fd = TClient_open(TS_IP, TS_PORT);
        if(fd <= 0)
        {
            sysLoger(LERROR, "handleExpMessage: connect ts fail, ret:%d", fd);
            break;
        }

        sysLoger(LDEBUG, "handleExpMessage: connect ts succ, fd:%d", fd);
        
        if(mode == TARGET_DEBUG_MODE_TASK) 
        {
			if(SYS_VERSION_1X == g_version)
			{
				//任务级调试模式,创建TDA
	            pTdaItem = resourceManager::getInstance()->createTda(pMsg->saAid);
	            if (NULL == pTdaItem)
	            {
					pAckString = "错误:创建TDA失败";
	                break;
	            }
	            daSaid = pTdaItem->tdaSaid;
			}
			else
			{
				daSaid = QUERY_TARGET_STATUS_AGENT_2X;
			}
					
        }
        else
        {
            //系统级调试,使用SDA
			if(SYS_VERSION_1X == g_version)
			{
				daSaid = QUERY_TARGET_STATUS_AGENT_1X;
			}
			else
			{
				daSaid = QUERY_TARGET_STATUS_AGENT_2X;
			}
        }

        //获取rsp server
        dllAid = resourceManager::getInstance()->allocTraServer(pMsg->saAid, daSaid);
        if (FAIL == dllAid)
        {
            pAckString = "错误:分配rsp服务失败";
            break;
        }
        
        //获取rsp server监听端口
        dllPort = resourceManager::getInstance()->getDllPort(dllAid);
        if (FAIL == dllPort)
        {
            pAckString = "错误:获取rsp服务端口失败";
            break;
        }

        sysLoger(LDEBUG, "handleExpMessage: active rsp server succ: aid:%d, port:%d", 
                                                                                                        dllAid, dllPort);

        //创建GDB
        pGdbItem = resourceManager::getInstance()->createGdb(pMsg->saAid);
        if (NULL == pGdbItem)
        {
            pAckString = "错误:创建GDB进程失败";
            break;
        }
        hChildStdoutRdDup = pGdbItem->hStdoutRdDup;
        hChildStdinWrDup = pGdbItem->hStdinWrDup;

        /*读取启动后的打印信息*/
        ret = readGdbResult(hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, &readLen, NULL);
        if (ret < 0)
        {
            sysLoger(LWARNING, "handleExpMessage: start gdb fail");
            pAckString = "错误:gdb启动失败";
            break;
        }

        /*向gdb装入符号文件*/
        isLoadFail = 0;
        i = 0;
        if(!strcmp(pMsg->symbolFiles[0], "")) //表示表达式中没有限定符号文件，则加入与该目标机对应的全部符号文件
        {
            CFileFind fileFind;
            char fileFindName[MAX_PATH_LEN] = "\0";
            char foundFileName[MAX_PATH_LEN] = "\0";
            int isFound;
            bool isFirstSymbol = true;

            sysLoger(LINFO, "handleExpMessage: user not input symbol file");
                
            sprintf(fileFindName, "%s\\%s\\*.*", g_envVar->symbolFilesPath, pTargetSymbolFolderName);
            isFound=fileFind.FindFile(fileFindName);
            while(isFound)
            {
                isFound=fileFind.FindNextFile();
                if(!fileFind.IsDots())
                {
                    if(!fileFind.IsDirectory())
                    {
                        if (isFirstSymbol)
                        {
                            sprintf(pRwBuf, "file %s\\%s\\%s\n", g_envVar->symbolFilesPath, pTargetSymbolFolderName,
                                                (char*)fileFind.GetFileName().GetBuffer(MAX_PATH_LEN));
                            isFirstSymbol = false;
                        }
                        else
                        {
                            T_WORD ret = 0;
                            T_UWORD address = 0;
                            T_CHAR symbolFilePath[MAX_PATH] = {0};
                            
                            /*获取符号表路径*/
                            sprintf(symbolFilePath, "%s\\%s\\%s", g_envVar->symbolFilesPath, pTargetSymbolFolderName,
                                                (char*)fileFind.GetFileName().GetBuffer(MAX_PATH_LEN));

                            /*解析ELF文件,得到TXT段地址*/
                            ret = fnELF_Get_Seg_Address(symbolFilePath, ".text", &address);
                            if(ret != SUCC)
                            {
                                sysLoger(LWARNING, "Get symbol file .text start address fail");
                                pAckString = "错误:gdb加载符号表失败";
                                break;
                            }

                            sysLoger(LDEBUG, "Get .text start address: 0x%x", address);

                             /*向GDB添加符号表*/
                             sprintf(pRwBuf, "add-symbol-file %s 0x%x\n", symbolFilePath, address);
                        }
                        sysLoger(LDEBUG, "handleExpMessage: find symbol:%s", pRwBuf);
                        ExtendESC(pRwBuf, TSS_GDB_RW_BUF_LEN);
                        ret = WriteFile(hChildStdinWrDup, pRwBuf, strlen(pRwBuf), &writtenLen, NULL);
                        if(!ret)
                        {
                            isLoadFail = 1;
                            break;
                        }
                        
                        /*读取加载文件后的打印信息*/
                        ret = readGdbResult(hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, 
                                                                                                &readLen, NULL);
                        if (ret < 0)
                        {
                            sysLoger(LWARNING, "handleExpMessage: read gdb load symbol file fail");
                            pAckString = "错误:读取gdb加载文件后的打印信息";
                            break;
                        }
                        sysLoger(LDEBUG, "handleExpMessage: gdb load symbol result:%s", pRwBuf);
                    }
                }
            }
            fileFind.Close();
        }
        else //仅装入命令中限定的符号表
        {
            sysLoger(LINFO, "handleExpMessage: use user symbol file");
            while((i < MAX_SYSMBOL_FILE_NUM) && strcmp(pMsg->symbolFiles[i], ""))
            {   
                if(!CheckFileExist(pMsg->symbolFiles[i]))
                {
                    isLoadFail = 1;
                    break;
                }
                sprintf(pRwBuf, "file %s\n", pMsg->symbolFiles[i]);
                sysLoger(LDEBUG, "handleExpMessage: user symbol:%s", pRwBuf);
                ExtendESC(pRwBuf, TSS_GDB_RW_BUF_LEN);
                ret = WriteFile(hChildStdinWrDup, pRwBuf, strlen(pRwBuf), &writtenLen, NULL);
                if(!ret)
                {
                    isLoadFail = 1;
                    break;
                }                
                i++;
                
                /*读取加载文件后的打印信息*/
                ret = readGdbResult(hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, 
                                                                                    &readLen, NULL);
                if (ret < 0)
                {
                    sysLoger(LWARNING, "handleExpMessage: read gdb load symbol file fail");
                    pAckString = "错误:读取gdb加载文件后的打印信息";
                    break;
                }
                sysLoger(LDEBUG, "handleExpMessage: gdb load symbol:%s", pRwBuf);
            }
        }
        
        if(isLoadFail)
        {
            sysLoger(LWARNING, "handleExpMessage: gdb load symbol file fail");
            pAckString = "错误:gdb加载文件失败";
            break;
        }

        /*建立gdb和rsp适配器的连接*/
        sprintf(pRwBuf, "target remote 127.0.0.1:%d\n", dllPort);
        ret = WriteFile(hChildStdinWrDup, pRwBuf, strlen(pRwBuf), &writtenLen, NULL);
        if(!ret)
        {
            sysLoger(LWARNING, "handleExpMessage: gdb connect to target fail");
            pAckString = "错误:gdb链接目标机错误";
            break;
        }
      
        /*读取建立连接后的打印信息*/
        ret = readGdbResult( hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, &readLen, NULL);
        if (ret < 0)
        {
            sysLoger(LWARNING, "handleExpMessage: read gdb connect to target fail");
            pAckString = "错误:读取GDB建立连接后的打印信息";
            break;
        }
        sysLoger(LDEBUG, "handleExpMessage: GDB connect target :%s", pRwBuf);

        //恢复表达式空格
        ResumeBlank(pMsg->expression);
      
        /*根据当前操作命令向gdb写入表达式求值命令*/
        if( pMsg->cmdType == COMMAND_TYPE_EXPRESSION )
        {
            sprintf(pRwBuf, "print %s\n", pMsg->expression);
        }
        else if( pMsg->cmdType == COMMAND_TYPE_OTHER )
        {
            //sprintf(pRwBuf, "x/10i %s\n", pMsg->expression);
            //sprintf(pRwBuf, "disassemble %s\n", pMsg->expression);
            sprintf(pRwBuf, "%s\n", pMsg->expression);
        }


        //sysLoger(LINFO, "handleExpMessage: process expression:%s", pMsg->expression);
        ret = WriteFile(hChildStdinWrDup, pRwBuf, strlen(pRwBuf), &writtenLen, NULL);
        if(!ret)
        {
            sysLoger(LWARNING, "handleExpMessage: gdb print fail");
            pAckString = "错误:gdb打印信息错误";
            break;
        }

        /*从gdb读取表达式求值的结果*/
        ret = readGdbResult( hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, &readLen, NULL);
        if(!ret)
        {
            sysLoger(LWARNING, "handleExpMessage: gdb get print result fail");
            pAckString = "错误:gdb获取结果失败";
            break;
        }

        if ((strstr(pRwBuf, "warning:")) && (pResult = strchr(pRwBuf, '$')))
        {
            //过滤"warning:"
            pRwBuf = pResult;
        }
		TranslateGdbErrorInfo::getInstance()->translateErrorInfo(pRwBuf,TSS_GDB_RW_BUF_LEN);
        sysLoger(LINFO, "handleExpMessage: get process expression result OK");
        
        /*将读取到的表达式执行结果回复给TSClient*/
        TClient_putpkt(fd, pMsg->caAid, 0, 0, pRwBuf, strlen(pRwBuf));

    }while (0);


    /*发送信息到shell agent 恢复任务运行*/
    TClient_putpkt(fd, pMsg->saAid, daSaid, 0, "ersm", strlen("ersm"));


    if (NULL != pAckString)
    {
        //发送错误回复信息
        TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
    }

    //删除TDA
    if (NULL != pTdaItem)
    {
        resourceManager::getInstance()->deleteTda(pTdaItem);
    }

    //释放rsp server
    if (FAIL != dllAid)
    {
        resourceManager::getInstance()->freeTraServer(dllAid);
    }

    //关闭gdb子进程
    if (NULL != pGdbItem)
    {
        resourceManager::getInstance()->deleteGdb(pGdbItem);
    }

    TClient_close(fd);

    free(pMsg);
    pMsg = NULL;
    return SUCC;
}

int readGdbResult
(
    HANDLE hFile,
    char* pBuffer,
    DWORD bufSize,
    DWORD* pReadSize,
    LPOVERLAPPED lpOverlapped
 )
{
    char *pBuf = pBuffer;
    char *pStr = NULL;
    DWORD size = 0;
    DWORD readSize = 0; 
    int ret = 0;
    
    memset(pBuffer, 0, bufSize);
    
    while (1)
    {
        ret = ReadFile(hFile, pBuf+size, MAX_MESSAGE_LEN-size, &readSize, lpOverlapped);
        if (0 == ret)
        {
            return GDB_READ_FAIL;
        }

        size += readSize;

        if (pStr = strstr(pBuffer, "\n(gdb)"))
        {
            //已读取完
            *pStr = '\0';
            *pReadSize = strlen(pBuffer);
            return ret;
        }
    }
}

