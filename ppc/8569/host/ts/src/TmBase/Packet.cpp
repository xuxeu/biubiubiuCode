/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  packet.cpp
* @brief
*       功能：
*       <li> C++TS数据包的管理和包的结构</li>
*/

/************************头文件********************************/
#include "Packet.h"
#include "log.h"

using namespace std;

/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/
PacketManager* PacketManager::pm = NULL;
int PacketManager::waterLevel = 0;
list<Packet*> PacketManager::freeList;
TS_Mutex PacketManager::mutex;


/**
* 功能:对分配的包指针由调用者负责释放,也可以通过调用PackFree函数来达到释放的目的
* @param pack 一个Packet对象
* @return 成功返回true，失败返回false
*/
Packet* PacketManager::alloc()
{
    TS_MutexLocker locker(&mutex);

    while(true)
    {
        if(freeList.size() > 0)
        {
            //空闲链表有数据则水位一定不会溢出
            Packet* pPacket = freeList.front();
            freeList.pop_front();
            memset((char*)pPacket,0,sizeof(Packet));
            return pPacket;
        }

        if(waterLevel < HIGH_LEVEL)
        {
            //低于高水位则分配空间
            Packet* pPacket = NULL;

            try
            {
                pPacket = new Packet();
                ++waterLevel;
                memset((char*)pPacket,0,sizeof(Packet));
            }

            catch(...)
            {
                int aaa = 0;
            }

            return pPacket;
        }

        //如果大于高水位,则等待
        if (waterLevel >= HIGH_LEVEL)
        {
            bufferNotFull.wait(locker.getMutex());
        }
    }

    return NULL;
}

/**
* 功能:释放一个缓冲包
* @param pack 一个Packet对象
* @return 成功返回true，失败返回false
*/
bool PacketManager::free(Packet* pack)
{
    TS_MutexLocker locker(&mutex);

    if(freeList.empty())
    {
        //空闲链表为空,缓冲包需求可能较大,可能超出高水位
        if (waterLevel >= LOW_LEVEL)
        {
            //大于低水位
            if(waterLevel >= HIGH_LEVEL)
            {
                //大于高水位
                if (LOW_LEVEL != HIGH_LEVEL)
                {
                    //高低水位不相等
                    --waterLevel;    //降低水位
                    delete pack;
                    pack = NULL;
                }

                else
                {
                    //当高低水位相等的时候,不进行包的删除,直接放入空闲链表
                    freeList.remove(pack);
                    freeList.push_back(pack);
                }

                bufferNotFull.wakeOne();    //唤醒条件变量
            }

            else
            {
                //删除高出低水位的数据包
                --waterLevel;    //降低水位
                delete pack;
                pack = NULL;
            }
        }

        else
        {
            //低于低水位,放入空闲链表
            freeList.remove(pack);
            freeList.push_back(pack);
        }
    }

    else
    {
        //空闲链表非空,当前缓冲包数量低于低水位
        freeList.remove(pack);
        freeList.push_back(pack);
    }

    return true;
}

void PacketManager::freeAllPacket()
{
    TS_MutexLocker locker(&mutex);

    if(freeList.empty())
    {
        return;
    }

    list<Packet*>::iterator iter;

    for(iter = freeList.begin(); iter != freeList.end(); ++iter)
    {
        delete (*iter);
        *iter = NULL;
    }

    freeList.clear();
}

PacketManager::~PacketManager()
{
    freeAllPacket();
}




