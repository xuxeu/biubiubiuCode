/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ������  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  consoleManager.h
* @brief
*       ���ܣ�
*       <li>����̨�����ඨ��</li>
*/ 

#ifndef _CONSOLEMANAGER_H
#define _CONSOLEMANAGER_H

/************************ͷ�ļ�********************************/
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
/************************�궨��********************************/
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
/* ʹ��C������ */
#define C_INTERPRETER         (1)

/* ʹ��AcoreOS������ */
#define COREOS_INTERPRETER     (2)

/* Ĭ�ϼ�¼��������Ŀ����  */
#define  DEFAULT_CMD_HISTORY_NUM    (20)

/************************���Ͷ���******************************/
/* ϵͳ������� */
//ɾ��CTA����CREATE_TARGET_CONNECT
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

/* �����ִ����� */
#define    CMDWITHARG        (1)
#define    CMDWITHOUTARG     (2)
#if 0
typedef struct _sysCmdTableStruct
{
    sysCmdLableEnum cmdLable;  /* ������� */
    char* cmdStr;              /* ����ؼ��� */
    int ifHaveArg;             /* �������Ƿ������ */
}sysCmdTableStruct;
#endif

typedef bool (*pCheckSysCmdSubArgv)(string& cmd);

typedef struct _sysCmdTableStruct
{
    sysCmdLableEnum cmdLable;  /* ������� */
    char* cmdStr;              /* ����ؼ��� */
    pCheckSysCmdSubArgv checkSubArgv;             /*�������Ʋ������ */
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

    /* ����TS���������� */
    int connectToTs(int TimeOut);

    /* ����Ŀ������� */
    int m_activeTargetConnect(char* pTargetName,bool isDefaultTargetConnect = false);

    /* ��ȡ��ǰ����Ŀ�����AID */
    int getAllServerAid(int pAidArray[], int arrayLen);
    
    /* ��ѯ��ǰTS�е�Ŀ���������Ϣ */
    int m_listTargetInfo();

    /* ��ʾ�û�����Ŀ������ӵ�������Ϣ */
    int inputSerCfg(ServerCfg *pSerCfg);
    
    /* ����������Ŀ�������*/
    int m_creatAndActiveTargetServer();

    /* �����ȡ�������� */
    int processCmd();

    /* ��ȡ����̨���� */
    int msgGet();

    /* ��ȡ�����ֶ�Ӧ�Ĳ����룬ͬʱȡȥ�������ж���Ŀո����Ʊ��*/
    sysCmdLableEnum getCmdLable(string& cmd);

    /* �л���C������ */
    int m_switchToExpInterpreter();

    /* �л������Ĳ���ϵͳ������ */
    int m_switchToCoreOSInterpreter();

    /* ��ȡ������Ϣ */
    int m_getHelp();

    /* ������Ϣ����������� */
    int m_sendCmdToInterpreter(string cmd);

    /* �����Ļ */
    int m_clearScreen();

    /* ��ʾ��ʷ��¼ */
    int m_printHistory(string cmd);

    /* �˳�shell�����ر�TS */
    int m_quit();

    /* �˳�shell���ر�TS */
    int m_exit();

    /*��ӷ��ű��ļ�*/
    int m_addSymbol(char* pFilePath);

    /*ɾ�����ű��ļ�*/
    int m_subSymbol(char* pFilePath);

    /* ����.exe����*/
    int launchProgram(char *fileName);

    /* ��ʼ�������� */
    int installInterpreter();

    /*�ͷŽ�����*/
    int uninstallInterpreter();

    /*���浱ǰ·��*/
    void savePath(char *path);

    /*�����ű��б��ļ��е����ݲ����Ʒ��ű�*/
    int syncSymbolFiles();

    /*��ӡ������Ϣ*/
    void printErrorInfo();

    /*����TS�Ƿ��Ѿ�����*/
    int findProcess(char *ProcessName);

    /*��ӡͼ��*/
    void printWelcome();

	/* ��ȡĿ¼�ļ��б� */
	void DirectoryFileList(string dirPath, vector<string> &fileList);
private:

    bool runFlag;

    bool ifInterpreterRun;

    int currentInterpreter;

    TMutex busyCmdListMtx;

    /* ��������Ϣ���б� */
    deque<string> cmdDeque;

    /* �ܹ�ִ�й��������� */
    int m_totalExecuteCmdNum;

    /* ָ���ı���������� */
    int m_storeCmdHistoryNum;

    char m_tmBuf[BUF_SIZE];
    
    int m_bufSize;

    char cur_path[MAX_PATH_LEN];

};

/************************�ӿ�����******************************/

#endif


