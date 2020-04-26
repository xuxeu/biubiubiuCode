/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsCommon.h
* @brief  
*       功能：
*       <li>定义错误值及其他宏</li>
*/
/************************头文件********************************/
#ifndef _TSFSCOMMON_H
#define _TSFSCOMMON_H

#include "stdafx.h"
#include "windows.h"
#include "tsfsLog.h"
#include "..\tsApi\tclientapi.h"
#include "tsfsServer.h"
#include "tsfsWorker.h"

/******************************* 声明部分 ************************************/

//声明使用全局Server对象
extern TSFS_Server *pTSFS_Server;

/************************宏定义********************************/
//启用调试模式
//#define DEBUG

//TSMAP AID
#define TSFS_TSMAP_ID                   1  

//回复定义
#define TSFS_SUCCESS                    "OK"                

#define TSFS_SERVER_EXIT                0       //TSFS Server正常退出

#define TSFS_SEND_REGDATA_FAILED        -1      //向TSMAP发送注册数据包失败
#define TSFS_DID_NOT_LOGOUT             -2      //没有向TSMAP注销
#define TSFS_LOGOUT_FAILED              -3      //向TSMAP注销失败
#define TSFS_REGISTER_FAILED            -4      //向TSMAP注册失败
#define TSFS_INVALID_INIT_ARGUMENT      -5      //初始化参数错误
#define TSFS_CONNECT_TS_FAILED          -6      //连接TS失败
#define TSFS_GET_PID_FAILED             -7      //获取控制台PID失败
#define TSFS_CREATE_SERVER_FAILED       -8      //创建Server失败
#define TSFS_SEND_LOGOUT_DATA_FAILED    -9      //向TSMAP发送注销数据包失败

//常量定义
const char TSFS_CONTROL_FLAG[] = ";";
const char TSFS_COMM_CONTROL_FLAG[] =  ":";
const char TSFS_FILE_NAME_BEGIN[] = "<";
const char TSFS_FILE_NAME_END[] = ">";

//支持接口命令
#define TSFS_OPEN_FILE       'o'
#define TSFS_CLOSE_FILE      'c'
#define TSFS_READ_FILE       'r'
#define TSFS_WRITE_FILE      'w'
#define TSFS_CREATE_FILE     'n'
#define TSFS_DELETE_FILE     'd'
#define TSFS_IO_CONTROL      'i'

//IO操作命令
#define TSFS_FIONREAD           1
#define TSFS_FIOSEEK            7
#define TSFS_FIOWHERE           8
#define TSFS_FIORENAME          10
#define TSFS_FIOMKDIR           31
#define TSFS_FIORMDIR           32
#define TSFS_FIOREADDIR         37
#define TSFS_FIOFSTATGETBYFD    38
#define TSFS_FIOFSTATGETBYNAME  39
#define TSFS_FIOTRUNC           43
#define TSFS_FIOTIMESET         45  
#define TSFS_FIOFSTATFSGET      47
#define TSFS_FIOCHECKROOT       49

//acoreos flag
#define	_FAPPEND	0x0008	/* append (writes guaranteed at the end) */
#define	_FCREAT		0x0200	/* open with file create */
#define	_FTRUNC		0x0400	/* open with truncation */
#define	_FEXCL		0x0800	/* error on open if file exists */
#define _FBINARY    0x10000
#define _FTEXT      0x20000


/************************接口声明********************************/
/**
 * 功能: 把int64转换成16进制的字符串, 必须保证size大小大于转换后的字符串大小
 * @param ptr 保存16进制字符串的缓冲区
 * @param size   缓冲区的大小
 * @param intValue   要转换的整形
 * @return 转换的大小
*/
unsigned int intToHex(char *ptr,  int size, __int64 intValue);

/**
* 功能: 将16进制字符串转换成对应的32位整数。
* @param ptr[out] 输入指向16进制字符串的指针，转换过程中指针同步前
*        进。输出转换结束时的指针。
* @param intValue[out]   转换后的32位整数
* @return 返回转换的16进制字符串长度。
*/
unsigned int hexToInt(char **ptr, int *intValue);

/**
* 功能: 将16进制字符串转换成对应的64位整数。
* @param ptr[out] 输入指向16进制字符串的指针，转换过程中指针同步前
*        进。输出转换结束时的指针。
* @param intValue[out]   转换后的64位整数
* @return 返回转换的16进制字符串长度。
*/
unsigned int hexToInt64(char **ptr, __int64 *intValue);

/**
* 功能: 将单个的16进制ASCII字符转换成对应的数字。
* @param ch    单个的16进制ASCII字符
* @param intValue[out]   转换后的32位整数
* @return 返回转换的16进制字符串长度。
*/
int hexToNum(unsigned char ch);

/**
 * @brief
 *   退出TSFS Server
 * @param CtrlType[in]:    关机信号
 */
BOOL WINAPI tsfs_serverExit( DWORD CtrlType );

/**
 * @brief
 *   退出TSFS Server
 * @param signal[in]:    信号,未使用
 */
void tsfs_serverSignalExit( int signal );

#endif

