/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file           :  tsServer.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> 功能</li> tsServer主体，负责向tsmap注册，消息的接收、派发等
                                 <li>设计思想</li> 
                                 <p> n     注意事项：n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008年9月23日 16:02:35
 * @History        : 
 *   
 */

#ifndef _TS_SERVER_H
#define _TS_SERVER_H

#include "common.h"
#include "sysTypes.h"
#include "ExpProcessor.h"
#include "RepProcessor.h"

/*错误号枚举类型*/
enum T_TsServer_ErrorNum
{
    ERR_NOT_FOUND_IP = -100,
    ERR_NOT_FOUND_EXPRESSION,
    ERR_TOO_MANY_SYMBOL_FILE,
    ERR_TARGET_QUEUE_FULL,
    ERR_MESSAGE_QUEUE_FULL,
    ERR_CREATE_OBJECT_FAIL
};

/*定义错误信息结构*/
typedef struct 
{
    int      erNo;
    char    *erStr;
}T_ERR_TBL;

/*tsServer环境变量结构*/
typedef struct T_TSS_Eviroment_Variables_Struct
{
    /*gdb到arch一级的目录*/
    T_CHAR gdbArchPath[MAX_PATH_LEN];
    /*symbolFiles目录路径*/
    T_CHAR symbolFilesPath[MAX_PATH_LEN];
}T_TSS_Eviroment_Variables;

/*目标机队列节点结构*/
typedef struct T_TSS_Target_Queue_Node_Struct{	
      /*目标机消息处理对象*/
      ExpProcessor* pExpProcessor;
       /*下一个节点指针*/
	struct T_TSS_Target_Queue_Node_Struct* next; 
}T_TSS_Target_Queue_Node;

typedef struct{
       /*目标机队列链表头指针*/	
	T_TSS_Target_Queue_Node* pHead; 
       /*目标机队列链表尾指针*/	
	T_TSS_Target_Queue_Node* pTail; 
       /*目标机队列最大节点数*/	
	T_WORD maxTargetNum;
       /*目标机队列当前节点数*/	
	T_WORD curTargetNum; 
       /*目标机队列互斥量*/
	TMutex *pMtx; 
} T_TSS_Target_Queue_head;

/*tsServer负责消息接收和派发的主处理类定义*/
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
    
    /*构造函数*/
    TsServer();

    /*析构函数*/
    ~TsServer();

public:
    /**
    * 得到唯一实例.
    * @return TsServer* 唯一实例
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
    
    /*程序的主线程,入口函数*/
    T_VOID main();

    /*停止tsServer*/
    T_VOID stop();

    /*向tsmap注册的接口*/
    T_WORD registerToTsmap();
    T_WORD unRegisterToTsmap();

    /*解析表达式求值消息接口*/
    T_WORD parseExpressionMessage(T_UHWORD srcAid, T_CHAR *pMsgStr, T_WORD len,
                                                           T_TSS_Expression_Message *pMsg, char *targetName);

    T_CHAR* getLambdaPath();
    
};

extern T_TSS_Target_Queue_head  *g_targetQueueHead;
extern CRITICAL_SECTION g_cs;
extern char* getAckStr(int eno);

#endif



