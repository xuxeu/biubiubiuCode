/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/

/**
 * @file             :  tsServer.cpp
 * @Version       :  1.0.0
 * @brief           :  *   <li> ����</li>tsServer������
                                <li>���˼��</li> 
                                <p> n     ע�����n</p> 
 *   
 * @author        :  zhagnxu
 * @create date :  2008��9��23�� 15:49:46
 * @History       : 
 *   
 */


/************************���ò���*****************************/

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
/************************ǰ����������***********************/

/************************���岿��*****************************/
T_ERR_TBL errTbl[100] = {
                        {SUCC, "�ɹ�;"},
                        {FAIL, "ʧ��;"},
                        {ERR_NOT_FOUND_IP, "����:��������û���ҵ�IP;"},
                        {ERR_NOT_FOUND_EXPRESSION, "����:��������-û���ҵ����ʽ;"},
                        {ERR_TOO_MANY_SYMBOL_FILE, "����:��������-���ű��ļ�����;"},
                        {ERR_TARGET_QUEUE_FULL, "����:Ŀ��������Ѿ��ﵽ���;"},
                        {ERR_MESSAGE_QUEUE_FULL, "����:��Ϣ�����Ѿ��ﵽ���;"},
                        {ERR_CREATE_OBJECT_FAIL, "����:��������ʧ��;"},
                        {1, ""}
                    };

T_TSS_Eviroment_Variables *g_envVar;
T_TSS_Target_Queue_head  *g_targetQueueHead;
CRITICAL_SECTION g_cs;
TsServer* TsServer::m_pTsServer = NULL;
                
/************************ʵ�ֲ���****************************/

/**
 * @Funcname:  TsServer
 * @brief        :  TsServer�Ĺ��캯�������·����ȡ��Ŀ�����
 *                      ��ͷ��ʼ���Ĺ���
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
 *   
**/
TsServer::TsServer()
{
    T_CHAR *pTmp = NULL;
    T_WORD isLog = 1;
    
     /*��ȡ�����ļ���GDB���·��*/
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

    /*��ʼ��Ŀ�������ͷ*/
    g_targetQueueHead = (T_TSS_Target_Queue_head*)malloc(sizeof(T_TSS_Target_Queue_head));
    g_targetQueueHead->pHead = NULL;
    g_targetQueueHead->pTail = NULL;
    g_targetQueueHead->maxTargetNum = MAX_TARGET_NUM;
    g_targetQueueHead->curTargetNum = 0;
    g_targetQueueHead->pMtx = new TMutex();

   /*��ʼ���ٽ����*/
    InitializeCriticalSection(&g_cs);
        
    /*����־�ļ�*/
    SystemLog::getInstance((LogLevel)isLog)->openLogFile(string("tsServer.log"));

    if (CheckFolderExist(g_envVar->symbolFilesPath) == FALSE)
    {
        _mkdir(g_envVar->symbolFilesPath);
    }

}

/**
 * @Funcname:  TsServer
 * @brief        :  TsServer�������������ͷ���Դ
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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
 * @brief        : TsServer����������
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 16:34:11
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
    
    /*�������ڽ��ձ��ʽ��ֵ��Ϣ������*/
    m_fd = TClient_open(TS_IP, TS_PORT);
    if(m_fd <= 0)
    {
        sysLoger(LWARNING, "tsServer::main: connetc ts fail");
        return;
    }

    sysLoger(LDEBUG, "tsServer::main: connetc ts succ");

    /*��tsmap ע��*/
    ret = registerToTsmap();
    if(FAIL == ret)
    {
        sysLoger(LWARNING, "tsServer::main: register tsmap fail");
        return;
    }

    sysLoger(LINFO, "tsServer::main: register tsmap succ, aid:%d", m_fd);

    /*�����ϱ���Ϣ����*/
    RepProcessor::getInstance()->run();

    /*ѭ�����ա�������ʽ��ֵ��Ϣ*/
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
        case 's':    //������־����
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
            
        case 'q':    //�˳�tsServer
            sysLoger(LINFO, "tsServer::main: exit");
            stop();
            return;
            break;
            
        case 'e':    //�������ʽ��ֵ��Ϣ
        case 'o':     // ���������������
        {
            /*�������ʽ��ֵ��Ϣ*/
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

            /*���Ҹ���Ϣ�����Ŀ�������Ӧ�Ĵ������*/
            for(pTarget = g_targetQueueHead->pHead; 
                pTarget != NULL; 
                pTarget = pTarget->next)
            {
                if(pTarget->pExpProcessor->GetSaAid() == msg.saAid)
                {
                    break;
                }
            }
            
            /*���û���ҵ���������򴴽��¶��󲢲��������*/
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

            /*����Ϣ����ö������Ϣ������*/
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
 * @brief        :  �̵߳�ֹͣ����
 * @return      : 
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��10��9�� 11:42:36
 *   
**/
T_VOID TsServer::stop()
{
    T_TSS_Target_Queue_Node *pTarget = NULL, *pTargetTmp = NULL;

    /*ֹͣ�ϱ���Ϣ������������*/
    RepProcessor::getInstance()->stop();
        
    /*ֹͣ���б��ʽ������������*/
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

    /*ɾ���ٽ����*/
    DeleteCriticalSection(&g_cs);

    /*�ر�����*/
    TClient_close(m_fd);
}

/**
 * @Funcname:  registerToTsmap
 * @brief        :  ��tsmapע��
 * @return      :   SUCC(�ɹ�)  FAIL(ʧ��)
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
 *   
**/
T_WORD TsServer::registerToTsmap()
{
    T_CHAR regBuf[200] = "\0";
    T_CHAR buf[10] = "\0";
    T_CHAR recvBuf[MAX_MESSAGE_LEN] = "\0";
    T_UHWORD srcAid, srcSaid, desAid, desSaid;
    T_WORD len, ret;
    
    /*ע����Ϣ��ʽ��
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
            //�Ѿ�ע��,ע��������ע��
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
 * @brief        :  ��tsmapע��
 * @return      :   SUCC(�ɹ�)  FAIL(ʧ��)
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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

    /*��ѯע��AID,��Ϣ��ʽ:
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
    
    //����ע��AID��ע��SAID
    pBuf = recvBuf + strlen("OK;");
    hex2int(&pBuf, &regAid);
    pBuf += strlen(";");
    
    hex2int(&pBuf, &regSaid);
    

    /*ע����Ϣ��ʽ��
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

    //�ر���������tsServer
    TClient_putpkt(m_fd, regAid, 0, 0, "q", strlen("q"));

    return SUCC;
}

/**
 * @Funcname:  getLambdaPath
 * @brief        :  ��ȡLambda��·��
 * @return      :  m_lambdaPath :Lambda��·��
 * @Author     :  qingxh
 *  
 * @History: 1.  Created this function on 2009��11��3��
 *   
**/
T_CHAR* TsServer::getLambdaPath()
{
	return m_lambdaPath;
}

/**
 * @Funcname:  parseExpressionMessage
 * @brief        :  �������ʽ��ֵ��Ϣ�ĺ���
 * @param[IN]: srcAid, ��Ϣ��ԴAID
 * @param[IN]: pMsgStr, ��Ϣ�ַ���
 * @param[IN]: len, ��Ϣ�ַ�������
 * @param[OUT]: pMsg, ��Ϣ�ṹ����
 * @param[OUT]: targetName, Ŀ�������
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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

    /* ���ʽ��ֵ���� */
    if(pBuf[0] == 'e')
    {
        pMsg->cmdType = COMMAND_TYPE_EXPRESSION;
    }
    else if(pBuf[0] == 'o')
    {
        pMsg->cmdType = COMMAND_TYPE_OTHER;
    }

    /* ����������"e:"����"o:" */
    pBuf++;
    pBuf++;
    
    /*��ȡ���Ͷ�AID*/
    pMsg->caAid = srcAid;
    
    /*��ȡĿ���saAid*/
    hex2int(&pBuf, &saAid);
    pMsg->saAid = saAid;

    /*��ȡĿ�������*/
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
        //����з����ļ���
        pBuf++;
        pTmp = strstr(pBuf, ">");
        if(NULL == pTmp)
        {
            return ERR_NOT_FOUND_EXPRESSION;
        }
        /*��ȡ���ʽ*/
        strcpy(pMsg->expression, pTmp+1);
        if (' ' != *(pTmp-1))
        {
            *pTmp = ' ';
        }        

        /*��ȡ�����ļ���,�Կո���Ϊ�����ļ����*/
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
 * @brief        :  ͨ������Ż�ȡ�����ַ���
 * @param[IN]: eno �����
 * @return      : �����ַ���
 * @Author     :  zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��23�� 15:56:24
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


