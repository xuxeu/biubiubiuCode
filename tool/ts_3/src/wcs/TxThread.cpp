/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  TxThread.cpp
 * @Version        :  1.0.0
 * @Brief           :  数据包发送线程
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:40:13
 * @History        : 
 *   
 */

#include "txthread.h"
#include "packet.h"
#include "log.h"
#include "sysType.h"
#include "comfunc.h"

/************************引用部分*****************************/

/************************前向声明部分***********************/
TxThread* TxThread:: m_pSelf= NULL;
QMutex TxThread::mtxInit;
/************************定义部分*****************************/

/************************实现部分****************************/

using namespace std;

TxThread::TxThread()
{
    m_pDevice = NULL;
    m_runFlag = false;
    m_packLoger.open("wcspacklog.log");
}

TxThread::~TxThread()
{
    if(m_pDevice != NULL)
    {
        delete m_pDevice;
        m_pDevice = NULL;
    }
    Packet *pack = NULL;
    while(!msgQueue.empty())
    {
        pack = msgQueue.front();
        msgQueue.pop();
        if(pack != NULL)
        {
            delete pack;
        }
    }
}

void TxThread::run()
{
    m_runFlag = true;
    Packet *pack = NULL;
    while(m_runFlag)
    {
        mtxQueNotEmpty.lock();
        queNotEmpty.wait(&mtxQueNotEmpty, 5000);
        mtxQueNotEmpty.unlock();
        pack = getPack();
        if(pack != NULL)
        {
            sendPack(pack);
        }
    }
    return;
}

void TxThread::stop()
{
     m_runFlag = false;
}

TxThread* TxThread::getInstance()
{
    if(NULL == m_pSelf)
    {
        mtxInit.lock();
        if(NULL == m_pSelf)
        {
            m_pSelf = new TxThread();
        }
        mtxInit.unlock();
    }
    return m_pSelf;
}

void TxThread::setDevice(tcpDevice * pDevice)
{
    m_pDevice = pDevice;
}

void TxThread::sendPack(Packet * pack, bool bNeedfree)
{
    QMutexLocker locker(&mtxSend);
    
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "TxThread::sendPack: argument pack is bad pointer!");
        return;
        }
    if(NULL == m_pDevice)
    {
        sysLoger(SLWARNING, "TxThread::sendPack: device had not been set!");
        return;
    }
    //sysLoger(SLINFO, "TxThread::sendPack:send ack packet to ts!");
    m_pDevice->write(pack);
    m_packLoger.write(SEND_PACK, pack->getData(), pack->getSize());
    if(bNeedfree)
    {
        PacketManager::getInstance()->free(pack);
        pack = NULL;
    }
    
    return;
}

int TxThread::recvPack(Packet * pack)
{
    int ret = 0;
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "TxThread::recvPack: Argument pack is bad pointer!");
        return FAIL;
    }

    if(NULL == m_pDevice)
    {
        sysLoger(SLWARNING, "TxThread::recvPack: device had not been set!");
        return FAIL;
    }
    ret = m_pDevice->read(pack); 
    m_packLoger.write(RECV_PACK, pack->getData(), pack->getSize());
    return ret;
}

