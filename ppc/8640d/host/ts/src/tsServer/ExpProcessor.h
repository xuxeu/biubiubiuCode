/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/
/**
 * @file           :  tsServer.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> 功能</li> tsServer主体,处理处理文件上载、下载、查询、删除等
                                 <li>设计思想</li> 
                                 <p> n     注意事项：n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008年9月23日 16:02:35
 * @History        : 
 *   
 */

#ifndef _EXP_PROCESSOR_H
#define _EXP_PROCESSOR_H

#include "tssConfig.h"
#include "tssComm.h"
#include "common.h"

/*目标机调试模式类型*/
typedef enum 
{
    TARGET_DEBUG_MODE_SYSTEM = 0,
    TARGET_DEBUG_MODE_TASK
}T_TSS_Target_Debug_Mode;

/*OS版本*/
typedef enum
{
	SYS_VERSION_1X,
	SYS_VERSION_2X
}T_TSS_Version;

/* 命令操作类型 */
typedef enum _T_TSS_Command_Type
{
    COMMAND_TYPE_EXPRESSION = 0,
    COMMAND_TYPE_OTHER = 1
}T_TSS_Command_Type;

typedef struct T_TSS_Expression_Message_Struct{
    /*消息发向的目标机所对应的SA的aid号*/ 	
	T_UHWORD saAid; 
    /*发送消息的CA的aid号*/	
	T_UHWORD caAid; 
    /*需要装入符号表的符号文件*/	
	T_CHAR symbolFiles[MAX_SYSMBOL_FILE_NUM][MAX_PATH_LEN]; 
    /*表达式*/	
	T_CHAR expression[MAX_EXPRESSION_LEN]; 

    /* 命令类型 */
	T_TSS_Command_Type cmdType;
    
    /*下一个节点指针*/
    T_TSS_Expression_Message_Struct* next;
}T_TSS_Expression_Message;

class ExpProcessor
{
private:
    /*对应的目标机saAid*/
    T_UHWORD m_saAid;
    /*对应的目标机IP*/
    T_CHAR m_targetSymbolFolderName[MAX_NAME_LEN];
    /*连接句柄*/
    T_UHWORD m_fd;
    /*当前消息数*/
    T_UWORD m_curMsgNum; 
    /*最大消息数*/
    T_UWORD m_maxMsgNum;
    /*主线程句柄*/
    HANDLE m_hThread;
    /*主线程ID*/
    T_UWORD m_threadID;
    /*工具链版本*/
    T_UWORD m_toolChainVersion;
    /*目标机体系结构*/
    T_UWORD m_targetArchType;
    /*信号量*/
    waitCondition *m_pSem;
    /*互斥量*/
    TMutex *m_pMtx; 
    /*主线程是否在运行*/
    T_UHWORD m_isRunning;
    /*消息队列头指针*/
    T_TSS_Expression_Message* m_pMsgQueueHead;
    /*消息队列尾指针*/
    T_TSS_Expression_Message* m_pMsgQueueTail;
public:
    /*构造函数*/
    ExpProcessor(T_UHWORD saAid, T_CHAR* targetName);
    /*析构函数*/
    ~ExpProcessor();
    /*向消息队列放入消息*/
    T_WORD PushMessage(T_TSS_Expression_Message* pMsg);
    /*从消息队列取消息*/
    T_VOID PopMessage(T_TSS_Expression_Message** pMsg);
    /*获取连接句柄*/
    T_UHWORD GetFd();
    /*获取当前消息数量*/
    T_UWORD GetCurMsgNum();
    /*获取目标机saAid*/
    T_UHWORD GetSaAid();
    /*获取目标机名字*/
    T_CHAR* GetTargetSymbolFolderName();
    /*获取主线程当前状态*/
    T_UHWORD GetState();
    /*主处理函数，启动主线程*/
    T_VOID run();
    /*停止主线程*/
    T_VOID stop();
    /*获取工具链版本*/
    T_WORD GetTargetToolChainVersion();
    /*获取体系结构*/
    T_WORD GetTargetArchType();
	/*C1目标机调试模式和运行状态判断和处理*/
	bool handleTarget_1X(T_TSS_Expression_Message *pMsg );
	/*C2目标机调试模式和运行状态判断和处理*/
	bool handleTarget_2X(T_TSS_Expression_Message *pMsg);
    
};

/*表达式处理主函数参数类型*/
typedef struct
{
    ExpProcessor *pExpProcessor;
    T_TSS_Expression_Message *pMsg;
    T_TSS_Target_Debug_Mode mode;
}T_TSS_EXP_PROC_ARGV;

/*ExpProcessor的主线程入口函数*/
extern T_UWORD __stdcall ExpProcessorThreadEntry(void *pParam);

/*表达式求值消息处理函数，同时也作为任务级模式时子线程的入口函数*/
extern T_UWORD __stdcall handleExpMessage(void *pParam);

#endif
