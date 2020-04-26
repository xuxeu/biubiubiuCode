/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  tsInclulde.h
 * @Version        :  1.0.0
 * @Brief           :  用于包含TS中的常用头文件
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006年12月20日 16:55:21
 * @History        : 
 *   
 */
#ifndef _TSINCLUDE_H_
#define _TSINCLUDE_H_

#include <QMap>
#include <QString>
#include <QStringList>
#include <QSemaphore>
#include <QVector>
#include <QDataStream>
#include <QFile>
#include <QTextStream> 
#include <QTextCodec>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

#include "windows.h"
#include "setupapi.h"
//#include "util.h"
#include "common.h"
/*Modified by tangxp on 2008.1.14 [begin]*/
/*  Modified brief: 增加互斥头文件*/
//#include "Mutex.h"
/*Modified by tangxp on 2008.1.14 [end]*/
#define  _CRT_SECURE_NO_DEPRECATE          //抑制sprintf类函数告警

#define UC unsigned char
#define US unsigned short
#define UI unsigned int
#define UL unsigned long

//文件传送过程状态
#ifndef _FILE_PROCESS_FLAG
#define _FILE_PROCESS_FLAG
#define TS_FILE_BEGIN                              1           //文件传送开始
#define TS_FILE_CONTINUE                       2           //文件传送中
#define TS_FILE_END                                  3          //文件传送完
#define TS_FILE_BEGIN_END                      4         //文件比较小，第一个包就可以传送完
#endif

#define MAX_KEY_LENGTH                  255
#define MAX_VALUE_NAME                  16383
//ts允许sa注册类型数
#define MAX_REG_TYPE  254
#define INVALID_REG_TYPE  255
#define MAX_CHANNEL_COUNT           10

/*Modified by tangxp on 2007.11.12 [begin]*/
/*  brief: 增加常驻模块的注册类型*/
#define PSA_REG_TYPE    254
/*Modified by tangxp on 2007.11.12 [end]*/

/****************定义错误值************************/
#define MAX_ERR_NUM                             100

#define TS_SUCCESS                              0                   //调用成功
#define TS_SOCKET_ERROR                         -1                 //socket错误

#define TS_UNKNOW_ERROR                         -2                 //一般错误
#define TS_SYSTEM_ERROR                         -3                 /*可能是对话管理映射表错误，也可能是多线程调用API引起的错误*/

#define TS_UNKNOW_DATA                          -6                 //收到未知消息，不是这次期望的消息，可能是上次回复的消息等。
#define TS_NULLPTR_ENCONTERED                   -7                 //参数出现空指针
#define TS_FILE_NOT_EXIST                       -8                 //ts, 或者DLL 等文件不存在
#define TS_INVALID_SERVICE_ID                   -10                //无效的服务ID号
#define TS_TIME_OUT                             -11                //超时
#define TS_INVALID_FILE_NAME                    -12                //无效文件名  :如超时定义的最大支持100字符的文件名
#define TS_DEVICE_CANNOT_USE                    -13                //设备目前不可用。
#define TS_REC_BUF_LIMITED                      -14                 //接收到的数据大于接收缓冲区大小
#define TS_INVALID_SERVICE_NAME                 -16                 //非法的服务名字
#define TS_UNKNOW_DATA_FORMAT                   -17                 //收到的数据包是非法格式
#define TS_INVALID_CH_COUNT                     -18                 //非法的通道总数
#define TS_INVALID_CH_ID                        -19                 //非法的通道号
#define TS_INVALID_IP                           -20                 //非法的IP
#define TS_INVALID_UART_PORT                    -21                 //非法的串口
#define TS_SERVICE_NOT_EXIST                    -22                 //指定的服务不存在,
#define TS_SERVICE_NAME_REPEATED                -23                 //创建重名,      
#define TS_UNKNOW_COMM_WAY                      -24                 //未知的通信方式
#define TS_UNKNOW_PROTOCOL                      -25                 //未知的通信协议    
     
#define TS_SRV_CFG_NOT_ENOUGH                   -27                 //服务配置不全

#define TS_SRV_NUM_EXPIRE                       -30                 //创建的服务已经达到最大数量       
#define TS_DLL_BUF_LOST                         -31                 //DLL服务的配置缓冲为空
#define TS_CH_SW_FUNC_LOCKED_BY_OHTER           -32                 //通道切换功能被其他锁定了
#define TS_SERVICE_CONFIG_ERR                   -33               //服务配置错误,如DLL服务配置的缓冲溢出等
#define TS_INVALID_IP_PORT                      -34              //无效的IP端口
#define TS_INVALID_DLL_NAME                     -35             //无效的DLL文件名                   
#define TS_SEND_DATA_FAILURE                    -36             //发送消息失败
#define TS_CHANGE_CHANNEL_FAILURE               -37             //通道切换失败
#define TS_SRV_CHANNEL_SWITCH_DISABLE     -38           //通道切换不允许
#define TS_INVALID_REG_TYPE                     -39              //无效的注册类型

#define FAILURE_ACK_LEN                             3                         //失败回复的长度Exx

#define ERR_UNKNOW_ERR                          "E01"                                     //未知错误
#define ERR_SYSTEM_ERR                            "E01"                                    //系统处理发生错误
#define ERR_UNKNOW_DATA                         "E02"                                   //未知数据
#define ERR_FILE_NOT_EXIST                      "E03"                                   //文件不存在
#define ERR_INVALID_SERVICE_ID                  "E04"                               //非法的服务ID
#define ERR_TIME_OUT                            "E05"                                           //超时
#define ERR_INVALID_FILE_NAME                   "E06"                                  //非法的文件名
#define ERR_DEVICE_CANNOT_USE                   "E07"                               //设备不可用
#define ERR_REC_BUF_LIMITED                     "E08"                                   //接收缓冲限制
#define ERR_INVALID_CH_ID                         "E09"                                   //非法的通道号
#define ERR_INVALID_SERVICE_NAME                "E10"                               //非法的服务名
#define ERR_INVALID_IP                          "E11"                                           //非法IP
#define ERR_INVALID_UART_PORT                   "E12"                                   //非法串口端口
#define ERR_SERVICE_NOT_EXIST                   "E13"                                   //服务不存在
#define ERR_SERVICE_NAME_REPEATED               "E14"                               //服务重名
#define ERR_UNKNOW_COMM_WAY                     "E15"                               //未知的通信方式
#define ERR_UNKNOW_PROTOCOL                     "E16"                                  //未知协议
#define ERR_CH_SW_FUNC_LOCKED_BY_OHTER          "E17"                                 //通道切换功能被其他锁定
#define ERR_SRV_CFG_NOT_ENOUGH                  "E18"                               //服务配置不全
#define ERR_SRV_CFG_ERR                         "E19"                                           //服务配置错误
#define ERR_SRV_NUM_EXPIRE                      "E20"                                       //创建的服务数量达到上限
#define ERR_DLL_BUF_LOST                        "E21"                                           //缺少DLL的BUF配置
#define ERR_CHANGE_CHANNEL_FAILURE              "E23"                                       //通道切换失败

#define ERR_SERVICE_IS_ALREADY_EXIT             "E24"                                      //服务已经存在
#define ERR_SRV_CHANNEL_SWITCH_DISABLE          "E25"                                 //服务不允许通道切换
#define ERR_FILE_ALREADY_EXIT                               "E26"                                   //文件已经存在(用于上传文件时)
//控制分隔符
//const char CONTROL_FLAG[] = ",";
const char CONTROL_FLAG_CHAR = ',';
const char ANSWER_FAIL[] = "E01"; 
const char SUCCESS_ACK[] = "OK";
#define CONTROL_FLAG_LEN         strlen(CONTROL_FLAG)
#define SUCCESS_ACK_LEN     strlen(SUCCESS_ACK)
#define ANSWER_FAIL_LEN    strlen(ANSWER_FAIL)

#define DIDLEN      sizeof(US)             //对话ID的字节长度
#define CONTROL_COMMAND_LEN      1                         //控制识别符的长度
#define FILEBUFSIZE 3*1024             //用于文件传送时每次传送的内容大小
#define EXT_INFO_LEN         128                                //srvcfg扩展字段，用于记录Arch等

//服务通信方式

//控制命令字
const char GETID='I';                                                       //控制包GETID的识别符
const char  CREATE_SERVER='C';                                  //控制包createServer的识别符
const char  CREATE_SERVERWITHDLL='T';                   //控制包createServerWithDll的识别符
const char  CLOSE_TS='D';                                               //关闭ts的识别符
const char  DELETE_SERVER='R';                                   //控制包deleteServer的识别符
const char  CONNECT_FLAG='?';                                   //调用TClient的识别符
const char  QUERY_SERVER_STATE='Q';                     //控制包queryServerState的识别符
const char  SEARCH_SERVER_COUNT='S';                    //控制包searchServerCount的识别符
const char  ACTIVE_SERVER='A';                                   //控制包TClient_activeServer的识别符
const char  REST_SERVER='P';                                        //控制包TClient_inActiveServer的识别符
const char  MODIFY_SERVER='M';                                  //控制包TClient_modifyServer的识别符
const char  REGISTER_SERVER='E';                                //控制包TClient_register的识别符
const char  UNREGISTER_SERVER='U';                          //控制包TClient_unregister的识别符
const char  SET_SERVER_TYPE='s';                                //控制包TClient_setRegisterType的识别符
const char  CLEAR_SERVER_TYPE='c';                          //控制包TClient_clearRegisterType的识别符
const char  CHANGE_CHANNEL_TYPE ='V';                   //控制包TClient_changeChannelType的识别符
//TS3.2新增的命令
const char  QUERY_HOST_COMM_CONFIG = 'p';         //控制包TClient_queryHostSerialPort的识别符
const char  SET_TARGET_SWITCH_FLAG = 'a';           //控制包TClient_setTargetChannelSwitchState识别符
const char  SET_TS_SYSTEM_LOGOUT_LEVEL  = 'L';    //控制包TClient_setSystemLogLevel的识别符
const char  SET_SERVICE_PACK_LOG_FLAG = 'l';         //控制包TClient_SetServiceAgentLog的识别符
const char  CLOSE_LINK = 'd';                                     //控制包TClient_Close的命令识别符
const char  UPLOAD_FILE = 'F';                                    //控制包TClient_UpLoadFile的命令识别字
const char  DOWNLOAD_FILE = 'f';                                //控制包TClient_DownLoadFile的命令识别字
const char  DELETE_FILE = 'K';                                        //控制包TClient_DeleteFile的命令识别字

/********************结构/数据类型定义**********************/

//定义BOOL类型
//WINDOWS.h里定义了FALSE/TRUE为宏，这里消除重定义
#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
enum __BOOL
{
    FALSE,
    TRUE
};
typedef __BOOL _BOOL;

//通信方式及协议方式的类型
enum _PROTOCOL
{
    TCP,
    UDP,
    UART,
    USB,
    DLL,
    BUTT = 255
};
typedef _PROTOCOL PROTOCOL;

#ifndef _SERVER_CONFIG
#define _SERVER_CONFIG
/*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [begin]*/
/*  Modified brief: 因串口号上限256，将串口名称长度改为7位*/
#define UART_NAME_BUF_LEN        7                               //串口名称长度
/*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [end]*/
#define IP_BUF_LEN              16                              //IP地址
#define DLL_BUF                 800                                //传递给动态库的字符串,以分割符号','来分割的
#define NAME_SIZE               100                                //名字的大小

typedef struct COMCONFIG 
{
    PROTOCOL way;									//通信方式的选择,0表示tcp方式,1表示udp方式,2表示串口,4标识动态库方式
    PROTOCOL protocolType;                           //协议类型
    							
    union
    {
        struct 
        {
            //网络参数配置(目标机端)
            char ip[IP_BUF_LEN];                    //IP地址
            UI port;                                //端口号
            UI timeout;                            //超时
            UI retry;                                //重试次数
        } NET;
        
        struct 
        {
            //串口参数配置(主机端)
            char channel[UART_NAME_BUF_LEN];        //串口
            UI baud;                                //波特率
            UI timeout;                            //超时
            UI retry;                                //重试次数
        } UART;
        
        struct 
        {
            char dllName[NAME_SIZE];                //动态库名字,可以带路径
            //动态库方式通信
            char buf[DLL_BUF];                        //动态库的缓冲区
        } DLL;
    }config;
    COMCONFIG* next;							//通信配置信息
}comConfig ;

typedef struct server_config 
{
	char			isLog;							//是否需要日志
	char		name[NAME_SIZE];				//名字
        char        extInfo[EXT_INFO_LEN];          //体系结构等
	US		aid;							//aid号
        UI            state;                            //状态
        UI            reportType;                        //上报类型
        UI            currentChannel;                    //当前通道号
        UI            channelCount;                    //通道数
	COMCONFIG*	pComConfig;						//下一链表节点
}ServerCfg;
#endif 


typedef enum _winversion
{
    UNKNOW,
    WIN95,
    WIN98,
    WINME,
    WINNT,
    WIN2K,
    WINXP,
    WIN2003
}WINVERSION;

extern WINVERSION getWindowsVersion();

#define GUID_LEN                 37

/*定义LambdaICE的GUID*/
DEFINE_GUID(LambdaICE_GUID, 
0x873fdf, 0x61a8, 0x11d1, 0xaa, 0x5e, 0x0, 0xc0, 0x4f, 0xb1, 0x72, 0x8b);
/*****************************************************************************/
extern char* getErrStrByErrValue(int errValue );

/**
 * @Funcname:  guidToString
 * @brief        :  将GUID转换成字符串
 * @para1      : guidData, guid数据
 * @para2      : upcase转换成大小写形式
 * @para3      : strGuid, 转换后的字符串，缓冲由用户分配
 * @para4      : buflen, 用户分配的缓冲大小
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年3月27日 10:44:03
 *   
**/
bool guidToString(GUID guidData, bool upcase, char *strGuid, unsigned short buflen);
/**
 * @Funcname:  stringToGUID
 * @brief        :  将字符串形式的GUID转换成GUID结构数据
 * @para1      : strGuid, 字符串形式的GUID
 * @para2      : len 字符串的长度 
 * @para3      : guidData, 转换后的结构
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年3月27日 10:44:03
 *   
**/
bool stringToGUID(char *strGuid, unsigned short len, GUID *guidData);

#endif /*_TSINCLUDE_H_*/

