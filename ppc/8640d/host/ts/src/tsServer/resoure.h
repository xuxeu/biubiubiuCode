/***************************************************************************
 * 
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 * 
***************************************************************************/


/**
 * &Log:resoure.h,v $
*/


/**
 * @file: resoure.h
 * @brief: 
 * <li>����: ����traServer����</li>
 * @author: tangxp
 * @date: 2009-2-6
 * <p>����:ϵͳ��
 */




/*****************************���ò���********************************/
#include <list>
#include <map>
#include "common.h"
#include "tssConfig.h"
#include "tsServer.h"

/*****************************��������********************************/
extern T_TSS_Eviroment_Variables *g_envVar;

/*****************************���岿��********************************/
typedef struct _TSS_Tda
{
    int tdaSaid;    //TDA SAID
    int saAid;      //TDA��ӦĿ���AID
}T_TSS_Tda;

typedef struct _TSS_Gdb
{
    HANDLE hProcess;    //GDB���̾��
    HANDLE hStdinRd;    //GDB��׼����ܵ���ȡ�˾��
    HANDLE hStdinWrDup; //GDB��׼����ܵ�д�˸��ƾ��
    HANDLE hStdoutWr;   //GDB��׼����ܵ�д�˾��
    HANDLE hStdoutRdDup;    //GDB��׼����ܵ���ȡ�˸��ƾ��
}T_TSS_Gdb;

/**
 * @brief:
 * <li>����:����traServer,gdb,tda
 * @author: tangxp
 * @version: 1.0
 * @date: 2009-2-6
*/
class resourceManager
{
public:
    /**
    * @brief: 
    *    ��ȡresourceManager����
    */
    static resourceManager* getInstance() 
    {
        if (m_rm == NULL)
        {
            TMutexLocker lock(&m_rmMutex);
            if (m_rm == NULL)
            {
                m_rm = new resourceManager();
                atexit(destroy);
            }            
        }

        return m_rm;
    }

    /**
    * @brief: 
    *    ���캯��,����TS
    */
    resourceManager();

    /**
    * @brief: 
    *    ��������,�ر�����,�ͷŴ�����traServer
    */
    ~resourceManager();

    /**
     * @brief: 
     *    ����һ��traServer
     * @param desAid: traServerͨ��desAid
     * @param desAid: traServerͨ��desSaid
     * @return �ɹ�����traServerId��ʧ�ܷ���FAIL
    */
    int allocTraServer(int desAid, int desSaid);
    
    /**
    * @brief: 
    *   �ͷ�һ��traServer
    * @param traServerId: traServer��ID
    */
    void freeTraServer(int traServerId);

    /**
    * @brief: 
    *   ��ȡtraServer������TCP�˿�
    * @param traServerId: traServer��ID
    * @return �ɹ����ض˿ڣ�ʧ�ܷ���FAIL
    */
    int getDllPort(int traServerId);

    /**
    * @brief: 
    *   �ͷ����е�traServer
    */
    void freeAllTraServer();

    /**
     * @brief: 
     *    ����һ��TDA
     * @param saAid: Ŀ���aid
     * @return �ɹ�����TDA��SAID��ʧ�ܷ���FAIL
    */
    T_TSS_Tda* createTda(int saAid);

    /**
    * @brief: 
    *   ɾ��һ��TDA
    * @param tpItem: ����TdaSaid�Ͷ�ӦĿ���aid
    * @return �ɹ�����SUCC��ʧ�ܷ���FAIL
    */
    int deleteTda(T_TSS_Tda* tpItem);

    /**
    * @brief: 
    *   ɾ�����е�TDA
    */
    void deleteAllTda();

    /**
     * @brief: 
     *    ע��һ��GDB
     * @param saAid: Ŀ���aid
     * @return �ɹ����ذ���GDB���̾��������,�������Ķ���
     *      ʧ�ܷ���NULL
    */
    T_TSS_Gdb* createGdb(int saAid);

    /**
    * @brief: 
    *   ע��һ��GDB
    * @param tpItem: ����TdaSaid�Ͷ�ӦĿ���aid
    * @return �ɹ�����SUCC��ʧ�ܷ���FAIL
    */
    int deleteGdb(T_TSS_Gdb* tpItem);

    /**
    * @brief: 
    *   ɾ�����е�GDB
    */
    void deleteAllGdb();    

private:

    /**
    * @brief: 
    *   ����traServerͨ��desSaid
    * @param traServerId: traServer��ID
    * @param desAid: traServerͨ��desSaid
    * @return �ɹ�����SUCC��ʧ�ܷ���FAIL
    */
    int setDesId(int traServerId, int newDesAid, int newDesSaid);

    /**
    * @brief: 
    *   �ͷ���Դ
    */
    static void destroy()
    {
        if (NULL != m_rm)
        {
            delete m_rm;
            m_rm = NULL;
        }
    }

    static resourceManager *m_rm;
    static TMutex m_rmMutex;                        //��Դ����Ա�������
    TMutex m_traMutex;                        //tra�������
    TMutex m_tdaMutex;                        //tda�������
    TMutex m_gdbMutex;                        //gdb�������
    static const int LOW_LEVEL = 5;     //���ˮλ��־
    static const int HIGH_LEVEL = 20;    //���λ��־
    unsigned long m_traServerSN;     //traServer���
    list<int> m_traFreeList;        //�������traServer����
    list<T_TSS_Tda*> m_tdaList;        //���洴����TDA�����ӦĿ���AID
    list<T_TSS_Gdb*> m_gdbList;        //���洴����GDB���̾�������д�˾��
    int m_traWaterLevel;                //��ǰˮλֵ 
    waitCondition m_traOverHighLevel;   //��������,�����ж��Ƿ���ڸ�ˮλ
    int m_fd;     //���Ӿ��    

};

