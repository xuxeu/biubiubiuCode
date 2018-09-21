/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  listen.cpp
* @brief
*       ���ܣ�
*       <li>�����߳�</li>
*/

/************************ͷ�ļ�********************************/

#include "Listen.h"
#include "Protocol.h"
#include "Manager.h"
#include "ClientAgent.h"
#include "tcpDevice.h"
#include "router.h"

#include "common.h"
/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/
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
* ���ܣ��߳����к���,���������ͻ��˵�����,��������Ӧ��CA�߳�
*/
void Listen::run()
{
    //���м���
    pDeviceBase = DeviceManage::getInstance()->createDevice(
                      DeviceConfigBase::TCP_DEVICE);    //����TCP��socket
    DeviceTcp* pDevTcpListen = (DeviceTcp*)pDeviceBase;    //ǿ��ת����TCP

    if(NULL == pDevTcpListen)
    {
        sysLoger(LERROR, "Listen::run: Alloc tcpdevice failure, ts will exit with error!");
        Manager::getInstance()->stop();    //�˳����߳�
        return ;
    }

    //����һ������������socket
    if(!pDevTcpListen->createServerSocket(port))
    {
        sysLoger(LERROR, "Listen::run: created the listen socket port failure!");
        Manager::getInstance()->stop();    //�˳����߳�
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
        {
            //����
            break;
        }

        else
        {
            //�пͻ�������
            try
            {
                pDeviceBase = DeviceManage::getInstance()->createDevice(
                                  DeviceConfigBase::TCP_DEVICE);    //����TCP��socket
                pProtocol = ProtocolManage::getInstance()->createProtocol(
                                ProtocolBase::TCP_PROTOCOL, pDeviceBase);
                pDeviceTcp = (DeviceTcp*)pProtocol->getDevice();    //ǿ��ת����TCP
                //�жϴ����豸��Э���Ƿ�ɹ�
                if((NULL == pDeviceTcp) || (NULL == pProtocol))
                {
                    sysLoger(LERROR, "Listen::run: Alloc device or protocol for the Client Agent failure!");
                    continue;
                }

                pDeviceTcp->setTcpSocket(svrSocket.m_sock);
                //����һ��ca����,������ca�߳�
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

                //�ж�TS�е�agent�Ƿ�ﵽ���
                if(Manager::getInstance()->isAgentCountReachedToMAX())
                {
                    //��ͻ��˻ظ������ٽ������ӣ��Ѿ��ﵽ���
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

                Router::getInstance()->registerAgent(pCA->getAID(), pCA);    //ע�ᵽrouter
                //��Manager ע��
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

