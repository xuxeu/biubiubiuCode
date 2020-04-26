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
* @file     listen.cpp
* @brief     监听线程
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#include "Listen.h"
#include "Protocol.h"
#include "Manager.h"
#include "ClientAgent.h"
#include "tcpDevice.h"
#include "router.h"
/////////////test
#include "common.h"

void Listen::close() 
{
    isRun = false;
    if (pDeviceBase != NULL)
    {
        pDeviceBase->close();
        pDeviceBase = NULL;
    }
}

/**
* 功能：线程运行函数,用来监听客户端的连接,并产生相应的CA线程
*/
void Listen::run() 
{    //进行监听
    pDeviceBase = DeviceManage::getInstance()->createDevice(
                                                                                DeviceConfigBase::TCP_DEVICE);    //创建TCP的socket
    DeviceTcp* pDevTcpListen = (DeviceTcp*)pDeviceBase;    //强制转换成TCP
    if(NULL == pDevTcpListen)
    {
        sysLoger(LERROR, "Listen::run: Alloc tcpdevice failure, ts will exit with error!");
        Manager::getInstance()->stop();    //退出主线程
        return ;
    }
    //创建一个服务器监听socket
    if(!pDevTcpListen->createServerSocket(port)) 
    {
        sysLoger(LERROR, "Listen::run: created the listen socket port failure!");
        Manager::getInstance()->stop();    //退出主线程
        return ;
    }
    
    ClientAgent* pCA = NULL;
    DeviceBase* pDeviceBase = NULL;
    ProtocolBase* pProtocol = NULL;
    DeviceTcp* pDeviceTcp = NULL;
    
    while (isRun) 
    {
        WSocket svrSocket;
        memset(&svrSocket, 0, sizeof(svrSocket));

        if(!pDevTcpListen->acceptSocket(svrSocket))
        {    //监听
            break;
        } 
        else 
        {    //有客户端连接
            try
            {
                pDeviceBase = DeviceManage::getInstance()->createDevice(
                                                                                DeviceConfigBase::TCP_DEVICE);    //创建TCP的socket
                pProtocol = ProtocolManage::getInstance()->createProtocol(
                                                                                ProtocolBase::TCP_PROTOCOL, pDeviceBase);
                pDeviceTcp = (DeviceTcp*)pProtocol->getDevice();    //强制转换成TCP
                //判断创建设备和协议是否成功
                if((NULL == pDeviceTcp) || (NULL == pProtocol))
                {
                    sysLoger(LERROR, "Listen::run: Alloc device or protocol for the Client Agent failure!");
                    continue;
                }
                pDeviceTcp->setTcpSocket(svrSocket.m_sock);
                //创建一个ca对象,并启动ca线程
                US aid = Manager::getInstance()->createID();
                if(0 == aid)
                {
                    sysLoger(LINFO, "Listen::run: Manager alloc ID for client agent failure! ");
                    continue;
                }
                pCA = new ClientAgent(pProtocol, aid);
                if(NULL == pCA)
                {
                    sysLoger(LERROR, "Listen::run: Alloc the ClientAgent failure!");
                    continue;
                }
                //判断TS中的agent是否达到最大
                if(Manager::getInstance()->isAgentCountReachedToMAX())
                {
                    //向客户端回复不能再进行连接，已经达到最大。
                    Packet pack;
                    int size = 0;;
                    pack.setSrc_aid(MANAGER_AID);
                    pack.setDes_aid(pCA->getAID());
                    size = pack.setErrorPacket(ERR_SRV_NUM_EXPIRE, FAILURE_ACK_LEN, 0);
                    pack.setSize(size);
                    pCA->putpkt(&pack);
                    sysLoger(LINFO, "Listen:: listen: TargetServer can't hold agent object any more!");
                    delete pCA;
                    pCA = NULL;
                    continue;
                }
                Router::getInstance()->registerAgent(pCA->getAID(), pCA);    //注册到router
                //向Manager 注册
                Manager::getInstance()->addServerAgent(pCA);
                sysLoger(LINFO, "Listen::run: detected a Client linked, client angent id : %d", aid);
                pCA->start();
            }
            catch(...)
            {
                sysLoger(LERROR, "Listen::run: Catch an exception when allloc the clientAgent!");
                if(pDeviceTcp != NULL)
                {
                    delete pDeviceTcp;
                    pDeviceTcp = NULL;
                }
                if(pProtocol != NULL)
                {
                    delete pProtocol;
                    pProtocol = NULL;
                }
                if(pCA != NULL)
                {
                    delete pCA;
                    pCA = NULL;
                }
                continue;
            }
        }
    }
}

Listen::Listen(int port) 
{
    this->port = port;
    isRun = true;
    pDeviceBase = NULL;
}

