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
* @file     ClientAgent.h
* @brief     ������ͻ���ͨ��
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/
#ifndef CLIENTAGENT_INCLUDE
#define CLIENTAGENT_INCLUDE

#include "tsInclude.h"
#include "Agent.h"
//#include "log.h"

//#define CLIENT_AGENT_DEBUG

class PackLog;
class Packet;
class ProtocolBase;
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:�õ���д���߳����滻QT�߳���*/
class ClientAgent : public baseThread,public AgentBase 
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
private:
    TMutex mutex;                //�Է��ͺͶ�ȡ���ݽ���ͬ��
    int socketDescriptor;        //��Ӧ������socket��	//δʹ��
    ProtocolBase* pProtocol;    //��Ӧ��ͨ��Э��
    bool runFlag ;                //�߳����б�־
    void close() ;
    int exitState;                //ca�߳��뿪��־,0��ʾû���뿪,1��ʾ�˳��߳�
#ifdef CLIENT_AGENT_DEBUG
    PackLog PacketLogger;    //��־
#endif

public:
    ~ClientAgent();
    ClientAgent(ProtocolBase* pProtocol,short aid);
    /**
    * ���ܣ������ݰ����͵�����
    * @param pack
     * @param isRelease�Ƿ��ͷŰ���Դfalse��ʾ���ͷ�,true��ʾ�ͷ�
    */
    void putpkt(Packet* pack,bool isRelease = true);
    /**
    * ���ܣ��÷��߳����б�־������ֹͣ�߳�
    */
    void stop();
    void run();
};


#endif









