
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
* TS数据包的管理和包的结构
*
* @file     packet.cpp
* @brief     C++TS数据包的管理和包的结构
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#include "Packet.h"
#include "log.h"
//////////////////////////////////////Packet
////////////////////////////////PacketManager
PacketManager* PacketManager::pm = NULL;
int PacketManager::waterLevel = 0;
QList<Packet*> PacketManager::freeList;
QMutex PacketManager::mutex;

/**
* TS数据包的管理和包的结构
*
* @file     packet.cpp
* @brief     C++TS数据包的管理和包的结构
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/
/**
* 功能:对分配的包指针由调用者负责释放,也可以通过调用PackFree函数来达到释放的目的
* @param pack 一个Packet对象
* @return 成功返回true，失败返回false
*/
Packet* PacketManager::alloc() 
{
    QMutexLocker locker(&mutex);
    
    while(true) 
    {
        if (freeList.size() > 0) 
        {    //空闲链表有数据则水位一定不会溢出
            Packet* pPacket = freeList.first();
            freeList.removeFirst();
            int a = sizeof(Packet);
            a = sizeof Packet;
            memset((char*)pPacket,0,sizeof(Packet));
            return pPacket;
        }

        if(waterLevel < HIGH_LEVEL) 
        {    //低于高水位则分配空间
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
        if (waterLevel >= HIGH_LEVEL)
            bufferNotFull.wait(locker.mutex());    //等待
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
    QMutexLocker locker(&mutex);
    if(freeList.isEmpty()) 
    {    //空的
        if (waterLevel >= LOW_LEVEL) 
        {    //如果大于低水位
            if(waterLevel >= HIGH_LEVEL)
            {    //大于高水位
                if (LOW_LEVEL != HIGH_LEVEL) 
                {//高低水位不相等
                    --waterLevel;    //降低水位
                    delete pack;
                    pack = NULL;
                }
                else 
                {//当高低水位相等的时候,不进行包的删除,直接放入空闲链表
                    if (freeList.indexOf(pack) == -1)    //没有找到则插入包
                        freeList.append(pack);    //添加到空闲链表
                    else
                        int a = 0;
                }
                bufferNotFull.wakeOne();    //唤醒条件变量
            } 
            else 
            {    //删除高出低水位的数据包
                --waterLevel;    //降低水位
                delete pack;    
                pack = NULL;
            }
        } 
        else 
        {
            if (freeList.indexOf(pack) == -1)    //没有找到则插入包
                freeList.append(pack);    //添加到空闲链表
            else
                int a = 0;
        }
    }
    else 
    {    
        if (freeList.indexOf(pack) == -1)    //没有找到则插入包
            freeList.append(pack);    //添加到空闲链表
        else
            int a = 0;
    }
    return true;

}






