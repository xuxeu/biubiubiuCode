/************************************************************************
*				北京科银京成技术有限公司 版权所有
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * $Log: memory.cpp,v $
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
 * Revision 1.3  2006/10/30 08:42:14  zhangym
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
 * Revision 1.4  2006/04/06 01:14:07  guojc
 * 修正了打开关闭的相关bug,确保动态库能够正确退出
 *
 * Revision 1.3  2006/03/29 08:38:13  guojc
 * 修正了关于软件断点指令插入检查
 *
 * Revision 1.1  2006/03/04 09:25:07  guojc
 * 首次提交ICE Server动态库的代码
 *
 * Revision 1.0  2006/01/12 01:10:14  guojc
 * 创建文件；实现RSP_TcpServer类的基本接口。
 */

/**
 * @file 	memory.cpp
 * @brief
 *	<li>功能：初始化内存对象的类 </li>
 * @author 	郭建川
 * @date 	2006-02-20
 * <p>部门：系统部
 */

/**************************** 引用部分 *********************************/
#include "memory.h"

/*************************** 前向声明部分 ******************************/

/**************************** 定义部分 *********************************/

QList<RSP_Memory*> RSP_MemoryManager::m_freeList;		//保存空闲包链表
T_WORD RSP_MemoryManager::m_waterLevel = NULL;				//当前水位值 
TMutex RSP_MemoryManager::m_mutex;						//互斥对象
	
/**************************** 实现部分 *********************************/

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
		if(pPacket != NULL) {
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
		leftCount += m_freeList.size();
	
	if(m_waterLevel < HIGH_LEVEL) 
		leftCount += HIGH_LEVEL - m_waterLevel - 1;

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
		{	//空闲链表有数据则水位一定不会溢出
			RSP_Memory* pPacket = m_freeList.first();
			m_freeList.removeFirst();
			return pPacket;
		}

		if(m_waterLevel < HIGH_LEVEL) 
		{	//低于高水位则分配空间
			RSP_Memory* pack = new RSP_Memory();
			++m_waterLevel;
			return pack;
		}
		if (m_waterLevel >= HIGH_LEVEL)
		{
			return NULL;
			//m_bufferNotFull.wait(locker.mutex());	//等待
		}
	}
	return NULL;
}

/**
* 功能:释放一个内存块
* @param[in] tpMem 要释放的内存块
* @return 成功返回true，失败返回false
*/
T_BOOL RSP_MemoryManager::Free(RSP_Memory* tpPack) {
	TMutexLocker locker(&m_mutex);
	if(m_freeList.isEmpty()) 
	{	//空的
		if (m_waterLevel >= LOW_LEVEL)
		{	//如果大于低水位
			if(m_waterLevel >= HIGH_LEVEL)
			{	//大于高水位
				if (LOW_LEVEL != HIGH_LEVEL)
				{//高低水位不相等
					--m_waterLevel;	//降低水位
					delete tpPack;
					tpPack = NULL;
				}
				else//当高低水位相等的时候,不进行包的删除,直接放入空闲链表
				{
					m_freeList.append(tpPack);
				}
			//	m_bufferNotFull.wakeOne();	//唤醒条件变量
			}
			else 
			{	//删除高出低水位的数据包
				--m_waterLevel;	//降低水位
				delete tpPack;	
				tpPack = NULL;
			}
		}
		else
		{
			m_freeList.append(tpPack);	//添加到空闲链表
		}
	}
	else
	{	
		m_freeList.append(tpPack);	//添加到空闲链表
	}

	return TRUE;

}

