/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: memory.h,v $
  * Revision 1.2  2008/03/19 10:31:52  guojc
  * Bug 编号  	0002430
  * Bug 名称 	ts启动后造成Windows任务栏凝固，无法响应快捷键
  *
  * 编码：唐兴培
  * 检视：卿孝海,郭建川
  *
  * Revision 1.2  2008/03/19 09:17:17  guojc
  * Bug 编号  	0002430
  * Bug 名称 	ts启动后造成Windows任务栏凝固，无法响应快捷键
  *
  * 编码：唐兴培
  * 检视：卿孝海,郭建川
  *
  * Revision 1.1  2008/02/04 05:35:00  zhangxc
  * 设计报告名称：《ACoreOS CVS代码仓库管理》
  * 编写人员：张晓超
  * 检视人员：张宇旻
  * 其它：从现有最新ACoreOS仓库中取出最新的源代码，整理后提交到新的仓库目录结构中管理。
  * 第一次提交src模块。
  *
  * Revision 1.1  2006/11/13 03:41:00  guojc
  * 提交ts3.1工程源码,在VC8下编译
  *
  * Revision 1.3  2006/10/30 08:42:15  zhangym
  * 修正了基于TA调试的内存优化算法，目前已经可以正常使用。
  *
  * Revision 1.2  2006/06/22 03:00:51  tanjw
  * 1.解决ts删除会话时可能导致异常退出的bug
  * 2.增加iceserver检查ice返回值正确性
  *
  * Revision 1.1  2006/06/13 10:58:21  tanjw
  * no message
  *
  * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
  * no message
  *
  * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
  * TS代码整理
  *
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * 创建文件;定义文件的基本接口
  */

/**
* @file 	memory.h
* @brief 	
*     <li>功能： 内存对象的类</li>
* @author 	郭建川
* @date 	2006-02-20
* <p>部门：系统部 
*/

#ifndef _MEMORY_H
#define _MEMORY_H


/************************************引用部分******************************************/
//#include <QMutexLocker>
//#include <QWaitCondition>
#include <QList>
#include "sysTypes.h"		///类型重定义
#include "common.h"
/************************************声明部分******************************************/


/************************************定义部分******************************************/

/**
* @brief
*	RSP_Memory是本地缓存目标机端内存信息的缓冲类，每个对象都拥有一个基地址和该基地址对应的内存块\n
* @author 	郭建川
* @version 1.0
* @date 	2006-02-20
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
	*	 功能: 构造函数
	*/
	RSP_Memory();

	/**
	* @brief 
	*	 功能: 析构函数
	*/
	~RSP_Memory();

	/**
	* @brief 
	*	 功能: 设置内存块的基地址
	* @param[in] wAddr 基地址
	*/
	T_VOID SetBaseAddr(T_WORD wAddr)
	{
		m_addr = wAddr;
	}

	/**
	* @brief 
	*	 功能: 得到内存块的基地址
	* @return 基地址
	*/
	T_WORD GetBaseAddr()
	{
		return m_addr;
	}

	/**
	* @brief 
	*	 功能: 得到内存块本地缓冲的数组指针
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
*	RSP_MemoryManager负责管理本地内存缓冲区，从而避免了频繁地创建和释放对象\n
* @author 	郭建川
* @version 1.0
* @date 	2006-02-20
* <p>部门：系统部 
*/
class RSP_MemoryManager {
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
	T_MODULE QList<RSP_Memory*> m_freeList;		//保存空闲包链表
	T_MODULE T_WORD m_waterLevel;				//当前水位值 
	T_MODULE TMutex m_mutex;						//互斥对象
	waitCondition m_bufferNotFull;		//条件变量,用来判断是否达到高水位判断

	T_CONST T_MODULE T_WORD LOW_LEVEL = 32;	//最低水位标志
	T_CONST T_MODULE T_WORD HIGH_LEVEL = 12800;	//最高位标志
};
#endif