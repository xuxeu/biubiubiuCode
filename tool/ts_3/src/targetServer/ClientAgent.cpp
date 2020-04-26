/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* 
*
* @file     ClientAgent.cpp
* @brief     
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/
#include "ClientAgent.h"
#include "common.h"
#include "Protocol.h"
#include "Router.h"
#include "tcpDevice.h"
//#include "util.h"
//#include "log.h"

#include "../include/wsocket.h"
ClientAgent::ClientAgent(ProtocolBase* pProtocol,short aid) : AgentBase(aid) 
{

    this->pProtocol = pProtocol;
    runFlag = true;
    type = AgentBase::CLIENT_AGENT;
    exitState = 0;
#ifdef CLIENT_AGENT_DEBUG
    char tmp[50]="\0";
    _itoa(aid, tmp, 10);
    strcat(tmp, "clientAgent.log");
    PacketLogger.open(tmp);
#endif
}

ClientAgent::~ClientAgent() 
{
    while (exitState != 1) 
    {
        Sleep(10);
    }
#ifdef CLIENT_AGENT_DEBUG
    PacketLogger.deleteLogFile();
#endif
}

void ClientAgent::close() 
{
    try
    {
        if (pProtocol != NULL)
        {
            DeviceTcp* pDevice = dynamic_cast<DeviceTcp*>(pProtocol->getDevice());
            if(pDevice != NULL)
            {
                delete pDevice;
                pDevice = NULL;
            }
            delete pProtocol;
            pProtocol = NULL;
        }
    }
    catch(...)
    {
        sysLoger(LERROR, "ClientAgent::close: delete protocol or device failure!");
        return;
    }
}

void ClientAgent::putpkt(Packet* pack,bool isRelease)
{
    TMutexLocker lock(&mutex);
    int ret = 0;
    if (pProtocol == NULL)
    {
        runFlag = false;
    }
    else if (runFlag)
    {
        if(NULL == pack)
        {
            sysLoger(LWARNING, "ClientAgent::putpkt: Bad pointer of pack!");
            return;
        }
        ret = pProtocol->putpkt(pack);
#ifdef CLIENT_AGENT_DEBUG
        if(ret > 0 )
        {
            PacketLogger.write(SEND_PACK, *pack);
        }
        else
        {
            sysLoger(LWARNING, "ClientAgent::putpkt:Put the packet to client failure!");
        }
#endif

    }

    //�ͷŷ���İ�
    if (isRelease)
    {
        PacketManager::getInstance()->free(pack);
    }
}

void ClientAgent::stop()
{
    TMutexLocker lock(&mutex);
    runFlag = false; 
    Router::getInstance()->unRegisterAgent(getAID());    //ע��ca��router��ע��    
    close();
}

void ClientAgent::run() 
{
    //��Ҫע��socket�Ĵ���������ͬһ���߳���,������Щ��Ϣ�����յ�
    while (runFlag) 
    {
        Packet* pack =PacketManager::getInstance()->alloc();
        if(NULL == pack)
        {
            sysLoger(LWARNING, "ClientAgent::run:Alloc pack failure!");
            continue;
        }
        pack->clearData();
        if((NULL == pProtocol) || (pProtocol->getpkt(pack) < 0))
        {  //�����쳣�����ӶϿ�ʱ��ֹͣ���У���Manager������֪ͨManagerɾ��CA�Լ�
            stop();
            
            char* pBuf = pack->getData();
            pack->clearData();
            __store(CURDID(getAID()), pBuf, DIDLEN);
            pBuf += DIDLEN;
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            memcpy(pBuf, &DELETE_SERVER, CONTROL_COMMAND_LEN);                                //deleteServer�Ŀ��ư���־��
            pBuf += CONTROL_COMMAND_LEN;
            pBuf += int2hex(pBuf,10,this->getAID());
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            
            pack->setSize(pBuf-pack->getData());
            pack->setDes_aid(0);    //��manger���Ϳ��������
            pack->setSrc_aid(getAID());
            pack->setDes_said(0);    //��manger���Ϳ��������
            pack->setSrc_said(0);
            Router::getInstance()->putpkt(pack);    //�ɷ���
        }
        else 
        {
            pack->setSrc_aid(getAID());                //����ԴAID��
            Router::getInstance()->putpkt(pack);    //�ɷ���    
#ifdef CLIENT_AGENT_DEBUG
            PacketLogger.write(RECV_PACK, *pack);
#endif

            sysLoger(LDEBUG, "ClientAgent::run:ClientAgent %d recieved a packet, size:%d", 
                                            getAID(), pack->getSize());
        } 
    }
    exitState = 1;
}

