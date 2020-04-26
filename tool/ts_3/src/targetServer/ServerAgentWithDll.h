#ifndef SERVERAGENTWITHDLL_INCLUDE
#define SERVERAGENTWITHDLL_INCLUDE

#include "agent.h"
#include "Windows.h"
#include "OwnerArchive.h"
#include "tsInclude.h"

/**
����: ��̬����TSע��Ļص�����, SA DLL��TX�̻߳���ô˷������յ����ݽ��д���
* @param id sa��Դsaid��
* @param des_said Ŀ��said��
* @param src_aid  ca��aid��
* @param src_said ca��said��
* @param pBuf ���͵Ļ�����
* @param size ��������С
* @return0��ʾʧ��,��0��ʾ�ɹ�
*/
typedef int (*PUTPKT)(short id,short des_said,short src_aid,short src_said,char* pBuf,int size);

/**
����: ��̬����TSע��Ļص�����, SA DLL��RX�̻߳���ô˷������ն�̬������ݽ���ת��
* @param id sa��Դsaid��
* @param des_said[out] Ŀ��said��
* @param src_aid[out]  ca��aid��
* @param src_said[out] ca��said��
* @param pBuf[out] ���͵Ļ�����
* @param size ��������С
* @return ���ݵĴ�С
*/
typedef int (*GETPKT)(short id,short *des_said,short *src_aid,short *src_said,char* pBuf,int size);

/**
����: �򿪶���,���ڶ�̬���ڲ�����ʼ������
* @param id sa��Դsaid��
* @param name ���ݵĲ���
* @return ���ض˿ں�,С��0���ʾʧ��
*/
typedef int (*OPEN)(short id,char* pBuf,char* pOutBuf);

/**
����: �رն���,���ڶ�̬���ڲ��ͷ�ռ����Դ
* @param id sa��Դsaid��
* @param name ���ݵĲ���
* @return �ɹ���ʧ��
*/
typedef int (*CLOSE)(short id);
/**
����: �ͷ�������̬����Դ
* @return 0��ʾ�ɹ�
*/
typedef int (*RELEASEDLL)();

class ServerAgentWithDll :
    public AgentWithQueue
{
private:
    HMODULE m_hDll;
    QString dllName;                        //��Ӧ��̬�������
    QString inBuff;                            //��Ӧ���ݸ���̬����ַ���,�������
    char outBuff[PACKET_DATASIZE];                        //��̬�⻺�������ظ�������,��������
    //��Ӧ�Ķ�̬�⺯��ָ��
    GETPKT get;
    PUTPKT put;
    OPEN open;
    CLOSE closeDll;
    RELEASEDLL releaseDll;    
public:
    PUTPKT getPutFunc() 
    {
        return put;
    }
    GETPKT getGetFunc() 
    {
        return get;
    }
    OPEN getOpenFunc() 
    {
        return open;
    }
    CLOSE getCloseFunc() 
    {
        return closeDll;
    }

    ServerAgentWithDll(unsigned short aid);
    virtual ~ServerAgentWithDll(void);
    /**
    * ���ܣ����ô��ݸ���̬����ַ���
    * @param buf �ַ���
    */
    void setInBuf(const QString& inBuff) 
    {
        this->inBuff = inBuff;
    }
    /**
    * ���ܣ��õ����ݸ���̬����ַ���
    * @return ��̬���ʹ�ò����ַ���
    */
    QString getInBuf() 
    {
        return inBuff;
    }

    /**
    * ���ܣ��õ��Ӷ�̬�ⷵ�صĻ�����
    * @return ��̬���ʹ�ò����ַ���
    */
    char* getOutBuf() 
    {
        return outBuff;
    }
    /**
    * ���ܣ����ö�̬������
    * @param QString ����
    */
    void setDllName(const QString& dllName) 
    {
        this->dllName = dllName;
    }
    /**
    * ���ܣ��õ���̬������
    * @return QString ����
    */
    QString getDllName() 
    {
        return dllName;
    }

    /**
    * ���ܣ���routerע��,����server���պͷ����߳�
    * @return true��ʾ�ɹ�
    */
    bool run() ;

    /**
    * ���ܣ��õ���̬����
    * @return ��̬����
    */
    HMODULE getDllHandler()
    {
        return m_hDll;
    }
    /**
    * ���ܣ�ֹͣserver�߳�
    */
    void stop();
    /**
    * ���ܣ��ر�sa��Ӧ��Դ
    */
    void close() ;
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
    * ���ܣ���������Ϣ���л���OwnerArchive����
    */
    void serialize(OwnerArchive& ar) ;
};
#if 0
//////////////ServerAgent�����߳�
class RXThreadWithDll : public QThread {
private:
    bool runFlag ;                //�߳����еı�־,true��ʾ����,false��ʾ�˳��߳�
    ServerAgentWithDll*  pServerAgent;    //��������������ServerAgentָ��
public:
    RXThreadWithDll (ServerAgentWithDll* pServerAgnet);
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
class TXThreadWithDll : public QThread {
private:
    ServerAgentWithDll*  pServerAgent;    //��������������ServerAgentָ��
    bool runFlag ;        //�߳����еı�־,true��ʾ����,false��ʾ�˳��߳�
    waitCondition queueNotFull;    //���в�Ϊ����������
    TMutex mutex;
public:
    TXThreadWithDll(ServerAgentWithDll* pServerAgnet);
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
#endif
