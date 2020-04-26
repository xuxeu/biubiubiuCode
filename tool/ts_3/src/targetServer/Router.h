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
* @brief     包的转发
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/
#ifndef ROUTER_INCLUDE
#define ROUTER_INCLUDE

#include "tsInclude.h"
#include "Agent.h"
//#include <QMutexLocker>
//#include "packet.h"
//////////////////////////数据包记录PackLog服务
class Packet;
#include "Manager.h"
class Router 
{
private:
    static TMutex mutex;                        //对缓冲区进行锁定,
    Router() {};
    static Router* pRouter;
    QMap<int ,AgentBase*> mapID2Agent;            //ID到对象
    /**
     * 功能：根据ID得到对应的ServerAgent指针
     * @param ID      ID号
     * 返回值ClientAgent* ,为NULL表示不存在此ID对应的SClientAgent*
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
     * 功能：将数据包发送到指定的ID去
     * @param pack     数据包指针
     * @param isRelease是否释放包资源false表示不释放,true表示释放
     */
    void putpkt(Packet* pack,bool isRelease=true)
    {
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [begin]*/
        /*  Modified brief: 增加数据包指针的检查及交换发送数据包和记录系统日志顺序*/
        AgentBase* pAgent = getAgentFromID(pack->getDes_aid());
        if ((pAgent != NULL) && (NULL != pack))
        {            
            sysLoger(LDEBUG, "Router::putpkt:Route a packet [size:%d] to agent [aid:%d]!", 
                                           pack->getSize(), pack->getDes_aid());
            pAgent->putpkt(pack,isRelease);
        }
        else 
        {
            //释放包
            if  (isRelease && (NULL != pack))
                PacketManager::getInstance()->free(pack);
        }
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [end]*/
    }

    /**
     * 功能：维护ServerAgent和ClientAgent路由表.提供动态路由功能
     * @param pSA ServerAgent指针
     * return ID 号
     */
    void registerAgent(int id,AgentBase* pAgent) 
    {
        TMutexLocker locker(&mutex);
        mapID2Agent.insert(id,pAgent);
    }
    /**
     * 功能:从ServerAgent和ClientAgent路由表注销ServerAgent
     * @param pSA ServerAgent指针
     */
    void unRegisterAgent(int id) 
    {
        TMutexLocker locker(&mutex);
        mapID2Agent.remove(id);   
    }
    friend class Manager;
};
#endif 
