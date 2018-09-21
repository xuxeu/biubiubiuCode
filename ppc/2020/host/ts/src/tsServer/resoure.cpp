/***************************************************************************
 * 
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:resoure.cpp,v $
*/


/**
 * @file: resoure.cpp
 * @brief: 
 * <li>功能: 实现traServer管理</li>
 * @author: tangxp
 * @date: 2009-2-6
 * <p>部门:系统部
 */




/*****************************引用部分********************************/
#include "resoure.h"
#include "TClientAPI.h"
#include "tssComm.h"

/*****************************前向声明部分**************************/

extern T_TSS_Target_Queue_head  *g_targetQueueHead;

/*****************************定义部分********************************/
#define BUF_SIZE 2048

/*****************************实现部分********************************/

resourceManager* resourceManager::m_rm = NULL;
TMutex resourceManager::m_rmMutex;

/**
 * @brief: 
 *    构造函数,连接TS
*/
resourceManager::resourceManager()
{
    m_fd = TClient_open(TS_IP, TS_PORT);
    m_traServerSN = 0;
    m_traWaterLevel = 0;
}

/**
 * @brief: 
 *    分配一个traServer
 * @param desAid: traServer通信desAid
 * @param desAid: traServer通信desSaid
 * @return 成功返回traServerId，失败返回FAIL
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
            //从空闲链条取traServer
            traServerId = m_traFreeList.front();
            m_traFreeList.pop_front();

            //重新设置desAid和desSaid
            ret = setDesId(traServerId, desAid, desSaid);

            return (FAIL == ret) ? FAIL : traServerId;
        }

        if(m_traWaterLevel < HIGH_LEVEL) 
        {    
            //低于高水位则新建traServer
            sprintf(dllServerConfig.name, "rspDllServer%d", m_traServerSN++);
            memset(&comConfig, 0, sizeof(comConfig));
            strcpy(comConfig.DLL.dllName, "tsrsptra.dll");
            sprintf(comConfig.DLL.buf, "%x,%x", desAid, desSaid);
            dllServerConfig.pComConfig = &comConfig;

            ret = TClient_getServerID(m_fd, dllServerConfig.name);
            if (ret > 0)
            {
                //删除traServer
                TClient_deleteServer(m_fd, ret);
            }

            //创建traServer
            traServerId = TClient_createServerWithDll(m_fd, &dllServerConfig);
            if(traServerId < 0)
            {
                sysLoger(LWARNING, "resourceManager::allocTraServer: create rsp server fail: ret:%d",
                                                                                            traServerId);
                return FAIL;
            }
            
            //激活traServer
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
        
        //大于高水位,等待其他线程释放资源,2秒超时
        if (!m_traOverHighLevel.wait(locker.getMutex(), 2000))
        {
            return FAIL;
        }
    }
}

/**
* @brief: 
*   释放一个traServer
* @param traServerId: traServer的ID
*/
void resourceManager::freeTraServer(int traServerId)
{
    TMutexLocker locker(&m_traMutex);

    if (m_traWaterLevel >= LOW_LEVEL)
    {
        //大于低水位,释放
        TClient_deleteServer(m_fd, traServerId);
        m_traWaterLevel--;
    }
    else
    {
        //低于低水位,放入空闲链表
        m_traFreeList.push_back(traServerId);
    }
}

/**
* @brief: 
*   获取traServer监听的TCP端口
* @param traServerId: traServer的ID
* @return 成功返回端口，失败返回FAIL
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

    //获取traServer信息
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
*   设置traServer通信desSaid
* @param traServerId: traServer的ID
* @param newDesSaid: traServer通信desSaid
* @return 成功返回SUCC，失败返回FAIL
*/
int resourceManager::setDesId(int traServerId, int newDesAid, int newDesSaid)
{
    char buf[BUF_SIZE] = "\0";
    int ret = 0;
    unsigned short srcAid, desSaid, srcSaid;

    //设置desAid
    sprintf(buf, "rsp-a:%x", newDesAid);
    TClient_putpkt(m_fd, traServerId, 1, 0, buf, BUF_SIZE);
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "resourceManager::setDesId: set tra desAid fail: wait ret:%d, "\
                        "dllAid:%d, desAid:%d", ret, traServerId, newDesAid);
        return FAIL;
    }

    //接收回复
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

    //设置desSaid
    sprintf(buf, "rsp-s:%x", newDesSaid);
    TClient_putpkt(m_fd, traServerId, 1, 0, buf, BUF_SIZE);
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "resourceManager::setDesId: set tra desSaid fail: wait ret:%d, "\
                        "dllAid:%d, desSaid:%d", ret, traServerId, newDesSaid);
        return FAIL;
    }

    //接收回复
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
*   释放所有的traServer
* @return 成功返回SUCC，失败返回FAIL
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
 *    分配一个TDA
 * @return 成功返回包含TdaSaid的T_TSS_Tda对象，失败返回NULL
*/
T_TSS_Tda* resourceManager::createTda(int saAid)
{
    int ret = 0;
    int tdaSaid = -1;
    unsigned short srcAid, desSaid, srcSaid;
    char recvBuf[BUF_SIZE] = "\0";
    char *pBuf = NULL;
    T_TSS_Tda *pItem = NULL;
    
    //创建TDA
    TClient_putpkt(m_fd, saAid, TARGET_SAID_SMA, 0, TARGET_CREATE_TDA_STRING, 
                                                                    strlen(TARGET_CREATE_TDA_STRING));    
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "handleExpMessage: create TDA fail, wait fd:%d, ret:%d,", m_fd, ret);
        return NULL;
    }

    //获取TDA_SAID
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

    //TDA创建任务并绑定
    TClient_putpkt(m_fd, saAid, tdaSaid, 0, TARGET_CREATE_ATTACH_TASK_STRING, 
                                                        strlen(TARGET_CREATE_ATTACH_TASK_STRING));    
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "handleExpMessage: TDA create attach task fail: wait ret:%d", ret);
        return NULL;
    }

    //接收回复
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

    //运行任务
    TClient_putpkt(m_fd, saAid, tdaSaid, 0, TARGET_TASK_CONTINUE, 
                                                                strlen(TARGET_TASK_CONTINUE));

    pItem = new T_TSS_Tda;
    if (NULL != pItem)
    {
        //放入链表
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
*   释放一个TDA
* @param tpItem: 包含TdaSaid和对应目标机aid
* @return 成功返回SUCC，失败返回FAIL
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
            //从链表删除
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

    //TDA解除绑定并删除任务
    TClient_putpkt(m_fd, tpItem->saAid, tpItem->tdaSaid, 0, TARGET_DETACH_DELETE_TASK_STRING,
                                                           strlen(TARGET_DETACH_DELETE_TASK_STRING));    
    ret = TClient_wait(m_fd, DEFAULT_WAIT_TIME);
    if(0 != ret) 
    {
        sysLoger(LWARNING, "handleExpMessage: TDA detach and delete task fail");
        return FAIL;
    }

    TClient_getpkt(m_fd, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_MESSAGE_LEN);
    
    //删除TDA
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
*   删除所有的TDA
*/
void resourceManager::deleteAllTda()
{
    T_TSS_Tda* pItem = NULL;

    m_tdaMutex.lock();
    if (m_tdaList.size() > 0)
    {
        pItem = m_tdaList.front();
        m_tdaMutex.unlock();

        //删除TDA时,会从链表删除节点
        deleteTda(pItem);
    }
    m_tdaMutex.unlock();
}

/**
 * @brief: 
 *    注册一个GDB
 * @param saAid: 目标机aid
 * @return 成功返回包含GDB进程句柄及输入,输出句柄的对象
 *      失败返回NULL
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
	
	/*查找该消息发向的目标机所对应的处理对象*/
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
	
	/*查询目标机工具链版本*/
	toolChainVer = pTarget->pExpProcessor->GetTargetToolChainVersion();
	
	/*查询目标机体系结构*/
	targetArch = pTarget->pExpProcessor->GetTargetArchType();
	if ((0xFFFFFFFF == toolChainVer)
		|| (0xFFFFFFFF == targetArch))
    {
        return NULL;
    }

	/*生成GDB 路径*/
	sprintf(pGdbPath, "%s\\%s\\%s\\gdb.exe", 
						TsServer::getInstance()->getLambdaPath(), 
						strToolChainPath[toolChainVer], 
						strArchPath[targetArch]);


    /*进入临界段*/
    EnterCriticalSection(&g_cs);

    /*获取父进程的标准输出句柄*/
    hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
    
    /*创建匿名管道接管GDB进程的输入输出*/
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    /*创建gdb子进程标准输出的管道*/
    CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0);

    /*设置进程的标准输出为管道的写端,这样创建子
    进程后子进程则继承该属性*/
    SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr);

    /*复制并关闭标准输出管道的读取端，防止子进程继承*/
    ret = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd,
                        GetCurrentProcess(), &hChildStdoutRdDup , 0,
                        FALSE,
                        DUPLICATE_SAME_ACCESS);
    CloseHandle(hChildStdoutRd);

    /*获取父进程的标准输入句柄*/
    hSaveStdin = GetStdHandle(STD_INPUT_HANDLE); 
    
    /*创建gdb子进程标准输入的管道*/
    CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0);

    /*设置进程的标准输入为管道的读端,这样创建子
    进程后子进程则继承该属性*/
    SetStdHandle(STD_OUTPUT_HANDLE, hChildStdinRd);

    /*复制并关闭标准输入管道的写端，防止子进程继承*/
    ret = DuplicateHandle(GetCurrentProcess(), hChildStdinWr,
                        GetCurrentProcess(), &hChildStdinWrDup , 0,
                        FALSE,
                        DUPLICATE_SAME_ACCESS);
    CloseHandle(hChildStdinWr);

    /*创建gdb子进程*/
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
    
    /*恢复父进程的标准输入、输出句柄*/
    SetStdHandle(STD_INPUT_HANDLE, hSaveStdin);
    SetStdHandle(STD_INPUT_HANDLE, hSaveStdin);
    
    /*离开临界段*/
    LeaveCriticalSection(&g_cs);

    pItem = new T_TSS_Gdb;
    if (NULL != pItem)
    {
        //放入GDB链表
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
*   注销一个GDB
* @param tpItem: 包含TdaSaid和对应目标机aid
* @return 成功返回SUCC，失败返回FAIL
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
            //从链表删除
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
    
    /*关闭gdb子进程*/
    TerminateProcess(tpItem->hProcess, 0);

    /*关闭管道*/
    CloseHandle(tpItem->hStdinRd);
    CloseHandle(tpItem->hStdinWrDup);
    CloseHandle(tpItem->hStdoutWr);
    CloseHandle(tpItem->hStdoutRdDup);

    delete tpItem;

    return SUCC;

}

/**
* @brief: 
*   删除所有的GDB
*/
void resourceManager::deleteAllGdb()
{
    T_TSS_Gdb* pItem = NULL;

    m_gdbMutex.lock();
    if (m_gdbList.size() > 0)
    {
        pItem = m_gdbList.front();
        m_gdbMutex.unlock();

        //删除GDDB时,会从链表删除节点
        deleteGdb(pItem);
    }
    m_gdbMutex.unlock();
}

/**
* @brief: 
*    析构函数,关闭连接,释放创建的traServer,tda,gdb
*/
resourceManager::~resourceManager()
{
    deleteAllGdb();

    deleteAllTda();
    
    freeAllTraServer();

    TClient_close(m_fd);
}

