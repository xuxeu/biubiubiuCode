/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/

/**
 * @file           :  tsServer.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> ����</li> ���ʽ�������߳�
                                 <li>���˼��</li> 
                                 <p> n     ע�����n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008��10��6�� 16:02:35
 * @History        : 
 *   
 */

/************************���ò���*****************************/
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
/************************ǰ����������***********************/
int readGdbResult
(
  HANDLE hFile,
  char* pBuffer,
  DWORD bufSize,
  DWORD* pReadSize,
  LPOVERLAPPED lpOverlapped
 );

/************************���岿��*****************************/
#define SDA_SAID 1
#define GDB_READ_FAIL -1

T_TSS_Version g_version;
/************************ʵ�ֲ���****************************/



/**
 * @Funcname:  ExpProcessor
 * @brief        : ���캯��
 * @param[IN]: saAid, ��Ϣ�����Ŀ�����Ӧ��saAid
 * @param[IN]: targetName, Ŀ�������
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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
 * @brief        :  �����������ͷ���Ϣ�����Լ��ź������������ȶ���
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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
 * @brief        :  ����Ϣ���з�����Ϣ
 * @param[IN]: pMsg, ��Ϣ��
 * @return     : SUCC �ɹ�
                     ERR_MESSAGE_QUEUE_FULL-��Ϣ������
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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
 * @brief        :  ����Ϣ����ȡ��Ϣ
 * @param[IN]: pMsg, ��Ϣ��
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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
 * @brief        :  ��ȡ���Ӿ��
 * @return      : ���Ӿ��
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
 *   
**/
T_UHWORD ExpProcessor::GetFd()
{
    return m_fd;
}

/**
 * @Funcname:  GetCurMsgNum
 * @brief        :  ��ȡ��ǰ��Ϣ����
 * @return      : ��ǰ��Ϣ����
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
 *   
**/
T_UWORD ExpProcessor::GetCurMsgNum()
{
    return m_curMsgNum;
}

/**
 * @Funcname:  GetSaAid
 * @brief        :  ��ȡĿ���saAid
 * @return      : Ŀ���saAid
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
 *   
**/
T_UHWORD ExpProcessor::GetSaAid()
{
    return m_saAid;
}

/**
 * @Funcname:  GetTargetSymbolFolderName
 * @brief        :  ��ȡĿ�������
 * @return      : Ŀ�������
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
 *   
**/
T_CHAR* ExpProcessor::GetTargetSymbolFolderName()
{
    return m_targetSymbolFolderName;
}

/**
 * @Funcname:  GetState
 * @brief        :  ��ȡ���̵߳�ǰ״̬
 * @return      : ���̵߳�ǰ״̬
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
 *   
**/
T_UHWORD ExpProcessor::GetState()
{
    return m_isRunning;
}

/**
 * @Funcname:  run
 * @brief        :  �����������������Ӳ��������߳�
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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

        /*�����߳����ڽ��ա������ϱ���Ϣ*/
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
 * @brief        :  ֹͣ���߳�
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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
 * @brief        :  ��ȡĿ�������ϵ�ṹ
 * @Author     :  qingxh
 *  
 * @History: 1.  Created this function on 2009��11��3��
 *   
**/
T_WORD ExpProcessor::GetTargetArchType()
{
    ServerCfg targetServerConfig;
    COMCONFIG comConfig;
    T_WORD ret = 0;

    if(m_targetArchType == 0xFFFFFFFF)
    {
        /*��ѯĿ�����ϵ�ṹ*/
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
 * @brief        :  ��ȡĿ����Ĺ������汾
 * @Author     :  qingxh
 *  
 * @History: 1.  Created this function on 2009��11��3��
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
        
         //��������
        rc = TClient_putpkt(m_fd, m_saAid, DES_TA_MANAGER_ID, src_said, cmdBuf, cnt) ;
        if (rc < 0)
        {    
            return 0xFFFFFFFF;     
        }    
        
        //�ȴ����
        rc = TClient_wait(m_fd,TS_DEFAULT_TIMEOUT);
        if (rc < 0)
        {
            return 0xFFFFFFFF;
        }
    
        //���ս��
        rc = TClient_getpkt(m_fd, &src_aid, &des_said, &src_said, cmdBuf, sizeof(cmdBuf)) ;
        if(rc < 0)
        {
            return 0xFFFFFFFF;
        }
    
        //����·��
        if ( (cmdBuf[0] == 'O') && (cmdBuf[1] == 'K') )
        {
             /*Ŀ���������Ϊ2.96*/
            if ( cmdBuf[2] == '0')
            {
                m_toolChainVersion = TOOL_CHAIN_VER_296;
    
            }
               
            /*Ŀ���������Ϊ3.4.4*/
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
	 /*��ȡĿ�����ǰ״̬*/
	TClient_putpkt(fd, pMsg->saAid, QUERY_TARGET_STATUS_AGENT_1X, 0, 
                        TARGET_STATUS_QUERY_STRING, strlen(TARGET_STATUS_QUERY_STRING));    
    ret = TClient_wait(fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
         sysLoger(LWARNING, "handleTarget_1X: wait target status fail, ret:%d", ret);
		 pAckString = "����:��ȡĿ�����ǰ״̬ʧ��";
         TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
         return false;
     }
        
     len = TClient_getpkt(fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
     if(len < 0) 
      {
          sysLoger(LWARNING, "handleTarget_1X: getpkt target status fail, ret:%d", ret);
          pAckString = "����:��ȡĿ�����ǰ״̬ʧ��";
          TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
          return false;
      }
      if(strncmp(recvBuf, "OK;", 3))
      {
          sysLoger(LWARNING, "handleTarget_1X: target status error, status:%s", recvBuf);
          pAckString = "����:��ȡĿ�����ǰ״̬ʧ��";
          TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
          return false;
      }
      pRecvBuf += 3;

      if(!strncmp(pRecvBuf, "system", strlen("system")))
      {
          /*Ŀ�������ϵͳ������ģʽ*/
          pRecvBuf = strstr(pRecvBuf, ";");
          pRecvBuf++;
          /*Ŀ���������ͣ״̬*/
          if(!strncmp(pRecvBuf, "suspend", strlen("suspend")))
          {
				 sysLoger(LDEBUG, "handleTarget_1X: system debug mode and suspend");
				 param.pExpProcessor = this;
				 param.pMsg = pMsg;
				 param.mode = TARGET_DEBUG_MODE_SYSTEM;
				 handleExpMessage((void*)&param);

		  }
            /*Ŀ�����������״̬*/
            else
            {
				sysLoger(LDEBUG, "handleTarget_1X: system debug mode and continue");

				/*����ͣĿ�����ִ�б��ʽ��ֵ���������ã����ٻָ�Ŀ�������*/
			   TClient_putpkt(fd, pMsg->saAid, QUERY_TARGET_STATUS_AGENT_1X, 0, 
					TARGET_TASK_STOP, strlen(TARGET_TASK_STOP)); 

				ret = TClient_wait(fd, DEFAULT_WAIT_TIME);
				if(TS_DATACOMING != ret) 
			    {
					sysLoger(LWARNING, "handleTarget_1X: wait stop target fail, ret:%d", ret);
					pAckString = "����:��ͣĿ���ʧ��";
					TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
					 return false;
				}

				len = TClient_getpkt(fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
			    if(len < 0) 
				{
					sysLoger(LWARNING, "handleTarget_1X: getpkt stop target fail, ret:%d", ret);
					pAckString = "����:��ͣĿ���ʧ��";
					TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
					 return false;
				 }
					
				if(strncmp(recvBuf, "S05", strlen("S05")))
				{
					sysLoger(LWARNING, "handleTarget_1X: getpkt stop target fail recv:%s", recvBuf);
					pAckString = "����:��ͣĿ���ʧ��";
					TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
					 return false;
				 }

				  T_UHWORD tmp_saAid = pMsg->saAid;
				  /*ִ�б��ʽ��ֵ���������ã�*/
				  param.pExpProcessor = this;
				  param.pMsg = pMsg;
				  param.mode = TARGET_DEBUG_MODE_SYSTEM;
				  handleExpMessage((void*)&param);

				  /*ִ�б��ʽ��ֵ���������ã���֮�󣬻ָ�����*/

				  TClient_putpkt(fd, tmp_saAid, QUERY_TARGET_STATUS_AGENT_1X, 0, 
					TARGET_TASK_CONTINUE, strlen(TARGET_TASK_CONTINUE)); 
				}
        }
        else
        { 
            /*Ŀ����������񼶵���ģʽ*/
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
                pAckString = "����:��������״ִ̬�б��ʽ�߳�ʧ��";
                TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
                 return false;
            }

            //����Ϊ�����ȼ�
            SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);

            //�ָ�����
            ResumeThread(hThread);
            
            CloseHandle(hThread);

		}
	return true;
}

bool ExpProcessor::handleTarget_2X(T_TSS_Expression_Message *pMsg )
{
   T_TSS_EXP_PROC_ARGV param;

	/*��shell���Ѿ��ж���C2ϵͳ���Ƿ�����ͣ״̬�����������ﲻ�����ж�*/
	param.pExpProcessor = this;
	param.pMsg = pMsg;
	param.mode = TARGET_DEBUG_MODE_SYSTEM;
	handleExpMessage((void*)&param);

	return true;
}
/**
 * @Funcname: ExpProcessorThreadEntry
 * @brief        : ExpProcessor������߳���ں���
 * @param[IN] : �̲߳���
 * @return      : �߳��˳���
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 16:34:11
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

		/*��ȡĿ���OS�汾*/
        TClient_putpkt(fd, pMsg->saAid, 0, 0, 
                        TARGET_MODE_QUERY_STRING, strlen(TARGET_MODE_QUERY_STRING));    
        TClient_wait(fd, DEFAULT_WAIT_TIME);

		TClient_getpkt(fd, &srcAid, &desSaid, &srcSaid, recvPbuf, MAX_MESSAGE_LEN);

		/*�Ƚϻظ����ַ���,�ж���1X����2X*/
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
 * @brief        : ���ʽ��ֵ��Ϣ��������ͬʱҲ��Ϊ����
                       ģʽʱ���̵߳���ں���
 * @param[IN]: pParam, ��������ָ�롢��Ϣ���Ŀ���״̬�Ĳ���
 * @return      : SUCC/FAIL
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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
				//���񼶵���ģʽ,����TDA
	            pTdaItem = resourceManager::getInstance()->createTda(pMsg->saAid);
	            if (NULL == pTdaItem)
	            {
					pAckString = "����:����TDAʧ��";
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
            //ϵͳ������,ʹ��SDA
			if(SYS_VERSION_1X == g_version)
			{
				daSaid = QUERY_TARGET_STATUS_AGENT_1X;
			}
			else
			{
				daSaid = QUERY_TARGET_STATUS_AGENT_2X;
			}
        }

        //��ȡrsp server
        dllAid = resourceManager::getInstance()->allocTraServer(pMsg->saAid, daSaid);
        if (FAIL == dllAid)
        {
            pAckString = "����:����rsp����ʧ��";
            break;
        }
        
        //��ȡrsp server�����˿�
        dllPort = resourceManager::getInstance()->getDllPort(dllAid);
        if (FAIL == dllPort)
        {
            pAckString = "����:��ȡrsp����˿�ʧ��";
            break;
        }

        sysLoger(LDEBUG, "handleExpMessage: active rsp server succ: aid:%d, port:%d", 
                                                                                                        dllAid, dllPort);

        //����GDB
        pGdbItem = resourceManager::getInstance()->createGdb(pMsg->saAid);
        if (NULL == pGdbItem)
        {
            pAckString = "����:����GDB����ʧ��";
            break;
        }
        hChildStdoutRdDup = pGdbItem->hStdoutRdDup;
        hChildStdinWrDup = pGdbItem->hStdinWrDup;

        /*��ȡ������Ĵ�ӡ��Ϣ*/
        ret = readGdbResult(hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, &readLen, NULL);
        if (ret < 0)
        {
            sysLoger(LWARNING, "handleExpMessage: start gdb fail");
            pAckString = "����:gdb����ʧ��";
            break;
        }

        /*��gdbװ������ļ�*/
        isLoadFail = 0;
        i = 0;
        if(!strcmp(pMsg->symbolFiles[0], "")) //��ʾ���ʽ��û���޶������ļ�����������Ŀ�����Ӧ��ȫ�������ļ�
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
                            
                            /*��ȡ���ű�·��*/
                            sprintf(symbolFilePath, "%s\\%s\\%s", g_envVar->symbolFilesPath, pTargetSymbolFolderName,
                                                (char*)fileFind.GetFileName().GetBuffer(MAX_PATH_LEN));

                            /*����ELF�ļ�,�õ�TXT�ε�ַ*/
                            ret = fnELF_Get_Seg_Address(symbolFilePath, ".text", &address);
                            if(ret != SUCC)
                            {
                                sysLoger(LWARNING, "Get symbol file .text start address fail");
                                pAckString = "����:gdb���ط��ű�ʧ��";
                                break;
                            }

                            sysLoger(LDEBUG, "Get .text start address: 0x%x", address);

                             /*��GDB��ӷ��ű�*/
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
                        
                        /*��ȡ�����ļ���Ĵ�ӡ��Ϣ*/
                        ret = readGdbResult(hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, 
                                                                                                &readLen, NULL);
                        if (ret < 0)
                        {
                            sysLoger(LWARNING, "handleExpMessage: read gdb load symbol file fail");
                            pAckString = "����:��ȡgdb�����ļ���Ĵ�ӡ��Ϣ";
                            break;
                        }
                        sysLoger(LDEBUG, "handleExpMessage: gdb load symbol result:%s", pRwBuf);
                    }
                }
            }
            fileFind.Close();
        }
        else //��װ���������޶��ķ��ű�
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
                
                /*��ȡ�����ļ���Ĵ�ӡ��Ϣ*/
                ret = readGdbResult(hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, 
                                                                                    &readLen, NULL);
                if (ret < 0)
                {
                    sysLoger(LWARNING, "handleExpMessage: read gdb load symbol file fail");
                    pAckString = "����:��ȡgdb�����ļ���Ĵ�ӡ��Ϣ";
                    break;
                }
                sysLoger(LDEBUG, "handleExpMessage: gdb load symbol:%s", pRwBuf);
            }
        }
        
        if(isLoadFail)
        {
            sysLoger(LWARNING, "handleExpMessage: gdb load symbol file fail");
            pAckString = "����:gdb�����ļ�ʧ��";
            break;
        }

        /*����gdb��rsp������������*/
        sprintf(pRwBuf, "target remote 127.0.0.1:%d\n", dllPort);
        ret = WriteFile(hChildStdinWrDup, pRwBuf, strlen(pRwBuf), &writtenLen, NULL);
        if(!ret)
        {
            sysLoger(LWARNING, "handleExpMessage: gdb connect to target fail");
            pAckString = "����:gdb����Ŀ�������";
            break;
        }
      
        /*��ȡ�������Ӻ�Ĵ�ӡ��Ϣ*/
        ret = readGdbResult( hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, &readLen, NULL);
        if (ret < 0)
        {
            sysLoger(LWARNING, "handleExpMessage: read gdb connect to target fail");
            pAckString = "����:��ȡGDB�������Ӻ�Ĵ�ӡ��Ϣ";
            break;
        }
        sysLoger(LDEBUG, "handleExpMessage: GDB connect target :%s", pRwBuf);

        //�ָ����ʽ�ո�
        ResumeBlank(pMsg->expression);
      
        /*���ݵ�ǰ����������gdbд����ʽ��ֵ����*/
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
            pAckString = "����:gdb��ӡ��Ϣ����";
            break;
        }

        /*��gdb��ȡ���ʽ��ֵ�Ľ��*/
        ret = readGdbResult( hChildStdoutRdDup, pRwBuf, TSS_GDB_RW_BUF_LEN, &readLen, NULL);
        if(!ret)
        {
            sysLoger(LWARNING, "handleExpMessage: gdb get print result fail");
            pAckString = "����:gdb��ȡ���ʧ��";
            break;
        }

        if ((strstr(pRwBuf, "warning:")) && (pResult = strchr(pRwBuf, '$')))
        {
            //����"warning:"
            pRwBuf = pResult;
        }
		TranslateGdbErrorInfo::getInstance()->translateErrorInfo(pRwBuf,TSS_GDB_RW_BUF_LEN);
        sysLoger(LINFO, "handleExpMessage: get process expression result OK");
        
        /*����ȡ���ı��ʽִ�н���ظ���TSClient*/
        TClient_putpkt(fd, pMsg->caAid, 0, 0, pRwBuf, strlen(pRwBuf));

    }while (0);


    /*������Ϣ��shell agent �ָ���������*/
    TClient_putpkt(fd, pMsg->saAid, daSaid, 0, "ersm", strlen("ersm"));


    if (NULL != pAckString)
    {
        //���ʹ���ظ���Ϣ
        TClient_putpkt(fd, pMsg->caAid, 0, 0, pAckString, strlen(pAckString));
    }

    //ɾ��TDA
    if (NULL != pTdaItem)
    {
        resourceManager::getInstance()->deleteTda(pTdaItem);
    }

    //�ͷ�rsp server
    if (FAIL != dllAid)
    {
        resourceManager::getInstance()->freeTraServer(dllAid);
    }

    //�ر�gdb�ӽ���
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
            //�Ѷ�ȡ��
            *pStr = '\0';
            *pReadSize = strlen(pBuffer);
            return ret;
        }
    }
}

