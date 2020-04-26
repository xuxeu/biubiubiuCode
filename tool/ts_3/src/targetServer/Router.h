/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* 
*
* @file     router.h
* @brief     ����ת��
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/
#ifndef ROUTER_INCLUDE
#define ROUTER_INCLUDE

#include "tsInclude.h"
#include "Agent.h"
//#include <QMutexLocker>
//#include "packet.h"
//////////////////////////���ݰ���¼PackLog����
class Packet;
#include "Manager.h"
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
            return mapID2Agent[id];
        else
            return NULL;
    }

public:
    static Router* getInstance() 
    {
        if (pRouter == NULL) 
        {
            TMutexLocker lock(&mutex);
            if (pRouter == NULL)
                pRouter = new Router();
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
            //�ͷŰ�
            if  (isRelease && (NULL != pack))
                PacketManager::getInstance()->free(pack);
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
#endif 
