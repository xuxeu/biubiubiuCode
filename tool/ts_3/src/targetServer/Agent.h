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
* @file     AgentBase.h
* @brief     �����Ĵ������
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/

#ifndef AGENT_INCLUDE
#define AGENT_INCLUDE

#include "tsInclude.h"
//#include "Packet.h"
#include "OwnerArchive.h"
//#include "thread.h"



#include <QtCore>

#define SASTATE_ACTIVE          1
#define SASTATE_INACTIVE      0
#define SASTATE_DELETED       2

//class ServerConfig;
#include "config.h"

class RXThread;
class TXThread;
class AgentBase : public QObject
{
private:
    unsigned short aid;                    //ÿ��agent��Ӧ��aid��
protected:
    int type;                                //������������,1��ʾ���Ƕ�̬��,2��ʾ�Ƕ�̬�⣬3��ʾCA���Ͷ���
public:
    AgentBase(unsigned short aid)
    {
        this->aid = aid;
    }
    virtual ~AgentBase();
    /**
    * ���ܣ��õ�AID��
    * @return short AID��
    */
    unsigned short getAID() 
    {
        return aid; 
    } 
    virtual void putpkt(Packet* pack,bool isRealse) = 0;
    /**
    * ���ܣ��õ��������������
    * @return ������������
    */
    int getType() 
    {
        return type;
    }
    void setType(UC type)
    {
        this->type = type;
        return;
    }
    /**
    * ���ܣ�ֹͣsa
    */
    virtual void stop() = 0;

    //ͨ�ŷ�ʽ
    static const int SERVER_AGENT = 1;                //���Ƕ�̬��
    static const int SERVER_AGENT_WITH_DLL = 2;        //�Ƕ�̬����ʽ��sa
    static const int CLIENT_AGENT = 3;               //CA���Ͷ���
};

const int TIMEOUT = 5000;
class PackLog;
class ProtocolBase;
class AgentWithQueue : public AgentBase 
{
    QList<Packet*> packetQueue;                //����Ҫ���͵�Server���ݰ�������
    waitCondition queueNotFull;            //��������,�����ж϶��в�Ϊ��
    TMutex mutex;                            //�Ի�������������,
    waitCondition channelCondition;            //��������,�����ж϶��в�Ϊ��
    TMutex mutexChannel;                            //�Ի�������������,
    short ackAid;                            //�л�ͨ��ide���͵�id��
protected:
    ProtocolBase* pProtocol;                //��Server��Ӧ��Э��    
    PackLog* pPacketLogger;            //���ݰ���־�����Ǹ���SA����ѡ�񴴽�
    RXThread* pThreadRX;                    //��Ӧ�Ľ����߳�
    TXThread* pThreadTX;                    //��Ӧ�ķ����߳�
    int  state;                                //1�Ǵ���Active 0��passive.
    bool isLog;                                //��־��¼��־
    QString name;                            //��Ӧserver������
    ServerConfig* pConfig;                    //������Ϣ
    int timeout;                            //��ʱ
    int channelType;                        //Ĭ�ϵ������0��ʾĬ��ͨ������
    int proChannelType;                        //�л�ͨ��֮ǰ������
    QVector<QString> vectorChannel;            //ͨ������ 
    QString strData;                        //�û����������
    int changeChannelState;                    //ͨ���л�״̬,0��ʾû���л�,1��ʾ�����л�,2��ʾ�л��ɹ�
    QString extInfo;                            //���ڴ����ϵ�ṹ��
    UC regType;                             //����ע������

    bool canSwitchCh;                          //�Ƿ�����ͨ���л�true,��ʾ����ͨ���л���false��ʾ��ֹ
    US   lockChCA;                               //��ֹͨ���л����ܵĿͻ���ID
    
public:

    AgentWithQueue(unsigned short aid);
    ~AgentWithQueue();
    /**
    * ���ܣ����ͨ��
    */
    void clearChannel() 
    {
        channelType = 0;
        vectorChannel.clear();
    }
    /**
    * ���ܣ��л�ͨ��
    * @return true��ʾ�л��ɹ�,false��ʾ�л�ʧ��
    */
    bool changeChannel();
    /**
    * ���ܣ�����ͨ��������Ϣ
    * @param : strChCfg  ͨ����������Ϣ
    * @return
    */
    void appendChannelConfig(QString strChCfg)
    {
        vectorChannel.append(strChCfg);
    }
    /**
    * ���ܣ��õ�ͨ��������Ϣ����
    * @return ͨ��������Ϣ����
    */
    QVector<QString>& getChannelInfo() 
    {
        return vectorChannel;
    }

    /**
    * ���ܣ��õ�ͨ����
    * @return �õ�ͨ����
    */
    int getChannelCount() 
    {
        return vectorChannel.size();
    }

    void setExtInfo(QString extInfo)
    {
        this->extInfo = extInfo;
        return;
    }

    QString getExtInfo()
    {
        return extInfo;
    }
    
    /**
    * ���ܣ�����ͨ���л�ʹ�ܱ��
    * @param flag, �Ƿ������л�
    * @param aid , ����ͨ���л����ܵ�CA
    * @return 
    */
    void setChannelSwitchState(bool flag, US aid)
    {
        canSwitchCh = flag;
        if(!flag)
        {
            lockChCA = aid;
        }
        else
        {
            lockChCA = 0;          //��ЧCA��AID����ʾͨ��û�б�����
        }
    }
    /**
    * ���ܣ���ȡ����ͨ���л����ܵ�CA aid
    * @return ����ͨ���л����ܵ�CA aid
    */
    US getDisableChannelSwitchCAaid()
    {
        return lockChCA;
    }
    /**
    * ���ܣ��õ�ͨ���л�ʹ�ܱ��
    * @return �õ�ͨ���л�ʹ�ܱ��
    */
    bool getChannelSwitchState()
    {
        return canSwitchCh;
    }
    /**
    * ���ܣ�ע���������
    * @return 
    */
    void setRegisterType(UC regType)
    {
        this->regType = regType;
    }
    /**
    * ���ܣ��õ������ע������
    * @return �����ע������
    */
    UC getRegisterType()
    {
        return regType;
    }
    /**
    * ���ܣ�����������Ϣ
    * @param strChannel ͨ��������Ϣ
    */
    void addChannel(QString& strChannel);
    /**
    * ���ܣ������л�״̬��Ϣ
    * @param �л�״̬��Ϣ
    */
    void setChangeChannelState(int changeChannelState) 
    {
        this->changeChannelState = changeChannelState;
    }
    /**
    * ���ܣ��õ��л�״̬��Ϣ
    * @return   �л�״̬��Ϣ
    */
    int getChangeChannelState() 
    {
        return changeChannelState;
    }
    /**
    * ���ܣ����÷����л�ͨ�������Դaid
    * @param �����л�ͨ�������Դaid
    */
    void setAckAid(short ackAid)
    {
        this->ackAid = ackAid;
    }
    /**
    * ���ܣ��õ������л�ͨ�������Դaid
    * @return   �����л�ͨ�������Դaid
    */
    int getAckAid()
    {
        return ackAid;
    }

    /**
    * ���ܣ��õ�Э��
    * @return   Э��
    */
    ProtocolBase* getProtocol()
    {
        return pProtocol;
    }

    /**
    * ���ܣ��õ�Э��
    * @return   Э��
    */
    void setProtocol(ProtocolBase* pProtocol)
    {
        this->pProtocol = pProtocol;
    }
    /**
    * ���ܣ�����������Ϣ
    * @param ServerConfig* ������Ϣָ��
    */
    void setConfig(ServerConfig* pConfig)
    {
        this->pConfig = pConfig;
    }
    /**
    * ���ܣ��õ�������Ϣ
    * @return ServerConfig*  ������Ϣָ��
    */
    ServerConfig* getConfig() 
    {
        return pConfig;
    }
    /**
    * ���ܣ��ͷ�������Ϣ
    * @return 
    */
    void clearConfig()
    {
        if(pConfig != NULL)
        {
            delete pConfig;
        }
        return;
    }
    /**
    * ���ܣ���������
    * @param QString ����
    */
    void setName(const QString& name)
    {
        this->name = name;
    }
    /**
    * ���ܣ��õ�����
    * @return QString ����
    */
    QString getName()
    {
        return name;
    }
    /**
    * ���ܣ�����״̬
    * @param state 1�Ǵ���Active 0��passive
    */
    void setState(int state) ;
    /**
    * ���ܣ��õ�״̬
    * @return int ״̬,1�Ǵ���Active 0��passive
    */
    int getState();
    /**
    * ���ܣ������Ƿ���־
    * @param isLog true��ʾ��־,flase��ʾ����¼
    */
    void setLogFlag(bool isLog);
    /**
    * ���ܣ��õ���־��־
    * @param isLog true��ʾ��־,flase��ʾ����¼
    */
    bool getLogFlag();

    /**
    * ���ܣ��õ���Ӧ����־����
    * @return PackLog* ��־����
    */
    PackLog* getPacketLogger() 
    {
        return pPacketLogger;
    }

    /**
    * ���ܣ����ö�Ӧ����־����
    */
    void setPacketLogger(PackLog* pPacketLogger) 
    {
        this->pPacketLogger = pPacketLogger;
    }


    /**
    * ���ܣ������û����������
    */
    void setData(QString data) 
    {
        this->strData = data;
    }

    /**
    * ���ܣ��õ��û����������
    */
    QString getData() 
    {
        return strData ;
    }

    int getChannelType() 
    {
        return channelType;
    }

    void setChannelType(int channelType) 
    {
        proChannelType = this->channelType;
        this->channelType = channelType;
    }
    /**
    * ���ܣ�������Ӧ��������Ϣ
    * @param mode ������Ϣ����
    * @param ServerConfig* ������������Ϣ
    */
    ServerConfig* createConfig(int way) ;

    /**
    * ���ܣ��ӻ��������ȡ��һ�������
    * @return ȡ���ɹ�����Packet����ʧ�ܷ���null
    */
    Packet* getpkt();

    /**
    * ���ܣ�����������뵽������ȥ
    * @param pack �����
     * @param isRelease�Ƿ��ͷŰ���Դfalse��ʾ���ͷ�,true��ʾ�ͷ�
    */
    void putpkt(Packet* pack,bool isRelease) 
    {
        mutex.lock();  
        packetQueue.append(pack);
        queueNotFull.wakeAll();
        mutex.unlock();
    }
    /**
    * ���ܣ�ȡ��ʱ,�ȴ��а�������,û�а���һֱ�ȴ�
    */
    void waitUp()
    {    //�ȴ������ź���
        mutex.lock();  
        queueNotFull.wait(&mutex,timeout);    //�ȴ�5��ĳ�ʱ
        mutex.unlock();
    }  
    /**
    * ���ܣ��а��򼤻������źŶ���queueNotFull
    */
    void wakeUp() 
    {    
        mutex.lock();  
        queueNotFull.wakeAll();
        mutex.unlock();
    } 
    /**
    * ���ܣ�����ͨ���л��߳�
    */
    void wakeUpChannel()                 
    {
        TMutexLocker lock(&mutexChannel);
        channelCondition.wakeAll();
    }

    /**
    * ���ܣ��ر�sa��Ӧ��Դ
    */
    virtual void close() ;
    /**
    * ���ܣ���������
    */
    virtual int sendPacket(Packet* pack) ;
    /**
    * ���ܣ���������
    */
    virtual int receivePacket(Packet* pack) ;
    /**
    * ���ܣ���������Ϣ���л���OwnerArchive����
    */
    virtual void serialize(OwnerArchive& ar) ;

    /**
    * ���ܣ���routerע��,����server���պͷ����߳�
    * @return true��ʾ�ɹ�
    */
    virtual bool run();
    /**
    * ���ܣ����豸�ͳ�ʼ��Э��
    * @return true��ʾ�ɹ�
    */
    virtual bool initDevice();

    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [begin]*/
    /*  Modified brief: ���ӻ�ȡ�����ú���*/
    /**
    * ���ܣ���ȡ������
    * @param pConfig ������
    * @param channelType ��ͨ������
    * @param aid ��ȡ���õ�SA��AID
    * @return true��ʾ�ɹ�
    */
    int getNewConfig(ServerConfig * pConfig, int channelType, int aid);
    /*Modified by tangxp for BUG NO.0000994 on 2008.3.28 [end]*/
};

class ProtocolBase;
//////////////ServerAgent�����߳�
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:�õ���д���߳����滻QT�߳���*/
class RXThread : public baseThread 
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
private:
    bool runFlag ;                    //�߳����еı�־,true��ʾ����,false��ʾ�˳��߳�
    AgentWithQueue*  pServerAgent;    //��������������ServerAgentָ��
    waitCondition m_waitCondition;    //ȷ�ϻظ���//δʹ��
    TMutex waitMutex;                //�Ի�������������,
public:
    RXThread (AgentWithQueue* pServerAgnet);
    /**
     * ���ܣ��÷��߳�������־������ֹͣ�߳�
     */
    void stop();
    /**
     * ���ܣ��߳����к���
     */
    void run();

};

//////////////ServerAgent�����߳�
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:�õ���д���߳����滻QT�߳���*/
class TXThread : public baseThread
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
private:
    AgentWithQueue*  pServerAgent;    //��������������ServerAgentָ��
    bool runFlag ;        //�߳����еı�־,true��ʾ����,false��ʾ�˳��߳�
    waitCondition queueNotFull;    //���в�Ϊ����������
    TMutex mutex;
public:
    TXThread(AgentWithQueue* pServerAgnet);
    /**
     * ���ܣ��÷��߳����б�־�����ô˷�����ֹͣ�߳�
     */
    void stop();
    /**
     * ���ܣ��߳����к���
     */
    void run();
};

#endif
