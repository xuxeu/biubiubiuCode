/***************************************************************************
 * 
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:usServer.cpp,v $
*/


/**
 * @file: usServer.cpp
 * @brief: 
 * <li>����: ucServer������, ucServer���ʵ��,������ա��������ݵ�</li>
 * @author: tangxp
 * @date: 2009-12-9
 * <p>����:ϵͳ��
 */




/*****************************���ò���********************************/
#include "stdafx.h"
#include "ucServer.h"

/*****************************ǰ����������**************************/


/*****************************���岿��********************************/


/*****************************ʵ�ֲ���********************************/

/*
 * @brief: 
 *      ��ȡ�û��ͻ���ͨ�Ŷ˿ں͵�ַ,
 *      �������û��ͻ��˵�����
 * @param[in] sdPort: ���Ͷ˿�
 * @param[in] rvPort: ���ն˿�
 * @param[in] sdAddr: ���͵�ַ
 * @param[in] rvAddr: ���յ�ַ
 * @return: 
 *      ��
 */
ucServer::ucServer()
{
    runFlag = true;
    busyPacketList.clear();
    agt2user.clear();
}

/*
 * @brief: 
 *      �Ͽ����û��ͻ�������
 * @return: 
 *      ��
 */
ucServer::~ucServer()
{
    disconnectAllUser();
    freeAllPacket();
}

/*
 * @brief: 
 *      �����ݷ��͵�Ŀ���
 * @param[in] size: �����С
 * @param[out] pDesAid: ���Ͷ�������AID
 * @param[out] pDesSaid: ���Ͷ�������SAID
 * @param[out] pData: ��������
 * @return: 
 *      SUCC: ���ͳɹ�
 *      FAIL: ����ʧ��
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

    //�ͷ����ݰ�
    PacketManager::getInstance()->free(pPack);

    return len;
}

/*
 * @brief: 
 *      ����Ŀ������͵�����
 * @param[in] srcAid: ��������AID
 * @param[in] srcSaid: ��������SAID
 * @param[in] pData: ��������
 * @param[in] size: ���ݴ�С
 * @return: 
 *      SUCC: ���ͳɹ�
 *      FAIL: ����ʧ��
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
    case 'r':    //ע���û��ͻ��˶˿ں͵�ַ
        registerAgent(srcAid, srcSaid, pData, size);
        break;

    case 'd':    //��������
        recvDataFromTarget(srcAid, srcSaid, pData, size);
        break;        
        
    case 'c':    //�Ͽ�����
        unregisterAgent(srcAid, srcSaid, pData, size);
        break;
        
    case 'a':    //ȷ�ϻظ�
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
 *      �����û��ͻ���
 * @param[in] pInfo: �û���Ϣ�ṹ
 * @param[in] addr: ������
 * @param[in] port: �˿ں�
 * @param[in] protocol: ͨ��Э��
 * @return: 
 *      SUCC: ���ͳɹ�
 *      FAIL: ����ʧ��
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
        
        //���������ӿͻ���
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
            //ת��������ΪIP��ַ
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
            //�ȴ�2����ٴ�����
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
        //UDP����
        sysLoger(LINFO, "ucServer::connectUser UDP protocol");
    }
    else
    {
        //��֧��Э��
        sysLoger(LWARNING, "ucServer::connectUser invalid ip protocol");
        return FAIL;
    }
    pInfo->pSocket = pClient;

    //���������߳�
    pRx = new ucRxThread(this, pInfo);
    if (NULL == pRx)
    {
        pClient->Close();
        delete pClient;
        pClient = NULL;
        return FAIL;
    }
    pInfo->pTx = pRx;

    //���������߳�,��ʼ��������
    pRx->start();

    sysLoger(LINFO, "ucServer::connectUser connect user succ");
    
    return SUCC;
}

/*
 * @brief: 
 *      �Ͽ������û��ͻ��ˣ�ɾ���߳�
 * @param[in] subId: ��������SAID
 * @return: 
 *      NOT_NEED_REPLY: ���ͳɹ�
 *      FAIL: ����ʧ��
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
        //�Ͽ��Ϳͻ��˵�����
        pInfo->pSocket->Close();
        delete pInfo->pSocket;
        pInfo->pSocket = NULL;
    }

    if (NULL != pInfo->pTx)
    {
        //ֹͣ�����߳�
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
 *      �Ͽ����������û��ͻ��ˣ�ɾ���߳�
 * @param[in] subId: ��������SAID
 * @return: 
 *      ��
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
            //�Ͽ��Ϳͻ��˵�����
            pInfo->pSocket->Close();
            delete pInfo->pSocket;
            pInfo->pSocket = NULL;
        }

        if (NULL != pInfo->pTx)
        {
            //�����߳��˳�
            pInfo->pTx->stop();
            delete pInfo->pTx;
            pInfo->pTx = NULL;
        }

        free(pInfo);
        pInfo = NULL;

    }  

    //���
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
 *      ֹͣucServer
 * @return: 
 *      ��
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
 *      ע��������ӿͻ���
 * @param[in] srcAid: ע���������AID
 * @param[in] srcSaid: ע���������SAID
 * @param[in] pData: ��������
 * @param[in] size: ���ݴ�С
 * @return: 
 *      SUCC: ע��ɹ�
 *      FAIL: ע��ʧ��
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

    //����protocol,sockId,port,addr
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

    //ע���û���Ϣ
    pInfo = registerUserInfo(srcAid, srcSaid, sockId);
    if (NULL == pInfo)
    {
        sendDataToTarget(srcAid, srcSaid, "FAIL", strlen("FAIL"), sockId);
        return FAIL;
    }
    
    //�����û��ͻ���
    ret = connectUser(pInfo, addr, port, protocol);
    if (FAIL == ret)
    {
        unregisterUserInfo(srcAid, srcSaid, sockId);
        sendDataToTarget(srcAid, srcSaid, "FAIL", strlen("FAIL"), sockId);
        return FAIL;
    }

    //�ظ�OK
    sendDataToTarget(srcAid, srcSaid, "OK", strlen("OK"), sockId);
    
    return SUCC;
}

/*
 * @brief: 
 *      ע��������¼�ͻ���port,addr�������ӿͻ���
 * @param[in] srcAid:  ע����������AID
 * @param[in] srcSaid:  ע����������SAID
 * @return: 
 *      SUCC: ע��ɹ�
 *      FAIL: ע��ʧ��
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
 *      ���ݴ���ID�������ݵ�Ŀ���
 * @param[in] desAid: ��������AID
 * @param[in] desSaid: ��������SAID
 * @param[in] pData: ����
 * @param[in] size: ��С
 * @return: 
 *      SUCC: ���ͳɹ�
 *      FAIL: ����ʧ��
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
        //�����
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

        //����������б�
        busyPacketListMtx.lock();

        busyPacketList.push_back(pPack);
        
        busyPacketListMtx.unlock();

        //���ѷ����߳�
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
 *      ��Ŀ����������ݰ������ͻ���
 *      ���������ϣ����͵��ͻ���
 * @param[in] srcAid: ��������AID
 * @param[in] srcSaid: ��������SAID
 * @param[in] pData: ��������
 * @param[in] size: ���ݴ�С
 * @return: 
 *      SUCC: �����ɹ�
 *      FAIL: ����ʧ��
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
        //��һ����û����,����Ŀ�������,���
        sysLoger(LWARNING, "ucServer::recvDataFromTarget: last data have not send, len:%d, aid:%d, said:%d",
                pUpack->allSize, srcAid, srcSaid);
        pUpack->curLen = 0;
        memset(pUpack->buf, 0, sizeof(pUpack->buf));
    }

    //���÷��������ܴ�С
    pUpack->allSize = dataSize;
    pBuf = &(pUpack->buf[pUpack->curLen]);
    curSize = size - (ptr - pData);
    memcpy(pBuf, ptr, curSize);
    pUpack->curLen += curSize;

    sysLoger(LINFO, "ucServer::recvDataFromTarget: recv data from target, len:%d, aid:%d, said:%d",
                curSize, srcAid, srcSaid);

    if (pUpack->curLen >= dataSize)
    {
        //�������ݵ��ͻ���
        pInfo->pSocket->Send(pUpack->buf, dataSize);

        //������ݰ�
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
 *      ע�����Ͷ�Ӧ�Ŀͻ���port,addr
 * @param[in] srcAid: ע���������AID
 * @param[in] srcSaid: ע���������SAID
 * @param[in] addr: �ͻ���addr
 * @param[in] port: �ͻ���port
 * @return: 
 *      �û���Ϣ�ṹ
 *      NULL: ע��ʧ��
 */
T_User_Info* ucServer::registerUserInfo(US srcAid, US srcSaid, int sockId)
{
    T_User_Info *pInfo = NULL;
    
    //��ѯ�Ƿ��Ѿ�ע��
    pInfo = queryUserInfo(srcAid, srcSaid, sockId);
    if (NULL != pInfo)
    {
        //�Ѿ�ע�ᣬ������Ŀ�������
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

    //����
    agt2user.push_back(pInfo);
    
    agt2userListMtx.unlock();

    sysLoger(LINFO, "ucServer::registerUserInfo register userInfo succ");
    
    return pInfo;
}

/*
 * @brief: 
 *      ע������Ͷ�Ӧ�Ŀͻ���
 * @param[in] srcAid: ע����������AID
 * @param[in] srcSaid: ע����������SAID
 * @return: 
 *      SUCC: ע���ɹ�
 *      FAIL: ע��ʧ��
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
 *      ��ѯ�����Ӧ�Ŀͻ���port,addr
 * @param[in] srcAid: ��ѯ��������AID
 * @param[in] srcSaid: ��ѯ��������SAID
 * @return: 
 *      �����Ӧ�ͻ�����Ϣ�ṹ
 *      NULL: ��ѯʧ��
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
 *      ȷ�Ϸ�������
 * @param[in] srcAid: ��������AID
 * @param[in] srcSaid: ��������SAID
 * @return: 
 *      NOT_NEED_REPLY: ����ظ�
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
*      UCServer��MAIN����
* @return: 
*      ��
*/
void ucServer::main(void)
{
    int ret = 0;
    char recvBuf[MAX_PACKET_SIZE] = "\0";
    unsigned short srcAid = 0;
    unsigned short srcSaid = 0;
    unsigned short desSaid = 0;

    /* ��TS�������� */
    lnkId = TClient_open(TS_IP, TS_PORT);
    if(lnkId <= 0)
    {
        sysLoger(LWARNING, "ucServer::main: connetc ts fail");
        return;
    }

    sysLoger(LINFO, "ucServer::main: connetc ts succ linkID:%d", lnkId);

    /* ��TSMAPע�� */
    ret = registerToTsMap();
    if(FAIL == ret)
    {
        sysLoger(LWARNING, "ucServer::main: register to TsMap fail");
        return;
    }

    sysLoger(LINFO, "ucServer::main: register to TsMap succ");

    /* ѭ�����ղ�����Ŀ����˷��͹��������� */
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

        /* ���շ��͹��������ݰ� */
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

        /* ������յ������� */
        putpkt(srcAid, srcSaid, recvBuf, ret);         

    }

}



/*
* @brief: 
*      �������ݵ�Ŀ������߳�
* @return: 
*      ��
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

        /* ��ȡ���������ݰ� */
        memset(sendBuf, 0, MAX_PACKET_SIZE);
        ret = getpkt(&pDesAid, &pDesSaid, sendBuf, MAX_PACKET_SIZE); 

        if(ret > 0)
        {
            /* �д��������ݰ�,���͵�TS */
            TClient_putpkt(lnkId, pDesAid, pDesSaid, 0, sendBuf, ret);
        }

    }

    return;
}


/*
* @brief: 
*      ��TSMAPע��UCServer��ID��
* @return: 
*      SUCC: ע��ɹ�
*      FAIL: ע��ʧ��
*/
int ucServer::registerToTsMap(void)
{
    char sendBuf[MAX_PACKET_SIZE] = "\0";
    char recvBuf[MAX_PACKET_SIZE] = "\0";
    unsigned short srcAid, srcSaid, desSaid;
    int len = 0;
    int ret = 0;

    /* ��д��TSMAPע����Ϣ��ʽ��
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
            //�Ѿ�ע��,ע��������ע��
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
*      ��TSMAPע��UCServer��ID��
* @return: 
*      SUCC: ע��ɹ�
*      FAIL: ע��ʧ��
*/
int ucServer::unregisterToTsMap(void)
{
    char regBuf[MAX_PACKET_SIZE] = "\0";
    char *pBuf = NULL;
    char recvBuf[MAX_PACKET_SIZE] = "\0";
    unsigned short srcAid, srcSaid, desSaid;
    int len, ret;
    int regAid, regSaid;

    /* ��ѯע��AID,��Ϣ��ʽ:
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
    
    //����ע��AID��ע��SAID
    pBuf = recvBuf + strlen("OK;");
    hex2int(&pBuf, &regAid);
    pBuf += strlen(";");
    
    hex2int(&pBuf, &regSaid);
    
    /*ע����Ϣ��ʽ��
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
        //�ȴ��߳��˳�
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

    //Rhapsodyͨ��Э��,�ȷ���6�ֽڰ�,֪ͨ���ݴ�С,Ȼ���ٰ�֪ͨ��С��������
    while (runFlag)
    {    
        //�������ݴ�С���ݰ�
        memset(buf, 0, MAX_LEN_STR+1);
        recvSize = recv(pSocket->m_sock, buf, MAX_LEN_STR, 0);

        if((SOCKET_ERROR == recvSize
            && (error = WSAGetLastError()) != WSAEWOULDBLOCK)
             || 0 == recvSize)
        {
            //�������
            sysLoger(LWARNING, "ucRxThread::run: link closed, ret:%d, wsaerror:%d", 
                         recvSize, error);
            break;
        }

        sysLoger(LINFO, "ucRxThread::run: recv data:%d", recvSize);

        //���͵�Ŀ��� 
        pUcServer->sendDataToTarget(info.aid, info.said, buf, recvSize, info.sockId, true, this);
        if (!runFlag)
        {
            break;
        }

        //�������ݴ�С
        if ((sscanf(buf, "%d", &dataLen) == 1) && (dataLen > 0) && (dataLen < sizeof(buf)))
        {
            //����С��������
            memset(buf, 0, sizeof(buf));
            recvSize = recv(pSocket->m_sock, buf, dataLen, 0);
            if((SOCKET_ERROR == recvSize
                && (error = WSAGetLastError()) != WSAEWOULDBLOCK)
                 || 0 == recvSize)
            {
                //�������
                sysLoger(LWARNING, "ucRxThread::run: link closed, ret:%d, wsaerror:%d", 
                             recvSize, error);
                break;
            }

            sysLoger(LINFO, "ucRxThread::run: recv data:%d", recvSize);
            
            //���͵�Ŀ���             
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
 *      ȷ�Ϸ�������
 * @param[in] srcAid: ��������AID
 * @param[in] srcSaid: ��������SAID
 * @return: 
 *      true: �ѻ�ȡACK
 *      false: ������ֹ�������˳�
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


