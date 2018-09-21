/***************************************************************************
 * 
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:resoure.cpp,v $
*/


/**
 * @file: resoure.cpp
 * @brief: 
 * <li>����: ʵ��traServer����</li>
 * @author: tangxp
 * @date: 2009-2-6
 * <p>����:ϵͳ��
 */




/*****************************���ò���********************************/
#include "resoure.h"
#include "TClientAPI.h"
#include "tssComm.h"

/*****************************ǰ����������**************************/

extern T_TSS_Target_Queue_head  *g_targetQueueHead;

/*****************************���岿��********************************/
#define BUF_SIZE 2048

/*****************************ʵ�ֲ���********************************/

resourceManager* resourceManager::m_rm = NULL;
TMutex resourceManager::m_rmMutex;

/**
 * @brief: 
 *    ���캯��,����TS
*/
resourceManager::resourceManager()
{
    m_fd = TClient_open(TS_IP, TS_PORT);
    m_traServerSN = 0;
    m_traWaterLevel = 0;
}

/**
 * @brief: 
 *    ����һ��traServer
 * @param desAid: traServerͨ��desAid
 * @param desAid: traServerͨ��desSaid
 * @return �ɹ�����traServerId��ʧ�ܷ���FAIL
*/
int resourceManager::allocTraServer(int desAid, int desSaid)
{
    int traServerId = -1;
    ServerCfg dllServerConfig = {1, "", "0", 0, 0, 0, 0, 1, NULL};
    COMCONFIG comConfig;
    int ret = 0;
    int dllPort = -1;    
    char buf[BUF_SIZE];
    TMutexLocker locker(&m_traMutex);
    
    while (true) 
    {
        if(m_traFreeList.size() > 0) 
        {
            //�ӿ�������ȡtraServer
            traServerId = m_traFreeList.front();
            m_traFreeList.pop_front();

            //��������desAid��desSaid
            ret = setDesId(traServerId, desAid, desSaid);

            return (FAIL == ret) ? FAIL : traServerId;
        }

        if(m_traWaterLevel < HIGH_LEVEL) 
        {    
            //���ڸ�ˮλ���½�traServer
            sprintf(dllServerConfig.name, "rspDllServer%d", m_traServerSN++);
            memset(&comConfig, 0, sizeof(comConfig));
            strcpy(comConfig.DLL.dllName, "tsrsptra.dll");
            sprintf(comConfig.DLL.buf, "%x,%x", desAid, desSaid);
            dllServerConfig.pComConfig = &comConfig;

            ret = TClient_getServerID(m_fd, dllServerConfig.name);
            if (ret > 0)
            {
                //ɾ��traServer
                TClient_deleteServer(m_fd, ret);
            }

            //����traServer
            traServerId = TClient_createServerWithDll(m_fd, &dllServerConfig);
            if(traServerId < 0)
            {
                sysLoger(LWARNING, "resourceManager::allocTraServer: create rsp server fail: ret:%d",
                                                                                            traServerId);
                return FAIL;
            }
            
            //����traServer
            memset(buf, 0, BUF_SIZE);
            ret = TClient_activeServerWithDll(m_fd, traServerId, buf, BUF_SIZE);
            if(ret < 0)
            {
                sysLoger(LWARNING, "resourceManager::allocTraServer: active rsp server fail: ret:%d", ret);
                return FAIL;
            }

            m_traWaterLevel++;
            
            return traServerId;
        }
        
        //���ڸ�ˮλ,�ȴ������߳��ͷ���Դ,2�볬ʱ
        if (!m_traOverHighLevel.wait(locker.getMutex(), 2000))
        {
            return FAIL;
        }
    }
}

/**
* @brief: 
*   �ͷ�һ��traServer
* @param traServerId: traServer��ID
*/
void resourceManager::freeTraServer(int traServerId)
{
    TMutexLocker locker(&m_traMutex);

    if (m_traWaterLevel >= LOW_LEVEL)
    {
        //���ڵ�ˮλ,�ͷ�
        TClient_deleteServer(m_fd, traServerId);
        m_traWaterLevel--;
    }
    else
    {
        //���ڵ�ˮλ,�����������
        m_traFreeList.push_back(traServerId);
    }
}

/**
* @brief: 
*   ��ȡtraServer������TCP�˿�
* @param traServerId: traServer��ID
* @return �ɹ����ض˿ڣ�ʧ�ܷ���FAIL
*/
int resourceManager::getDllPort(int traServerId)
{
    ServerCfg dllServerConfig;
    COMCONFIG comConfig;
    int ret = 0;
    int dllPort = -1;
    char *pBuf = NULL;
    
    memset(&dllServerConfig, 0, sizeof(dllServerConfig));
    memset(&comConfig, 0, sizeof(comConfig));
    dllServerConfig.pComConfig = &comConfig;
    dllServerConfig.aid = traServerId;

    //��ȡtraServer��Ϣ
    ret = TClient_queryServer(m_fd, &dllServerConfig);
    if (ret < 0)
    {
        sysLoger(LWARNING, "resourceManager::getDllPort: get tra port fail: ret:%d, "\
                        "dllAid:%d", ret, traServerId);
        return FAIL;
    }
    
    pBuf = dllServerConfig.pComConfig->DLL.buf;
    if (hex2int(&pBuf, &dllPort) <= 0)
    {
        return FAIL;
    }
    
    return dllPort;
}

/**
* @brief: 
*   ����traServerͨ��desSaid
* @param traServerId: traServer��ID
* @param newDesSaid: traServerͨ��desSaid
* @return �ɹ�����SUCC��ʧ�ܷ���FAIL
*/
int resourceManager::setDesId(int traServerId, int newDesAid, int newDesSaid)
{
    char buf[BUF_SIZE] = "\0";
    int ret = 0;
    unsigned short srcAid, desSaid, srcSaid;

    //����desAid
    sprintf(buf, "rsp-a:%x", newDesAid);
    TClient_putpkt(m_fd, traServerId, 1, 0, buf, BUF_SIZE);
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "resourceManager::setDesId: set tra desAid fail: wait ret:%d, "\
                        "dllAid:%d, desAid:%d", ret, traServerId, newDesAid);
        return FAIL;
    }

    //���ջظ�
    ret = TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, buf, BUF_SIZE);
    if(ret < 0) 
    {
        sysLoger(LWARNING, "resourceManager::setDesId: set tra desAid fail: getpkt ret:%d, "\
                        "dllAid:%d, desAid:%d", ret, traServerId, newDesAid);
        return FAIL;
    }
    if(strncmp(buf, "OK", 2))
    {
        sysLoger(LWARNING, "resourceManager::setDesId: set tra desAid fail: recv:%s, "\
                        "dllAid:%d, desAid:%d", buf, traServerId, newDesAid);
        return FAIL;
    }

    //����desSaid
    sprintf(buf, "rsp-s:%x", newDesSaid);
    TClient_putpkt(m_fd, traServerId, 1, 0, buf, BUF_SIZE);
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "resourceManager::setDesId: set tra desSaid fail: wait ret:%d, "\
                        "dllAid:%d, desSaid:%d", ret, traServerId, newDesSaid);
        return FAIL;
    }

    //���ջظ�
    ret = TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, buf, BUF_SIZE);
    if(ret < 0) 
    {
        sysLoger(LWARNING, "resourceManager::setDesId: set tra desSaid fail: getpkt ret:%d, "\
                        "dllAid:%d, desSaid:%d", ret, traServerId, newDesSaid);
        return FAIL;
    }
    if(strncmp(buf, "OK", 2))
    {
        sysLoger(LWARNING, "resourceManager::setDesId: set tra desSaid fail: recv:%s, "\
                        "dllAid:%d, desSaid:%d", buf, traServerId, newDesSaid);
        return FAIL;
    }

    return SUCC;
}

/**
* @brief: 
*   �ͷ����е�traServer
* @return �ɹ�����SUCC��ʧ�ܷ���FAIL
*/
void resourceManager::freeAllTraServer()
{
    int traServerId = -1;

    TMutexLocker locker(&m_traMutex);
    while (m_traFreeList.size() > 0)
    {
        traServerId = m_traFreeList.front();
        m_traFreeList.pop_front();

        TClient_deleteServer(m_fd, traServerId);
    }
}


/**
 * @brief: 
 *    ����һ��TDA
 * @return �ɹ����ذ���TdaSaid��T_TSS_Tda����ʧ�ܷ���NULL
*/
T_TSS_Tda* resourceManager::createTda(int saAid)
{
    int ret = 0;
    int tdaSaid = -1;
    unsigned short srcAid, desSaid, srcSaid;
    char recvBuf[BUF_SIZE] = "\0";
    char *pBuf = NULL;
    T_TSS_Tda *pItem = NULL;
    
    //����TDA
    TClient_putpkt(m_fd, saAid, TARGET_SAID_SMA, 0, TARGET_CREATE_TDA_STRING, 
                                                                    strlen(TARGET_CREATE_TDA_STRING));    
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "handleExpMessage: create TDA fail, wait fd:%d, ret:%d,", m_fd, ret);
        return NULL;
    }

    //��ȡTDA_SAID
    ret = TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, recvBuf, BUF_SIZE);
    if(ret < 0) 
    {
        sysLoger(LWARNING, "handleExpMessage: create TDA fail, get tdaSaid ret:%d", ret);
        return NULL;
    }
    if(strncmp(recvBuf, "OK;", 3))
    {
        sysLoger(LWARNING, "handleExpMessage: create TDA fail, recv:%s", recvBuf);
        return NULL;
    }
    pBuf = recvBuf+3;
    hex2int(&pBuf, &tdaSaid);

    //TDA�������񲢰�
    TClient_putpkt(m_fd, saAid, tdaSaid, 0, TARGET_CREATE_ATTACH_TASK_STRING, 
                                                        strlen(TARGET_CREATE_ATTACH_TASK_STRING));    
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "handleExpMessage: TDA create attach task fail: wait ret:%d", ret);
        return NULL;
    }

    //���ջظ�
    ret = TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
    if(ret < 0) 
    {
        sysLoger(LWARNING, "handleExpMessage: TDA create attach task fail:getpkt ret:%d", ret);
        return NULL;
    }
    if(strncmp(recvBuf, "OK", 2))
    {
        sysLoger(LWARNING, "handleExpMessage: TDA create attach task fail: recv:%s", recvBuf);
        return NULL;
    }

    //��������
    TClient_putpkt(m_fd, saAid, tdaSaid, 0, TARGET_TASK_CONTINUE, 
                                                                strlen(TARGET_TASK_CONTINUE));

    pItem = new T_TSS_Tda;
    if (NULL != pItem)
    {
        //��������
        pItem->saAid = saAid;
        pItem->tdaSaid = tdaSaid;        

        m_tdaMutex.lock();
        m_tdaList.push_back(pItem);
        m_tdaMutex.unlock();
    }
    
    return pItem;
}

/**
* @brief: 
*   �ͷ�һ��TDA
* @param tpItem: ����TdaSaid�Ͷ�ӦĿ���aid
* @return �ɹ�����SUCC��ʧ�ܷ���FAIL
*/
int resourceManager::deleteTda(T_TSS_Tda* tpItem)
{
    int ret = 0;
    int len = 0;
    unsigned short srcAid, desSaid, srcSaid;
    char recvBuf[BUF_SIZE] = "\0";
    list<T_TSS_Tda*>::iterator iter;
    bool isFount = false;

    if (NULL == tpItem)
    {
        return FAIL;
    }

    m_tdaMutex.lock();
    for (iter = m_tdaList.begin(); iter != m_tdaList.end(); iter++)
    {
        if (tpItem == *iter)
        {
            //������ɾ��
            m_tdaList.remove(tpItem);
            isFount = true;
            break;
        }
    }
    m_tdaMutex.unlock();

    if (!isFount)
    {
        return FAIL;
    }

    //TDA����󶨲�ɾ������
    TClient_putpkt(m_fd, tpItem->saAid, tpItem->tdaSaid, 0, TARGET_DETACH_DELETE_TASK_STRING,
                                                           strlen(TARGET_DETACH_DELETE_TASK_STRING));    
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "handleExpMessage: TDA detach and delete task fail");
        return FAIL;
    }

    TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
    
    //ɾ��TDA
    memset(recvBuf, 0, MAX_MESSAGE_LEN);
    len = sprintf(recvBuf, "%s:%x", TARGET_DELETE_TDA_STRING, tpItem->tdaSaid);
    TClient_putpkt(m_fd, tpItem->saAid, TARGET_SAID_SMA, 0, recvBuf, len);    
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "handleExpMessage: delete TDA fail");
        return FAIL;
    }
    TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);

    delete tpItem;

    return SUCC;
}

/**
* @brief: 
*   ɾ�����е�TDA
*/
void resourceManager::deleteAllTda()
{
    T_TSS_Tda* pItem = NULL;

    m_tdaMutex.lock();
    if (m_tdaList.size() > 0)
    {
        pItem = m_tdaList.front();
        m_tdaMutex.unlock();

        //ɾ��TDAʱ,�������ɾ���ڵ�
        deleteTda(pItem);
    }
    m_tdaMutex.unlock();
}

/**
 * @brief: 
 *    ע��һ��GDB
 * @param saAid: Ŀ���aid
 * @return �ɹ����ذ���GDB���̾��������,�������Ķ���
 *      ʧ�ܷ���NULL
*/
T_TSS_Gdb* resourceManager::createGdb(int saAid)
{
    int ret = 0;
    HANDLE hSaveStdout, hSaveStdin;
    SECURITY_ATTRIBUTES saAttr; 
    HANDLE hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup;
    HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup;
    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFO siStartInfo;
    T_TSS_Gdb *pItem = NULL;
    T_CHAR pGdbPath[MAX_PATH_LEN] = "\0";
	T_WORD toolChainVer = 0;
	T_WORD targetArch = 0;
	T_TSS_Target_Queue_Node  *pTarget = NULL;
	T_CHAR *strArchPath[TARGET_ARCH_MAX] = {"x86", "arm", "ppc"};
	T_CHAR *strToolChainPath[TOOL_CHAIN_VER_MAX] = {"gnu\\gdb7.2", "gnu\\gdb7.2"};
	
	/*���Ҹ���Ϣ�����Ŀ�������Ӧ�Ĵ������*/
    for(pTarget = g_targetQueueHead->pHead; 
        pTarget != NULL; 
        pTarget = pTarget->next)
    {
        if(pTarget->pExpProcessor->GetSaAid() == saAid)
        {
            break;
        }
    }

	if(NULL == pTarget)
    {
        return NULL;
    }
	
	/*��ѯĿ����������汾*/
	toolChainVer = pTarget->pExpProcessor->GetTargetToolChainVersion();
	
	/*��ѯĿ�����ϵ�ṹ*/
	targetArch = pTarget->pExpProcessor->GetTargetArchType();
	if ((0xFFFFFFFF == toolChainVer)
		|| (0xFFFFFFFF == targetArch))
    {
        return NULL;
    }

	/*����GDB ·��*/
	sprintf(pGdbPath, "%s\\%s\\%s\\gdb.exe", 
						TsServer::getInstance()->getLambdaPath(), 
						strToolChainPath[toolChainVer], 
						strArchPath[targetArch]);


    /*�����ٽ��*/
    EnterCriticalSection(&g_cs);

    /*��ȡ�����̵ı�׼������*/
    hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
    
    /*���������ܵ��ӹ�GDB���̵��������*/
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    /*����gdb�ӽ��̱�׼����Ĺܵ�*/
    CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0);

    /*���ý��̵ı�׼���Ϊ�ܵ���д��,����������
    ���̺��ӽ�����̳и�����*/
    SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr);

    /*���Ʋ��رձ�׼����ܵ��Ķ�ȡ�ˣ���ֹ�ӽ��̼̳�*/
    ret = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd,
                        GetCurrentProcess(), &hChildStdoutRdDup , 0,
                        FALSE,
                        DUPLICATE_SAME_ACCESS);
    CloseHandle(hChildStdoutRd);

    /*��ȡ�����̵ı�׼������*/
    hSaveStdin = GetStdHandle(STD_INPUT_HANDLE); 
    
    /*����gdb�ӽ��̱�׼����Ĺܵ�*/
    CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0);

    /*���ý��̵ı�׼����Ϊ�ܵ��Ķ���,����������
    ���̺��ӽ�����̳и�����*/
    SetStdHandle(STD_OUTPUT_HANDLE, hChildStdinRd);

    /*���Ʋ��رձ�׼����ܵ���д�ˣ���ֹ�ӽ��̼̳�*/
    ret = DuplicateHandle(GetCurrentProcess(), hChildStdinWr,
                        GetCurrentProcess(), &hChildStdinWrDup , 0,
                        FALSE,
                        DUPLICATE_SAME_ACCESS);
    CloseHandle(hChildStdinWr);

    /*����gdb�ӽ���*/
    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdInput = hChildStdinRd;
    siStartInfo.hStdOutput= hChildStdoutWr;
    siStartInfo.hStdError = hChildStdoutWr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
    ret = CreateProcess(NULL, pGdbPath, NULL, NULL, TRUE, 0, NULL, NULL, 
                                                                                    &siStartInfo, &piProcInfo);  
    if(!ret)
    {
        sysLoger(LWARNING, "handleExpMessage: create gdb proccess fail, lastError:%d", 
                                                                                                    GetLastError());
        CloseHandle(hChildStdinRd);
        CloseHandle(hChildStdinWrDup);
        CloseHandle(hChildStdoutWr);
        CloseHandle(hChildStdoutRdDup);
        LeaveCriticalSection(&g_cs);
        return NULL;
    }
    
    /*�ָ������̵ı�׼���롢������*/
    SetStdHandle(STD_INPUT_HANDLE, hSaveStdin);
    SetStdHandle(STD_INPUT_HANDLE, hSaveStdin);
    
    /*�뿪�ٽ��*/
    LeaveCriticalSection(&g_cs);

    pItem = new T_TSS_Gdb;
    if (NULL != pItem)
    {
        //����GDB����
        pItem->hProcess = piProcInfo.hProcess;
        pItem->hStdinRd = hChildStdinRd;
        pItem->hStdinWrDup = hChildStdinWrDup;
        pItem->hStdoutWr = hChildStdoutWr;
        pItem->hStdoutRdDup = hChildStdoutRdDup;
        
        m_gdbMutex.lock();
        m_gdbList.push_back(pItem);
        m_gdbMutex.unlock();
    }

    return pItem;
}

/**
* @brief: 
*   ע��һ��GDB
* @param tpItem: ����TdaSaid�Ͷ�ӦĿ���aid
* @return �ɹ�����SUCC��ʧ�ܷ���FAIL
*/
int resourceManager::deleteGdb(T_TSS_Gdb* tpItem)
{
    list<T_TSS_Gdb*>::iterator iter;
    bool isFount = false;
    
    if (NULL == tpItem)
    {
        return FAIL;
    }
    
    m_gdbMutex.lock();
    for (iter = m_gdbList.begin(); iter != m_gdbList.end(); iter++)
    {
        if (tpItem == *iter)
        {
            //������ɾ��
            m_gdbList.remove(tpItem);
            isFount = true;
            break;
        }
    }
    m_gdbMutex.unlock();

    if (!isFount)
    {
        return FAIL;
    }
    
    /*�ر�gdb�ӽ���*/
    TerminateProcess(tpItem->hProcess, 0);

    /*�رչܵ�*/
    CloseHandle(tpItem->hStdinRd);
    CloseHandle(tpItem->hStdinWrDup);
    CloseHandle(tpItem->hStdoutWr);
    CloseHandle(tpItem->hStdoutRdDup);

    delete tpItem;

    return SUCC;

}

/**
* @brief: 
*   ɾ�����е�GDB
*/
void resourceManager::deleteAllGdb()
{
    T_TSS_Gdb* pItem = NULL;

    m_gdbMutex.lock();
    if (m_gdbList.size() > 0)
    {
        pItem = m_gdbList.front();
        m_gdbMutex.unlock();

        //ɾ��GDDBʱ,�������ɾ���ڵ�
        deleteGdb(pItem);
    }
    m_gdbMutex.unlock();
}

/**
* @brief: 
*    ��������,�ر�����,�ͷŴ�����traServer,tda,gdb
*/
resourceManager::~resourceManager()
{
    deleteAllGdb();

    deleteAllTda();
    
    freeAllTraServer();

    TClient_close(m_fd);
}

