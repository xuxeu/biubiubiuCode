/***************************************************************************
 * 
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:ucServer.h,v $
*/


/**
 * @file: ucServer.h
 * @brief: 
 * <li>����: �����û�ͨ�ŷ�������ݽṹ</li>
 * @author: tangxp
 * @date: 2009-12-9
 * <p>����:ϵͳ��
 */




/*****************************���ò���********************************/
#include "common.h"
#include <string.h>
#include "../tsApi/tclientapi.h"
#include "../../include/wsocket.h"

/*****************************��������********************************/


/*****************************���岿��********************************/

#define FUNC_INT extern "C" __declspec(dllexport) int   
#define FUNC_VOID extern "C" __declspec(dllexport) void  
#define IP_V4 16
#define NOT_NEED_REPLY 1
#define SUCC    0
#define FAIL     -1
#define MAX_LEN_STR 6
#define MAX_PACKET_NUM 20
#define MAX_PACKET_SIZE 1024
#define TSMAP_AID 1 
#define DEFAULT_WAIT_TIME 2
#define TS_IP           "127.0.0.1"
#define TS_PORT       8000

typedef enum
{
    RROTOCOL_TCP = 0,
    RROTOCOL_UDP
}Ip_Protocol;

class ucRxThread;

typedef struct
{
    int curLen;     //���ͻ����е�ǰ���ݳ���
    int allSize;    //��¼׼�����ͳ���,����ʶ��ǰ���ͻ����������Ƿ����û���������
    char buf[1028*8];
}T_UPacket;

typedef struct
{
    short aid;
    short said;
    int sockId;
    
    WSocket *pSocket;
    ucRxThread *pTx;
    
    T_UPacket upack;
}T_User_Info;

class ucServer: public baseThread
{
public:
    ucServer();
    ~ucServer();

    /*
     * @brief: 
     *      �����ݷ��͵�Ŀ���
     * @param[in] size: �����С
     * @param[out] pDesAid: ���Ͷ�������AID
     * @param[out] pDesSaid: ���Ͷ�������SAID
     * @param[out] pData: ��������
     * @return: 
     *      SUCC: ���ͳɹ�
     *      FAIL: ����ʧ��
     */
    int getpkt(US *pDesAid, US *pDesSaid, char *pData, int size);

    /*
     * @brief: 
     *      ����Ŀ������͵�����
     * @param[in] srcAid: ��������AID
     * @param[in] srcSaid: ��������SAID
     * @param[in] pData: ��������
     * @param[in] size: ���ݴ�С
     * @return: 
     *      SUCC: ���ͳɹ�
     *      FAIL: ����ʧ��
     */
    int putpkt(US srcAid, US srcSaid, char *pData, int size);

    /*
     * @brief: 
     *      ֹͣucServer
     * @return: 
     *      ��
     */
    void stop();

    /*
     * @brief: 
     *      �������ݵ�Ŀ���uc
     * @param[in] pPack: ���ݰ�
     * @param[in] getAck: �Ƿ���ջظ�
     * @return: 
     *      SUCC: �����ɹ�
     *      FAIL: ����ʧ��
     */
    int sendDataToTarget(US desAid, US desSaid, char *buf, int size, int sockId, bool isUserData = false, ucRxThread *pRx = NULL);

    /*
     * @brief: 
     *      UCServer��MAIN����
     * @return: 
     *      ��
     */
    void main(void);


    /*
     * @brief: 
     *      �������ݵ�TS���߳�
     * @return: 
     *      ��
     */
    void run();

    /*
    * @brief: 
    *      ��TSMAPע��UCServer��ID��
    * @return: 
    *      SUCC: ע��ɹ�
    *      FAIL: ע��ʧ��
    */
    int unregisterToTsMap(void);


    
private:
    
    //�ظ�Ŀ���
    waitCondition replyCondition;
    bool runFlag;

    //Ŀ����Ϳͻ��˶�Ӧ��
    list<T_User_Info*> agt2user;
    TMutex agt2userListMtx;

    //��������Ϣ���б�
    list<Packet*> busyPacketList;
    TMutex busyPacketListMtx;
    
    unsigned short lnkId;        //���Ӿ��


    /*
     * @brief: 
     *      ע�������¼�ͻ���port,addr�������ӿͻ���
     * @param[in] srcAid: ע���������AID
     * @param[in] srcSaid: ע���������SAID
     * @param[in] pData: ��������
     * @param[in] size: ���ݴ�С
     * @return: 
     *      SUCC: ע��ɹ�
     *      FAIL: ע��ʧ��
     */
    int registerAgent(US srcAid, US srcSaid, char *pData, int size);

    /*
     * @brief: 
     *      ע��������¼�ͻ���port,addr�������ӿͻ���
     * @param[in] srcAid:  ע����������AID
     * @param[in] srcSaid:  ע����������SAID
     * @return: 
     *      SUCC: ע��ɹ�
     *      FAIL: ע��ʧ��
     */
    int unregisterAgent(US srcAid, US srcSaid, char *pData, int size);

    /*
     * @brief: 
     *      �����û��ͻ���
     * @param[in] pInfo: �û���Ϣ�ṹ
     * @param[in] addr: ������
     * @param[in] port: �˿ں�
     * @param[in] protocol: ͨ��Э��
     * @return: 
     *      SUCC: ���ͳɹ�
     *      FAIL: ����ʧ��
     */
    int connectUser(T_User_Info *pInfo, char *addr, int port, int protocol);

    /*
     * @brief: 
     *      �Ͽ������û��ͻ��ˣ�ɾ���߳�
     * @param[in] aid: ��������AID
     * @param[in] said: ��������SAID
     * @return: 
     *      SUCC: ���ͳɹ�
     *      FAIL: ����ʧ��
     */
    int disconnectUser(int aid, int said, int sockId);

    /*
     * @brief: 
     *      �Ͽ����������û��ͻ��ˣ�ɾ���߳�
     * @param[in] subId: ��������SAID
     * @return: 
     *      ��
     */
    void disconnectAllUser();

    void freeAllPacket();

        
    /*
     * @brief: 
     *      ��Ŀ����������ݰ������Ϳͻ��˻���
     *      ���������ϣ����ѷ����̷߳��͵��ͻ���
     * @param[in] srcAid: ��������AID
     * @param[in] srcSaid: ��������SAID
     * @param[in] pData: ��������
     * @param[in] size: ���ݴ�С
     * @return: 
     *      SUCC: �����ɹ�
     *      FAIL: ����ʧ��
     */
    int recvDataFromTarget(US srcAid, US srcSaid, char *pData, int size);

    /*
     * @brief: 
     *      ע�����Ͷ�Ӧ�Ŀͻ���port,addr
     * @param[in] srcAid: ע���������AID
     * @param[in] srcSaid: ע���������SAID
     * @param[in] addr: �ͻ���addr
     * @param[in] port: �ͻ���port
     * @return: 
     *      SUCC: ע��ɹ�
     *      FAIL: ע��ʧ��
     */
    T_User_Info* registerUserInfo(US srcAid, US srcSaid, int sockId);

    /*
     * @brief: 
     *      ע������Ͷ�Ӧ�Ŀͻ���port,addr
     * @param[in] srcAid: ע����������AID
     * @param[in] srcSaid: ע����������SAID
     * @return: 
     *      SUCC: ע���ɹ�
     *      FAIL: ע��ʧ��
     */
    int unregisterUserInfo(US srcAid, US srcSaid, int sockId);

    /*
     * @brief: 
     *      ��ѯ�����Ӧ�Ŀͻ���port,addr
     * @param[in] srcAid: ��ѯ��������AID
     * @param[in] srcSaid: ��ѯ��������SAID
     * @return: 
     *      �����Ӧ�ͻ�����Ϣ�ṹ
     *      NULL: ��ѯʧ��
     */
    T_User_Info* queryUserInfo(US srcAid, US srcSaid, int sockId);

    /*
     * @brief: 
     *      ȷ�Ϸ�������
     * @param[in] srcAid: ��������AID
     * @param[in] srcSaid: ��������SAID
     * @return: 
     *      SUCC: ȷ�ϳɹ�
     *      FAIL: ȷ��ʧ��
     */
    int ackSendData(US srcAid, US srcSaid, char *pData, int size);


    /*
     * @brief: 
     *      �������ݵ�Ŀ���
     * @return: 
     *      ��
     */
    void sendMsgToTarget(void);
        

    /*
     * @brief: 
     *      ��TSMAPע��UCServer��ID��
     * @return: 
     *      SUCC: ע��ɹ�
     *      FAIL: ע��ʧ��
     */
    int registerToTsMap(void);

};


class ucRxThread: public baseThread
{
public:
    ucRxThread(ucServer *pServer, T_User_Info *pInfo);
    void run();
    void stop();
    void ack();
    bool getAck();

private:

    ucServer *pUcServer;
    bool runFlag;
    T_User_Info info;
    waitCondition ackSem;
};


