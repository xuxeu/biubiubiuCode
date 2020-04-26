/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/
/**
 * @file           :  tsServer.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> ����</li> tsServer����,�������ļ����ء����ء���ѯ��ɾ����
                                 <li>���˼��</li> 
                                 <p> n     ע�����n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008��9��23�� 16:02:35
 * @History        : 
 *   
 */

/************************���ò���*****************************/
#include "windows.h"
#include <process.h>
#include "common.h"
#include "tClientAPI.h"
#include "tssConfig.h"
#include "tssComm.h"
#include "RepProcessor.h"
#include "tsServer.h"

 
/************************ǰ����������***********************/

/************************���岿��*****************************/
RepProcessor *RepProcessor::m_pRepProcessor = NULL;

/************************ʵ�ֲ���****************************/

/**
 * @Funcname:  RepProcessor
 * @brief        :  RepProcessor�Ĺ��캯�������г�Ա������ʼ��
 *                      ��ͷ��ʼ���Ĺ���
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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
 * @brief        : RepProcessor��������������������ϱ���Ϣ�����߳�
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 16:34:11
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

        /*�����߳����ڽ��ա������ϱ���Ϣ*/
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
 * @brief        : ��ֹ�ϱ���Ϣ�����߳�
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 16:34:11
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
 * @brief        : ��ȡ��������Ӿ��
 * @return      : ���Ӿ��
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 16:34:11
 *   
**/
T_UHWORD RepProcessor::GetFd()
{
    return m_fd;
}

/**
 * @Funcname: RepProcessorThreadEntry
 * @brief        : RepProcessor������߳���ں���
 * @param[IN] : �̲߳���
 * @return      : �߳��˳���
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 16:34:11
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
                
            /*����յ�SA��ɾ�����ϱ���Ϣ��
            ��ɾ����Ӧ��Ŀ����ڵ�*/
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
