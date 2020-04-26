/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  TxThread.h
 * @Version        :  1.0.0
 * @Brief           :  数据包发送线程
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:39:53
 * @History        : 
 *   
 */

#ifndef _TX_THREAD_H_
#define _TX_THREAD_H_

#include "qthread.h"
#include "qmap.h"
#include "qstring.h"
#include "qmutex.h"
#include "qsemaphore.h"
#include "qwaitCondition.h"
#include "qlist.h"
#include "tcpDevice.h"
#include "packet.h"
#include <queue>



class TxThread    //:public QThread
{
private:
    tcpDevice* m_pDevice;
    QMutex  mtxMsg;
    QMutex  mtxQueNotEmpty;
    QMutex  mtxSend;
    QWaitCondition   queNotEmpty;
    queue<Packet *> msgQueue;
    bool        m_runFlag;
    static QMutex mtxInit;
    static TxThread* m_pSelf;
    
    PackLog m_packLoger;
    
public:
    TxThread();
    ~TxThread();
    void setDevice(tcpDevice* pDevice);
    static TxThread* getInstance();

    void run();
    void stop();
    void pushPack(Packet *pack)
    {
        mtxMsg.lock();
        msgQueue.push(pack);
        mtxQueNotEmpty.lock();
        queNotEmpty.wakeOne();
        mtxQueNotEmpty.unlock();
        //sendPack(pack);
        sysLoger(SLINFO, "TxThread::pushPack:Recived ack packet to ts!");
        mtxMsg.unlock();
    }

    Packet* getPack()
    {
        Packet *pack = NULL;
        mtxMsg.lock();
        if(!msgQueue.empty())
        {
            pack = msgQueue.front();
            msgQueue.pop();
        }
        mtxMsg.unlock();
        return pack;
    }
    void sendPack(Packet *pack, bool bNeedfree = true);
    int    recvPack(Packet *pack);
};

#endif /*_TX_THREAD_H_*/
