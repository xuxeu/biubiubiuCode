/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/
/**
 * @file           :  tsServer.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> 功能</li> tsServer主体,处理处理文件上载、下载、查询、删除等
                                 <li>设计思想</li> 
                                 <p> n     注意事项：n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008年9月23日 16:02:35
 * @History        : 
 *   
 */

/************************引用部分*****************************/
#include "windows.h"
#include <process.h>
#include "common.h"
#include "tClientAPI.h"
#include "tssConfig.h"
#include "tssComm.h"
#include "RepProcessor.h"
#include "tsServer.h"

 
/************************前向声明部分***********************/

/************************定义部分*****************************/
RepProcessor *RepProcessor::m_pRepProcessor = NULL;

/************************实现部分****************************/

/**
 * @Funcname:  RepProcessor
 * @brief        :  RepProcessor的构造函数，进行成员变量初始化
 *                      列头初始化的工作
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 15:56:24
 *   
**/
RepProcessor::RepProcessor()
{
    m_fd = 0;
    m_hThread = NULL;
    m_threadID = 0;
    m_isRunning = 0;
}

/**
 * @Funcname: run
 * @brief        : RepProcessor对象的主处理函数，启动上报消息处理线程
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 16:34:11
 *   
**/
T_VOID RepProcessor::run()
{
    if(!m_isRunning)
    {
        m_fd = TClient_open(TS_IP, TS_PORT);
        if(m_fd <= 0)
        {
            sysLoger(LWARNING, "RepProcessor::run: connect ts fail, ret:%d", m_fd);
            return;
        }

        /*创建线程用于接收、处理上报消息*/
        m_hThread = (HANDLE)_beginthreadex(NULL,
                                    1024*10, 
                                    RepProcessorThreadEntry, 
                                    (T_VOID*)this, 
                                    CREATE_SUSPENDED, 
                                    &m_threadID);
        if(!m_hThread)
        {
           sysLoger(LWARNING, "RepProcessor::run: create thread for recevel");
        }
        m_isRunning = 1;
        ResumeThread(m_hThread); 
    }     
}

/**
 * @Funcname: stop
 * @brief        : 终止上报消息处理线程
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 16:34:11
 *   
**/
T_VOID RepProcessor::stop()
{
    if(m_isRunning)
    {
        TerminateThread(m_hThread, 0);
        TClient_close(m_fd);    
        m_isRunning = 0;
    }
}

/**
 * @Funcname: GetFd
 * @brief        : 获取对象的连接句柄
 * @return      : 连接句柄
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 16:34:11
 *   
**/
T_UHWORD RepProcessor::GetFd()
{
    return m_fd;
}

/**
 * @Funcname: RepProcessorThreadEntry
 * @brief        : RepProcessor类的主线程入口函数
 * @param[IN] : 线程参数
 * @return      : 线程退出码
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008年9月23日 16:34:11
 *   
**/
T_UWORD __stdcall RepProcessorThreadEntry(void *pParam)
{
    RepProcessor* pRepProcessor = (RepProcessor*)pParam;
    T_TSS_Target_Queue_Node *pTarget = NULL, *pTargetTmp = NULL;
    ServerCfg serverConfig;
    COMCONFIG comConfig;
    T_UHWORD fd;
    T_WORD ret;
    
    fd = pRepProcessor->GetFd();
    
    while(1)
    {
        memset(&serverConfig, 0, sizeof(serverConfig));
        serverConfig.pComConfig = &comConfig;
        ret = TClient_getSAReport(fd, &serverConfig, 10);
        if(0  == ret)
        {            
            switch(serverConfig.state)
            {
                sysLoger(LINFO, "RepProcessorThreadEntry: get server report, state:%d", 
                                                                                        serverConfig.state);
                
            /*如果收到SA被删除的上报信息，
            则删除相应的目标机节点*/
            case SASATE_DELETED:
            {
                sysLoger(LWARNING, "RepProcessorThreadEntry: server is deleted");
                g_targetQueueHead->pMtx->lock();
                if(g_targetQueueHead->pHead->pExpProcessor->GetSaAid() == serverConfig.aid)
                {
                    pTarget = g_targetQueueHead->pHead;
                    g_targetQueueHead->pHead = g_targetQueueHead->pHead->next;
                    delete pTarget->pExpProcessor;
                    free(pTarget);
                }
                for(pTargetTmp = g_targetQueueHead->pHead; 
                    pTargetTmp->next != NULL; 
                    pTargetTmp = pTargetTmp->next)
                {
                    if(pTargetTmp->next->pExpProcessor->GetSaAid() == serverConfig.aid)
                    {
                        pTarget = pTargetTmp->next;
                        pTargetTmp->next = pTargetTmp->next->next;
                        delete pTarget->pExpProcessor;
                        free(pTarget);
                    }
                }
                g_targetQueueHead->curTargetNum--;
                g_targetQueueHead->pMtx->unlock();
                break;
            }
            default:
                break;
            }
        }
    }
}
