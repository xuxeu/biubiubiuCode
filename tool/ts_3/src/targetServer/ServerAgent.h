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
* @file     ServerAgent.cpp
* @brief     server�����ʵ��
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/



#ifndef SERVERAGENT_INCLUDE
#define SERVERAGENT_INCLUDE


#include "tsInclude.h"


#include "Agent.h"
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


#endif








