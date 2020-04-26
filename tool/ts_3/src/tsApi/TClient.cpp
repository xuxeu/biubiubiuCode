/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit,  including
*    copying it,  modifying it,  compiling it,  and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice,  this paragraph,  and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* 实现TClient的接口
*
* @file     TClient.cpp
* @brief     实现TClient的接口
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
* @history:  
            1、v3.2 版本中增加对话管理，返回值整理，新增接口等

* 部门：系统部
*
*/

#include "stdafx.h"

//#include "..\..\include\wsocket.h"
#include "wsocket.h"
#include <map>
#include <string>

#include <algorithm>
#include <stdio.h>
#include "windows.h"

#include "TClientAPI.h"
#include "common.h"

using namespace std;


/*****************************定义部分*************************************/

/*Added by qingxh on 2006-10-20 begin*/
#define NULL_PTR  0
#define IsInvalidAID(aid) ((aid < 0 || aid > 32767) ?false:true)
#define CheckInvalidPort(port) (((port<0)||(port>65535))?false:true)
//check the pack by the size , judge the pack is valid one or not
#define CheckValidPackBySize(size) ((size< (int)(DIDLEN + 2* CONTROL_FLAG_LEN +\
                                                                min(ANSWER_SUCEESS_LEN, ANSWER_FAIL_LEN)))?\
                                                                false:true)
//检查注册类型是否合法
#define checkValidRegType(type) ((type < 0 || type > MAX_REG_TYPE)?false:true)
/*Added End */

//控制分隔符
//const char CONTROL_FLAG[] = ",";
char TS_PROGRAM_NAME[] = "ts.exe";
#define CONTROL_FLAG_LEN         strlen(CONTROL_FLAG)
const char ANSWER_SUCEESS[] = "OK";             //返回包成功的字符串
#define ANSWER_SUCEESS_LEN         strlen(ANSWER_SUCEESS)

const char ANSWER_FAIL[] = "E01"; 
#define ANSWER_FAIL_LEN    strlen(ANSWER_FAIL)

/**
 单个16进制ASCII字符。
 */
const char hexchars[]="0123456789abcdef";

#define DIDLEN      sizeof(US)             //对话ID的字节长度
#define FILEBUFSIZE 3*1024             //用于文件传送时每次传送的内容大小

map<US, WSocket*> mapIntToWSocket;            //连接的AID号到wsocket对象的关联
TMutex                      MtxSocket;                              //mapIntToWSocket的保护锁
map<US,  US> mapLnkToDID;                                //连接句柄到对话ID的映射
TMutex           mtxDID;                                    //mapLnkToDID的保护锁

//
typedef map<US,  WSocket* > INT2SOCKET;
typedef map<US,  US> LNK2DID;

 int TIMEOUT_GETPKT = 5;                               //控制包接收返回包超时时间, 单位是秒
const int MAX_TIME_OUT = 15;            //最大超时时间为15秒
const int MIN_TIME_OUT = 5;             //最小的超时时间为5秒
/************定义错误值和消息映射*****************/
#define MAX_ERR_NUM     100
#define ERR_MSG_CHAR_NUM    3
#define CONTROL_COMMAND_LEN      1                         //控制识别符的长度
const char ERRHEAD[] = "E";

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
const char  QUERY_HOST_COMM_CONFIG = 'p';         //控制包TClient_queryHostSerialPort的识别符
const char  SET_TARGET_SWITCH_FLAG = 'a';           //控制包TClient_setTargetChannelSwitchState识别符
const char  SET_TS_SYSTEM_LOGOUT_LEVEL  = 'L';    //控制包TClient_setSystemLogLevel的识别符
const char  SET_SERVICE_PACK_LOG_FLAG = 'l';         //控制包TClient_SetServiceAgentLog的识别符
const char  CLOSE_LINK = 'd';                                     //控制包TClient_Close的命令识别符
const char  UPLOAD_FILE = 'F';                                    //控制包TClient_UpLoadFile的命令识别字
const char  DOWNLOAD_FILE = 'f';                                //控制包TClient_DownLoadFile的命令识别字
const char  DELETE_FILE = 'K';                                        //控制包TClient_DeleteFile的命令识别字

#define ERR_UNKNOW_ERR                          "E01"                                     //未知错误
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
#define ERR_CH_SW_FUNC_LOCKED_BY_OHTER      "E17"                                 //通道切换功能被其他锁定
#define ERR_SRV_CFG_NOT_ENOUGH                  "E18"                               //服务配置不全
#define ERR_SRV_CFG_ERR                         "E19"                                           //服务配置错误
#define ERR_SRV_NUM_EXPIRE                      "E20"                                       //创建的服务数量达到上限
#define ERR_DLL_BUF_LOST                        "E21"                                           //缺少DLL的BUF配置
#define ERR_CHANGE_CHANNEL_FAILURE      "E23"                                       //通道切换失败
#define ERR_SRV_CHANNEL_SWITCH_DISABLE   "E25"                                 //通道切换不允许
#define ERR_FILE_ALREADY_EXIT                               "E26"                            //文件已经存在(文件上传)
/*Modifide by tangxp on 2007.9.26 [begin]*/
/*  Modified brief:增加GUID错误类型 */
#define ERR_INVALID_GUID                        "E27"                       //GUID错误
/*Modifide by tangxp on 2007.9.26 [end] */
typedef struct 
{
    char ErrData[10];
    short ErrValue;
}ERRMSG;
//错误消息包与错误值之间的映射
ERRMSG ERR_MAP[MAX_ERR_NUM] =
{
    {ERR_UNKNOW_ERR,  TS_UNKNOW_ERROR}, 
    {ERR_UNKNOW_DATA,  TS_UNKNOW_DATA}, 
    {ERR_FILE_NOT_EXIST,  TS_FILE_NOT_EXIST}, 
    {ERR_INVALID_SERVICE_ID,  TS_INVALID_SERVICE_ID}, 
    {ERR_TIME_OUT,  TS_TIME_OUT}, 
    {ERR_INVALID_FILE_NAME,  TS_INVALID_FILE_NAME}, 
    {ERR_DEVICE_CANNOT_USE,  TS_DEVICE_CANNOT_USE}, 
    {ERR_REC_BUF_LIMITED,  TS_REC_BUF_LIMITED}, 
    {ERR_INVALID_CH_ID, TS_INVALID_CH_ID},
    {ERR_INVALID_SERVICE_NAME,  TS_INVALID_SERVICE_NAME}, 
    {ERR_INVALID_IP,  TS_INVALID_IP}, 
    {ERR_INVALID_UART_PORT,  TS_INVALID_UART_PORT}, 
    {ERR_SERVICE_NOT_EXIST,  TS_SERVICE_NOT_EXIST}, 
    {ERR_SERVICE_NAME_REPEATED,  TS_SERVICE_NAME_REPEATED}, 
    {ERR_UNKNOW_COMM_WAY,  TS_UNKNOW_COMM_WAY}, 
    {ERR_UNKNOW_PROTOCOL,  TS_UNKNOW_PROTOCOL}, 
    {ERR_SRV_CFG_NOT_ENOUGH,  TS_SRV_CFG_NOT_ENOUGH}, 
    {ERR_SRV_CFG_ERR,  TS_SERVICE_CONFIG_ERR}, 
    {ERR_SRV_NUM_EXPIRE,  TS_SRV_NUM_EXPIRE}, 
    {ERR_DLL_BUF_LOST,  TS_DLL_BUF_LOST}, 
    {ERR_CHANGE_CHANNEL_FAILURE, TS_CHANGE_CHANNEL_FAILURE},
    {ERR_CH_SW_FUNC_LOCKED_BY_OHTER, TS_CH_SW_FUNC_LOCKED_BY_OHTER},
    {ERR_SRV_CHANNEL_SWITCH_DISABLE, TS_SRV_CHANNEL_SWITCH_DISABLE},
    {ERR_FILE_ALREADY_EXIT, TS_FILE_ALREADY_EXITS},
    /*Modifide by tangxp on 2007.9.26 [begin]*/
    /*  Modified brief:增加GUID错误类型映射 */
    {ERR_INVALID_GUID, TS_INVALID_GUID},  
    /*Modifide by tangxp on 2007.9.26 [end] */
    {"", 0}
    
};

/*************前向声明***************************/
//将回复消息包中的DID与当前映射表中的DID进行比较
//如果回复消息包中的DID小于当前映射表中的DID
//则接收下一个回复消息, 否则如果消息中的DID大小
//当前映射表中的DID，则返回系统错误
#define AssertPackDIDAndCurDID(packDID,  curDID) {\
                                                                    if((65535 == packDID) && (0 == curDID))\
                                                                    {\
                                                                        continue;\
                                                                    }\
                                                                    else if(packDID > curDID)\
                                                                    {\
                                                                        return TS_SYSTEM_ERROR;\
                                                                    }\
                                                                    else\
                                                                    {\
                                                                        continue;\
                                                                    }\
                                                                 }



//从指定的错误字符串返回错误值
static short returnErrValueFromErrMsg(const char *buf);
//检查指定IP是否非法IP
static bool checkInvalidIP(const char *ip,  size_t iplen);
//动态调整超时时间
static void adjustSocketTime(bool timeoutFlag);
//过滤控制命令函数内部处理错误
static int filtrateError(int errValue);
/*************实现部分*************************/

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        WSocket::Init();    //初始化socket环境
    return TRUE;
}


/**
 * 功能: 将value的值, 保存在缓冲区dest中, 占用的大小位bytes
 * @param value  要保存的数据
 * @param dest   缓冲区指针
 * @param bytes  要保存数据的大小
 * @return 0表示成功
 */
template<class T> int __store(T value, char *dest, size_t bytes) 
{
    int upper_bit = (value < 0 ? 128 : 0);
    size_t i;

    if (value < 0)
    {
        T oldvalue = value;
        value = -value;
        if (oldvalue != -value)
            return 1;
    }

    size_t iCount = (sizeof (value) < bytes ? sizeof (value) : bytes) ;
    for(i = 0 ; i < iCount ; i++)
    {
        dest[i] = (char)( value & (i == (bytes - 1) ? 127 : 255));
        value = value / 256;
    }

    if (value && value != -1)
        return 1;

    for(; i < bytes ; i++)
        dest[i] = 0;
    dest[bytes - 1] |= upper_bit;
    return 0;
}

/**
 * 功能: 从缓冲区dest中读取bytes字节, 并保存到dest中
 * @param value  要保存的数据
 * @param dest   缓冲区指针
 * @param bytes  要保存数据的大小
 * @return 0表示成功
 */
template<class T> int __fetch(T *dest, char *source, size_t bytes) 
{
    T value = 0;
    int i;

    for (i = bytes - 1; (int) i > (sizeof (*dest) - 1); i--)
        if (source[i] & ((int) i == (bytes - 1) ? 127 : 255 ))
            return 1;

    for (; i >= 0; i--)
        value = value * 256 + (source[i] & ((int)i == (bytes - 1) ? 127 : 255));

    if ((source[bytes - 1] & 128) && (value > 0))
        value = - value;

    *dest = value;
    return 0;
}



/**
 * 功能: 将单个的16进制ASCII字符转换成对应的数字。
 * @param ch    单个的16进制ASCII字符
 * @param intValue[OUT]   转换后的32位整数
 * @return 返回转换的16进制字符串长度。
*/
int hex2num(unsigned char ch)
{
  if (ch >= 'a' && ch <= 'f')
    return ch-'a'+10;
  if (ch >= '0' && ch <= '9')
    return ch-'0';
  if (ch >= 'A' && ch <= 'F')
    return ch-'A'+10;
  return -1;
}
/**
 * 功能: 将16进制字符串转换成对应的32位整数。
 * @param ptr[OUT] 输入指向16进制字符串的指针，转换过程中指针同步前
 *        进。输出转换结束时的指针。
 * @param intValue[OUT]   转换后的32位整数
 * @return 返回转换的16进制字符串长度。
*/
unsigned int hex2short(char **ptr,  short *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
        hexValue = hex2num(**ptr);
        if (hexValue < 0)
            break;

        *intValue = (*intValue << 4) | hexValue;
        numChars ++;

        (*ptr)++;
    }

  return (numChars);
}
/**
 * 功能: 将16进制字符串转换成对应的32位整数。
 * @param ptr[OUT] 输入指向16进制字符串的指针，转换过程中指针同步前
 *        进。输出转换结束时的指针。
 * @param intValue[OUT]   转换后的32位整数
 * @return 返回转换的16进制字符串长度。
*/
unsigned int hex2int(char **ptr,  int *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
      hexValue = hex2num(**ptr);
      if (hexValue < 0)
        break;

      *intValue = (*intValue << 4) | hexValue;
      numChars ++;

      (*ptr)++;
    }

  return (numChars);
}


/**
 * 功能: 把int转换成16进制的字符串, 必须保证size大小大于转换后的字符串大小
 * @param ptr 保存16进制字符串的缓冲区
 * @param size   缓冲区的大小
 * @param intValue   要转换的整形
 * @return 转换的大小
*/
unsigned int int2hex(char *ptr,  int size, int intValue)
{
    memset(ptr, '\0', size);    //设置大小, 最大缓冲区的大小
    sprintf_s(ptr, size,  "%x", intValue);
    return (UI)strlen(ptr);
}

/********************************************************************
 * 函数名:   strtoi
 --------------------------------------------------------------------
 * 功能:     将字符串转换成整数。
 --------------------------------------------------------------------
 * 输入参数: nptr ---指向字符串的指针。
 *             base ---转换的进制。
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无。
 --------------------------------------------------------------------
 * 返回值:     返回转换后的整数。
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
int strtoi(char *nptr,  int base)
{
#define LONG_MIN_ (0x80000000)
#define LONG_MAX_ (0x7fffffff)
    register const char *s = nptr;
    register unsigned int acc;
    register int c;
    register unsigned int cutoff;
    register int neg = 0,  any,  cutlim;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0,  allow 0x for hex and 0 for octal,  else
     * assume decimal; if base is already 16,  allow 0x.
     */
    do
    {
        c = *s++;
    } while (isspace(c));
    if (c == '-') 
    {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) 
        && (c == '0') && (*s == 'x' || *s == 'X')) 
    {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value,  divided by the
     * base.  An input number that is greater than this value,  if
     * followed by a legal input character,  is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance,  if the range for longs is
     * [-2147483648..2147483647] and the input base is 10, 
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1),  meaning that if we have accumulated
     * a value > 214748364,  or equal but the next digit is > 7 (or 8), 
     * the number is too big,  and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    cutoff = neg ? (-(unsigned int)LONG_MIN_) : LONG_MAX_;
    cutlim = cutoff % (unsigned int)base;
    cutoff /= (unsigned int)base;
    for (acc = 0,  any = 0;; c = *s++) 
    {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else 
        {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0) 
    {
        acc = neg ? LONG_MIN_ : LONG_MAX_;
    } else if (neg)
        acc = -acc;
    return (acc);
}

/**
 * 功能: 关闭指定的连接
 * @param fd  连接句柄
 * @return : 
         TS_INVALID_LINK_HANDLE: 参数fd是无效的句柄
         TS_SOCKET_ERROR:  如果发给TS命令包时SOCEKT发生错误或关闭返回该值
         TS_SUCCESS:    成功关闭。    
 */
DLL_FUNC_INT TClient_close(US fd)
{
    char buf[100] = "\0";
    char *pBuf = buf;
    US curDID = 0;
    int ret = 0;
    
    //找到响应的wsocket对象
    TMutexLocker lockerSocket(&MtxSocket);
    TMutexLocker lockerDID(&mtxDID);
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    INT2SOCKET::iterator  m_ItrSocket;
    m_ItrSocket = mapIntToWSocket.find(fd);
    WSocket* sktClient = NULL;
    //发送消息给TS，以通知其删除相关数据
    if(m_ItrDID == mapLnkToDID.end())
    {
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf,  &CLOSE_LINK,  CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd,  0,  0,  0,  buf,  (UI)(pBuf - buf));   
    if(ret <= 0)
    {
        //消息未发送成功
        return filtrateError(ret);
    }
    //发送成功，删除本端对象及数据
    if (m_ItrSocket != mapIntToWSocket.end()) 
    {
        sktClient = m_ItrSocket->second;
        sktClient->Close();
        mapIntToWSocket.erase(m_ItrSocket);
        delete sktClient;
        sktClient = NULL;
    }
    if(m_ItrDID != mapLnkToDID.end())
    {
        mapLnkToDID.erase(m_ItrDID);
    }
    return TS_SUCCESS;
    
}

/**
 * 功能: 将数据发送到指定的目标服务的应用代理处
 * @param fd  连接句柄
 * @param des_aid  目标aid号
 * @param des_said 目标said号
 * @param src_said 源said号
 * @param data  数据包
 * @param size  数据包大小
 * @return
        如果发送数据成功返回实际发送的数据的字节大小
        TS_NULLPTR_ENCONTERED: 包内容的缓冲为空指针
        TS_INVALID_LINK_HANDLE: 传入的连接句柄号为空
        TS_SOCKET_ERROR:  发包时SOCKET发生错误
 */
DLL_FUNC_INT TClient_putpkt(US fd, US des_aid, US des_said, US src_said, 
                             const char* data, UI size) 
{
    if (data == NULL)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    //通过AID找到对应的socket对象
    MtxSocket.lock();
    INT2SOCKET::iterator m_ItrSocket;
    m_ItrSocket = mapIntToWSocket.find(fd);
    WSocket sktClient;
    if (m_ItrSocket != mapIntToWSocket.end()) 
    {
        sktClient = *(m_ItrSocket->second);
        MtxSocket.unlock();
    }
    else
    {
        MtxSocket.unlock();
        return TS_INVALID_LINK_HANDLE;        
    }

    //对包头进行字节序列操作
    char head[PACKET_HEADSIZE] = "\0";
    char* pBuf = head;
    __store(size, pBuf, sizeof(int));
    pBuf += sizeof(int);
    __store(des_aid, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store((short)fd, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(des_said, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(src_said, pBuf, sizeof(short));
    int reserve = 0;
    int iRet = 0;
    iRet = sktClient.Send(head,  PACKET_HEADSIZE, 0);    //发送包头信息
    if (iRet == -1)    //连接的socket不存在
    {
        return TS_SOCKET_ERROR;
    }
    iRet = sktClient.Send(data,  size,  0);                //发送包体信息
    if (iRet == -1)    //连接的socket不存在
    {
        return TS_SOCKET_ERROR;
    }
    return iRet;
}
/**
 * 功能: 接收目标服务的数据
 * @param fd  连接句柄
 * @param des_aid  目标aid号
 * @param des_said 目标said号
 * @param src_said 源said号
 * @param data  数据包
 * @param dataSize  缓冲区的大小
 * @return 
        如果成功便返回实际得到数据包的字节大小
        TS_NULLPTR_ENCONTERED:接收缓冲的指针为空.
        TS_INVALID_LINK_HANDLE: 无效的句柄号
        TS_SOCKET_ERROR:收包时发生SOCKET错误.
        TS_REC_BUF_LIMITED:  接收缓冲的大小小于实际接收到的数据的字节大小
 */
DLL_FUNC_INT TClient_getpkt(US fd, US* src_aid, US* des_said, US* src_said, 
                            char* data, UI dataSize) 
{
    if (src_aid == NULL || des_said == NULL || src_said == NULL || data == NULL)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    /*Modified by qingxiaohai on 2006年11月17日 11:07:46 for BUG 1102 [Begin]*/
    /*  Modified brief:初始化接收缓冲*/
    memset(data,  0,  dataSize);
    /*Modified for BUG  [End]*/
    //通过AID找到对应的socket对象
    MtxSocket.lock();
    INT2SOCKET::iterator m_ItrSocket;
    m_ItrSocket = mapIntToWSocket.find(fd);
    WSocket sktClient;
    if (m_ItrSocket != mapIntToWSocket.end()) 
    {
        sktClient = *(m_ItrSocket->second);
        MtxSocket.unlock();
    }
    else
    {
        MtxSocket.unlock();
        return TS_INVALID_LINK_HANDLE;        
    }
    
    char recvbuf[PACKET_HEADSIZE] = "\0";
    char* pBuff = recvbuf;

    //读取包头
    UI size = 0;
    int rs = sktClient.Recv(recvbuf,  sizeof(recvbuf));
    if ( rs <= 0 )
    {
        return TS_SOCKET_ERROR;
    }
    //检查缓冲区的大小与收到包的大小
    //对接收到的包头按照字节码的形式解码
    __fetch(&size, recvbuf, sizeof(int));
    pBuff += sizeof(int);
    
    short des_aid = 0;
    __fetch(&des_aid, pBuff, sizeof(short));
    pBuff += sizeof(short);    
    __fetch(src_aid, pBuff, sizeof(short));
    pBuff += sizeof(short);
    __fetch(des_said, pBuff, sizeof(short));
    pBuff += sizeof(short);
    __fetch(src_said, pBuff, sizeof(short));
    pBuff += sizeof(short);
    int reserve = 0;
    __fetch(&reserve, pBuff, sizeof(int));
    //要接收的数据大于缓冲区的数据
    if (size > dataSize)
    {
        return TS_REC_BUF_LIMITED;
    }
    //读取包体
    rs = sktClient.Recv(data,  size,  0) ;
    if ( rs <= 0 ) 
    {
        return TS_SOCKET_ERROR;
    }

    return rs;
}
/**
 * 功能: 提供非阻塞通信方式, 用户可以设定一定时间的timeout
 * @param fd  连接句柄
 * @param timeout  等待时间,单位:秒
 * @return 
        TS_DATACOMING:在指定的时间内有数据到来
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_TIME_OUT: 指定的时间内没有数据到来
        TS_SOCKET_ERROR:在查询SOCKET缓冲时SOCKET错误,可能是对端关闭连接.
 */
DLL_FUNC_INT TClient_wait(US fd, int timeout) 
{
    //通过AID找到对应的socket对象
    MtxSocket.lock();
    INT2SOCKET::iterator m_ItrSocket;
    m_ItrSocket = mapIntToWSocket.find(fd);
    WSocket  sktClient;
    if (m_ItrSocket != mapIntToWSocket.end()) 
    {
        sktClient = *(m_ItrSocket->second);
        MtxSocket.unlock();
    }
    else
    {
        MtxSocket.unlock();
        return TS_INVALID_LINK_HANDLE;        
    }
    
    if (timeout > -1) 
    {
        int flags = 0;
        struct timeval overtime;
        //连接超时66毫秒
        overtime.tv_sec = timeout;
        overtime.tv_usec = 0;
        FD_SET RSet;
        FD_SET WSet;
        FD_ZERO(&RSet);
        FD_ZERO(&WSet);
        FD_SET(sktClient.m_sock,  &RSet);
        FD_SET(sktClient.m_sock,  &WSet);

        int ret = select(0,  &RSet,  NULL,  NULL,  &overtime);
        if ( ret == 0 ) 
        {
            adjustSocketTime(true);
            return TS_TIME_OUT;        //超时
        }
        else if (ret < 0)    //socket关闭
            return TS_SOCKET_ERROR;
    }
    adjustSocketTime(false);
    return TS_DATACOMING;
}
/**
 * 功能: 与指定IP地址和端口号的TS建立连接
 * @param IP  TS所在的IP地址
 * @param port  端口号, 0标识采用默认端口port, 非0表示指定端口
 * @return 
        操作成功返回大于0表示得到的aid号
        TS_NULLPTR_ENCONTERED :传入的IP缓冲为空指针
        TS_INVALID_IP: IP为无效IP,如0.0.0.0/255.255.255.255等.
        TS_SOCKET_ERROR: 试图和对端建立连接的时候SOCKET发生错误
        TS_ISNOT_TS_SERVER:  对端没有启动TS服务.
        TS_UNKNOW_DATA  :对端返回的消息是未知消息包,或格式不可识别
 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: 将TClient_open返回类型又短整形(short)改为整形(int)*/
DLL_FUNC_INT TClient_open(const char* IP, US port)
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end] */
{
    if (IP == NULL)
        return TS_NULLPTR_ENCONTERED;
    if(!checkInvalidIP(IP, (UI)strlen(IP)))
    {
        return TS_INVALID_IP;
    }
    WSocket *sktClient = new WSocket;
    sktClient->Create(AF_INET,  SOCK_STREAM,  0);    //创建面向连接socket
    // Connect
    if(port == 0)
    {
        port = LISTEN_PORT;
    }
    if( sktClient->Connect(IP,  port) != true ) 
    {
        delete sktClient;
        return TS_SOCKET_ERROR;
    }
    //向manager发送字符'?', 接收到manager为此分配的AID号
    /*Modified by qingxiaohai on 2006年12月14日 15:48:19 for  TS3.2 [Begin]*/
    /*  Modified brief:增加控制消息的对话ID*/
    US dialogID = 0, packContentLen = 0;
    char* pBuf = NULL;
    char buf[5] = "\0";
    pBuf = buf;
    //fill dialog ID
    __store(dialogID,  pBuf,  DIDLEN);            //填充对话ID
    pBuf+= DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);          //控制分隔符
    pBuf += CONTROL_FLAG_LEN;
    //control command
    memcpy(pBuf,   &CONNECT_FLAG,  CONTROL_COMMAND_LEN);                                //打开连接的消息标识
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    packContentLen = pBuf - buf;
    /*Modified for BUG  [End]*/
        
    //对包头进行字节序列操作
    char head[PACKET_HEADSIZE] = "\0";
    pBuf = head;
    __store((int)(packContentLen) , pBuf, sizeof(int));    //发送DIDLEN+2个字节'?'
    pBuf += sizeof(int);
    __store(0, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(0, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(0, pBuf, sizeof(short));
    pBuf += sizeof(short);
    __store(0, pBuf, sizeof(short));
    int reserve = 0;

    sktClient->Send(head,  PACKET_HEADSIZE, 0);    //发送包头信息
    sktClient->Send(buf,  (int)packContentLen,  0);                //发送包体信息

    //设置等待时间
    int flags = 0;
    struct timeval overtime;
    //连接超时5 秒
    overtime.tv_sec = TIMEOUT_GETPKT;
    overtime.tv_usec = 0;
    FD_SET RSet;
    FD_SET WSet;
    FD_ZERO(&RSet);
    FD_ZERO(&WSet);
    FD_SET(sktClient->m_sock,  &RSet);
    FD_SET(sktClient->m_sock,  &WSet);

    int ret = select(0,  &RSet,  NULL,  NULL,  &overtime);
    if ( ret <= 0 ) 
    {    //超时或者socket有异常
        sktClient->Close();
        delete sktClient;
        return TS_ISNOT_TS_SERVER;
    }

    US des_aid, src_aid, des_said, src_said;
    UI size;
    //读取manager返回的数据
    char recvbuf[PACKET_HEADSIZE] = "\0";
    char* pBuff = recvbuf;
    US  recvDID = 0;
    //读取消息包包头
    int rs = sktClient->Recv(recvbuf,  sizeof(recvbuf));
    if( rs <= 0 ) 
    {
        return TS_SOCKET_ERROR;
    }
    /*接收到的包格式
    ---------------
    |MSGHEAD|DID|, |
    ---------------
    */

    if(rs != PACKET_HEADSIZE)
    {
        return TS_UNKNOW_DATA;
    }
    //对接收到的包头按照字节码的形式解码
    __fetch(&size, pBuff, sizeof(UI));
    pBuff += sizeof(UI);

    __fetch(&des_aid, pBuff, sizeof(US));
    pBuff += sizeof(US);
    __fetch(&src_aid, pBuff, sizeof(US));
    pBuff += sizeof(US);
    __fetch(&des_said, pBuff, sizeof(US));
    pBuff += sizeof(US);
    __fetch(&src_said, pBuff, sizeof(US));
    pBuff += sizeof(US);
    __fetch(&reserve, pBuff, sizeof(int));
    pBuff += sizeof(int);

    if (size == DIDLEN+CONTROL_FLAG_LEN) 
    {
        //接收包体
        rs = sktClient->Recv(recvbuf,  DIDLEN+CONTROL_FLAG_LEN);
        if( rs <= 0 ) 
        {
            return TS_SOCKET_ERROR;
        }
        pBuff = recvbuf;
        __fetch(&recvDID,  pBuff,  DIDLEN);
        if(recvDID != dialogID)
        {
            return TS_UNKNOW_DATA;
        }
        //接收到连接的信息
        MtxSocket.lock();
        mtxDID.lock();
        mapIntToWSocket.insert(INT2SOCKET::value_type(des_aid, sktClient));
        mapLnkToDID.insert(LNK2DID::value_type(des_aid,  dialogID));
        MtxSocket.unlock();
        mtxDID.unlock();
        return des_aid;
    }
    else 
    {
        sktClient->Close();
        delete sktClient;
        return TS_UNKNOW_DATA;
    }
    
}

/**
 * 功能:  根据目标服务的名字查找对应的ID号
 * @param fd  连接句柄
 * @param name  目标名字, 一个以0结尾的字符串
 * @return 
    操作成功返回服务对应的目标服务的ID号
    TS_NULLPTR_ENCONTERED: name为空指针
    TS_INVALID_LINK_HANDLE : 无效的连接句柄
    TS_INVALID_SERVICE_NAME: name字符串长度大小系统规定的名字长度
    TS_UNKNOW_DATA: 未知的数据格式或数据包
    TS_SYSTEM_ERROR:  系统处理发生错误,有可能是收到的数据包的对话
                                    ID号与期待的不一致,也有可能返回的是无效的AID号
    TS_TIME_OUT:  在指定的收包时间内没有收到数据包
    
                                    
发送格式：
                            -------------------
                            |DID|, |I name|, |
                            -------------------
接收格式：
                          回复数据包格式
                            -------------------
                            |DID|, |OK|, |serverID|,|
                            -------------------
                
*/
DLL_FUNC_INT TClient_getServerID(US fd, char* name) 
{
    //进行发送名字的组包
    if(name == NULL)
        return TS_NULLPTR_ENCONTERED;

    if (!strlen(name) || strlen(name) > (NAME_SIZE-1))
        return TS_INVALID_SERVICE_NAME;
    int nameSize = (int)strlen(name);
    char buf[PACKET_DATASIZE] = "\0";
    char* pBuff = buf;
    int ret = 0;
    US  packDID = 0,  curDID = 0;
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
     curDID = m_ItrDID->second +1;
     m_ItrDID->second = curDID;
     mtxDID.unlock();
    //fill dialog ID
    __store(curDID,  pBuff,  DIDLEN);
    pBuff += DIDLEN;
    memcpy(pBuff,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuff += CONTROL_FLAG_LEN;
    //fill the command
    memcpy(pBuff,  &GETID,  CONTROL_COMMAND_LEN);                                //getID的控制包标志符
    pBuff += CONTROL_COMMAND_LEN;
    memcpy(pBuff, name, nameSize);                            //名字
    pBuff += nameSize;
    memcpy(pBuff,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuff += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuff-buf));        //向manager发送命令包
    if (ret < 0 )
        return filtrateError(ret);
    
    //读取manager返回的数据
    US des_aid, des_said=0, src_said;
    int bufSize = sizeof(buf);
    US aid = 0;
    char* pStr = NULL;
    while(true)
    {
        if (TClient_wait(fd,  TIMEOUT_GETPKT) == TS_DATACOMING)   //等待5秒, 在5秒内有数据则进行数据的接收
        {
            //动态调整超时时间
            adjustSocketTime(false);
            //读取manager返回的数据, 假使返回的信息不会超过1个数据包的大小, 即大小不能超过PACKET_DATASIZE
            ret = TClient_getpkt(fd,  &des_aid,  &des_said,  &src_said,  buf,  sizeof(buf));
            if (ret < 0 )
                return filtrateError(ret);
            //进行收包的大小判断
            /**
            正确包的格式为
            -------------------
            |DID|, |OK|, |serverID|,|
            -------------------
            */
            pBuff = buf;
            if(!CheckValidPackBySize(ret))
            {
                return TS_UNKNOW_DATA;
            }
            __fetch(&packDID,  pBuff,  DIDLEN);
            pBuff +=  DIDLEN + CONTROL_FLAG_LEN;
            if(packDID == curDID)
            {
                //解析数据包
                ret = returnErrValueFromErrMsg(pBuff);
                if(ret != TS_SUCCESS)
                {
                    return ret;
                }
                
                pStr = strstr(pBuff, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                pStr += CONTROL_FLAG_LEN;
                hex2short(&pStr, (short*)&aid);
                if(!IsInvalidAID(aid))
                {
                    return TS_SYSTEM_ERROR;
                }
                return aid;
            }        
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }            
            
        }
        else
        {
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
     
    };

    return TS_SYSTEM_ERROR;
}


/**
 * 功能: 接收数据包并解析TS返回的数据包的正确或具体错误值信息.
                   它一般用于解析只有操作状态信息,而没有操作结果
                   数据的控制包的解析,格式如: |DID,|OK/Exx,|
 * @param fd   连接句柄
 * @return 
        TS_SUCCESS:   操作成功
        TS_INVALID_LINK_HANDLE: 无效的句柄号
        TS_UNKNOW_DATA: 无效的数据,一般是数据长度小于期望的最小长度
        TS_SYSTEM_ERROR: 系统发生错误
        TS_TIME_OUT: 在指定时间内没有收到数据
        以及其它TS返回的具体错误值
*/
int getAnswerPacketState(US fd) 
{
    US  packDID = 0,  curDID = 0;
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = m_ItrDID->second;
    mtxDID.unlock();
    US des_aid, des_said=0, src_said;
    char recBuf[PACKET_DATASIZE] = "\0";
    int bufSize = sizeof(recBuf);
    int ret = 0;
    char *pBuf = NULL; 
     while(true)
     {
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING)   //等待5秒, 在5秒内有数据则进行数据的接收
        {
            //动态调整超时时间
            adjustSocketTime(false);
            //读取manager返回的数据, 假使返回的信息不会超过1个数据包的大小, 即大小不能超过PACKET_DATASIZE
            ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recBuf, sizeof(recBuf));
            if (ret < 0 )
                return filtrateError(ret);
            //对数据包作初步判断
            if(!CheckValidPackBySize(ret))
            {
                return TS_UNKNOW_DATA;
            }
            __fetch(&packDID,  recBuf,  DIDLEN);
            pBuf = recBuf + DIDLEN + CONTROL_FLAG_LEN;
            if(packDID == curDID)
            {
                //解析数据包
                return returnErrValueFromErrMsg(pBuf);
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }            
                
        }
        else
        {
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
         
     };
     return TS_SYSTEM_ERROR;
}
/**
 * 功能: 在TM中创建一个DLL形式的目标服务配置.
 * @param fd  连接句柄
 * @param pServerConfig: 服务代理的配置结构指针
 * @return  
        如果操作成功返回AID号
        TS_INVALID_LINK_HANDLE: 无效的句柄号
        TS_UNKNOW_DATA: 无效的数据,一般是数据长度小于期望的最小长度
        TS_UNKNOW_DATA_FORMAT:  无法识别的数据格式
        TS_SYSTEM_ERROR: 系统发生错误
        TS_TIME_OUT: 在指定时间内没有收到数据
        以及其它TS返回的具体错误值
 
发送格式: "DID, TserviceName,  dllName,  isLog,  DllBuf,"
接收格式: "DID, OK, aid,"
 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: 将TClient_createServerWithDll返回类型又短整形(short)改为整形(int)*/
DLL_FUNC_INT TClient_createServerWithDll(US fd, ServerCfg* pServerConfig) 
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end] */
{
    if (pServerConfig == NULL || pServerConfig->pComConfig == NULL)
        return TS_NULLPTR_ENCONTERED;

    ServerCfg* m_pServerConfig =pServerConfig;
    char buf[PACKET_DATASIZE] = "\0";
    char* pBuf = buf;
    US  dialogID = 0;             //当前对话ID
    US  packDID = 0;            //收到回复包中的对话ID
    US des_aid, des_said=0, src_said;
    int bufSize = PACKET_DATASIZE;
    US aid = 0;
    char *pStr = NULL;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    dialogID = m_ItrDID->second +1;
    //并将当前DID存入映射表中
    m_ItrDID->second = dialogID;
    mtxDID.unlock();
    //构造消息
    __store(dialogID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    //增加控制字符
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    memcpy(pBuf,  &CREATE_SERVERWITHDLL,  CONTROL_COMMAND_LEN);                             //createServerWithDll的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;

    // the server name
    memcpy(pBuf, m_pServerConfig->name, strlen(m_pServerConfig->name));       //名字
    pBuf += strlen(m_pServerConfig->name);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    // extInfo
    memcpy(pBuf, m_pServerConfig->extInfo, sizeof(m_pServerConfig->extInfo));
    pBuf += sizeof(m_pServerConfig->extInfo);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //动态库名字
    memcpy(pBuf, m_pServerConfig->pComConfig->DLL.dllName, strlen(m_pServerConfig->pComConfig->DLL.dllName));    
    pBuf += strlen(m_pServerConfig->pComConfig->DLL.dllName);
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;

    //日志
    pBuf += int2hex(pBuf, 10, m_pServerConfig->isLog);        //日志标记
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;

    //判断组装字符串大小是否超过缓冲区大小
    if (pBuf-buf+DLL_BUF > PACKET_DATASIZE)
        return TS_SERVICE_CONFIG_ERR;
    memcpy(pBuf,  m_pServerConfig->pComConfig->DLL.buf,  DLL_BUF);
    pBuf += DLL_BUF;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));           //向manager发送命令包
    if (ret < 0 )
        return filtrateError(ret);
    //循环接收消息，直到接收到本次的回复消息为止
    
    while(true)
    {
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING)
        {
            //动态调整超时时间
            adjustSocketTime(false);
            pBuf = buf;
            bufSize = TClient_getpkt(fd, &des_aid, &des_said, &src_said, buf, sizeof(buf));
            if(bufSize < 0)
            {
                return filtrateError(bufSize);
            }
            //根据消息长度对消息作初步判断
            if(!CheckValidPackBySize(bufSize))
            {
                return TS_UNKNOW_DATA;
            }
            //取出消息中的DID
            __fetch(&packDID,  pBuf,  DIDLEN);
            //将pbuf指定包内容开始部分, 即跳过DID部分
            pBuf += DIDLEN + CONTROL_FLAG_LEN;
            //将指针指向消息的开始部分,  即OK, xx
            if(packDID == dialogID)
            {
                //收到正确的包，进行包的解析
                if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
                {
                    return ret;
                }
                //取出包中的aid
                pStr = strstr(pBuf,  CONTROL_FLAG);
                if(pStr != NULL)
                {
                    pStr += CONTROL_FLAG_LEN;
                    hex2short(&pStr,  (short*)&aid);
                    if(!IsInvalidAID(aid))
                    {
                        return TS_SYSTEM_ERROR;
                    }
                    return aid;
                }
                else
                {
                    //解析数据包错误，回复包格式错误
                    return TS_UNKNOW_DATA_FORMAT;
                }
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  dialogID)
            }
        }
        else
        {
            //动态调整超时时间
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        
    };
    return TS_SYSTEM_ERROR;

}
/**
 * 功能: 在TM中创建一个目标服务配置.
 * @param fd  连接句柄
 * @param pServerCfg  目标机配置信息
 * @return 
        如果操作成功返回AID号
        TS_INVALID_LINK_HANDLE: 无效的句柄号
        TS_UNKNOW_DATA: 无效的数据,一般是数据长度小于期望的最小长度
        TS_UNKNOW_DATA_FORMAT:  无法识别的数据格式
        TS_SYSTEM_ERROR: 系统发生错误
        TS_TIME_OUT: 在指定时间内没有收到数据
        TS_NULLPTR_ENCONTERED: pServerCfg或pServerCfg->pComConfig指针为空
        TS_INVALID_SERVICE_NAME:  没有指定服务代理的名字
        TS_INVALID_IP: 配置了无效的IP
        以及其它TS返回的具体错误值

发送格式: |DID|, Csa名字,extInfo,  日志,通道数, +根据way选择各类配置信息(以分割符)
接收格式:      |DID|, |OK|, |aid|


 */
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [begin]*/
/*  Modified brief: 将TClient_createServer返回类型又短整形(short)改为整形(int)*/
DLL_FUNC_INT TClient_createServer(US fd, ServerCfg* pServerCfg) 
/*Modified by tangxp for BUG:0002260 on 2007.9.26 [end] */
{
    int ret = 0, size = 0;
    char buf[PACKET_DATASIZE] = "\0";
    char* pBuf = buf;
    int ipSize = 0;
    US curDID = 0,  packDID = 0;
    
    if (pServerCfg == NULL || pServerCfg->pComConfig == NULL)
        return TS_NULLPTR_ENCONTERED;
    if (!strlen(pServerCfg->name))
        return TS_INVALID_SERVICE_NAME;
    if((pServerCfg->pComConfig->way != SERVER_WAY_TCP) 
        && (pServerCfg->pComConfig->way != SERVER_WAY_UDP )
        && (pServerCfg->pComConfig->way != SERVER_WAY_USB )
        && (pServerCfg->pComConfig->way != SERVER_WAY_UART))
    {
        return TS_SERVICE_CONFIG_ERR;
    }
    //填充消息
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = m_ItrDID->second + 1;
    m_ItrDID->second = curDID;
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //进行发送名字的组包
    
    memcpy(pBuf,  &CREATE_SERVER,  CONTROL_COMMAND_LEN);                                //createServer的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    //name
    size = (int)strlen(pServerCfg->name);    //得到名字缓冲区的大小
    memcpy(pBuf,  pServerCfg->name,  strlen(pServerCfg->name));                            //名字
    pBuf += size;

    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    //arch info 
    memcpy(pBuf, pServerCfg->extInfo, sizeof(pServerCfg->extInfo));
    pBuf += sizeof(pServerCfg->extInfo);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);   
    pBuf += CONTROL_FLAG_LEN;
    //log
    pBuf += int2hex(pBuf, 10, pServerCfg->isLog);        //日志标记
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;

    pBuf += int2hex(pBuf, 10, pServerCfg->channelCount);        //通道数
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //增加控制符号
    pBuf += CONTROL_FLAG_LEN;

    COMCONFIG* pCommConfig = pServerCfg->pComConfig;;
    for (UI i=0;i<pServerCfg->channelCount;++i) 
    {
        if(NULL == pCommConfig)
        {
            return TS_SERVICE_CONFIG_ERR;
        }
        pBuf += int2hex(pBuf, 10, pCommConfig->protocolType);        //协议方式
        memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //增加控制符号
        pBuf += CONTROL_FLAG_LEN;

        pBuf += int2hex(pBuf, 10, pCommConfig->way);                    //通信方式
        memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //增加控制符号
        pBuf += CONTROL_FLAG_LEN;

        switch (pCommConfig->way)
        {
            case SERVER_WAY_TCP:    //TCP
            case SERVER_WAY_UDP:    //UDP
                
                ipSize = (int)min(strlen(pCommConfig->NET.ip), IP_BUF_LEN);
                /*Modified by qingxiaohai on 2006年11月17日 11:25:13 for BUG 1102 [Begin]*/
                /*  Modified brief:无效IP检查*/
                if(!checkInvalidIP(pCommConfig->NET.ip,  (UI)strlen(pCommConfig->NET.ip)))
                {
                    return TS_INVALID_IP;
                }
                /*Modified for BUG  [End]*/
                memcpy(pBuf, pCommConfig->NET.ip, ipSize);        //IP地址
                pBuf += ipSize;

                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
                pBuf += CONTROL_FLAG_LEN;
                if(false == CheckInvalidPort(pCommConfig->NET.port))
                {
                    return TS_INVALID_IP_PORT;
                }
                pBuf += int2hex(pBuf, 10, pCommConfig->NET.port);    //端口

                memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
                pBuf += CONTROL_FLAG_LEN;

                pBuf += int2hex(pBuf, 10, pCommConfig->NET.timeout);    //超时

                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
                pBuf += CONTROL_FLAG_LEN;

                pBuf += int2hex(pBuf, 10, pCommConfig->NET.retry);    //重试次数

                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
                pBuf += CONTROL_FLAG_LEN;                
                break;
                
            case SERVER_WAY_UART:
                
                ipSize = (int)min(strlen(pCommConfig->UART.channel), UART_NAME_BUF_LEN);
                //串口端口
                memcpy(pBuf,  pCommConfig->UART.channel,  ipSize);        
                pBuf += ipSize;
                memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //增加控制符号
                pBuf += CONTROL_FLAG_LEN;
                //波特率
                pBuf += int2hex(pBuf, 10, pCommConfig->UART.baud);
                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //增加控制符号
                pBuf += CONTROL_FLAG_LEN;
                 //超时
                pBuf += int2hex(pBuf, 10, pCommConfig->UART.timeout);   
                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //增加控制符号
                pBuf += CONTROL_FLAG_LEN;
                //重试次数
                pBuf += int2hex(pBuf, 10, pCommConfig->UART.retry);        
                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //增加控制符号
                pBuf += CONTROL_FLAG_LEN;
                
                break;
            case SERVER_WAY_USB:
                //USB device GUID
                memcpy(pBuf, pCommConfig->USB.GUID, strlen(pCommConfig->USB.GUID));
                pBuf += strlen(pCommConfig->USB.GUID);
                memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);                    //增加控制符号
                pBuf += CONTROL_FLAG_LEN;
                break;
                
            case SERVER_WAY_DLL:  
            default:
                return TS_SYSTEM_ERROR;                
        }
        //下一个链表
        pCommConfig = pCommConfig->next;
    }
    
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //向manager发送命令包
    if (ret < 0 )
        return filtrateError(ret);
    //读取manager返回的数据
    
    US des_aid,  des_said=0,  src_said,  aid = 0;
    char recBuf[20] = "\0";
    char *pdest = NULL,  *pStr = NULL;
    /**
     返回的数据包格式
     ---------------
     |DID|, |OK|, |aid|
     ---------------
    */
    while(true)
    {
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING)
        {
            //动态调整超时时间
            adjustSocketTime(false);
            pBuf = recBuf;
            ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recBuf, sizeof(recBuf));
            if(ret < 0)
            {
                return filtrateError(ret);
            }
            //根据消息长度对消息作初步判断
            if(!CheckValidPackBySize(ret))
            {
                return TS_UNKNOW_DATA;
            }
            //取出消息中的DID
            __fetch(&packDID,  pBuf,  DIDLEN);
            //将pbuf指定包内容开始部分, 即跳过DID部分
            pBuf += DIDLEN + CONTROL_FLAG_LEN;
            //将指针指向消息的开始部分,  即OK, xx
            if(packDID == curDID)
            {
                //收到正确的包，进行包的解析
                if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
                {
                    return ret;
                }
                //取出包中的aid
                pStr = strstr(pBuf,  CONTROL_FLAG);
                if(pStr != NULL)
                {
                    pStr += CONTROL_FLAG_LEN;
                    hex2short(&pStr,  (short*)&aid);
                    if(!IsInvalidAID(aid))
                    {
                        return TS_SYSTEM_ERROR;
                    }
                    return aid;
                }
                else
                {
                    //解析数据包错误，回复包格式错误
                    return TS_UNKNOW_DATA_FORMAT;
                }
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }
        }
        else
        {
            //动态调整超时时间
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        
    };
    return TS_SYSTEM_ERROR;
}

/**
 * 功能: 关闭指定的TS
 * @param fd   连接句柄
 * @return 
        TS_SUCCESS操作成功
        TS_INVALID_LINK_HANDLE:  无效的连接句柄号
        TS_SOCKET_ERROR:   发送数据包时SOCKET发生错误
 */
DLL_FUNC_INT TClient_closeTM(US fd) 
{
    //进行发送名字的组包
    char buf[100] = "\0";
    char *pBuf = buf;
    US curDID = 0,  packDID = 0;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator  m_ItrDID  = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf,  &CLOSE_TS,  CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd,  0,  0,  0,  buf,  (UI)(pBuf - buf));   
    if(ret <= 0)
    {
        //消息未发送成功
        return filtrateError(ret);
    }
    //如果对端成功收到并处理消息，
    //则返回OK,  这边就删除对应的连接句柄
    ret =  getAnswerPacketState(fd);
    if(ret != TS_SUCCESS)
    {
        return ret;
    }
    MtxSocket.lock();
    mtxDID.lock();
    INT2SOCKET::iterator m_ItrSocket = mapIntToWSocket.find(fd);
    m_ItrDID = mapLnkToDID.find(fd);
    WSocket* sktClient = NULL;
    if(m_ItrSocket != mapIntToWSocket.end())
    {
        sktClient = m_ItrSocket->second;
        sktClient->Close();
        mapIntToWSocket.erase(m_ItrSocket);
        delete sktClient;
        sktClient = NULL;

    }
    if(m_ItrDID != mapLnkToDID.end())
    {
        mapLnkToDID.erase(m_ItrDID);
    }
    mtxDID.unlock();
    MtxSocket.unlock();
    return TS_SUCCESS;

}

/**
 * 功能: 通道切换
 * @param fd   连接句柄
 * @param AID  目标服务的AID号
 * @param channelType  要切换到的通道ID
 * @return 
        TS_SUCCESS: 操作成功
        TS_INVALID_SERVICE_ID:无效的服务ID
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_CHANGE_CHANNEL_FAILURE  : 通道切换失败
        TS_SRV_CHANNEL_SWITCH_DISABLE: 通道切换不允许,被自己禁用
        TS_CH_SW_FUNC_LOCKED_BY_OHTER: 通道切换功能被其他锁定了
        以及其他一些TS返回值
        
 */
DLL_FUNC_INT TClient_changeChannelType(US fd,  US aid, UC channelType)
{
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0,  packDID = 0 ;
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    memcpy(pBuf,  &CHANGE_CHANNEL_TYPE,  CONTROL_COMMAND_LEN);                                //deleteServer的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;

    pBuf += int2hex(pBuf, 10, channelType);            //类型
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);    //增加控制符号
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);

}
/**
 * 功能: 删除TM一个目标服务配置.
 * @param fd   连接句柄
 * @param AID  目标服务的AID号
 * @return 
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_SERVICE_ID:无效的服务ID
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        以及其他TS返回的一些具体错误值
发送格式: DID,RID,
接收格式:DID,OK/Exx,
*/
DLL_FUNC_INT TClient_deleteServer(US fd, US aid) 
{
    char buf[100] = "\0";
    char* pBuf = buf;
    US curDID = 0,  packDID = 0;
    int ret = 0;
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf,  &DELETE_SERVER,  CONTROL_COMMAND_LEN);    //deleteServer的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}
/**
 * 功能: 修改一个已经存在在TS中的目标服务配置
 * @param fd   连接句柄
 * @param pServerCfg   指定目标server的配置结构, 是个输出参数
 * @return 
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_SERVICE_ID:无效的服务ID
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_SERVICE_CONFIG_ERR : 指定的通道配置信息错误
        TS_UNKNOW_COMM_WAY: 通道配置指定的通信方式错误
        以及其他TS返回的一些具体错误值
    
 发送格式: 
 NET/UART方式
 -----------------------------------------------------------------------------
 |DID|,|Maid|,|name|,|extInfo|,|isLog|,|channelCount|,|curChannel|,|channelConfig..............|
 -----------------------------------------------------------------------------
 DLL方式
  -----------------------------------------------------------------------------
 |DID|,|Maid|,|name|,|extInfo|,|isLog|,|way|,|DllBuf|
 -----------------------------------------------------------------------------
接收格式: DID,OK/Exx,
*/
DLL_FUNC_INT TClient_modifyServer(US fd, ServerCfg* pServerConfig) 
{
    if (pServerConfig == NULL || pServerConfig->pComConfig == NULL)
        return TS_NULLPTR_ENCONTERED;
    ServerCfg* pServerCfg = pServerConfig;
    char buf[PACKET_DATASIZE] = "\0";
    char* pBuf = buf;
    int dllBufSize = 0,  curBufSize= 0;
    int ipSize = 0,  ret = 0;
    US curDID = 0,  packDID = 0;

    mtxDID.lock();
    LNK2DID::iterator  m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID,  pBuf,  DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &MODIFY_SERVER,  CONTROL_COMMAND_LEN);                                        //modifyServerState的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    if(!IsInvalidAID(pServerCfg->aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    pBuf += int2hex(pBuf, 10, pServerCfg->aid);                //AID号
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    //name
    memcpy(pBuf, pServerCfg->name, strlen(pServerCfg->name));    //名字
    pBuf += strlen(pServerCfg->name);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    //extInfo
    memcpy(pBuf, pServerCfg->extInfo, sizeof(pServerCfg->extInfo));
    pBuf += sizeof(pServerCfg->extInfo);
    memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);           
    pBuf += CONTROL_FLAG_LEN;
    //log
    pBuf += int2hex(pBuf,  10,  pServerCfg->isLog);                //日志记录
    memcpy(pBuf,  CONTROL_FLAG,  CONTROL_FLAG_LEN);            //增加控制符号
    pBuf += CONTROL_FLAG_LEN;
    //动态库类型
    if (pServerCfg->pComConfig->way == SERVER_WAY_DLL) 
    {
        pBuf += int2hex(pBuf, 10, pServerCfg->pComConfig->way);                //通信方式
        memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //增加控制符号
        pBuf += CONTROL_FLAG_LEN;

        curBufSize = (UI)(pBuf-buf);
        if (curBufSize+DLL_BUF> PACKET_DATASIZE -1)
        {
            return TS_SERVICE_CONFIG_ERR;
        }            
        memcpy(pBuf, pServerCfg->pComConfig->DLL.buf, DLL_BUF);        //
        pBuf += DLL_BUF;
        memcpy(pBuf, CONTROL_FLAG,  CONTROL_FLAG_LEN);            //增加控制符号
        pBuf += CONTROL_FLAG_LEN;
    }
    else 
    {
        pBuf += int2hex(pBuf, 10, pServerCfg->channelCount);        //通道数
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //增加控制符号
        pBuf += CONTROL_FLAG_LEN;

        pBuf += int2hex(pBuf, 10, pServerCfg->currentChannel);    //当前通道
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //增加控制符号
        pBuf += CONTROL_FLAG_LEN;

        COMCONFIG* pCommConfig = pServerCfg->pComConfig;;
        for (UI i=0;i<pServerCfg->channelCount;++i) 
        {
            if(NULL == pCommConfig)
            {
                if(i != 0)
                {
                    break;               //非第一个通道,即只填写修改的一个通道信息
                }
                return TS_SERVICE_CONFIG_ERR;
            }
            pBuf += int2hex(pBuf, 10, pCommConfig->protocolType);//协议类型
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //协议类型
            pBuf += CONTROL_FLAG_LEN;

            pBuf += int2hex(pBuf, 10, pCommConfig->way);            //通信方式
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //增加控制符号
            pBuf += CONTROL_FLAG_LEN;
            switch (pCommConfig->way) 
            {
                case SERVER_WAY_TCP:    //TCP
                case SERVER_WAY_UDP:    //UDP
                    //IP地址
                    ipSize = (int)min(strlen(pCommConfig->NET.ip), IP_BUF_LEN);
                    memcpy(pBuf, pCommConfig->NET.ip, ipSize);    
                    pBuf += ipSize;

                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);    //增加控制符号
                    pBuf += CONTROL_FLAG_LEN;
                    //端口号
                    pBuf += int2hex(pBuf, 10, pCommConfig->NET.port);    
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //增加控制符号
                    pBuf += CONTROL_FLAG_LEN;
                    //超时
                    pBuf += int2hex(pBuf, 10, pCommConfig->NET.timeout);    
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //增加控制符号
                    pBuf += CONTROL_FLAG_LEN;
                     //重试次数
                    pBuf += int2hex(pBuf, 10, pCommConfig->NET.retry);   
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //增加控制符号
                    pBuf += CONTROL_FLAG_LEN;            
                    break;
                    
                case SERVER_WAY_UART:

                    ipSize = (int)min(UART_NAME_BUF_LEN, strlen(pCommConfig->UART.channel));
                    //串口端口
                    memcpy(pBuf, pCommConfig->UART.channel, ipSize);    
                    pBuf += ipSize;
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //增加控制符号
                    pBuf += CONTROL_FLAG_LEN;
                     //波特率
                    pBuf += int2hex(pBuf, 10, pCommConfig->UART.baud);   
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //增加控制符号
                    pBuf += CONTROL_FLAG_LEN;
                    //超时
                    pBuf += int2hex(pBuf, 10, pCommConfig->UART.timeout);
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                //增加控制符号
                    pBuf += CONTROL_FLAG_LEN;
                    //重试次数
                    pBuf += int2hex(pBuf, 10, pCommConfig->UART.retry);    
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //增加控制符号
                    pBuf += CONTROL_FLAG_LEN;          
                    break;
                case SERVER_WAY_USB:
                    memcpy(pBuf, pCommConfig->USB.GUID, strlen(pCommConfig->USB.GUID));
                    pBuf += strlen(pCommConfig->USB.GUID);
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //增加控制符号
                    pBuf += CONTROL_FLAG_LEN;          
                    break;
                    
                default:
                    return TS_UNKNOW_COMM_WAY;
            }

            //下一个通道配置
            pCommConfig = pCommConfig->next;
        }
    }
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}


/**
 * 功能: 通过指定目标服务ID号查找该目标机的配置参数和状态信息
 * @param fd   连接句柄
 * @param pServerCfg[OUT]   指定目标server的配置结构, 是个输出参数
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_NULLPTR_ENCONTERED:  pServerCfg为空指针
        TS_INVALID_SERVICE_ID:  无效的AID
        TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
        TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值
 发送格式：
 -------------
 |DID|,|Qaid|,|
 -------------
查询服务返回的格式为：
TCP/UDP/UART
OK, name, extInfo, state, log, chcount, curch, config of each ch..
DLL
OK, name, extInfo, state, log, chcount, curch, protocol, way, dllname, outbuf, inbuf, 
*/
DLL_FUNC_INT TClient_queryServer(US fd, ServerCfg* pServerCfg) 
{
    if (NULL == pServerCfg || NULL == pServerCfg->pComConfig)
        return TS_NULLPTR_ENCONTERED;
    
    char recBuf[PACKET_DATASIZE] = "\0";
    char* pBuf = recBuf;
    int ret = 0, bufSize = sizeof(recBuf), currentChannel = 0;
    size_t dllBufsize = 0;
    US des_aid, des_said=0, src_said;
    char *pdest = NULL, *pStr = NULL;
    COMCONFIG* pCommConfig = NULL;
    US curDID = 0, packDID = 0, idx = 0;
    
    mtxDID.lock();
    LNK2DID::iterator  m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the contorl command
    
    memcpy(pBuf, &QUERY_SERVER_STATE, CONTROL_COMMAND_LEN);                                //queryServerState的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    if(!IsInvalidAID(pServerCfg->aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    pBuf += int2hex(pBuf, 10, pServerCfg->aid);                                //设置AID号
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                //控制包标志符
    pBuf += CONTROL_FLAG_LEN;
    ret = TClient_putpkt(fd, 0, 0, 0, recBuf, (UI)(pBuf-recBuf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);


    while(true)
    {
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING) 
        {
            //动态调整超时时间
            adjustSocketTime(false);
            pBuf = recBuf;
            ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recBuf, sizeof(recBuf));
            if(ret < 0)
            {
                return filtrateError(ret);
            }
            //根据数据包大小作数据合法的初步判断
            if(!CheckValidPackBySize(ret))
            {
                return TS_UNKNOW_DATA;
            }
            //取出消息中的DID
            __fetch(&packDID,  pBuf,  DIDLEN);
            //将pbuf指定包内容开始部分, 即跳过DID部分
            pBuf += DIDLEN + CONTROL_FLAG_LEN;
            //将指针指向消息的开始部分,  即OK, xx
            if(packDID == curDID)
            {
                //收到正确的包，进行包的解析
                if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
                {
                    return ret;
                }
                //正确回复的消息包的格式为
                /**
                TCP/UDP/UART
                OK, name, extInfo, state, log, chcount, CurChannel, config of each ch..
                DLL
                OK, name, extInfo, state, log, chcount, CurChannel, protocol, way, dllname, outbuf, inbuf, 
                */
                //OK,
                pdest = strstr(pBuf, CONTROL_FLAG);        //是否有分割符
                if (pdest == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                pdest += CONTROL_FLAG_LEN;
                //name,
                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                //服务名解析出错
                if (pStr-pdest > NAME_SIZE)
                    return TS_INVALID_SERVICE_NAME;
                
                memcpy(pServerCfg->name, pdest, pStr-pdest);            //名字
                pServerCfg->name[pStr-pdest] = '\0';                //给字符串结尾符
                pdest = pStr + CONTROL_FLAG_LEN;
                //extInfo
                if(bufSize < pdest - recBuf + EXT_INFO_LEN + CONTROL_FLAG_LEN)
                {
                    return TS_UNKNOW_DATA_FORMAT;
                }
                memcpy(pServerCfg->extInfo, pdest, EXT_INFO_LEN);
                pServerCfg->extInfo[EXT_INFO_LEN-1] = '\0';
                pdest += EXT_INFO_LEN + CONTROL_FLAG_LEN;
                
                //state,
                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pServerCfg->state);        //状态
                pdest = pStr + CONTROL_FLAG_LEN;

                //isLog,
                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                int logflag = 0;
                hex2int(&pdest, &logflag);
                pServerCfg->isLog = ((logflag == 0)?0:1);        //日志记录
                pdest = pStr + CONTROL_FLAG_LEN;
                //chCount,
                pStr = strstr(pdest, CONTROL_FLAG);        //通道数
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pServerCfg->channelCount);        //通道数
                
                pdest = pStr + CONTROL_FLAG_LEN;
                if (pServerCfg->channelCount > MAX_CHANNEL_COUNT)
                    return TS_INVALID_CH_COUNT;
                //CurChannel,
                pStr = strstr(pdest, CONTROL_FLAG);        //当前激活的通道
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pServerCfg->currentChannel);
                pdest = pStr + CONTROL_FLAG_LEN;

                //special config of each channel
                //这里用do-while而不用for是因为DLL服务是没有通道数的，
                //但其通道1配置是有效的
                idx = pServerCfg->channelCount;
                pCommConfig = pServerCfg->pComConfig;
                do
                {
                    if(NULL == pCommConfig)
                    {
                        return TS_REC_BUF_LIMITED;
                    }
                    //当前的通道类型
                    pStr = strstr(pdest, CONTROL_FLAG);        
                    if (pStr == NULL )
                        return TS_UNKNOW_DATA_FORMAT;
                    hex2int(&pdest, (int*)&pCommConfig->protocolType);
                    pdest = pStr + CONTROL_FLAG_LEN;

                    //得到当前通道配置方式
                    pStr = strstr(pdest, CONTROL_FLAG);        
                    if (pStr == NULL )
                        return TS_UNKNOW_DATA_FORMAT;
                    hex2int(&pdest, (int*)&pCommConfig->way); 
                    pdest = pStr + CONTROL_FLAG_LEN;

                    switch(pCommConfig->way)
                    {
                        case SERVER_WAY_TCP:
                        case SERVER_WAY_UDP:
                            //find control flag after IP
                            pStr = strstr(pdest, CONTROL_FLAG);       
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            if (pStr-pdest > IP_BUF_LEN)
                                return TS_INVALID_IP;
                            memcpy(pCommConfig->NET.ip, pdest, pStr-pdest);               
                            pCommConfig->NET.ip[pStr-pdest] = '\0';    
                            pdest = pStr + CONTROL_FLAG_LEN;                            
                            //find control flag after port
                            pStr = strstr(pdest, CONTROL_FLAG);        
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->NET.port = strtoi(pdest, 16);    //端口
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find control flag after timout
                            pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->NET.timeout = strtoi(pdest, 16);    //超时
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find control flag after retry
                            pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->NET.retry = strtoi(pdest, 16);    //重试次数
                            pdest = pStr + CONTROL_FLAG_LEN;                            
                            break;
                            
                        case SERVER_WAY_UART:
                            //find the control flag after UART-channel
                            pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                             if (pStr-pdest > UART_NAME_BUF_LEN)
                                return TS_INVALID_UART_PORT;
                             //COMM port
                            memcpy(pCommConfig->UART.channel, pdest, pStr-pdest);                                  
                            pCommConfig->UART.channel[pStr-pdest] = '\0';    //给字符串结尾符
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find the control flag after baud
                            pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->UART.baud = strtoi(pdest, 16);    //波特率
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find control flag after timout
                            pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->UART.timeout = strtoi(pdest, 16);    //超时
                            pdest = pStr + CONTROL_FLAG_LEN;                            

                            //find control flag after retry
                            pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pCommConfig->UART.retry = strtoi(pdest, 16);    //重试次数
                            pdest = pStr + CONTROL_FLAG_LEN;                            
                            break;
                            
                        case SERVER_WAY_DLL:
                            //find the control flag after DllName
                            pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
        
                            memset(pCommConfig->DLL.dllName, 0, NAME_SIZE);  
                            if (pStr-pdest > NAME_SIZE)
                                return TS_INVALID_DLL_NAME;
                            memcpy(pCommConfig->DLL.dllName, pdest, pStr-pdest);             
                            pdest = pStr + CONTROL_FLAG_LEN;   
                            //find the control flag after outbuf
                            pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符[outbuf]
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            pStr = strstr(pdest+CONTROL_FLAG_LEN,  CONTROL_FLAG);        //是否有分割符[inbuf]
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            dllBufsize = (size_t)(pStr - pdest+2*CONTROL_FLAG_LEN);
                            memcpy(pCommConfig->DLL.buf, pdest, dllBufsize);    //大小
                            pdest = pStr+CONTROL_FLAG_LEN;

                            break;

                            /*Modified bu tangxp on 2007.9.26 [begin]*/
                            /*  Modified brief:增加查询USB通信方式的目标机配置*/
                        case SERVER_WAY_USB:
                            //finde the control flag after GUID
                            pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                            if (pStr == NULL )
                                return TS_UNKNOW_DATA_FORMAT;
                            //GUID
                            memset(pCommConfig->USB.GUID, 0, GUID_LEN);
                            if (pStr-pdest > GUID_LEN)
                                return TS_INVALID_GUID;
                            memcpy(pCommConfig->USB.GUID, pdest, pStr-pdest); 
                            break;
                            /*Modified bu tangxp on 2007.9.26 [end] */
                            
                        default:
                            
                            return TS_UNKNOW_COMM_WAY;
                    }                   
                    
                    pCommConfig = pCommConfig->next;
                        
                }while(--idx > 0);
                
                return TS_SUCCESS;
                
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }
        }
        else
        {
            //动态调整超时时间
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        
    };
    return TS_SYSTEM_ERROR;
}
/**
 * 功能: 激活指定的处于休眠状态的目标服务
 * @param fd   连接句柄
 * @param aid   目标的aid号
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_SERVICE_ID:  无效的AID
        TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
        TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值
 发送格式: DID, Aaid,
接收格式: DID, OK/Exx,
*/
DLL_FUNC_INT TClient_activeServer(US fd, US aid)
{
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command     
    memcpy(pBuf, &ACTIVE_SERVER, CONTROL_COMMAND_LEN);                                //activeServer的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}

/***************************************************************************
 *   函 数    名： TClient_activeServerWithDll
 *   函数说明： 激活指定的处于休眠状态的DLL_SA
 *   参      数1   ：fd:连接句柄
 *   参      数2   ：aid:目标服务的Aid号
 *   参      数3   ：char* anserInfo:  TC收到的TS激操作的信息
 *   参      数4   ：infolen[in/out]  指定answerInfo的字节大小，
                                        返回它实际内容的字节大小
 *   返  回   值：
                                如果操作成功则返回接收到的回复字节数
                                TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                                TS_NULLPTR_ENCONTERED:  answerInfo为空指针
                                TS_INVALID_SERVICE_ID:  无效的AID
                                TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
                                TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
                                TS_TIME_OUT:  指定时间内没有收到数据包
                                TS_REC_BUF_LIMITED:  指定的answerInfo空间长度不足
                                以及其他TS返回的一些具体错误值
       发送格式: DID,Aaid,
 *    接收格式:DID,Exx,  // DID, OK, outbuflen, outbuf...,                               
 *   作           者： qingxh
 *   创建时间： 2006年10月20日 15:00:11
 *   修改历史：1、modified on 2006-12-19 for TS3.2
 *   
***************************************************************************/
DLL_FUNC_INT TClient_activeServerWithDll(US fd,  US aid,  char* answerInfo, int infolen)
{
    if(NULL_PTR == answerInfo)
    {
        return TS_NULLPTR_ENCONTERED;        //无效的AID号或者空指针
    }
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret =0;
    US curDID = 0, packDID = 0;
    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command    
    memcpy(pBuf, &ACTIVE_SERVER, CONTROL_COMMAND_LEN);                                //activeServer的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);

    //读取manager返回的激活操作的参数op_result_para
    /*
    *       Ts返回的帧格式为：
    *       _________________________________________
    *       |op_flag(E01/OK)| ,  |Aid| ,  |len|op_result_para|
    *       |______________|__|__|__|__|_____________|
    */
    US des_aid, des_said=0, src_said;
    char recBuf[1000] = "\0";
    int bufSize = PACKET_DATASIZE;        
    int retbuflen = 0;   //返回信息的长度
    char *pdest = NULL;
    while(true)
    {
        //等待5秒, 在5秒内有数据则进行数据的接收
        if (TClient_wait(fd, TIMEOUT_GETPKT) == TS_DATACOMING)
        {      
            //动态调整超时时间
            adjustSocketTime(false);
            //读取manager返回的数据, 假使返回的信息不会超过1个数据包的大小, 即大小不能超过PACKET_DATASIZE
            
            ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recBuf, sizeof(recBuf));
            if (ret < 0 )
                return filtrateError(ret);
            //根据消息包的长度作数据合法的初步判断
            if(!CheckValidPackBySize(ret) )
            {
                return TS_UNKNOW_DATA;
            }
            //取出DID,进一步判断是否是当前的回复数据
            pBuf = recBuf;
            __fetch(&packDID, pBuf, DIDLEN);
            //将pbuf指定包内容开始部分, 即跳过DID部分
            pBuf += DIDLEN + CONTROL_FLAG_LEN;
            //将指针指向消息的开始部分,  即OK, xx
            if(packDID == curDID)
            {
                //收到正确的包，进行包的解析
                if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
                {
                    return ret;
                }
                //跳过OK,
                pBuf += ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN;
                //取出包中的aid
                pdest = strstr(pBuf,  CONTROL_FLAG);
                if(NULL == pdest)
                {
                    return TS_UNKNOW_DATA_FORMAT;
                }
                //指向len部分
                pBuf = pdest +CONTROL_FLAG_LEN;
                pdest = strstr(pBuf, CONTROL_FLAG);
                if(NULL == pdest)
                {
                    return TS_UNKNOW_DATA_FORMAT;
                }
                hex2int(&pBuf,  &retbuflen);
                pdest += CONTROL_FLAG_LEN;
                if(retbuflen > infolen - 1)
                {
                    memcpy(answerInfo, pdest,  infolen-1);
                    *(answerInfo + infolen)= '\0';
                    return TS_REC_BUF_LIMITED;
                }
                memcpy(answerInfo, pdest,  retbuflen);
                *(answerInfo + retbuflen)= '\0';
                return retbuflen;              
                
            }
            else
            {
                AssertPackDIDAndCurDID(packDID,  curDID)
            }
        }
        else
        {
            //动态调整超时时间
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        
    };

    return TS_SYSTEM_ERROR;
    
}

/**
 * 功能: 休眠指定的处于激活状态的目标服务
 * @param fd   连接句柄
 * @param aid   目标的aid号
 * @return
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_SERVICE_ID:  无效的AID
        TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
        TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值
 发送格式:DID, Paid,
接收格式:DID,OK/Exx,
*/
DLL_FUNC_INT TClient_inActiveServer(US fd, US aid) 
{
    char buf[100] = "\0";
    char* pBuf = buf;
    US curDID = 0;
    if(!IsInvalidAID(aid))
    {
        return TS_INVALID_SERVICE_ID;
    }

    mtxDID.lock();
    LNK2DID::iterator m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &REST_SERVER, CONTROL_COMMAND_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    int ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}

/**
 * 功能: 运行TS
 * @param path   TS的带路径的文件名
 * @return 
        TS_SUCCESS:   加载成功
        TS_NULLPTR_ENCONTERED: path :为空指针
        TS_INVALID_FILE_NAME: 文件名大小系统规定的文件名大小
        TS_FILE_NOT_EXIST:  文件名对应的TS文件不存在
        TS_LAUNCH_TS_FAILURE:  启动TS进程失败
 */
DLL_FUNC_INT TClient_launchTM(const char* path) 
{
    WIN32_FIND_DATA d;
    if (NULL_PTR == path)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    if (strlen(path) > NAME_SIZE)
        return TS_INVALID_FILE_NAME;

    char tsName[NAME_SIZE] = "\0";
    char* pName = tsName;
    memcpy(pName, path, strlen(path));
    pName += strlen(path);

    *pName = '\0';

    HANDLE hHandle = FindFirstFile(tsName,  &d);
    if ( hHandle == INVALID_HANDLE_VALUE )
    {
        return TS_FILE_NOT_EXIST;
    }
    else 
    {
        FindClose( hHandle );
        STARTUPINFO startInfo={0};
        startInfo.cb = sizeof(startInfo);
        PROCESS_INFORMATION ProcInfo;
        BOOL bCreatSucces = CreateProcess(tsName,  tsName, NULL, NULL, FALSE,  0,  NULL,  NULL,  &startInfo,  &ProcInfo);
        if (bCreatSucces)
            return TS_SUCCESS;
    }
    
    return TS_LAUNCH_TS_FAILURE;
}


/**
 * 功能:工具可选用单独的线程阻塞在该方法。
 *              得到TS自动上报的信息。
 *               可能是消息, 也可能是状态
 *               内存空间由调用者来分配
 * @param fd   连接句柄
 * @param pServerCfg    上报信息头指针
 * @return 
         如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_SERVICE_ID:  无效的AID
        TS_UNKNOW_DATA_FORMAT:  对端发过来的数据包格式不可识别
        TS_UNKNOW_COMM_WAY:  对端发过来的数据包的代理的通信方式不可识别
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值

 格式:sa的AID号, 名字, 上报类型, 状态, 日志, 通信方式, 根据不同的通信方式的配置信息(以分割符来分割)
 */
DLL_FUNC_INT TClient_getSAReport(US fd, ServerCfg *pArrayServerCfg, UI timeout) 
{
    if (pArrayServerCfg == NULL)
        return TS_NULLPTR_ENCONTERED;
    US des_aid, des_said=0, src_said;
    char recvbuf[PACKET_DATASIZE] = "\0";
    int bufSize = 0, packDID = 0;
    int count = 0, size = 0;
    COMCONFIG* pCommConfig = NULL;

    if(timeout != 0)
    {
        if(TClient_wait(fd, timeout) != TS_DATACOMING)
        {
            //动态调整超时时间
            adjustSocketTime(true);
            return TS_TIME_OUT;
        }
        //动态调整超时时间
        adjustSocketTime(false);
    }
    bufSize = TClient_getpkt(fd, &des_aid, &des_said, &src_said, recvbuf, sizeof(recvbuf));
    if (bufSize < 0 )
            return filtrateError(bufSize);
    char* pdest = recvbuf;
    //消息包的长度至少应该为对话ID部分所占字节
    if((UI)bufSize < (DIDLEN + CONTROL_FLAG_LEN))
    {
        return TS_UNKNOW_DATA;
    }
    /**
    上报报文格式：(数值类型都是以十六进制存储的)
    ----------------------------------------------------------------------------
    |DID,|aid,|name,|extInfo|,|reportType,|sate,|islog,|channelCount,|currentChannel,|config of each channel,|
    ----------------------------------------------------------------------------
    */

    //取DID部分
    __fetch(&packDID, pdest, DIDLEN);
    pdest += DIDLEN + CONTROL_FLAG_LEN;
    
    char* pStr = NULL;
    ServerCfg *pServerCfg = pArrayServerCfg;
    
    //AID
    pStr = strstr(pdest, CONTROL_FLAG);            //aid
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2short(&pdest, (short*)&pServerCfg->aid);            //aid
    pdest = pStr + CONTROL_FLAG_LEN;
    //Name
    pStr = strstr(pdest, CONTROL_FLAG);            //是否有分割符
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    if (pStr-pdest > NAME_SIZE)
        return TS_INVALID_SERVICE_NAME;
    memcpy(pServerCfg->name, pdest, pStr-pdest);    //名字
    pServerCfg->name[pStr-pdest] = '\0';        //给字符串结尾符
    pdest = pStr + CONTROL_FLAG_LEN;
    //extInfo
    if(bufSize < pdest - recvbuf + EXT_INFO_LEN + CONTROL_FLAG_LEN)
    {
        return TS_UNKNOW_DATA_FORMAT;
    }
    memcpy(pServerCfg->extInfo, pdest, EXT_INFO_LEN);
    pServerCfg->extInfo[EXT_INFO_LEN-1] = '\0';
    pdest += EXT_INFO_LEN + CONTROL_FLAG_LEN;
    //reportType
    pStr = strstr(pdest, CONTROL_FLAG);            //上报类型
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2int(&pdest, (int*)&pServerCfg->reportType);
    pdest = pStr + CONTROL_FLAG_LEN;
    //state
    pStr = strstr(pdest, CONTROL_FLAG);            //状态
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2int(&pdest, (int*)&pServerCfg->state);
    pdest = pStr + CONTROL_FLAG_LEN;
    //isLog
    pStr = strstr(pdest, CONTROL_FLAG);            //日志
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    int logflag;
    hex2int(&pdest, &logflag);
    pServerCfg->isLog = ((logflag == 0)?0:1);        //日志记录
    pdest = pStr + CONTROL_FLAG_LEN;
    //ChannnelCount
    pStr = strstr(pdest, CONTROL_FLAG);            //通道数
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2int(&pdest, (int*)&pServerCfg->channelCount);
    pdest = pStr + CONTROL_FLAG_LEN;
    //currentChannel
    pStr = strstr(pdest, CONTROL_FLAG);            //当前通道
    if (pStr == NULL )
        return TS_UNKNOW_DATA_FORMAT;
    hex2int(&pdest, (int*)&pServerCfg->currentChannel);
    pdest = pStr + CONTROL_FLAG_LEN;
    //通道具体配置
    pCommConfig = pServerCfg->pComConfig;
    for (UI i=0;i<pServerCfg->channelCount; ++i) 
    {
        if(NULL == pCommConfig)
        {
            return TS_REC_BUF_LIMITED;
        }
        pStr = strstr(pdest, CONTROL_FLAG);        //当前的通道协议类型
        if (pStr == NULL )
            return TS_UNKNOW_DATA_FORMAT;
        hex2int(&pdest, (int*)&pCommConfig->protocolType);        //当前的通道协议类型
        pdest = pStr + CONTROL_FLAG_LEN;


        pStr = strstr(pdest, CONTROL_FLAG);        //得到当前通道的通信方式
        if (pStr == NULL )
            return TS_UNKNOW_DATA_FORMAT;
        hex2int(&pdest, (int*)&pCommConfig->way);        //得到当前通道的通信方式
        pdest = pStr + CONTROL_FLAG_LEN;

        switch (pCommConfig->way)
        {
            case SERVER_WAY_TCP:                                        //TCP, UDP
            case SERVER_WAY_UDP:                                        //TCP, UDP
                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                
                if (pStr-pdest > IP_BUF_LEN)
                    return TS_INVALID_IP;
                memcpy(pCommConfig->NET.ip, pdest, pStr-pdest);                //IP地址
                pCommConfig->NET.ip[pStr-pdest] = '\0';    //给字符串结尾符
                pdest = pStr + CONTROL_FLAG_LEN;

                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->NET.port);        
                pdest = pStr + CONTROL_FLAG_LEN;

                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->NET.timeout);        
                pdest = pStr + CONTROL_FLAG_LEN;

                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->NET.retry);        
                pdest = pStr + CONTROL_FLAG_LEN;
                break;
                
            case SERVER_WAY_UART:
                //解析串口端口
                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                if (pStr-pdest > UART_NAME_BUF_LEN)
                    return TS_INVALID_UART_PORT;
                memcpy(pCommConfig->UART.channel, pdest, pStr-pdest);                
                pCommConfig->UART.channel[pStr-pdest] = '\0';    //给字符串结尾符
                pdest = pStr + CONTROL_FLAG_LEN;
                //解析波特率
                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->UART.baud); 
                pdest = pStr + CONTROL_FLAG_LEN;
                //解析超时时间
                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->UART.timeout);     
                pdest = pStr + CONTROL_FLAG_LEN;
                //解析重试次数
                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                hex2int(&pdest, (int*)&pCommConfig->UART.retry);      
                pdest = pStr + CONTROL_FLAG_LEN;
                
                break;
                
            case SERVER_WAY_DLL:    //表示以动态库通信的sa
                //解析DLL文件名
                pStr = strstr(pdest, CONTROL_FLAG);        //是否有分割符
                if (pStr == NULL )
                    return TS_UNKNOW_DATA_FORMAT;
                if (pStr-pdest > NAME_SIZE)
                    return TS_INVALID_FILE_NAME;
                memcpy(pCommConfig->DLL.dllName, pdest, pStr - pdest);
                pdest = pStr + CONTROL_FLAG_LEN;
                //解析DLL缓冲，这里应该是最后一段，
                //所以直接移动到消息的结尾处
                pStr = recvbuf + bufSize;
                if(pStr - pdest > DLL_BUF)
                    return TS_UNKNOW_DATA_FORMAT;
                size = (int)min(DLL_BUF, pStr - pdest);
                memcpy(pCommConfig->DLL.buf, pdest, size);
                pdest = pStr;
                break;
                
            case SERVER_WAY_USB://USB
                break;
            default:
                break;
        }
        
        pCommConfig = pCommConfig->next;           
        
    }
    if(pCommConfig != NULL)
        pCommConfig->next = NULL;    //节点结束赋空

    return TS_SUCCESS;
}

/**
 * 功能: TS进行注册, 注册后, TS会向TC自动发送消息和状态
 * @param fd   连接句柄
 * @param regType   注册类型
 * @return 
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_REG_TYPE:  无效的注册类型号
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值

发送格式: DID,Etype,
接收格式: DID,OK/Exx,
 */
DLL_FUNC_INT TClient_register(US fd, int regType)
{
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    if(!checkValidRegType(regType))
    {
        return TS_INVALID_REG_TYPE;
    }

    mtxDID.lock();
    LNK2DID::iterator  m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }

    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &REGISTER_SERVER, CONTROL_COMMAND_LEN);                                //register的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, regType);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    //return getAnswerPacketState(fd);
    return TS_SUCCESS;
    
}
/**
 * 功能: 取消某种类型的注册登记
 * @param fd   连接句柄
 * @param regType   注册类型
 * @return 
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_REG_TYPE:  无效的注册类型号
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值

 发送格式：DID,UregType,
接收格式：DID,OK/Exx,
*/
DLL_FUNC_INT TClient_unregister(US fd, int regType)
{
    char buf[100] = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    if(!checkValidRegType(regType))
    {
        return TS_INVALID_REG_TYPE;
    }
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the contorl command    
    memcpy(pBuf, &UNREGISTER_SERVER, CONTROL_COMMAND_LEN);                                //unregister的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;

    pBuf += int2hex(pBuf, 10, regType);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);            //控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    //return getAnswerPacketState(fd);
    return TS_SUCCESS;
    
}

/**
 * 功能: 设置sa的注册类型
 * @param fd   连接句柄
 * @param aid   要设置sa的aid号
 * @param registerType   类型
 * @return 
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_REG_TYPE:  无效的注册类型号
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值
        
 发包格式: DID,s aid, registerType,
 接收格式: DID,OK/Exx,
 */
DLL_FUNC_INT TClient_setRegisterType(US fd, US aid, int registerType) 
{
    char buf[100]  = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    if(!checkValidRegType(registerType))
    {
        return TS_INVALID_REG_TYPE;
    }

    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &SET_SERVER_TYPE, CONTROL_COMMAND_LEN);                                //activeServer的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    pBuf += int2hex(pBuf, 10, registerType);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));    //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    
    return getAnswerPacketState(fd);
}
/**
 * 功能: 清除sa的注册类型
 * @param fd   连接句柄
 * @param aid   要设置sa的aid号
 * @param registerType   要注销类型
 * @return 
        如果操作成功则返回TS_SUCCESS
        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
        TS_INVALID_REG_TYPE:  无效的注册类型号
        TS_TIME_OUT:  指定时间内没有收到数据包
        以及其他TS返回的一些具体错误值
        
 发包格式: DID,c aid, registerType,
 接收格式: DID,OK/Exx,
 */
DLL_FUNC_INT TClient_clearRegisterType(US fd, US aid,  int registerType)
{
    char buf[100]  = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0;
    if(!checkValidRegType(registerType))
    {
        return TS_INVALID_REG_TYPE;
    }
    
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &CLEAR_SERVER_TYPE, CONTROL_COMMAND_LEN);                                //activeServer的控制包标志符
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    pBuf += int2hex(pBuf, 10, registerType);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));    //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);

}

/***********************TS3.2新增API**************************************/
/**
 * @Funcname:  Tclient_queryHostSerialPort
 * @brief        :  查询TS所在主机的串口配置
 * @para1      : fd  连接句柄号
 * @para2      : buf  接收缓冲
 * @para3      : 操作成功:size  接收缓冲的字节大小
 * @return      : 
                    操作成功返回接收缓冲中的实际字节大小
                    TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                    TS_TIME_OUT:  指定时间内没有收到数据包
                    TS_REC_BUF_LIMITED: 缓冲中指定的大小小于实际接收到的字节大小
                    以及其他TS返回的一些具体错误值
                    
    发送格式: DID, p,
    接收格式: |DID|,|OK|,|COM1|,|COM2|,|COM3|,|或者DID,Exx,

 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月20日 9:48:39
 *   
**/
DLL_FUNC_INT Tclient_queryHostSerialPort(US fd, char *recvBuf, US size)
{
    if(NULL == recvBuf)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    //串口最多配置10个
    char buf[1000]  = "\0";
    char* pBuf = buf;
    int ret = 0;
    US curDID = 0, packDID = 0;
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &QUERY_HOST_COMM_CONFIG, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf-buf));    //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);

    //接收回复消息
    US des_aid, des_said=0, src_said;
    int bufsize = sizeof(buf);        
    char *pdest = NULL, *pStr = NULL;
    
    while(true)
    {
        ret = TClient_wait(fd, TIMEOUT_GETPKT);
        if(ret != TS_DATACOMING)
        {
            //动态调整超时时间
            adjustSocketTime(true);
            return ret;
        }
        //动态调整超时时间
        adjustSocketTime(false);
        pdest = buf;
        bufsize = TClient_getpkt(fd, &des_aid, &des_said, &src_said, buf, sizeof(buf));
        if(bufsize <= 0)
        {
            return filtrateError(bufsize);
        }
        //根据数据包大小作数据合法性初步判断
        if(!CheckValidPackBySize(bufsize))
        {
            return TS_UNKNOW_DATA;
        }
        /**
        回复的数据 ----------------------------------
                                         |DID|,|OK|,|COM1|,|COM2|,|COM3|,|
                                        ---------------------------------
        若主机无串口则回复：  "DID,OK,"                               
        */
        //取DID
        __fetch(&packDID, pdest, DIDLEN);
        //将指针指向消息内容起始部分
        pdest += DIDLEN + CONTROL_FLAG_LEN;
        if(packDID == curDID)
        {
            //是本次的回复，进行数据的解析
            ret = returnErrValueFromErrMsg(pdest);
            if(ret != TS_SUCCESS)
            {
                return ret;
            }
            //跳过OK,部分
            pdest += ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN;
            //如果无串口配置
            if(strstr(pdest, CONTROL_FLAG) == NULL)
            {
                memset(recvBuf, 0, size);
                return 0;
            }
            //从DID部分开始进行拷贝
            pdest = buf + DIDLEN + CONTROL_FLAG_LEN + ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN;
            //COM配置数据的长度
            ret = bufsize - (DIDLEN + CONTROL_FLAG_LEN + 
                                        ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN);
            if(ret > size)
            {
                memcpy(recvBuf, pdest, size);
                return TS_REC_BUF_LIMITED;
            }
            memcpy(recvBuf, pdest, ret);
            return ret;
        }
        else
        {
            //看是否还要继续接收消息
            AssertPackDIDAndCurDID(packDID, curDID);
        }
    };
    return TS_UNKNOW_DATA;
}


/**
 * @Funcname:  TClient_setTargetChannelSwitchState
 * @brief        :  设置目标机通道切换标志
 * @para1      : fd : 连接句柄
 * @para2      : aid  目标服务ID号
 * @para3      :  flag :false禁止通道切换true:允许通道切换
 * @return      : 
                        TS_SUCESS 设置成功,
                        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                        TS_TIMEOUT: 收取回复消息包超时
                        TS_CH_SW_FUNC_LOCKED_BY_OHTER: 通道切换被其他客户端锁定
                        以及其他一些返回值

    发包格式:DID,a aid, flag 
    接收格式:DID,OK,/Exx,
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月20日 10:46:31
 *   
**/
DLL_FUNC_INT TClient_setTargetChannelSwitchState(US fd, US aid, _BOOL flag)
{
    char buf[100] = "\0";
    char *pBuf = buf;
    US  curDID = 0, packDID = 0;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &SET_TARGET_SWITCH_FLAG, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    //aid
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;
    //switch flag
    pBuf += int2hex(pBuf, 10, flag);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
    if(ret <= 0)
    {
        return filtrateError(ret);
    }

    return getAnswerPacketState(fd);
}

/**
 * @Funcname:  DLL_FUNC_INT  TClient_SetSystemLogLevel
 * @brief        :  设置TS系统日志的输出级别
 * @para1      : fd:  连接句柄号
 * @para2      : level 日志级别
 * @return      : 
                        TS_SUCESS 设置成功,
                        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                        TS_SOCKET_ERROR: 发送命令包时SOCKET错误

    发包格式:DID,L level, 
    无回复数据
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月20日 11:20:21
 *   
**/
DLL_FUNC_INT  TClient_setSystemLogLevel(US fd, LogLevel level)
{
    char buf[100] = "\0";
    char *pBuf = buf;
    US  curDID = 0, packDID = 0;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &SET_TS_SYSTEM_LOGOUT_LEVEL, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    pBuf += int2hex(pBuf, 10, level);
    //control char 
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
    if(ret <= 0)
    {
        return filtrateError(ret);
    }
    return TS_SUCCESS;

}

/**
 * @Funcname:  TClient_SetServiceAgentLog
 * @brief        :  设置指定服务的数据包日志标记
 * @para1      : fd : 连接句柄
 * @para2      : aid  目标服务ID号
 * @para3      :  logflag :0不输出指定服务的日志1:输出指定服务的消息日志
 * @return      : 
                        TS_SUCESS 设置成功
                        TS_INVALID_LINK_HANDLE: 无效的连接句柄号
                        TS_SOCKET_ERROR: 发送命令包时SOCKET错误

    发包格式:DID,l aid,logflag, 
    接收格式: DID, Ok/Exx,
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月20日 11:32:21
 *   
**/
DLL_FUNC_INT  TClient_SetServiceAgentLog(US fd, US aid, _BOOL logflag)
{
    char buf[100] = "\0";
    char *pBuf = buf;
    US  curDID = 0, packDID = 0;
    int ret = 0;
    
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //fill the control command
    memcpy(pBuf, &SET_SERVICE_PACK_LOG_FLAG, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    //aid
    pBuf += int2hex(pBuf, 10, aid);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;
    //PackLog flag
    pBuf += int2hex(pBuf, 10, logflag);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                                //restServer的控制包标志符
    pBuf += CONTROL_FLAG_LEN;

    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
    if(ret <= 0)
    {
        return filtrateError(ret);
    }
    return getAnswerPacketState(fd);
}

/**
 * @Funcname:  TClient_UpLoadFile
 * @brief        :  传送指定文件到TS服务器
 * @para1      : fd: 与TS服务器的连接句柄
 * @para2      : filename需要传送的文件名
 * @para3      : isReplace是否需要覆盖同名文件
 * @para4      : savedname, 保存在TS上的文件名
 * @return      : 
                        TS_SUCCESS传送成功
                        TS_FILE_ALREADY_EXIT已经存在同名文件
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年3月2日 14:51:36
 *   
 * 通信格式："DID, Ffilename, aid, isReplace, isComplete, curLen, content"
**/
DLL_FUNC_INT TClient_UpLoadFile(US fd, const char* filename, const char *savedname, int isReplace)
{
    char buf[PACKET_DATASIZE] = "\0";
    char *pBuf = buf, *fileBuf = NULL;
    US  curDID = 0, packDID = 0;
    int ret = 0, contentidx = 0, cursize = 0; //当前读取文件内容的大小
    FILE *fp = NULL;
    long curLocation = 0;     //文件当前位置
    if(NULL == filename)
    {
        return TS_NULLPTR_ENCONTERED;
    }
    if((strlen(filename)<=0) || (strlen(filename)>MAX_PATH))
    {
        return TS_INVALID_FILE_NAME;
    }
        
    //open the file
    fp = fopen(filename, "rb");
    if(NULL == fp)
    {
        return TS_FILE_NOT_EXIST;
    }
    //alloc buf for file
    fileBuf = (char*) malloc(FILEBUFSIZE);
    if(NULL == fileBuf)
    {
        fclose(fp);
        return TS_SYSTEM_ERROR;
    }
    //alloc and refresh the DID
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //Ffilename
    memcpy(pBuf, &UPLOAD_FILE, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, savedname, strlen(savedname));
    pBuf += strlen(savedname);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //isReplace
    pBuf += int2hex(pBuf, 10, isReplace);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //save the last content idx in buf.
    contentidx = (int)(pBuf - buf);    
    int icount = 0;
    
    while(!feof(fp))
    {
        memset(fileBuf, 0, FILEBUFSIZE);
        fseek(fp, curLocation, SEEK_SET);
        cursize = fread(fileBuf, sizeof(char), FILEBUFSIZE, fp);
        curLocation = ftell(fp);
        if(cursize <= 0)
        {
            icount++;
            continue;
        }
        pBuf = buf + contentidx;
        //iscomplete
        if(!feof(fp))
        {
            if(icount == 0)
            {
                pBuf += int2hex(pBuf, 10, TS_FILE_BEGIN);
            }
            else
            {
                pBuf += int2hex(pBuf, 10, TS_FILE_CONTINUE);
            }
        }
        else
        {
            if(icount == 0)
            {
                pBuf += int2hex(pBuf, 10, TS_FILE_BEGIN_END);
            }
            else
            {
                pBuf += int2hex(pBuf, 10, TS_FILE_END);
            }
        }
         memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
         pBuf += CONTROL_FLAG_LEN;
        //store the cursize to packet.
        pBuf += int2hex(pBuf, 10, cursize);
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        //copy the data
        memcpy(pBuf, fileBuf, cursize);
        pBuf += cursize;
        
        //send the packet
        ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
        if(ret <= 0)
        {
            free(fileBuf);
            fclose(fp);
            return filtrateError(ret);
        }
        if(feof(fp))
        {
            ret = getAnswerPacketState(fd);
        }
        icount++;
    }
    
    free(fileBuf);
    fclose(fp);
    return ret;
    
}

/**
 * @Funcname:  TClient_DownLoadFile
 * @brief        :  从TS服务器下载文件到工具端
 * @para1      : fd: 与TS服务器的连接句柄
 * @para2      :  filename需要下载的文件名
 * @para3      : savefilename下载文件名需要保存的文件名
                        (带全路径,否则将保存在TS运行的当前目录)
 * @return      : 
                         TS_SUCCESS传送成功
                         TS_FILE_已经存在同名文件
                         TS_FILE_NOT_EXIST指定的文件不存在
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年3月2日 15:00:31
 *   
**/
DLL_FUNC_INT TClient_DownLoadFile(US fd, const char*filename, const char*savefilename)
{
    char buf[PACKET_DATASIZE] = "\0";
    char recvFn[MAX_PATH] = "\0";                  //接收到的文件名
    char *pBuf = buf, *fileBuf = NULL, *pStr = NULL;
    US  curDID = 0, packDID = 0;
    int ret = 0, contentidx = 0, cursize = 0; //当前读取文件内容的大小
    FILE *fp = NULL;
    int iCount = 0, recvAid = 0, isComplete = 0, curLen = 0;     
    US des_aid = 0, des_said = 0, src_said = 0;
    long curLocation = 0;       //当前文件位置
    
    if((NULL == filename) ||(NULL == savefilename))
    {
        return TS_NULLPTR_ENCONTERED;
    }
    if((strlen(filename)<=0) || (strlen(filename)>MAX_PATH) 
        ||(strlen(savefilename)<=0) ||(strlen(savefilename)>MAX_PATH))
    {
        return TS_INVALID_FILE_NAME;
    }
    //发包请求文件下载
     //alloc and refresh the DID
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //f filename
    memcpy(pBuf, &DOWNLOAD_FILE, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, filename, strlen(filename));
    pBuf += strlen(filename);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    //打开文件
    fp = fopen(savefilename, "wb");
    if(NULL == fp)
    {
        return TS_SYSTEM_ERROR;
    }
    ret = TClient_putpkt(fd, 0, 0, 0, buf, (UI)(pBuf - buf));
    if(ret <= 0)
    {
        /*Modifide by tangxp for BUG NO.0002555 on 2008.1.18 [begin]*/
        /*  Modified brief: 关闭文件后再退出*/
        fclose(fp);
        return filtrateError(ret);
    }
    
    while(true)
    {
        ret = TClient_getpkt(fd, &des_aid, &des_said, &src_said, buf, PACKET_DATASIZE);
        if(ret <= 0)
        {
            fclose(fp);
            return filtrateError(ret);
        }
        if(ret < DIDLEN + CONTROL_COMMAND_LEN + 2* CONTROL_FLAG_LEN)
        {
            fclose(fp);
            return TS_UNKNOW_DATA;
        }
        //进行消息包的解析
        __fetch(&packDID, buf, DIDLEN);
        pBuf = buf + DIDLEN + CONTROL_FLAG_LEN;
        if((ret = returnErrValueFromErrMsg(pBuf)) != TS_SUCCESS)
        {
            fclose(fp);
            return ret;
        }
        /*Modified by tangxp for BUG NO.0002548 on 2008.1.16 [begin]*/
        /*  Modified brief: 解析数据包跳过OK字符*/
        //跳过OK
        pBuf = pBuf + ANSWER_SUCEESS_LEN + CONTROL_FLAG_LEN;
        /*Modified by tangxp for BUG NO.0002548 on 2008.1.16 [end]*/
        //filename
        pStr = strstr(pBuf, CONTROL_FLAG);
        if(NULL == pStr)
        {
            fclose(fp);
            return TS_UNKNOW_DATA_FORMAT;
        }
        memcpy(recvFn, pBuf, (UI)(pStr - pBuf));
        //proccess flag
        pBuf = pStr + CONTROL_FLAG_LEN;
        pStr = strstr(pBuf, CONTROL_FLAG);
        if(NULL == pStr)
        {
            fclose(fp);
            return TS_UNKNOW_DATA_FORMAT;
        }
        hex2int(&pBuf, &isComplete);
        //curLen
        pBuf = pStr + CONTROL_FLAG_LEN;
        pStr = strstr(pBuf, CONTROL_FLAG);
        if(NULL == pStr)
        {
            fclose(fp);
            /*Modifide by tangxp for BUG NO.0002555 on 2008.1.18 [end]*/
            return TS_UNKNOW_DATA_FORMAT;
        }
        hex2int(&pBuf, &curLen);
        //进行文件写入
        pBuf = pStr + CONTROL_FLAG_LEN;
        fwrite(pBuf, sizeof(char), curLen, fp);
        fflush(fp);
        if((isComplete == TS_FILE_BEGIN_END) || (isComplete == TS_FILE_END))
        {
            fclose(fp);
            return TS_SUCCESS;
        }
    }
    //进行收包
}


/**
 * @Funcname:  TClient_DeleteFile
 * @brief        :  从TS服务器删除指定文件
 * @para1      : fd: 与TS服务器的连接句柄
 * @para2      :  filename需要下载的文件名
 * @return      : 
                         TS_SUCCESS传送成功
                         TS_FILE_NOT_EXIST指定的文件不存在
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年3月11日 11:00:31
 *   
**/
DLL_FUNC_INT TClient_DeleteFile(US fd, const char*filename)
{
    char sendBuf[100] = "\0";
    char *pBuf = sendBuf;
    US  curDID = 0, packDID = 0;
    int ret = 0;
    
    if(NULL == filename)
    {
        return TS_NULLPTR_ENCONTERED;
    }
     //alloc and refresh the DID
    mtxDID.lock();
    LNK2DID::iterator   m_ItrDID = mapLnkToDID.find(fd);
    if(m_ItrDID == mapLnkToDID.end())
    {
        mtxDID.unlock();
        return TS_INVALID_LINK_HANDLE;
    }
    curDID = ++(m_ItrDID->second);
    mtxDID.unlock();
    
    //fill the dialog ID
    __store(curDID, pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //f filename
    memcpy(pBuf, &DELETE_FILE, CONTROL_COMMAND_LEN);
    pBuf += CONTROL_COMMAND_LEN;
    memcpy(pBuf, filename, strlen(filename));
    pBuf += strlen(filename);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //send the pack
    ret = TClient_putpkt(fd, 0, 0, 0, sendBuf, (UI)(pBuf-sendBuf));        //向manager发送命令包
    if (ret < 0)
        return filtrateError(ret);
    return getAnswerPacketState(fd);
}


/**
 * @Funcname:  filtrateError
 * @brief        :  过滤控制命令函数内部处理错误。
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年1月4日 14:43:43
 *   
**/
int filtrateError(int errValue)
{
    if((errValue != TS_SOCKET_ERROR) 
        &&(errValue != TS_INVALID_LINK_HANDLE))
    {
        return TS_SYSTEM_ERROR;
    }
    return errValue;
}

/***************************************************************************
 * @Funcname:  bool checkInvalidIP
 * @brief        :  检查IP是否非法
 * @para1      : char *ip : IP
 * @para2      : iplen: IP的长度
 * @return      :  合法返回true, 非法返回false
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年11月17日 11:32:52
 *   
***************************************************************************/
bool checkInvalidIP(const char *ip,  size_t iplen)
{
    /*Modified by tangxp for BUG NO.3081 on 2008年4月14日 [begin]*/
    /*  Modified brief: 修正检查IP地址*/
    char tmpIP[IP_BUF_LEN] = "\0";
    char *pcur = NULL;
    int intIp = 0;
    memcpy(tmpIP, ip, min(iplen, sizeof(tmpIP)));
    pcur = tmpIP;
    
    for(int i = 0; i < 4; ++i)
    {
        intIp = atoi(pcur);
        if((intIp < 0) || (intIp > 255) 
            || (((0 == intIp) || (255 == intIp)) && (0 == i)))
        {
            //每段在0到255之间,且第一段不能等于0或255
            return false;  //非法IP
        }

        if(i < 3)
        {
            //IPv4只有三个"."
            pcur = strstr(pcur, ".");
            if(!pcur)
            {
                return false;  //非法IP
            }
            ++pcur;
        }
    }
    
    return true;
    /*Modified by tangxp for BUG NO.3081 on 2008年4月14日 [end]*/
}

/***
 *   函 数    名： returnErrValueFromErrMsg
 *   函数说明： 在返回的消息包中查找是否是错误消息包，
 *              如果是将其转换成对应的错误值返回
 *   参      数1   ：buf消息内容
 *
 *   返  回   值：对应的错误值，NO_ERR:是正常响应包，不是错误包
 *   作           者： qingxh
 *   创建时间： 2006年10月31日 9:50:13
 *   修改历史：
 *   
**/

short returnErrValueFromErrMsg(const char *buf)
{
    /*Modified by tangxp for BUG NO. 0002549 on 2008.1.16 [begin]*/
    /*  Modified brief: 更改匹配消息包类型算法*/
    const char *strBuf = buf;
    char errmsg[ERR_MSG_CHAR_NUM+1];
    
    memset(errmsg, 0, sizeof(errmsg));    
    if(strlen(buf) == 0)
    {
        return TS_RECV_DATA_ERROR;
    }
    //在strBuf里查找字母E,找到就表示是错误消息
    if(strncmp(strBuf, ERRHEAD, 1) == 0)
    {
        strncpy(errmsg, strBuf, ERR_MSG_CHAR_NUM);
        for(UI i = 0; i < MAX_ERR_NUM, ERR_MAP[i].ErrValue != 0; i++)
        {
            if(strcmp(errmsg, ERR_MAP[i].ErrData) == 0)
            {
                return ERR_MAP[i].ErrValue;
            }
        }
        return TS_UNKNOW_ERROR;
    }
    else if(strncmp(strBuf, ANSWER_SUCEESS, 2) == 0)
    {
        //找字母OK，如果找以就表示是正确回复消息
        return TS_SUCCESS;
    }
    else
    {
        //即不是Exx开头的错误回复消息,也不是OK开头的正确回复消息
        return TS_UNKNOW_DATA;
    }
    /*Modified by tangxp for BUG NO. 0002549 on 2008.1.16 [end]*/
}


/**
 * @Funcname:  adjustSocketTime
 * @brief        :  动态调整SOCKET的超时等待时间，
 *                     请在SOCKET等待超时后调用该函数进行动态调整
 * @param    : flag:  等待超时与否，如果超时填为1，否则为0
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年1月4日 14:53:59
 *   
**/
void adjustSocketTime(bool timeoutFlag)
{
    static int ifailCount = 0;
    //如果超时则增加超时统计
    if(timeoutFlag)
    {
        ifailCount++;
    }
    else
    {
        if(ifailCount > 0)
        {
            ifailCount--;
        }
    }
    //连续两次以上超时则将超时时间增加一秒
    if(ifailCount >= 2)
    {
        if(TIMEOUT_GETPKT < MAX_TIME_OUT)
        {
            TIMEOUT_GETPKT++;
        }        
    }
    else if(ifailCount < 2)
    {
        if(TIMEOUT_GETPKT > MIN_TIME_OUT)
        {
            TIMEOUT_GETPKT--;
        }     
    }
    return;
}

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
DLL_FUNC_INT guidToString(GUID guidData, bool upcase, char *strGuid, unsigned short buflen)
{
    char *fmtStr = NULL;
    if((NULL == strGuid) || (buflen < sizeof(GUID)+1))
    {
        return -1;
    }
    memset(strGuid, 0, buflen);
    if(upcase)
    {
        fmtStr = "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";
    }
    else
    {
        fmtStr = "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";
    }
    sprintf_s(strGuid, buflen, fmtStr, guidData.Data1, 
                                                         guidData.Data2,
                                                         guidData.Data3, 
                                                         guidData.Data4[0],
                                                         guidData.Data4[1],
                                                         guidData.Data4[2],
                                                         guidData.Data4[3],
                                                         guidData.Data4[4],
                                                         guidData.Data4[5],
                                                         guidData.Data4[6],
                                                         guidData.Data4[7]);
    strGuid[buflen-1] = '\0';
    return 0;
}/**
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
DLL_FUNC_INT stringToGUID(char *strGuid, unsigned short len, GUID *guidData)
{
    if((NULL == strGuid) || (len < GUID_LEN -1))
    {
        return -1;
    }
    /*GUID字符串形式
            00873FDF-61A8-11D1-AA5E-00C04FB1728B
    */
    int tmpvalue[11] = {0};
    sscanf(strGuid, "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
                                                                    &tmpvalue[0], 
                                                                    &tmpvalue[1],
                                                                    &tmpvalue[2],
                                                                    &tmpvalue[3],
                                                                    &tmpvalue[4],
                                                                    &tmpvalue[5],
                                                                    &tmpvalue[6],
                                                                    &tmpvalue[7],
                                                                    &tmpvalue[8],
                                                                    &tmpvalue[9],
                                                                    &tmpvalue[10]);
    guidData->Data1 = tmpvalue[0];
    guidData->Data2 = tmpvalue[1];
    guidData->Data3 = tmpvalue[2];
    for(int i = 0; i < 8; i++)
    {
        guidData->Data4[i] = tmpvalue[i + 3];
    }
    
    return 0;
}


