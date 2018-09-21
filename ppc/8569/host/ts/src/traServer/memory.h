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
* @file     memory.h
* @brief
*     <li>功能： 内存对象的类</li>
*
*/

#ifndef _MEMORY_H
#define _MEMORY_H


/************************头文件********************************/

#include <QList>
#include "sysTypes.h"       ///类型重定义
#include "common.h"

/************************宏定义********************************/


/************************类型定义******************************/

/**
* @brief
*   RSP_Memory是本地缓存目标机端内存信息的缓冲类，每个对象都拥有一个基地址和该基地址对应的内存块\n
* @author   郭建川
* @version 1.0
* @date     2006-02-20
* <p>部门：系统部
*/
class RSP_Memory
{
public:
    T_MODULE T_CONST T_WORD MEMORY_BLOCK_BIT = 6;    ///内存块大小设置位
    T_MODULE T_CONST T_WORD MEMORY_BLOCK_SIZE = 1 << MEMORY_BLOCK_BIT;   ///系统定义本地内存块缓存大小
    T_MODULE T_CONST T_WORD MEMORY_BLOCK_MASK = (~0) - MEMORY_BLOCK_SIZE + 1;  //计算内存基地址的掩码

    /**
    * @brief
    *    功能: 构造函数
    */
    RSP_Memory();

    /**
    * @brief
    *    功能: 析构函数
    */
    ~RSP_Memory();

    /**
    * @brief
    *    功能: 设置内存块的基地址
    * @param[in] wAddr 基地址
    */
    T_VOID SetBaseAddr(T_WORD wAddr)
    {
        m_addr = wAddr;
    }

    /**
    * @brief
    *    功能: 得到内存块的基地址
    * @return 基地址
    */
    T_WORD GetBaseAddr()
    {
        return m_addr;
    }

    /**
    * @brief
    *    功能: 得到内存块本地缓冲的数组指针
    * @return 基地址
    */
    T_CHAR* GetValue()
    {
        return m_value;
    }

private:

    T_WORD m_addr;
    T_CHAR m_value[MEMORY_BLOCK_SIZE];
};

/**
* @brief
*   RSP_MemoryManager负责管理本地内存缓冲区，从而避免了频繁地创建和释放对象\n
* @author   郭建川
* @version 1.0
* @date     2006-02-20
* <p>部门：系统部
*/
class RSP_MemoryManager
{
public:
    RSP_MemoryManager();

    ~RSP_MemoryManager();

    /**
    * 功能:申请一个空闲的内存块
    * @return 成功返回该内存块，失败返回NULL
    */
    RSP_Memory* Alloc() ;

    /**
    * 功能:释放一个内存块
    * @param[in] tpMem 要释放的内存块
    * @return 成功返回true，失败返回false
    */
    T_BOOL Free(RSP_Memory* tpMem) ;

    T_WORD RSP_MemoryManager::LeftPackets();
private:
    T_MODULE QList<RSP_Memory*> m_freeList;     //保存空闲包链表
    T_MODULE T_WORD m_waterLevel;               //当前水位值
    T_MODULE TMutex m_mutex;                        //互斥对象
    waitCondition m_bufferNotFull;      //条件变量,用来判断是否达到高水位判断

    T_CONST T_MODULE T_WORD LOW_LEVEL = 32; //最低水位标志
    T_CONST T_MODULE T_WORD HIGH_LEVEL = 12800; //最高位标志
};
#endif