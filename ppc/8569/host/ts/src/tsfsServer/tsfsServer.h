/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/


/**
* @file     tsfsServer.h
* @brief  
*       ���ܣ�
*       <li>����Server�������ļ����������հ�������������worker����</li>
*/
/************************ͷ�ļ�********************************/
#ifndef _TSFSSERVER_H
#define _TSFSSERVER_H

#include <iostream>
#include <list>
#include <queue>
#include "tsfsWorker.h"
#include "tsfsMutex.h"
using namespace std;

/************************�궨��********************************/
//Worker�̳߳ش�С
#define TSFS_WORKER_MAX_NUM         20

//Packet��������
#define TSFS_PACKET_MAX_NUM         TSFS_WORKER_MAX_NUM

//Worker������д�С
#define TSFS_WORKER_QUEUE_SIZE      1    

//IP��ַ����
#define TSFS_IP4_LEN                16      

//Packet�쳣
#define TSFS_INVALID_PACKET         -40

//Server�˳�
#define TSFS_LINK_ClOSED            -41

#define TSFS_SUCC                   1
#define TSFS_FAIL                   0  

/************************���Ͷ���******************************/
class TSFS_Server
{
private:
    US m_lnkId;                             //���Ӿ��
    char m_tsIpAddr[TSFS_IP4_LEN];          //TS IP��ַ
    char m_userDir[MAX_PATH];               //�û�����·��
    bool m_runFlag;                         //Server���б�־

    list <int> m_fileHandle;                //�򿪵��ļ��������
    TMutex m_fpMutex;                       //�ļ����������  

    //Worker�̳߳�
    TSFS_Worker* m_workerThreadPool[TSFS_WORKER_MAX_NUM];
    list <TSFS_Worker*> m_freeWorkerList;   //����Worker����
    TMutex m_workerMutex;                   //Worker������
    waitcondition *m_pWorkerSemaphore;      //Worker���ź���
    int m_curWorkerNum;                     //������,��¼��ǰWorker����

    //Packet�����
    Packet* m_packetPool[TSFS_PACKET_MAX_NUM];
    list <Packet*> m_emptyPacketList;       //��Packet����
    TMutex m_packMutex;                     //packet������
    waitcondition *m_pPacketSemaphore;      //Packet���ź���
    int m_curPacketNum;                     //������,��¼ĿǰPacket����

    /*
     * @brief
     *   �������մ�������
     * @param packet[out]: ������������ݰ�
     * @return���յ����ݰ����򷵻ؽ��յ����ݰ��ֽڴ�С
     */
    int recvData(Packet *packet);

    /*
     * @brief
     *   ��ȡһ��Pakcet
     * @return��Pakcet
     */
    Packet* getPacket();

    /*
     * @brief
     *   ���մ�������
     * @param packet[out]: ������������ݰ�
     * @return�����յ���������,�ظ�TSFS_SUCC
                ���ӶϿ�,�ظ�TS_SOCKET_ERROR
     */
    int recvPacket(Packet** packet);

    /*
     * @brief
     *   ��ȡ����Worker��
     * @return������Worker
     */
    TSFS_Worker* getFreeWorker();

    /*
     * @brief
     *   ��Worker��������
     * @param packet[in]: �������ݰ�
     * @param worker[in]: ���������Worker
     */
    void processData(Packet *packet, TSFS_Worker *worker);

    /*
     * @brief
     *   ��ȡServer����״̬��־
     * @return��Server����״̬��־
     */
    bool bRuning();

public:
    /**
     * @brief
     *   �����������ݣ������ͷ���Worker��ת����Worker����
     */
    void main();

    /*
     * @brief
     *   �˳�Server
     */
    void stop();

    /**
     * @brief
     *   ��ȡ�û�����·��
     * @return���û�����·��
     */
    char* getUserDir();

    /**
     * @brief
     *   ��ȡTS IP��ַ
     * @return��TS IP��ַ
     */
    char* getTsIPAddr();

    /**
     * @brief
     *   �����ļ��ľ����������������
     * @param fp[in]: �򿪵��ļ����
     */
    void insertFileHandle(int fp);

    /**
     * @brief
     *   �Ӿ������������ɾ���Ѿ��رյ��ļ����
     * @param fp[in]:  �ѹرյ��ļ����
     */
    void removeFileHandle(int fp);

    /**
     * @brief
     *   �ھ�����������в����ļ����
     * @param fp[in]:   ����ѯ���ļ����
     * @return�����ҳɹ����򷵻�TSFS_SUCC
     *          ����ʧ�ܣ��򷵻�TSFS_FAIL
     */
    int searchFileHandle(int fp);

    /**
     * @brief
     *   ��ʹ�����Packet�����Packet����
     * @param pack[in]:   ʹ����ϵ�packet
     */
    void recyclePacket(Packet *pack);

    /**
     * @brief
     *   ������Worker�������Worker����
     * @param pack[in]:   ʹ����ϵ�packet
     */
    void recycleWorker(TSFS_Worker *worker);

    /**
     * @brief
     *    ���캯�������TSFS_Server����ĳ�ʼ�����������������Server�����
     * @param linkId[in]: TSFS Server����������TS�����Ӿ��
     * @param tsIp[in]:  TS IP��ַ
     * @param path[in]: �û�����Ŀ¼
     */
    TSFS_Server(US linkId, char *tsIp, char *path);

    /**
     * @brief
     *    �����������ͷ�Worker�̳߳ء��������������ռ�õ��ڴ�
     */
    ~TSFS_Server();

};

#endif