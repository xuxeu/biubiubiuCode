/*
* 更改历史：
* 2004-12-6  董洪涛  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 唐兴培  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  expressionProcess.h
* @brief
*       功能：
*       <li>表达式函数执行处理类定义</li>
*/ 

#ifndef _EXPRESSIONPROCESS_H
#define _EXPRESSIONPROCESS_H

/************************头文件********************************/
#include <windows.h>
#include <list>
#include <string>
#include <iostream>
#include "shellMain.h"
#include "common.h"
#include "..//tsApi//tclientapi.h"

/************************宏定义********************************/
/* tsmap默认aid号 */
#define TS_MAP_ID (1)

/*目标机shellAgent表达式求值的SAid*/
#define TA_TASK_SHELLAGENT_CALL_FUN (11)

/*默认的行数*/
#define DEFAULT_LINES (10)

/*最大行数*/
#define DISAS_MAX_LINES (64)

/*tsmap出错*/
#define TSMAP_ERROR (-2)


/************************类型定义******************************/
class expressionProcess
{
public:

    expressionProcess();

    ~expressionProcess();

    void stop();

    /* 插入待发送命令到核心操作系统解析器 */
    int insertData(string cmd);


    /* 发送数据包到tsServer */
    int sendDataToTsServer(char* pBuf, int size);


    /* 发送数据包到目标机的shellCallFun*/
    int sendDataToShellCallFun(char* pBuf, int size);

	/* 从ts收包 */
    int recvDataFromTs(char* pBuf, int size);

    /* 处理命令 */
    int processData();

    /* 从tsmap 获取tsServer的Aid号 */
    int getTsServerAidFromTsMap();

    /*处理反汇编指令函数*/
    int m_processDisas(string &cmd);



private:

    bool runFlag;

    TMutex expCmdListMtx;

    /* 待发送消息包列表 */
    list<string> expCmdList;

};

/************************接口声明******************************/

#endif
