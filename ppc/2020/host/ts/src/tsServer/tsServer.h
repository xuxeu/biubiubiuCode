/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file           :  tsServer.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> ����</li> tsServer���壬������tsmapע�ᣬ��Ϣ�Ľ��ա��ɷ���
                                 <li>���˼��</li> 
                                 <p> n     ע�����n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008��9��23�� 16:02:35
 * @History        : 
 *   
 */

#ifndef _TS_SERVER_H
#define _TS_SERVER_H

#include "common.h"
#include "sysTypes.h"
#include "ExpProcessor.h"
#include "RepProcessor.h"

/*�����ö������*/
enum T_TsServer_ErrorNum
{
    ERR_NOT_FOUND_IP = -100,
    ERR_NOT_FOUND_EXPRESSION,
    ERR_TOO_MANY_SYMBOL_FILE,
    ERR_TARGET_QUEUE_FULL,
    ERR_MESSAGE_QUEUE_FULL,
    ERR_CREATE_OBJECT_FAIL
};

/*���������Ϣ�ṹ*/
typedef struct 
{
    int      erNo;
    char    *erStr;
}T_ERR_TBL;

/*tsServer���������ṹ*/
typedef struct T_TSS_Eviroment_Variables_Struct
{
    /*gdb��archһ����Ŀ¼*/
    T_CHAR gdbArchPath[MAX_PATH_LEN];
    /*symbolFilesĿ¼·��*/
    T_CHAR symbolFilesPath[MAX_PATH_LEN];
}T_TSS_Eviroment_Variables;

/*Ŀ������нڵ�ṹ*/
typedef struct T_TSS_Target_Queue_Node_Struct{	
      /*Ŀ�����Ϣ�������*/
      ExpProcessor* pExpProcessor;
       /*��һ���ڵ�ָ��*/
	struct T_TSS_Target_Queue_Node_Struct* next; 
}T_TSS_Target_Queue_Node;

typedef struct{
       /*Ŀ�����������ͷָ��*/	
	T_TSS_Target_Queue_Node* pHead; 
       /*Ŀ�����������βָ��*/	
	T_TSS_Target_Queue_Node* pTail; 
       /*Ŀ����������ڵ���*/	
	T_WORD maxTargetNum;
       /*Ŀ������е�ǰ�ڵ���*/	
	T_WORD curTargetNum; 
       /*Ŀ������л�����*/
	TMutex *pMtx; 
} T_TSS_Target_Queue_head;

/*tsServer������Ϣ���պ��ɷ����������ඨ��*/
class TsServer
{
private:
    static TsServer* m_pTsServer;
    static void destory()
    {
        if (NULL != m_pTsServer)
        {
            delete m_pTsServer;
            m_pTsServer = NULL;
        }
    }
    
    T_UHWORD m_fd;
    RepProcessor *m_pRepProcessor;
    T_UWORD m_curTargetNum;
    T_CHAR m_lambdaPath[MAX_PATH_LEN];
    
    /*���캯��*/
    TsServer();

    /*��������*/
    ~TsServer();

public:
    /**
    * �õ�Ψһʵ��.
    * @return TsServer* Ψһʵ��
    */
    static TsServer* getInstance() 
    {
        if (NULL == m_pTsServer)
        {
            m_pTsServer = new TsServer();
            atexit(destory);
        }

        return m_pTsServer;
    }
    
    /*��������߳�,��ں���*/
    T_VOID main();

    /*ֹͣtsServer*/
    T_VOID stop();

    /*��tsmapע��Ľӿ�*/
    T_WORD registerToTsmap();
    T_WORD unRegisterToTsmap();

    /*�������ʽ��ֵ��Ϣ�ӿ�*/
    T_WORD parseExpressionMessage(T_UHWORD srcAid, T_CHAR *pMsgStr, T_WORD len,
                                                           T_TSS_Expression_Message *pMsg, char *targetName);

    T_CHAR* getLambdaPath();
    
};

extern T_TSS_Target_Queue_head  *g_targetQueueHead;
extern CRITICAL_SECTION g_cs;
extern char* getAckStr(int eno);

#endif



