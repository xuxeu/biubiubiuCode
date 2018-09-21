/*
* ������ʷ��
* 2005-02-08 ������  ���������������޹�˾
*                    �������ļ���
*/



/**
*
* @file     gdbServer.h
* @brief
*      ���ܣ�
*      <li> ����ת��GDB��TA����������
*      </li>
*
*/


#ifndef GDBSERVER_INCLUDE
#define GDBSERVER_INCLUDE

/************************ͷ�ļ�********************************/
#include "sysTypes.h"
#include "stdafx.h"
#include "memory.h"
#include "common.h"
#include <QList>
#include <QtCore>
#include <QMap>
#include <list>
#include <cassert>
using namespace std;

/************************�궨��********************************/
#define PACKET_SIZE 1024

/************************���Ͷ���******************************/

class RecvPacket
{
public:
    RecvPacket()
    {
        m_desAid = -1;
        m_desSaid = -1;
        m_srcSaid = 0; //Ĭ����ID 0
    };

    T_HWORD m_desAid;
    T_HWORD m_desSaid;
    T_HWORD m_srcSaid;
    int size;
    char data[PACKET_SIZE];
};

class Cmd
{
public:
    T_WORD gdbAddr;
    T_WORD gdbSize;
    T_WORD traAddr;
    T_WORD traSize;


    Cmd()
    {

    }

    void setValid(T_BOOL flag)
    {
        TMutexLocker locker(&m_cmdMutex);

        valid = flag;
    }

    T_BOOL getValid()
    {
        TMutexLocker locker(&m_cmdMutex);
        return valid;
    }

private:
    TMutex m_cmdMutex;              //�����������������
    T_VBOOL valid;
};

class GdbServer
{
private:
    TMutex clientMutex; //������֤�Կͻ��˻������
    TMutex recvListMutex;   //�����б���
    int port;
    int selfId;
    list<RecvPacket*> recvBufList;  //������հ��Ļ����б�
    HANDLE  bufSemaphore;               //���������
    HANDLE m_hReadThread;       //�����߳̾��
    HANDLE m_hExitEvent;        //��̬���뿪�¼�����
    char* pOutBuf;              //�����ߴ��ݽ����Ļ�������ַ

    RSP_MemoryManager memMan;  //�ڴ����Ԫ
    Cmd m_lastCmd;         //����GDB��һ������ļ�¼
    QMap<T_WORD, RSP_Memory*> m_addr2mem; //�ڴ�ӳ���
    T_WORD GdbServer::SetMop(T_BOOL isMop);
    T_BOOL GdbServer::GetMop();
    TMutex m_mopMutex;
    T_BOOL m_mopFlag;  //�ڴ��Ż��㷨������־

    TMutex m_resetMutex;
    T_BOOL m_resetFlag;


    TMutex m_gdbCmdMutex;
    T_BOOL m_isProcessingGdb;  //�Ƿ��ڴ���GDB����״̬

    TMutex m_aidMutex;

#ifdef TRA_DEBUG
    T_VOID GdbServer::SetLog(T_BOOL isLog);
    T_BOOL GdbServer::IsLog();
    TMutex m_logMutex;
    T_BOOL m_isLog;  //�Ƿ���Ҫ���ݰ���־ѡ�Ĭ��ΪFALSE

    PacketLogger m_logger;
#endif

    void ReadMemory(T_WORD wAddr, T_WORD wSize);
    T_WORD GetLocalMemory(T_WORD wAddr, T_WORD wSize, T_CHAR *cpBuf);
    T_WORD GetData(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD PutData(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD MessageHandler(RecvPacket *tPack);
    T_VOID RefreshMemory();
    T_VOID UpdateMemory(T_CHAR *pBuf, T_WORD size);
    T_UWORD Hex2int(T_CHAR **cptr, T_WORD *wIntValue);
    T_UWORD Int2hex(T_CHAR *cpPtr, T_WORD wSize,T_WORD wIntValue);
    T_WORD Hex2num(T_UBYTE vubCh);
    T_VOID HandleIDECmd(T_HWORD hwSrcAid,T_HWORD hwSrcSaid, T_CHAR* cpBuf,T_WORD wSize);
    T_VOID SendIDEResult(T_CHAR *cpBuf, T_WORD wSize, T_HWORD hwSrcAid,T_HWORD hwSrcSaid);

    /*
    * ���� �����������������
    */
    void clearList() ;
    /*
    * ���� �������ж���һ����,��ɾ��������ڵ�
    * @return  pReceiveBuf    ���յ��İ�
    */
    RecvPacket* getPacketFromList() ;
public:
    volatile bool   m_runFlag;          //���б�־
    WSocket serverSocket;   //��̬���Ӧ�ķ�����socket
    WSocket *sClient;
    ~GdbServer();
    GdbServer(short id);
    short des_aid;
    short des_said;
    short src_said;

    T_VBOOL m_enableSet;      //�Ƿ����������ڴ��Ż��㷨

    T_WORD GdbServer::SetReset(T_BOOL isReset);
    T_BOOL GdbServer::GetReset();

    //�Ե���״̬���Ƿ����������ڴ��Ż��㷨���б�־�ж�
    T_VOID SetIsProcessingGdb(T_BOOL flag);
    T_BOOL IsProcessingGdb();
    /**
    * ���ܣ��رտͻ��˵�����,ȷ���ͻ��˵����Ӵ��ڿ���ɾ��״̬
    */
    void closeClient();

    void insertList(RecvPacket* pReceiveBuf);
    static DWORD WINAPI handlerThread(LPVOID lpvoid);
    /**
    * ���ܣ������ݰ����͵�GDB
    * @param id sa��Դsaid��
    * @param des_said Ŀ��said��
    * @param src_aid  ca��aid��
    * @param src_said ca��said��
    * @param pBuf ���͵Ļ�����
    * @param size ��������С
    * @return 0��ʾ�ɹ�,��0��ʾʧ��
    */
    int putpkt(short des_said,short src_aid,short src_said,char* pBuf,int size);

    /**
    * ����:  SA DLL��RX�̻߳���ô˷������ն�̬������ݽ���ת��
    * @param id sa��Դsaid��
    * @param des_aid[out] Ŀ��aid��
    * @param des_said[out]  Ŀ��said��
    * @param src_said[out] ca��said��
    * @param pBuf[out] ���͵Ļ�����
    * @param size ��������С
    * @return ���ݵĴ�С
    */
    int getpkt(short *des_aid,short *des_said,short *src_said,char* pBuf,int size);

    /**
    * ����: �򿪶���,���ڶ�̬���ڲ�����ʼ������
    * @param id sa��Դaid��
    * @param name ���ݵĲ���
    * @return 0��ʾ�ɹ�,
    */
    int open(int port);

    /**
    * ����: �رն���,���ڶ�̬���ڲ��ͷ�ռ����Դ
    * @param id sa��Դsaid��
    * @param name ���ݵĲ���
    * @return 0��ʾ�ɹ�
    */
    int close();

    /**
    * ����: ���û�����
    */
    void setOutBuf(char* pOutBuf)
    {
        this->pOutBuf = pOutBuf;
    }
    /**
    * ����: �õ�������
    */
    char* getOutBuf()
    {
        return pOutBuf;
    }
};


#endif