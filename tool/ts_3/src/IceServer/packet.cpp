/************************************************************************
*				北京科银京成技术有限公司 版权所有
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * $Log: packet.cpp,v $
 * Revision 1.1  2008/04/02 03:36:57  guojc
 * Bug 编号  	0003005
 * Bug 名称 	ICE调试失败
 *
 * 编码：唐兴培
 * 检视：郭建川
 *
 * Revision 1.1  2006/11/13 03:40:55  guojc
 * 提交ts3.1工程源码,在VC8下编译
 *
 * Revision 1.2  2006/07/17 09:30:01  tanjw
 * 1.增加arm,x86 rsp日志打开和关闭功能
 * 2.增加arm,x86 rsp内存读取优化算法打开和关闭功能
 *
 * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
 * no message
 *
 * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
 * TS代码整理
 *
 * Revision 1.2  2006/03/08 06:18:56  guojc
 * 对注释进行了修正和补充
 *
 * Revision 1.1  2006/03/04 09:25:08  guojc
 * 首次提交ICE Server动态库的代码
 *
 * Revision 1.0  2006/01/12 01:10:14  guojc
 * 创建文件；实现RSP_Packet类的基本接口。
 */

/**
 * @file 	packet.cpp
 * @brief
 *	<li>功能：缓冲包对象的类 </li>
 * @author 	郭建川
 * @date 	2006-02-20
 * <p>部门：系统部
 */

/**************************** 引用部分 *********************************/
#include "packet.h"

/*************************** 前向声明部分 ******************************/

/**************************** 定义部分 *********************************/

RSP_PacketManager* RSP_PacketManager::m_pm = NULL;
T_WORD RSP_PacketManager::m_waterLevel = 0;
QList<RSP_Packet*> RSP_PacketManager::m_freeList;
QMutex RSP_PacketManager::m_mutex;
/**************************** 实现部分 *********************************/

RSP_Packet::RSP_Packet()
{
	m_desAid = -1;
	m_desSaid = -1;
	m_srcSaid = 0; //默认用ID 0
}

RSP_Packet::~RSP_Packet()
{

}

/**
* 功能:申请一个缓冲包
* @return 成功返回缓冲包，失败返回NULL
*/
RSP_Packet* RSP_PacketManager::Alloc() 
{
	QMutexLocker locker(&m_mutex);
	
	while(true)
	{
		if (m_freeList.size() > 0) 
		{	//空闲链表有数据则水位一定不会溢出
			RSP_Packet* pRSP_Packet = m_freeList.first();
            memset(pRSP_Packet, 0, sizeof(RSP_Packet));
			m_freeList.removeFirst();
			return pRSP_Packet;
		}

		if(m_waterLevel < HIGH_LEVEL) 
		{	//低于高水位则分配空间
			RSP_Packet* pack = new RSP_Packet();
			++m_waterLevel;
			return pack;
		}
		if (m_waterLevel >= HIGH_LEVEL)
			m_bufferNotFull.wait(locker.mutex());	//等待
	}
	return NULL;

 }

/**
* 功能:释放一个缓冲包
* @param pack 一个RSP_Packet对象
* @return 成功返回true，失败返回false
*/
T_BOOL RSP_PacketManager::Free(RSP_Packet* pack) 
{
	QMutexLocker locker(&m_mutex);
	//pack->SetDesAid( -1);
	//pack->SetDesSaid( -1);
	//pack->SetSrcSaid(0); //默认用ID 0

	if(m_freeList.isEmpty()) 
	{	//空的
		if (m_waterLevel >= LOW_LEVEL) 
		{	//如果大于低水位
			if(m_waterLevel >= HIGH_LEVEL)
			{	//大于高水位
				if (LOW_LEVEL != HIGH_LEVEL) 
				{//高低水位不相等
					--m_waterLevel;	//降低水位
					delete pack;
					pack = NULL;
				}
				else//当高低水位相等的时候,不进行包的删除,直接放入空闲链表
				{
					m_freeList.append(pack);
				}

				m_bufferNotFull.wakeOne();	//唤醒条件变量
			} 
			else 
			{	//删除高出低水位的数据包
				--m_waterLevel;	//降低水位
				delete pack;	
				pack = NULL;
			}
		} 
		else
		{ 
			m_freeList.append(pack);	//添加到空闲链表
		}
	}
	else 
	{	
		m_freeList.append(pack);	//添加到空闲链表
	}
	return true;

}
