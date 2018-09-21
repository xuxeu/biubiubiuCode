/*
* 更改历史：
* 2006-02-20 郭建川  北京科银技术有限公司
*                    创建该文件。
* 2006-04-06 张宇旻  北京科银技术有限公司
*                    TS代码整理。
* 2006-06-22 tanjw   北京科银技术有限公司
*                    解决ts删除会话时可能导致异常退出的bug。
*                    增加iceserver检查ice返回值正确性。
* 2006-10-30 张宇旻  北京科银技术有限公司
*                    修正基于TA调试的内存优化算法。
* 2008-03-19 唐兴培  北京科银技术有限公司
*                    解决ts启动后造成Windows任务栏凝固，无法响应快捷键的bug。
*/

/**
 * @file    memory.cpp
 * @brief
 *  <li>功能：初始化内存对象的类 </li>
*
 */


/**************************** 引用部分 *********************************/
#include "memory.h"

/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/

QList<RSP_Memory*> RSP_MemoryManager::m_freeList;       //保存空闲包链表
T_WORD RSP_MemoryManager::m_waterLevel = NULL;              //当前水位值
TMutex RSP_MemoryManager::m_mutex;                      //互斥对象

/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/

RSP_Memory::RSP_Memory()
{
}

RSP_Memory::~RSP_Memory()
{
    //暂时没有什么资源可以释放
}

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<memory manager>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

RSP_MemoryManager::RSP_MemoryManager()
{

}

RSP_MemoryManager::~RSP_MemoryManager()
{
    //释放所有资源
    while (!m_freeList.isEmpty())
    {
        RSP_Memory* pPacket = m_freeList.first();
        m_freeList.removeFirst();

        if(pPacket != NULL)
        {
            delete pPacket;
            pPacket = NULL;
        }
    }
}

//得到缓冲区还能够申请的缓冲块数目大小
T_WORD RSP_MemoryManager::LeftPackets()
{
    TMutexLocker locker(&m_mutex);
    T_WORD leftCount = 0;

    if (m_freeList.size() > 0)
    {
        leftCount += m_freeList.size();
    }

    if(m_waterLevel < HIGH_LEVEL)
    {
        leftCount += HIGH_LEVEL - m_waterLevel - 1;
    }

    return leftCount;
}

/**
* 功能:申请一个空闲的内存块
* @return 成功返回该内存块，失败返回NULL
*/
RSP_Memory* RSP_MemoryManager::Alloc()
{
    TMutexLocker locker(&m_mutex);

    while(true)
    {
        if (m_freeList.size() > 0)
        {
            //空闲链表有数据则水位一定不会溢出
            RSP_Memory* pPacket = m_freeList.first();
            m_freeList.removeFirst();
            return pPacket;
        }

        if(m_waterLevel < HIGH_LEVEL)
        {
            //低于高水位则分配空间
            RSP_Memory* pack = new RSP_Memory();

            if(pack == NULL)
            {
                return NULL;
            }

            ++m_waterLevel;
            return pack;
        }

        if (m_waterLevel >= HIGH_LEVEL)
        {
            return NULL;
            //m_bufferNotFull.wait(locker.mutex()); //等待
        }
    }

    return NULL;
}

/**
* 功能:释放一个内存块
* @param[in] tpMem 要释放的内存块
* @return 成功返回true，失败返回false
*/
T_BOOL RSP_MemoryManager::Free(RSP_Memory* tpPack)
{
    TMutexLocker locker(&m_mutex);

    if(m_freeList.isEmpty())
    {
        //空的
        if (m_waterLevel >= LOW_LEVEL)
        {
            //如果大于低水位
            if(m_waterLevel >= HIGH_LEVEL)
            {
                //大于高水位
                if (LOW_LEVEL != HIGH_LEVEL)
                {
                    //高低水位不相等
                    --m_waterLevel; //降低水位
                    delete tpPack;
                    tpPack = NULL;
                }

                else//当高低水位相等的时候,不进行包的删除,直接放入空闲链表
                {
                    m_freeList.append(tpPack);
                }

                //  m_bufferNotFull.wakeOne();  //唤醒条件变量
            }

            else
            {
                //删除高出低水位的数据包
                --m_waterLevel; //降低水位
                delete tpPack;
                tpPack = NULL;
            }
        }

        else
        {
            m_freeList.append(tpPack);  //添加到空闲链表
        }
    }

    else
    {
        m_freeList.append(tpPack);  //添加到空闲链表
    }

    return TRUE;

}

