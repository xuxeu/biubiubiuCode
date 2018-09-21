/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  serveragent.h
* @brief
*       ���ܣ�
*       <li> server���������</li>
*/




#ifndef SERVERAGENT_INCLUDE
#define SERVERAGENT_INCLUDE

/************************ͷ�ļ�********************************/
#include "tsInclude.h"


#include "Agent.h"

/************************�궨��********************************/

/************************���Ͷ���******************************/
class DeviceBase;
class Packet;
class ProtocolBase;
class ServerAgent;
class ServerConfig;

class ServerAgent : public AgentWithQueue
{
private:
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.26 [begin]*/
    /*  Modified brief: ���ӷ��ͻ���*/
    TMutex sendMutex;
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.26 [end]*/
public:
    ServerAgent (unsigned short aid);
    virtual ~ServerAgent();

    /**
    * ���ܣ���routerע��,����server���պͷ����߳�
    * @return true��ʾ�ɹ�
    */
    bool run();

    /**
    * ���ܣ�ֹͣserver�߳�
    */
    void stop();

    /**
    * ���ܣ��ر�sa��Ӧ��Դ
    */
    void close() ;

    /**
    * ���ܣ���������Ϣ���л���OwnerArchive����
    */
    void serialize(OwnerArchive& ar) ;

    /**
    * ���ܣ���������
    */
    int sendPacket(Packet* pack);

    /**
    * ���ܣ���������
    * @return ���հ��Ĵ�С
    */
    int receivePacket(Packet* pack);

    /**
    * ���ܣ����豸�ͳ�ʼ��Э��
    * @return true��ʾ�ɹ�
    */
    bool initDevice();
};

/************************�ӿ�����******************************/

#endif








