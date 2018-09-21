/***************************************************************************
 * 
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:ucServer.h,v $
*/


/**
 * @file: ucServer.h
 * @brief: 
 * <li>功能: 定义用户通信服务端数据结构</li>
 * @author: tangxp
 * @date: 2009-12-9
 * <p>部门:系统部
 */




/*****************************引用部分********************************/
#include "common.h"
#include <string.h>
#include "../tsApi/tclientapi.h"
#include "../../include/wsocket.h"

/*****************************声明部分********************************/


/*****************************定义部分********************************/

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
    int curLen;     //发送缓冲中当前数据长度
    int allSize;    //记录准备发送长度,用于识别当前发送缓冲中数据是否是用户发送数据
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
     *      将数据发送到目标机
     * @param[in] size: 缓冲大小
     * @param[out] pDesAid: 发送二级代理AID
     * @param[out] pDesSaid: 发送二级代理SAID
     * @param[out] pData: 发送数据
     * @return: 
     *      SUCC: 发送成功
     *      FAIL: 发送失败
     */
    int getpkt(US *pDesAid, US *pDesSaid, char *pData, int size);

    /*
     * @brief: 
     *      处理目标机发送的数据
     * @param[in] srcAid: 二级代理AID
     * @param[in] srcSaid: 二级代理SAID
     * @param[in] pData: 接收数据
     * @param[in] size: 数据大小
     * @return: 
     *      SUCC: 发送成功
     *      FAIL: 发送失败
     */
    int putpkt(US srcAid, US srcSaid, char *pData, int size);

    /*
     * @brief: 
     *      停止ucServer
     * @return: 
     *      无
     */
    void stop();

    /*
     * @brief: 
     *      发送数据到目标机uc
     * @param[in] pPack: 数据包
     * @param[in] getAck: 是否接收回复
     * @return: 
     *      SUCC: 操作成功
     *      FAIL: 操作失败
     */
    int sendDataToTarget(US desAid, US desSaid, char *buf, int size, int sockId, bool isUserData = false, ucRxThread *pRx = NULL);

    /*
     * @brief: 
     *      UCServer的MAIN函数
     * @return: 
     *      无
     */
    void main(void);


    /*
     * @brief: 
     *      发送数据到TS的线程
     * @return: 
     *      无
     */
    void run();

    /*
    * @brief: 
    *      向TSMAP注销UCServer的ID号
    * @return: 
    *      SUCC: 注册成功
    *      FAIL: 注册失败
    */
    int unregisterToTsMap(void);


    
private:
    
    //回复目标机
    waitCondition replyCondition;
    bool runFlag;

    //目标机和客户端对应表
    list<T_User_Info*> agt2user;
    TMutex agt2userListMtx;

    //待发送消息包列表
    list<Packet*> busyPacketList;
    TMutex busyPacketListMtx;
    
    unsigned short lnkId;        //连接句柄


    /*
     * @brief: 
     *      注册代理，记录客户端port,addr，并连接客户端
     * @param[in] srcAid: 注册二级代理AID
     * @param[in] srcSaid: 注册二级代理SAID
     * @param[in] pData: 发送数据
     * @param[in] size: 数据大小
     * @return: 
     *      SUCC: 注册成功
     *      FAIL: 注册失败
     */
    int registerAgent(US srcAid, US srcSaid, char *pData, int size);

    /*
     * @brief: 
     *      注销代理，记录客户端port,addr，并连接客户端
     * @param[in] srcAid:  注销二级代理AID
     * @param[in] srcSaid:  注销二级代理SAID
     * @return: 
     *      SUCC: 注册成功
     *      FAIL: 注册失败
     */
    int unregisterAgent(US srcAid, US srcSaid, char *pData, int size);

    /*
     * @brief: 
     *      连接用户客户端
     * @param[in] pInfo: 用户信息结构
     * @param[in] addr: 主机名
     * @param[in] port: 端口号
     * @param[in] protocol: 通信协议
     * @return: 
     *      SUCC: 发送成功
     *      FAIL: 发送失败
     */
    int connectUser(T_User_Info *pInfo, char *addr, int port, int protocol);

    /*
     * @brief: 
     *      断开连接用户客户端，删除线程
     * @param[in] aid: 二级代理AID
     * @param[in] said: 二级代理SAID
     * @return: 
     *      SUCC: 发送成功
     *      FAIL: 发送失败
     */
    int disconnectUser(int aid, int said, int sockId);

    /*
     * @brief: 
     *      断开连接所有用户客户端，删除线程
     * @param[in] subId: 二级代理SAID
     * @return: 
     *      无
     */
    void disconnectAllUser();

    void freeAllPacket();

        
    /*
     * @brief: 
     *      从目标机接收数据包到发送客户端缓冲
     *      如果接受完毕，则唤醒发送线程发送到客户端
     * @param[in] srcAid: 二级代理AID
     * @param[in] srcSaid: 二级代理SAID
     * @param[in] pData: 接收数据
     * @param[in] size: 数据大小
     * @return: 
     *      SUCC: 操作成功
     *      FAIL: 操作失败
     */
    int recvDataFromTarget(US srcAid, US srcSaid, char *pData, int size);

    /*
     * @brief: 
     *      注册代理和对应的客户端port,addr
     * @param[in] srcAid: 注册二级代理AID
     * @param[in] srcSaid: 注册二级代理SAID
     * @param[in] addr: 客户端addr
     * @param[in] port: 客户端port
     * @return: 
     *      SUCC: 注册成功
     *      FAIL: 注册失败
     */
    T_User_Info* registerUserInfo(US srcAid, US srcSaid, int sockId);

    /*
     * @brief: 
     *      注销代理和对应的客户端port,addr
     * @param[in] srcAid: 注销二级代理AID
     * @param[in] srcSaid: 注销二级代理SAID
     * @return: 
     *      SUCC: 注销成功
     *      FAIL: 注销失败
     */
    int unregisterUserInfo(US srcAid, US srcSaid, int sockId);

    /*
     * @brief: 
     *      查询代理对应的客户端port,addr
     * @param[in] srcAid: 查询二级代理AID
     * @param[in] srcSaid: 查询二级代理SAID
     * @return: 
     *      代理对应客户端信息结构
     *      NULL: 查询失败
     */
    T_User_Info* queryUserInfo(US srcAid, US srcSaid, int sockId);

    /*
     * @brief: 
     *      确认发送数据
     * @param[in] srcAid: 二级代理AID
     * @param[in] srcSaid: 二级代理SAID
     * @return: 
     *      SUCC: 确认成功
     *      FAIL: 确认失败
     */
    int ackSendData(US srcAid, US srcSaid, char *pData, int size);


    /*
     * @brief: 
     *      发送数据到目标机
     * @return: 
     *      无
     */
    void sendMsgToTarget(void);
        

    /*
     * @brief: 
     *      向TSMAP注册UCServer的ID号
     * @return: 
     *      SUCC: 注册成功
     *      FAIL: 注册失败
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


