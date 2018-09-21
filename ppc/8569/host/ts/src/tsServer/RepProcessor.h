/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/
/**
 * @file           :  RepProcessor.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> 功能</li> 上报消息处理类定义
                                 <li>设计思想</li> 
                                 <p> n     注意事项：n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008年9月23日 16:02:35
 * @History        : 
 *   
 */

#ifndef _REP_PROCESSOR_H
#define _REP_PROCESSOR_H

/*目标机状态*/
typedef enum
{
    SASTATE_INACTIVE = 0,
    SASTATE_ACTIVE,
    SASATE_DELETED
}T_SA_STATE; 

/*上报消息处理类定义*/
class RepProcessor
{
private:
    static RepProcessor* m_pRepProcessor;
    static void destroy()
    {
        if (NULL != m_pRepProcessor)
        {
            delete m_pRepProcessor;
            m_pRepProcessor = NULL;
        }
    }
    
    T_WORD m_fd;
    HANDLE m_hThread;
    T_UWORD m_threadID;
    T_UHWORD m_isRunning;
    /*构造函数*/
    RepProcessor();

public:
     /**
    * 得到唯一实例.
    * @return RepProcessor* 唯一实例
    */
    static RepProcessor* getInstance() 
    {
        if (NULL == m_pRepProcessor)
        {
            m_pRepProcessor = new RepProcessor();
            atexit(destroy);
        }

        return m_pRepProcessor;
    }
     /*获取fd*/
    T_UHWORD GetFd();

     /*运行上报消息处理线程*/
    T_VOID run();

     /*停止上报消息处理线程*/
    T_VOID stop();
};

/*上报消息处理线程入口函数*/
T_UWORD __stdcall RepProcessorThreadEntry(void *pParam);

#endif

