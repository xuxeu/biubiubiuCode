/***************************************************************************
 * 
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:usServer.cpp,v $
*/


/**
 * @file: usServer.cpp
 * @brief: 
 * <li>功能: ucServer的主体, ucServer类的实现,处理接收、发送数据等</li>
 * @author: tangxp
 * @date: 2009-12-9
 * <p>部门:系统部
 */




/*****************************引用部分********************************/
#include "stdafx.h"
#include "ucServer.h"

/*****************************前向声明部分**************************/


/*****************************定义部分********************************/


/*****************************实现部分********************************/

/*
 * @brief: 
 *      获取用户客户端通信端口和地址,
 *      建立和用户客户端的连接
 * @param[in] sdPort: 发送端口
 * @param[in] rvPort: 接收端口
 * @param[in] sdAddr: 发送地址
 * @param[in] rvAddr: 接收地址
 * @return: 
 *      无
 */
ucServer::ucServer()
{
    runFlag = true;
    busyPacketList.clear();
    agt2user.clear();
}

/*
 * @brief: 
 *      断开和用户客户端连接
 * @return: 
 *      无
 */
ucServer::~ucServer()
{
    disconnectAllUser();
    freeAllPacket();
}

/*
 * @brief: 
 *      将数据发送到目标机
 * @param[in] size: 缓冲大小
 * @param[out] pDesAid: 发送二级代理AID
 * @param[out] pDesSaid: 发送二级代理SAID
 * @param[out] pData: 发送数据
 * @return: 
 *      SUCC: 发送成功
 *      FAIL: 发送失败
 */
int ucServer::getpkt(US *pDesAid, US *pDesSaid, char *pData, int size)
{
    int len = 0;
    Packet *pPack = NULL;
    
    if (NULL == pData || NULL == pDesAid || NULL == pDesSaid)
    {
        return FAIL;
    }

    while(1)
    {
        replyCondition.wait();
        
        if (!runFlag)
        {
            return 0;
        }
        
        busyPacketListMtx.lock();

        if (busyPacketList.empty())
        {
            busyPacketListMtx.unlock();
            continue;
        }
        break;
    }
    
    pPack = busyPacketList.front();
    busyPacketList.pop_front();

    busyPacketListMtx.unlock();
    
    *pDesAid = pPack->getDes_aid();
    *pDesSaid = pPack->getDes_said();
    len = pPack->getSize();
    memset(pData, 0, size);
    memcpy(pData, pPack->getData(), len);

    //释放数据包
    PacketManager::getInstance()->free(pPack);

    return len;
}

/*
 * @brief: 
 *      处理目标机发送的数据
 * @param[in] srcAid: 二级代理AID
 * @param[in] srcSaid: 二级代理SAID
 * @param[in] pData: 接收数据
 * @param[in] size: 数据大小
 * @return: 
 *      SUCC: 发送成功
 *      FAIL: 发送失败
 */
int ucServer::putpkt(US srcAid, US srcSaid, char *pData, int size)
{
    char *ptr = pData;
    
    if (NULL == pData)
    {
        return FAIL;
    }

    switch (pData[0])
    {
    case 'r':    //注册用户客户端端口和地址
        registerAgent(srcAid, srcSaid, pData, size);
        break;

    case 'd':    //发送数据
        recvDataFromTarget(srcAid, srcSaid, pData, size);
        break;        
        
    case 'c':    //断开连接
        unregisterAgent(srcAid, srcSaid, pData, size);
        break;
        
    case 'a':    //确认回复
        ackSendData(srcAid, srcSaid, pData, size);
        break;
        
    default:
        return FAIL;
        break;
    }

    return SUCC;
}


/*
 * @brief: 
 *      连接用户客户端
 * @param[in] pInfo: 用户信息结构
 * @param[in] addr: 主机名
 * @param[in] port: 端口号
 * @param[in] protocol: 通信协议
 * @return: 
 *      SUCC: 发送成功
 *      FAIL: 发送失败
 */
int ucServer::connectUser(T_User_Info *pInfo, char *addr, int port, int protocol)
{
    int ret = 0;
    ucRxThread *pRx = NULL;
    WSocket *pClient = NULL;
    hostent *pHostInfo = NULL;
    char *pIp = NULL;

    if (NULL == pInfo || NULL == addr)
    {
        return FAIL;
    }

    if (RROTOCOL_TCP == protocol)
    {
        sysLoger(LINFO, "ucServer::connectUser TCP protocol");
        
        //非阻塞连接客户端
        pClient = new WSocket;
        if (NULL == pClient)
        {
            return FAIL;
        }
        
        ret = pClient->Create(AF_INET, SOCK_STREAM, 0);
        if (!ret)
        {
            sysLoger(LWARNING, "ucServer::connectUser create socket fail");
            delete pClient;
            pClient = NULL;
            return FAIL;
        }

        if (strcmp(addr, "127.0.0.1") != 0)
        {
            //转换主机名为IP地址
            pHostInfo = gethostbyname(addr);
            if (NULL == pHostInfo)
            {
                sysLoger(LWARNING, "ucServer::connectUser: get host by name fail, addr:%s, wsaerror:%d",
                            addr, WSAGetLastError());
                delete pClient;
                pClient = NULL;
                return FAIL;
            }

            pIp = inet_ntoa(*(struct in_addr*)pHostInfo->h_addr_list[0]);

        }
        else
        {
            pIp = addr;
        }

        sysLoger(LINFO, "ucServer::connectUser connect user %s:%d", pIp, port);
        ret = pClient->Connect(pIp, port);
        if (!ret) 
        {
            //等待2秒后再次连接
            Sleep(2000);
            ret = pClient->Connect(pIp, port);
            if (!ret)
            {
                sysLoger(LWARNING, "ucServer::connectUser connect user fail");
                delete pClient;
                pClient = NULL;
                return FAIL;
            }
        }
    }
    else if(RROTOCOL_UDP == protocol)
    {
        //UDP连接
        sysLoger(LINFO, "ucServer::connectUser UDP protocol");
    }
    else
    {
        //不支持协议
        sysLoger(LWARNING, "ucServer::connectUser invalid ip protocol");
        return FAIL;
    }
    pInfo->pSocket = pClient;

    //创建接收线程
    pRx = new ucRxThread(this, pInfo);
    if (NULL == pRx)
    {
        pClient->Close();
        delete pClient;
        pClient = NULL;
        return FAIL;
    }
    pInfo->pTx = pRx;

    //启动接收线程,开始接受数据
    pRx->start();

    sysLoger(LINFO, "ucServer::connectUser connect user succ");
    
    return SUCC;
}

/*
 * @brief: 
 *      断开连接用户客户端，删除线程
 * @param[in] subId: 二级代理SAID
 * @return: 
 *      NOT_NEED_REPLY: 发送成功
 *      FAIL: 发送失败
 */
int ucServer::disconnectUser(int aid, int said, int sockId)
{
    T_User_Info *pInfo = NULL;

    pInfo = queryUserInfo(aid, said, sockId);
    if (NULL == pInfo)
    {
        return FAIL;
    }

    if (NULL != pInfo->pSocket)
    {
        //断开和客户端的连接
        pInfo->pSocket->Close();
        delete pInfo->pSocket;
        pInfo->pSocket = NULL;
    }

    if (NULL != pInfo->pTx)
    {
        //停止接收线程
        pInfo->pTx->stop();
        delete pInfo->pTx;
        pInfo->pTx = NULL;
    }

    free(pInfo);
    pInfo = NULL;

    sysLoger(LINFO, "ucServer::disconnectUser: disconnect user");
    
    return SUCC;
}

/*
 * @brief: 
 *      断开连接所有用户客户端，删除线程
 * @param[in] subId: 二级代理SAID
 * @return: 
 *      无
 */
void ucServer::disconnectAllUser()
{
    list<T_User_Info*>::iterator iter;
    T_User_Info *pInfo = NULL;
    TMutexLocker locker(&agt2userListMtx);
   
    for(iter = agt2user.begin(); iter != agt2user.end(); ++iter)
    {
        pInfo = *iter;

        if (NULL != pInfo->pSocket)
        {
            //断开和客户端的连接
            pInfo->pSocket->Close();
            delete pInfo->pSocket;
            pInfo->pSocket = NULL;
        }

        if (NULL != pInfo->pTx)
        {
            //接收线程退出
            pInfo->pTx->stop();
            delete pInfo->pTx;
            pInfo->pTx = NULL;
        }

        free(pInfo);
        pInfo = NULL;

    }  

    //清空
    agt2user.clear();
    
    return;
}

void ucServer::freeAllPacket()
{
    Packet *pPack = NULL;

    TS_MutexLocker locker(&busyPacketListMtx);

    while(!busyPacketList.empty())
    {
        pPack = busyPacketList.front();
        busyPacketList.pop_front();
        PacketManager::getInstance()->free(pPack);
    }
}

/*
 * @brief: 
 *      停止ucServer
 * @return: 
 *      无
 */
void ucServer::stop()
{
    sysLoger(LINFO, "ucServer::stop: stop ucServer");
    unregisterToTsMap();
    runFlag = false;
    disconnectAllUser();
    replyCondition.wakeup();
    Sleep(3000);
}

/*
 * @brief: 
 *      注册代理，连接客户端
 * @param[in] srcAid: 注册二级代理AID
 * @param[in] srcSaid: 注册二级代理SAID
 * @param[in] pData: 发送数据
 * @param[in] size: 数据大小
 * @return: 
 *      SUCC: 注册成功
 *      FAIL: 注册失败
 */
int ucServer::registerAgent(US srcAid, US srcSaid, char *pData, int size)
{
    int ret = 0;
    int port = 0;
    int protocol = 0;
    int sockId = 0;
    char *addr = NULL;
    char *ptr = pData;
    T_User_Info *pInfo = NULL;
    Packet *pPack = NULL;

    //解析protocol,sockId,port,addr
    //r:protocol,sockId,port,addr;
    ptr = pData + 1;
    if (!(':' == *(ptr++)
        && hex2int(&ptr,&protocol)
        && ',' == *(ptr++)
        && hex2int(&ptr,&sockId)
        && ',' == *(ptr++)
        && hex2int(&ptr,&port)
        && ',' == *(ptr++)))
    {
        sysLoger(LWARNING, "ucServer::registerAgent invalid cmd");
        sendDataToTarget(srcAid, srcSaid, "FAIL", strlen("FAIL"), sockId);
        return FAIL;
    }

    addr = ptr;
    ptr = strchr(ptr, ';');
    if(NULL == ptr)
    {
        sysLoger(LWARNING, "ucServer::registerAgent invalid addr");
        sendDataToTarget(srcAid, srcSaid, "FAIL", strlen("FAIL"), sockId);
        return FAIL;
    }
    *ptr = '\0';

    //注册用户信息
    pInfo = registerUserInfo(srcAid, srcSaid, sockId);
    if (NULL == pInfo)
    {
        sendDataToTarget(srcAid, srcSaid, "FAIL", strlen("FAIL"), sockId);
        return FAIL;
    }
    
    //连接用户客户端
    ret = connectUser(pInfo, addr, port, protocol);
    if (FAIL == ret)
    {
        unregisterUserInfo(srcAid, srcSaid, sockId);
        sendDataToTarget(srcAid, srcSaid, "FAIL", strlen("FAIL"), sockId);
        return FAIL;
    }

    //回复OK
    sendDataToTarget(srcAid, srcSaid, "OK", strlen("OK"), sockId);
    
    return SUCC;
}

/*
 * @brief: 
 *      注销代理，记录客户端port,addr，并连接客户端
 * @param[in] srcAid:  注销二级代理AID
 * @param[in] srcSaid:  注销二级代理SAID
 * @return: 
 *      SUCC: 注册成功
 *      FAIL: 注册失败
 */
int ucServer::unregisterAgent(US srcAid, US srcSaid, char *pData, int size)
{
    char *ptr = NULL;
    int sockId = 0;

    ptr = pData + 1;
    if (!(':' == *(ptr++)
        && hex2int(&ptr,&sockId)))
    {
        sysLoger(LWARNING, "ucServer::unregisterAgent invalid cmd");
        return FAIL;
    }

    disconnectUser(srcAid, srcSaid, sockId);

    unregisterUserInfo(srcAid, srcSaid, sockId);

    return SUCC;
}

/*
 * @brief: 
 *      根据代理ID发送数据到目标机
 * @param[in] desAid: 二级代理AID
 * @param[in] desSaid: 二级代理SAID
 * @param[in] pData: 数据
 * @param[in] size: 大小
 * @return: 
 *      SUCC: 发送成功
 *      FAIL: 发送失败
 */
int ucServer::sendDataToTarget(US desAid, US desSaid, char *buf, int size, int sockId, bool isUserData, ucRxThread *pRx)
{
    int sendLen = 0;
    int curLen = 0;
    int preLen = 0;
    Packet *pPack = NULL;
    char *ptr = NULL;
    if (NULL == buf)
    {
        return FAIL;
    }

    while (sendLen < size)
    {
        //分配包
        pPack = PacketManager::getInstance()->alloc();
        if (NULL == pPack)
        {
            return FAIL;
        }
        
        memset(pPack, 0, sizeof(Packet));
        ptr = pPack->getData();

        if (isUserData)
        {
            //d:len,sockId:data
            preLen = sprintf(ptr, "d:%x,%x:", size, sockId);
            curLen = min(size - sendLen, MAX_PACKET_SIZE - preLen);
            pPack->setSize(curLen + preLen);
            memcpy(ptr + preLen, buf + sendLen, curLen);
        }
        else
        {
            curLen = min(size, MAX_PACKET_SIZE);
            pPack->setSize(curLen);
            memcpy(ptr, buf + sendLen, curLen);
        }
        pPack->setDes_aid(desAid);
        pPack->setDes_said(desSaid);
        
        sendLen += curLen;

        //放入待发送列表
        busyPacketListMtx.lock();

        busyPacketList.push_back(pPack);
        
        busyPacketListMtx.unlock();

        //唤醒发送线程
        replyCondition.wakeup();

        sysLoger(LINFO, "ucServer::sendDataToTarget: send data :%d, aid:%d, said:%d",
                        curLen, desAid, desSaid);
        
        if (NULL != pRx)
        {
            if(!(pRx->getAck()))
            {
                return SUCC;
            }
        }
    }
    

    return SUCC;

}

/*
 * @brief: 
 *      从目标机接收数据包到发送缓冲
 *      如果接受完毕，则发送到客户端
 * @param[in] srcAid: 二级代理AID
 * @param[in] srcSaid: 二级代理SAID
 * @param[in] pData: 接收数据
 * @param[in] size: 数据大小
 * @return: 
 *      SUCC: 操作成功
 *      FAIL: 操作失败
 */
int ucServer::recvDataFromTarget(US srcAid, US srcSaid, char *pData, int size)
{
    T_User_Info *pInfo = NULL;
    T_UPacket *pUpack = NULL;
    char *ptr = pData;
    char *pBuf = NULL;
    int dataSize = 0;
    int curSize = 0;
    int sockId = 0;
    
    if (NULL == pData)
    {
        return FAIL;
    }

    //d:len,sockId:data
    if (!('d' == *(ptr++)
        && ':' == *(ptr++)
        && hex2int(&ptr,&dataSize)
        && ',' == *(ptr++)
        && hex2int(&ptr,&sockId)
        && ':' == *(ptr++)))
    {
        sysLoger(LWARNING, "ucServer::recvDataFromTarget: invalid data, aid:%d, said:%d",
                srcAid, srcSaid);
        return FAIL;
    }

    pInfo = queryUserInfo(srcAid, srcSaid, sockId);
    if (NULL == pInfo || NULL == pInfo->pSocket)
    {
        sysLoger(LWARNING, "ucServer::recvDataFromTarget: have not connected user, aid:%d, said:%d",
                srcAid, srcSaid);
        return FAIL;
    }
    pUpack = &(pInfo->upack);

    if ((pUpack->curLen+size) > sizeof(pUpack->buf))
    {
        sysLoger(LWARNING, "ucServer::recvDataFromTarget: data size limite, aid:%d, said:%d",
                srcAid, srcSaid);
        return FAIL;
    }

    if ((pUpack->allSize != dataSize) && (0 != pUpack->allSize))
    {
        //上一个包没发完,可能目标机重启,清空
        sysLoger(LWARNING, "ucServer::recvDataFromTarget: last data have not send, len:%d, aid:%d, said:%d",
                pUpack->allSize, srcAid, srcSaid);
        pUpack->curLen = 0;
        memset(pUpack->buf, 0, sizeof(pUpack->buf));
    }

    //设置发送数据总大小
    pUpack->allSize = dataSize;
    pBuf = &(pUpack->buf[pUpack->curLen]);
    curSize = size - (ptr - pData);
    memcpy(pBuf, ptr, curSize);
    pUpack->curLen += curSize;

    sysLoger(LINFO, "ucServer::recvDataFromTarget: recv data from target, len:%d, aid:%d, said:%d",
                curSize, srcAid, srcSaid);

    if (pUpack->curLen >= dataSize)
    {
        //发送数据到客户端
        pInfo->pSocket->Send(pUpack->buf, dataSize);

        //清空数据包
        pUpack->allSize = 0;
        pUpack->curLen = 0;
        memset(pUpack->buf, 0, sizeof(pUpack->buf));

        sysLoger(LINFO, "ucServer::recvDataFromTarget: send data to user, len:%d, aid:%d, said:%d",
                dataSize, srcAid, srcSaid);
    }

    return SUCC;
}

/*
 * @brief: 
 *      注册代理和对应的客户端port,addr
 * @param[in] srcAid: 注册二级代理AID
 * @param[in] srcSaid: 注册二级代理SAID
 * @param[in] addr: 客户端addr
 * @param[in] port: 客户端port
 * @return: 
 *      用户信息结构
 *      NULL: 注册失败
 */
T_User_Info* ucServer::registerUserInfo(US srcAid, US srcSaid, int sockId)
{
    T_User_Info *pInfo = NULL;
    
    //查询是否已经注册
    pInfo = queryUserInfo(srcAid, srcSaid, sockId);
    if (NULL != pInfo)
    {
        //已经注册，可能是目标机重启
        disconnectUser(srcAid, srcSaid, sockId);
        unregisterUserInfo(srcAid, srcSaid, sockId);
    }

    pInfo = (T_User_Info*)malloc(sizeof(T_User_Info));
    if (NULL == pInfo)
    {
        sysLoger(LWARNING, "ucServer::registerUserInfo malloc userinfo fail");
        return NULL;
    }
    memset(pInfo, 0, sizeof(T_User_Info));
    
    pInfo->aid = srcAid;
    pInfo->said = srcSaid;
    pInfo->sockId = sockId;

    agt2userListMtx.lock();

    //插入
    agt2user.push_back(pInfo);
    
    agt2userListMtx.unlock();

    sysLoger(LINFO, "ucServer::registerUserInfo register userInfo succ");
    
    return pInfo;
}

/*
 * @brief: 
 *      注销代理和对应的客户端
 * @param[in] srcAid: 注销二级代理AID
 * @param[in] srcSaid: 注销二级代理SAID
 * @return: 
 *      SUCC: 注销成功
 *      FAIL: 注销失败
 */
int ucServer::unregisterUserInfo(US srcAid, US srcSaid, int sockId)
{
    list<T_User_Info*>::iterator iter;
    T_User_Info *pInfo = NULL;
    TMutexLocker locker(&agt2userListMtx);
   
    for(iter = agt2user.begin(); iter != agt2user.end(); ++iter)
    {
        pInfo = *iter;
        if ((srcAid == pInfo->aid) && (srcSaid == pInfo->said) && (sockId == pInfo->sockId))
        {
            agt2user.remove(pInfo);
            free(pInfo);
            pInfo = NULL;
            return SUCC;
        }
    }    
    
    return FAIL;
}

/*
 * @brief: 
 *      查询代理对应的客户端port,addr
 * @param[in] srcAid: 查询二级代理AID
 * @param[in] srcSaid: 查询二级代理SAID
 * @return: 
 *      代理对应客户端信息结构
 *      NULL: 查询失败
 */
T_User_Info* ucServer::queryUserInfo(US srcAid, US srcSaid, int sockId)
{
    list<T_User_Info*>::iterator iter;
    T_User_Info *pInfo = NULL;
    TMutexLocker locker(&agt2userListMtx);
   
    for(iter = agt2user.begin(); iter != agt2user.end(); ++iter)
    {
        pInfo = *iter;
        if ((srcAid == pInfo->aid) && (srcSaid == pInfo->said) && (sockId == pInfo->sockId))
        {
            return pInfo;
        }
    }
    
    return NULL;
}

/*
 * @brief: 
 *      确认发送数据
 * @param[in] srcAid: 二级代理AID
 * @param[in] srcSaid: 二级代理SAID
 * @return: 
 *      NOT_NEED_REPLY: 不需回复
 */
int ucServer::ackSendData(US srcAid, US srcSaid, char *pData, int size)
{
    T_User_Info *pInfo = NULL;
    char *ptr = pData;
    int sockId = 0;

    ptr = pData + 1;
    if (!(':' == *(ptr++)
        && hex2int(&ptr,&sockId)))
    {
        sysLoger(LWARNING, "ucServer::ackSendData invalid cmd");
        return FAIL;
    }

    pInfo = queryUserInfo(srcAid, srcSaid, sockId);
    if (NULL == pInfo)
    {
        return FAIL;
    }

    if (NULL != pInfo->pTx)
    {
        sysLoger(LINFO, "ucServer::ackSendData: recv ack");
        pInfo->pTx->ack();
    }

    return SUCC;
}




/*
* @brief: 
*      UCServer的MAIN函数
* @return: 
*      无
*/
void ucServer::main(void)
{
    int ret = 0;
    char recvBuf[MAX_PACKET_SIZE] = "\0";
    unsigned short srcAid = 0;
    unsigned short srcSaid = 0;
    unsigned short desSaid = 0;

    /* 与TS建立连接 */
    lnkId = TClient_open(TS_IP, TS_PORT);
    if(lnkId <= 0)
    {
        sysLoger(LWARNING, "ucServer::main: connetc ts fail");
        return;
    }

    sysLoger(LINFO, "ucServer::main: connetc ts succ linkID:%d", lnkId);

    /* 向TSMAP注册 */
    ret = registerToTsMap();
    if(FAIL == ret)
    {
        sysLoger(LWARNING, "ucServer::main: register to TsMap fail");
        return;
    }

    sysLoger(LINFO, "ucServer::main: register to TsMap succ");

    /* 循环接收并处理目标机端发送过来的数据 */
    while(1)
    {
        if(!runFlag)
        {
            return;
        }

        ret = TClient_wait(lnkId, DEFAULT_WAIT_TIME);
        if(TS_DATACOMING != ret) 
        {
            if (TS_INVALID_LINK_HANDLE == ret)
            {
                sysLoger(LWARNING, "ucServer::main: link closed");
                return;
            }
            continue;
        }

        /* 接收发送过来的数据包 */
        memset(recvBuf, 0, MAX_PACKET_SIZE);
        ret = TClient_getpkt(lnkId, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_PACKET_SIZE);
        if(ret < 0)
        {
            if (TS_SOCKET_ERROR == ret)
            {
                sysLoger(LWARNING, "ucServer::main: link closed");
                return;
            }
            continue;
        }

        sysLoger(LINFO, "ucServer::main: recv msg:%c", recvBuf[0] );

        /* 处理接收到的数据 */
        putpkt(srcAid, srcSaid, recvBuf, ret);         

    }

}



/*
* @brief: 
*      发送数据到目标机的线程
* @return: 
*      无
*/
void ucServer::run(void)
{

    int ret = 0;
    unsigned short pDesAid = 0;
    unsigned short pDesSaid = 0;
    char sendBuf[MAX_PACKET_SIZE] = "\0";

    sysLoger(LINFO, "ucServer::run: enter run");

    while(1)
    {
    
        if(!runFlag)
        {
            return;
        }

        /* 获取处理后的数据包 */
        memset(sendBuf, 0, MAX_PACKET_SIZE);
        ret = getpkt(&pDesAid, &pDesSaid, sendBuf, MAX_PACKET_SIZE); 

        if(ret > 0)
        {
            /* 有处理后的数据包,发送到TS */
            TClient_putpkt(lnkId, pDesAid, pDesSaid, 0, sendBuf, ret);
        }

    }

    return;
}


/*
* @brief: 
*      向TSMAP注册UCServer的ID号
* @return: 
*      SUCC: 注册成功
*      FAIL: 注册失败
*/
int ucServer::registerToTsMap(void)
{
    char sendBuf[MAX_PACKET_SIZE] = "\0";
    char recvBuf[MAX_PACKET_SIZE] = "\0";
    unsigned short srcAid, srcSaid, desSaid;
    int len = 0;
    int ret = 0;

    /* 填写向TSMAP注册消息格式：
    r:name;exclusive;regAid;regSaid;queryAid;querySaid;qrlen;queryStr;[datalen;data;]*/
    memset(sendBuf, 0, MAX_PACKET_SIZE);
    sprintf(sendBuf, "r:UCServer;0;%x;0;0;0;%x;None;", lnkId, strlen("None"));

    while(1)
    {
        TClient_putpkt(lnkId, TSMAP_AID, 0, 0, sendBuf, strlen(sendBuf));
        ret = TClient_wait(lnkId, DEFAULT_WAIT_TIME);
        if(TS_DATACOMING != ret) 
        {
            return FAIL;
        }
        memset(recvBuf, 0, MAX_PACKET_SIZE);
        len = TClient_getpkt(lnkId, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_PACKET_SIZE);
        if((len < 0) || (TSMAP_AID != srcAid))
        {
            return FAIL;
        }
        else if ((strncmp(recvBuf, "OK;", 3)) && !(strncmp(recvBuf, "E04", 3)))
        {
            //已经注册,注销后重新注册
            ret = unregisterToTsMap();
            if (FAIL == ret)
            {
                return FAIL;
            }
            continue;
        }   
        return SUCC;
    }

}


/*
* @brief: 
*      向TSMAP注销UCServer的ID号
* @return: 
*      SUCC: 注册成功
*      FAIL: 注册失败
*/
int ucServer::unregisterToTsMap(void)
{
    char regBuf[MAX_PACKET_SIZE] = "\0";
    char *pBuf = NULL;
    char recvBuf[MAX_PACKET_SIZE] = "\0";
    unsigned short srcAid, srcSaid, desSaid;
    int len, ret;
    int regAid, regSaid;

    /* 查询注册AID,消息格式:
    q:name;queryAid;querySaid;qrlen;queryStr;*/
    memset(regBuf, 0, MAX_PACKET_SIZE);
    sprintf(regBuf, "q:UCServer;%x;0;%x;None;", lnkId, strlen("None"));
    TClient_putpkt(lnkId, TSMAP_AID, 0, 0, regBuf, strlen(regBuf));
    ret = TClient_wait(lnkId, DEFAULT_WAIT_TIME);
    if(TS_DATACOMING != ret) 
    {
        return FAIL;
    }
    len = TClient_getpkt(lnkId, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_PACKET_SIZE);
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
    sprintf(regBuf, "u:UCServer;%x;%x;0;0;%x;None;", regAid, regSaid, strlen("None"));
    
    TClient_putpkt(lnkId, TSMAP_AID, 0, 0, regBuf, strlen(regBuf));
    ret = TClient_wait(lnkId, DEFAULT_WAIT_TIME);
    if(TS_DATACOMING != ret) 
    {
        return FAIL;
    }
    len = TClient_getpkt(lnkId, &srcAid, &desSaid, &srcSaid, recvBuf, MAX_PACKET_SIZE);
    if((len < 0) ||(strncmp(recvBuf, "OK;", 3))||(TSMAP_AID != srcAid))
    {
        return FAIL;
    }

    return SUCC;

}




ucRxThread::ucRxThread(ucServer *pServer, T_User_Info *pInfo)
{
    pUcServer = pServer;
    info = *pInfo;
    runFlag = true;
}

void ucRxThread::stop()
{    
    runFlag = false;
    
    ackSem.wakeup();
    
    sysLoger(LINFO, "ucRxThread::stop: stop thread");
    
    if (isRunning())
    {
        //等待线程退出
        Sleep(10);
    }
}

void ucRxThread::run()
{
    int error = 0;
    int dataLen = 0;
    int recvSize = 0;
    WSocket *pSocket = NULL;
    char buf[1024*8];

    if (NULL == pUcServer || NULL == info.pSocket)
    {
        return;
    }

    pSocket = info.pSocket;

    //Rhapsody通信协议,先发送6字节包,通知数据大小,然后再按通知大小发送数据
    while (runFlag)
    {    
        //接收数据大小数据包
        memset(buf, 0, MAX_LEN_STR+1);
        recvSize = recv(pSocket->m_sock, buf, MAX_LEN_STR, 0);

        if((SOCKET_ERROR == recvSize
            && (error = WSAGetLastError()) != WSAEWOULDBLOCK)
             || 0 == recvSize)
        {
            //网络断了
            sysLoger(LWARNING, "ucRxThread::run: link closed, ret:%d, wsaerror:%d", 
                         recvSize, error);
            break;
        }

        sysLoger(LINFO, "ucRxThread::run: recv data:%d", recvSize);

        //发送到目标机 
        pUcServer->sendDataToTarget(info.aid, info.said, buf, recvSize, info.sockId, true, this);
        if (!runFlag)
        {
            break;
        }

        //解析数据大小
        if ((sscanf(buf, "%d", &dataLen) == 1) && (dataLen > 0) && (dataLen < sizeof(buf)))
        {
            //按大小接收数据
            memset(buf, 0, sizeof(buf));
            recvSize = recv(pSocket->m_sock, buf, dataLen, 0);
            if((SOCKET_ERROR == recvSize
                && (error = WSAGetLastError()) != WSAEWOULDBLOCK)
                 || 0 == recvSize)
            {
                //网络断了
                sysLoger(LWARNING, "ucRxThread::run: link closed, ret:%d, wsaerror:%d", 
                             recvSize, error);
                break;
            }

            sysLoger(LINFO, "ucRxThread::run: recv data:%d", recvSize);
            
            //发送到目标机             
            pUcServer->sendDataToTarget(info.aid, info.said, buf, recvSize, info.sockId, true, this);
        }
    }

    sysLoger(LINFO, "ucRxThread::run: thread quit");
    runFlag = false;
}

void ucRxThread::ack()
{
    ackSem.wakeup();
}

/*
 * @brief: 
 *      确认发送数据
 * @param[in] srcAid: 二级代理AID
 * @param[in] srcSaid: 二级代理SAID
 * @return: 
 *      true: 已获取ACK
 *      false: 运行中止，立即退出
 */
bool ucRxThread::getAck()
{
    int ret = 0;
    do
    {
        ret = ackSem.wait(1000);
        if (!runFlag)
        {
            return false;
        }
    }while(!ret);
    
    return true;
}


