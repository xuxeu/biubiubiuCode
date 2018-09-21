/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  ClientAgent.h
* @brief
*       ���ܣ�
*       <li>������ͻ���ͨ��</li>
*/


#ifndef CLIENTAGENT_INCLUDE
#define CLIENTAGENT_INCLUDE

/************************ͷ�ļ�********************************/
#include "tsInclude.h"
#include "Agent.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/

class PackLog;
class Packet;
class ProtocolBase;

/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:�õ���д���߳����滻QT�߳���*/
class ClientAgent : public baseThread, public AgentBase
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
private:
    enum ClientAgentStatus
    {
        Initialized = 0,
        Started,
        Finished
    };
    TMutex mutex;                //�Է��ͺͶ�ȡ���ݽ���ͬ��
    int socketDescriptor;        //��Ӧ������socket��   //δʹ��
    ProtocolBase* pProtocol;    //��Ӧ��ͨ��Э��
    bool runFlag ;                //�߳����б�־
    void close() ;
    ClientAgentStatus exitState;   //ca�߳��뿪��־,0��ʾû���뿪,1��ʾ�˳��߳�
#ifdef CLIENT_AGENT_DEBUG
    PackLog PacketLogger;    //��־
#endif

public:
    ~ClientAgent();
    ClientAgent(ProtocolBase* pProtocol, short aid);

    /**
    * ���ܣ������ݰ����͵�����
    * @param pack
     * @param isRelease�Ƿ��ͷŰ���Դfalse��ʾ���ͷ�,true��ʾ�ͷ�
    */
    void putpkt(Packet* pack, bool isRelease = true);

    /**
    * ���ܣ��÷��߳����б�־������ֹͣ�߳�
    */
    void stop();
    void run();
};

/************************�ӿ�����******************************/
#endif









