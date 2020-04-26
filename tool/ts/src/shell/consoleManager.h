/*
* 更改历史：
* 2004-12-6  董洪涛  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 唐兴培  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  consoleManager.h
* @brief
*       功能：
*       <li>控制台管理类定义</li>
*/ 

#ifndef _CONSOLEMANAGER_H
#define _CONSOLEMANAGER_H

/************************头文件********************************/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include "common.h"
#include <list>
#include <iostream>
#include <deque>
#include "..//tsApi//tclientapi.h"
#include "shellMain.h"
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string>
/************************宏定义********************************/
#define TS_DEFAULT_IP          ("127.0.0.1")
#define TS_DEFAULT_PORT        (8000)

#define BUF_SIZE               (4096)
#define MAX_CMD_MSG_SISE       (4096)

#define SASTATE_INACTIVE      0
#define SASTATE_ACTIVE          1
#define SASTATE_DELETED       2

#define MAX_CHARS	260
#define MAX_LINES	128

#define EXPRESSION_PROMPT     ("->")
#define COREOS_PROMPT         ("[coreos]#")

#define MAX_PATH_LEN (260)

#define ERROR_BUF_SIZE (1024)
/* 使用C解析器 */
#define C_INTERPRETER         (1)

/* 使用AcoreOS解析器 */
#define COREOS_INTERPRETER     (2)

/* 默认记录的命令数目上限  */
#define  DEFAULT_CMD_HISTORY_NUM    (20)

/************************类型定义******************************/
/* 系统命令序号 */
//删除CTA命令CREATE_TARGET_CONNECT
typedef enum _sysCmdLableEnum
{
	ERROR_ENUM = -1,
	SYS_ERROR_ARGV = -2,
	ACTIVE_TARGET_CONNECT = 0,
	LIST_TARGET_CONNECT,
	SWITCH_TO_EXP_LABLE,
	SWITCH_TO_COREOS_LABLE,
	QUIT_LABLE,
	EXIT_LABLE,
	HELP_LABLE,
	CLEAR_SCREEN_LABLE,
	PRINT_HISTROY,
	SEND_TO_INTERPRETER_LABLE,
	ADD_SYMBOL,
	SUB_SYMBOL
}sysCmdLableEnum;

/* 命令字带参数 */
#define    CMDWITHARG        (1)
#define    CMDWITHOUTARG     (2)
#if 0
typedef struct _sysCmdTableStruct
{
    sysCmdLableEnum cmdLable;  /* 命令序号 */
    char* cmdStr;              /* 命令关键字 */
    int ifHaveArg;             /* 命令字是否带参数 */
}sysCmdTableStruct;
#endif

typedef bool (*pCheckSysCmdSubArgv)(string& cmd);

typedef struct _sysCmdTableStruct
{
    sysCmdLableEnum cmdLable;  /* 命令序号 */
    char* cmdStr;              /* 命令关键字 */
    pCheckSysCmdSubArgv checkSubArgv;             /*二级名称参数检查 */
}sysCmdTableStruct;

typedef struct _symbolFileStruct
{
	char symbolFileName;
	char symbolFilePath;

}symbolFileStruct;


class consoleManager
{
public:
    consoleManager(int interpreter = C_INTERPRETER);

    ~consoleManager();

    void run(char *targetName,int timeout);

    void stop();

    /* 启动TS并建立连接 */
    int connectToTs(int TimeOut);

    /* 激活目标机连接 */
    int m_activeTargetConnect(char* pTargetName,bool isDefaultTargetConnect = false);

    /* 获取当前所有目标机的AID */
    int getAllServerAid(int pAidArray[], int arrayLen);
    
    /* 查询当前TS中的目标机连接信息 */
    int m_listTargetInfo();

    /* 提示用户输入目标机连接的输入信息 */
    int inputSerCfg(ServerCfg *pSerCfg);
    
    /* 创建并激活目标机连接*/
    int m_creatAndActiveTargetServer();

    /* 处理获取到的命令 */
    int processCmd();

    /* 获取控制台输入 */
    int msgGet();

    /* 获取命令字对应的操作码，同时取去除命令中多余的空格与制表符*/
    sysCmdLableEnum getCmdLable(string& cmd);

    /* 切换到C解析器 */
    int m_switchToExpInterpreter();

    /* 切换到核心操作系统解析器 */
    int m_switchToCoreOSInterpreter();

    /* 获取帮助信息 */
    int m_getHelp();

    /* 发送信息到命令解析器 */
    int m_sendCmdToInterpreter(string cmd);

    /* 清空屏幕 */
    int m_clearScreen();

    /* 显示历史记录 */
    int m_printHistory(string cmd);

    /* 退出shell，不关闭TS */
    int m_quit();

    /* 退出shell，关闭TS */
    int m_exit();

    /*添加符号表文件*/
    int m_addSymbol(char* pFilePath);

    /*删除符号表文件*/
    int m_subSymbol(char* pFilePath);

    /* 启动.exe程序*/
    int launchProgram(char *fileName);

    /* 初始化解析器 */
    int installInterpreter();

    /*释放解析器*/
    int uninstallInterpreter();

    /*保存当前路径*/
    void savePath(char *path);

    /*检查符号表列表文件中的内容并复制符号表*/
    int syncSymbolFiles();

    /*打印错误信息*/
    void printErrorInfo();

    /*查找TS是否已经启动*/
    int findProcess(char *ProcessName);

    /*打印图标*/
    void printWelcome();

	/* 获取目录文件列表 */
	void DirectoryFileList(string dirPath, vector<string> &fileList);
private:

    bool runFlag;

    bool ifInterpreterRun;

    int currentInterpreter;

    TMutex busyCmdListMtx;

    /* 待发送消息包列表 */
    deque<string> cmdDeque;

    /* 总共执行过的命令数 */
    int m_totalExecuteCmdNum;

    /* 指定的保存命令个数 */
    int m_storeCmdHistoryNum;

    char m_tmBuf[BUF_SIZE];
    
    int m_bufSize;

    char cur_path[MAX_PATH_LEN];

};

/************************接口声明******************************/

#endif


