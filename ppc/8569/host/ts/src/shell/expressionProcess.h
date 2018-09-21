/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ������  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  expressionProcess.h
* @brief
*       ���ܣ�
*       <li>���ʽ����ִ�д����ඨ��</li>
*/ 

#ifndef _EXPRESSIONPROCESS_H
#define _EXPRESSIONPROCESS_H

/************************ͷ�ļ�********************************/
#include <windows.h>
#include <list>
#include <string>
#include <iostream>
#include "shellMain.h"
#include "common.h"
#include "..//tsApi//tclientapi.h"

/************************�궨��********************************/
/* tsmapĬ��aid�� */
#define TS_MAP_ID (1)

/*Ŀ���shellAgent���ʽ��ֵ��SAid*/
#define TA_TASK_SHELLAGENT_CALL_FUN (11)

/*Ĭ�ϵ�����*/
#define DEFAULT_LINES (10)

/*�������*/
#define DISAS_MAX_LINES (64)

/*tsmap����*/
#define TSMAP_ERROR (-2)


/************************���Ͷ���******************************/
class expressionProcess
{
public:

    expressionProcess();

    ~expressionProcess();

    void stop();

    /* ���������������Ĳ���ϵͳ������ */
    int insertData(string cmd);


    /* �������ݰ���tsServer */
    int sendDataToTsServer(char* pBuf, int size);


    /* �������ݰ���Ŀ�����shellCallFun*/
    int sendDataToShellCallFun(char* pBuf, int size);

	/* ��ts�հ� */
    int recvDataFromTs(char* pBuf, int size);

    /* �������� */
    int processData();

    /* ��tsmap ��ȡtsServer��Aid�� */
    int getTsServerAidFromTsMap();

    /*�������ָ���*/
    int m_processDisas(string &cmd);



private:

    bool runFlag;

    TMutex expCmdListMtx;

    /* ��������Ϣ���б� */
    list<string> expCmdList;

};

/************************�ӿ�����******************************/

#endif
