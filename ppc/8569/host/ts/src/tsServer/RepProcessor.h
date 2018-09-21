/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/
/**
 * @file           :  RepProcessor.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> ����</li> �ϱ���Ϣ�����ඨ��
                                 <li>���˼��</li> 
                                 <p> n     ע�����n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008��9��23�� 16:02:35
 * @History        : 
 *   
 */

#ifndef _REP_PROCESSOR_H
#define _REP_PROCESSOR_H

/*Ŀ���״̬*/
typedef enum
{
    SASTATE_INACTIVE = 0,
    SASTATE_ACTIVE,
    SASATE_DELETED
}T_SA_STATE; 

/*�ϱ���Ϣ�����ඨ��*/
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
    /*���캯��*/
    RepProcessor();

public:
     /**
    * �õ�Ψһʵ��.
    * @return RepProcessor* Ψһʵ��
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
     /*��ȡfd*/
    T_UHWORD GetFd();

     /*�����ϱ���Ϣ�����߳�*/
    T_VOID run();

     /*ֹͣ�ϱ���Ϣ�����߳�*/
    T_VOID stop();
};

/*�ϱ���Ϣ�����߳���ں���*/
T_UWORD __stdcall RepProcessorThreadEntry(void *pParam);

#endif

