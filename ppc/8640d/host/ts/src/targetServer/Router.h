/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  router.h
* @brief
*       ���ܣ�
*       <li> ����ת������</li>
*/


#ifndef ROUTER_INCLUDE
#define ROUTER_INCLUDE

/************************ͷ�ļ�********************************/

#include "tsInclude.h"
#include "Agent.h"
#include "Manager.h"
/************************�궨��********************************/

/************************���Ͷ���******************************/

//////////////////////////���ݰ���¼PackLog����
class Packet;

class Router
{
private:
    static TMutex mutex;                        //�Ի�������������,
    Router() {};
    static Router* pRouter;
    QMap<int ,AgentBase*> mapID2Agent;            //ID������

    /**
     * ���ܣ�����ID�õ���Ӧ��ServerAgentָ��
     * @param ID      ID��
     * ����ֵClientAgent* ,ΪNULL��ʾ�����ڴ�ID��Ӧ��SClientAgent*
     */
    AgentBase* getAgentFromID(int id)
    {
        TMutexLocker locker(&mutex);

        if (mapID2Agent.contains(id))
        {
            return mapID2Agent[id];
        }

        else
        {
            return NULL;
        }
    }

public:
    static Router* getInstance()
    {
        if (pRouter == NULL)
        {
            TMutexLocker lock(&mutex);

            if (pRouter == NULL)
            {
                pRouter = new Router();
            }
        }

        return pRouter;
    }

    /**
     * ���ܣ������ݰ����͵�ָ����IDȥ
     * @param pack     ���ݰ�ָ��
     * @param isRelease�Ƿ��ͷŰ���Դfalse��ʾ���ͷ�,true��ʾ�ͷ�
     */
    void putpkt(Packet* pack,bool isRelease=true)
    {
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [begin]*/
        /*  Modified brief: �������ݰ�ָ��ļ�鼰�����������ݰ��ͼ�¼ϵͳ��־˳��*/
        AgentBase* pAgent = getAgentFromID(pack->getDes_aid());

        if ((pAgent != NULL) && (NULL != pack))
        {
            sysLoger(LDEBUG, "Router::putpkt:Route a packet [size:%d] to agent [aid:%d]!",
                     pack->getSize(), pack->getDes_aid());
            pAgent->putpkt(pack,isRelease);
        }

        else
        {
            /*Modified by tangxp for BUG NO.3666 on 2008��7��17�� [begin]*/
            /*  Modified brief: ���ӳ����쳣ʱ��ϵͳ��־��¼*/
            sysLoger(LWARNING, "Router::putpkt:can not find agent [pAgent:0x%x], [pPack:0x%x ]",
                     pAgent, pack);

            /*Modified by tangxp for BUG NO.3666 on 2008��7��17�� [end]*/
            //�ͷŰ�
            if  (isRelease && (NULL != pack))
            {
                PacketManager::getInstance()->free(pack);
            }
        }

        /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [end]*/
    }

    /**
     * ���ܣ�ά��ServerAgent��ClientAgent·�ɱ�.�ṩ��̬·�ɹ���
     * @param pSA ServerAgentָ��
     * return ID ��
     */
    void registerAgent(int id,AgentBase* pAgent)
    {
        if (pAgent == NULL)
        {
            return;
        }

        TMutexLocker locker(&mutex);
        mapID2Agent.insert(id,pAgent);
    }

    /**
     * ����:��ServerAgent��ClientAgent·�ɱ�ע��ServerAgent
     * @param pSA ServerAgentָ��
     */
    void unRegisterAgent(int id)
    {
        TMutexLocker locker(&mutex);
        mapID2Agent.remove(id);
    }
    friend class Manager;
};

/************************�ӿ�����******************************/
#endif
