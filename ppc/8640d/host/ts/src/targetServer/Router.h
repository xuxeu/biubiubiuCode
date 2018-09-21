/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  router.h
* @brief
*       功能：
*       <li> 包的转发声明</li>
*/


#ifndef ROUTER_INCLUDE
#define ROUTER_INCLUDE

/************************头文件********************************/

#include "tsInclude.h"
#include "Agent.h"
#include "Manager.h"
/************************宏定义********************************/

/************************类型定义******************************/

//////////////////////////数据包记录PackLog服务
class Packet;

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
            /*Modified by tangxp for BUG NO.3666 on 2008年7月17日 [begin]*/
            /*  Modified brief: 增加出现异常时的系统日志记录*/
            sysLoger(LWARNING, "Router::putpkt:can not find agent [pAgent:0x%x], [pPack:0x%x ]",
                     pAgent, pack);

            /*Modified by tangxp for BUG NO.3666 on 2008年7月17日 [end]*/
            //释放包
            if  (isRelease && (NULL != pack))
            {
                PacketManager::getInstance()->free(pack);
            }
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
        if (pAgent == NULL)
        {
            return;
        }

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

/************************接口声明******************************/
#endif
