/*
* 更改历史：
* 2005-02-08 郭建川  北京科银技术有限公司
*                    创建该文件。
*/



/**
*
* @file     gdbServer.h
* @brief
*      功能：
*      <li> 处理转发GDB与TA交互的数据
*      </li>
*
*/


#ifndef GDBSERVER_INCLUDE
#define GDBSERVER_INCLUDE

/************************头文件********************************/
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

/************************宏定义********************************/
#define PACKET_SIZE 1024

/************************类型定义******************************/

class RecvPacket
{
public:
    RecvPacket()
    {
        m_desAid = -1;
        m_desSaid = -1;
        m_srcSaid = 0; //默认用ID 0
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
    TMutex m_cmdMutex;              //对命令缓冲区进行锁定
    T_VBOOL valid;
};

class GdbServer
{
private:
    TMutex clientMutex; //用来保证对客户端互斥操作
    TMutex recvListMutex;   //用于列表互斥
    int port;
    int selfId;
    list<RecvPacket*> recvBufList;  //保存接收包的缓冲列表
    HANDLE  bufSemaphore;               //计数器句柄
    HANDLE m_hReadThread;       //监听线程句柄
    HANDLE m_hExitEvent;        //动态库离开事件对象
    char* pOutBuf;              //调用者传递进来的缓冲区地址

    RSP_MemoryManager memMan;  //内存管理单元
    Cmd m_lastCmd;         //对于GDB上一条命令的记录
    QMap<T_WORD, RSP_Memory*> m_addr2mem; //内存映射表
    T_WORD GdbServer::SetMop(T_BOOL isMop);
    T_BOOL GdbServer::GetMop();
    TMutex m_mopMutex;
    T_BOOL m_mopFlag;  //内存优化算法开启标志

    TMutex m_resetMutex;
    T_BOOL m_resetFlag;


    TMutex m_gdbCmdMutex;
    T_BOOL m_isProcessingGdb;  //是否处于处理GDB命令状态

    TMutex m_aidMutex;

#ifdef TRA_DEBUG
    T_VOID GdbServer::SetLog(T_BOOL isLog);
    T_BOOL GdbServer::IsLog();
    TMutex m_logMutex;
    T_BOOL m_isLog;  //是否需要数据包日志选项，默认为FALSE

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
    * 功能 清空链表中所有数据
    */
    void clearList() ;
    /*
    * 功能 从链表中读第一个表,并删除此链表节点
    * @return  pReceiveBuf    接收到的包
    */
    RecvPacket* getPacketFromList() ;
public:
    volatile bool   m_runFlag;          //运行标志
    WSocket serverSocket;   //动态库对应的服务器socket
    WSocket *sClient;
    ~GdbServer();
    GdbServer(short id);
    short des_aid;
    short des_said;
    short src_said;

    T_VBOOL m_enableSet;      //是否允许设置内存优化算法

    T_WORD GdbServer::SetReset(T_BOOL isReset);
    T_BOOL GdbServer::GetReset();

    //对调试状态下是否允许设置内存优化算法进行标志判断
    T_VOID SetIsProcessingGdb(T_BOOL flag);
    T_BOOL IsProcessingGdb();
    /**
    * 功能：关闭客户端的连接,确保客户端的连接处于可以删除状态
    */
    void closeClient();

    void insertList(RecvPacket* pReceiveBuf);
    static DWORD WINAPI handlerThread(LPVOID lpvoid);
    /**
    * 功能：将数据包发送到GDB
    * @param id sa的源said号
    * @param des_said 目标said号
    * @param src_aid  ca的aid号
    * @param src_said ca的said号
    * @param pBuf 发送的缓冲区
    * @param size 缓冲区大小
    * @return 0表示成功,非0表示失败
    */
    int putpkt(short des_said,short src_aid,short src_said,char* pBuf,int size);

    /**
    * 功能:  SA DLL的RX线程会调用此方法接收动态库的数据进行转发
    * @param id sa的源said号
    * @param des_aid[out] 目标aid号
    * @param des_said[out]  目标said号
    * @param src_said[out] ca的said号
    * @param pBuf[out] 发送的缓冲区
    * @param size 缓冲区大小
    * @return 数据的大小
    */
    int getpkt(short *des_aid,short *des_said,short *src_said,char* pBuf,int size);

    /**
    * 功能: 打开动作,便于动态库内部做初始化工作
    * @param id sa的源aid号
    * @param name 传递的参数
    * @return 0表示成功,
    */
    int open(int port);

    /**
    * 功能: 关闭动作,便于动态库内部释放占用资源
    * @param id sa的源said号
    * @param name 传递的参数
    * @return 0表示成功
    */
    int close();

    /**
    * 功能: 设置缓冲区
    */
    void setOutBuf(char* pOutBuf)
    {
        this->pOutBuf = pOutBuf;
    }
    /**
    * 功能: 得到缓冲区
    */
    char* getOutBuf()
    {
        return pOutBuf;
    }
};


#endif