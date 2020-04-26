/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/
/**
 * @file           :  tsServer.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> ����</li> tsServer����,�������ļ����ء����ء���ѯ��ɾ����
                                 <li>���˼��</li> 
                                 <p> n     ע�����n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008��9��23�� 16:02:35
 * @History        : 
 *   
 */

#ifndef _EXP_PROCESSOR_H
#define _EXP_PROCESSOR_H

#include "tssConfig.h"
#include "tssComm.h"
#include "common.h"

/*Ŀ�������ģʽ����*/
typedef enum 
{
    TARGET_DEBUG_MODE_SYSTEM = 0,
    TARGET_DEBUG_MODE_TASK
}T_TSS_Target_Debug_Mode;

/*OS�汾*/
typedef enum
{
	SYS_VERSION_1X,
	SYS_VERSION_2X
}T_TSS_Version;

/* ����������� */
typedef enum _T_TSS_Command_Type
{
    COMMAND_TYPE_EXPRESSION = 0,
    COMMAND_TYPE_OTHER = 1
}T_TSS_Command_Type;

typedef struct T_TSS_Expression_Message_Struct{
    /*��Ϣ�����Ŀ�������Ӧ��SA��aid��*/ 	
	T_UHWORD saAid; 
    /*������Ϣ��CA��aid��*/	
	T_UHWORD caAid; 
    /*��Ҫװ����ű�ķ����ļ�*/	
	T_CHAR symbolFiles[MAX_SYSMBOL_FILE_NUM][MAX_PATH_LEN]; 
    /*���ʽ*/	
	T_CHAR expression[MAX_EXPRESSION_LEN]; 

    /* �������� */
	T_TSS_Command_Type cmdType;
    
    /*��һ���ڵ�ָ��*/
    T_TSS_Expression_Message_Struct* next;
}T_TSS_Expression_Message;

class ExpProcessor
{
private:
    /*��Ӧ��Ŀ���saAid*/
    T_UHWORD m_saAid;
    /*��Ӧ��Ŀ���IP*/
    T_CHAR m_targetSymbolFolderName[MAX_NAME_LEN];
    /*���Ӿ��*/
    T_UHWORD m_fd;
    /*��ǰ��Ϣ��*/
    T_UWORD m_curMsgNum; 
    /*�����Ϣ��*/
    T_UWORD m_maxMsgNum;
    /*���߳̾��*/
    HANDLE m_hThread;
    /*���߳�ID*/
    T_UWORD m_threadID;
    /*�������汾*/
    T_UWORD m_toolChainVersion;
    /*Ŀ�����ϵ�ṹ*/
    T_UWORD m_targetArchType;
    /*�ź���*/
    waitCondition *m_pSem;
    /*������*/
    TMutex *m_pMtx; 
    /*���߳��Ƿ�������*/
    T_UHWORD m_isRunning;
    /*��Ϣ����ͷָ��*/
    T_TSS_Expression_Message* m_pMsgQueueHead;
    /*��Ϣ����βָ��*/
    T_TSS_Expression_Message* m_pMsgQueueTail;
public:
    /*���캯��*/
    ExpProcessor(T_UHWORD saAid, T_CHAR* targetName);
    /*��������*/
    ~ExpProcessor();
    /*����Ϣ���з�����Ϣ*/
    T_WORD PushMessage(T_TSS_Expression_Message* pMsg);
    /*����Ϣ����ȡ��Ϣ*/
    T_VOID PopMessage(T_TSS_Expression_Message** pMsg);
    /*��ȡ���Ӿ��*/
    T_UHWORD GetFd();
    /*��ȡ��ǰ��Ϣ����*/
    T_UWORD GetCurMsgNum();
    /*��ȡĿ���saAid*/
    T_UHWORD GetSaAid();
    /*��ȡĿ�������*/
    T_CHAR* GetTargetSymbolFolderName();
    /*��ȡ���̵߳�ǰ״̬*/
    T_UHWORD GetState();
    /*�����������������߳�*/
    T_VOID run();
    /*ֹͣ���߳�*/
    T_VOID stop();
    /*��ȡ�������汾*/
    T_WORD GetTargetToolChainVersion();
    /*��ȡ��ϵ�ṹ*/
    T_WORD GetTargetArchType();
	/*C1Ŀ�������ģʽ������״̬�жϺʹ���*/
	bool handleTarget_1X(T_TSS_Expression_Message *pMsg );
	/*C2Ŀ�������ģʽ������״̬�жϺʹ���*/
	bool handleTarget_2X(T_TSS_Expression_Message *pMsg);
    
};

/*���ʽ������������������*/
typedef struct
{
    ExpProcessor *pExpProcessor;
    T_TSS_Expression_Message *pMsg;
    T_TSS_Target_Debug_Mode mode;
}T_TSS_EXP_PROC_ARGV;

/*ExpProcessor�����߳���ں���*/
extern T_UWORD __stdcall ExpProcessorThreadEntry(void *pParam);

/*���ʽ��ֵ��Ϣ��������ͬʱҲ��Ϊ����ģʽʱ���̵߳���ں���*/
extern T_UWORD __stdcall handleExpMessage(void *pParam);

#endif
