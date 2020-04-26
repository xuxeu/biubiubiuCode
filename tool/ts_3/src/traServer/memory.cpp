/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * $Log: memory.cpp,v $
 * Revision 1.2  2008/03/19 10:31:52  guojc
 * Bug ���  	0002430
 * Bug ���� 	ts���������Windows���������̣��޷���Ӧ��ݼ�
 *
 * ���룺������
 * ���ӣ���Т��,������
 *
 * Revision 1.2  2008/03/19 09:17:17  guojc
 * Bug ���  	0002430
 * Bug ���� 	ts���������Windows���������̣��޷���Ӧ��ݼ�
 *
 * ���룺������
 * ���ӣ���Т��,������
 *
 * Revision 1.1  2008/02/04 05:35:00  zhangxc
 * ��Ʊ������ƣ���ACoreOS CVS����ֿ����
 * ��д��Ա��������
 * ������Ա������F
 * ����������������ACoreOS�ֿ���ȡ�����µ�Դ���룬������ύ���µĲֿ�Ŀ¼�ṹ�й���
 * ��һ���ύsrcģ�顣
 *
 * Revision 1.1  2006/11/13 03:41:00  guojc
 * �ύts3.1����Դ��,��VC8�±���
 *
 * Revision 1.3  2006/10/30 08:42:14  zhangym
 * �����˻���TA���Ե��ڴ��Ż��㷨��Ŀǰ�Ѿ���������ʹ�á�
 *
 * Revision 1.2  2006/06/22 03:00:51  tanjw
 * 1.���tsɾ���Ựʱ���ܵ����쳣�˳���bug
 * 2.����iceserver���ice����ֵ��ȷ��
 *
 * Revision 1.1  2006/06/13 10:58:21  tanjw
 * no message
 *
 * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
 * no message
 *
 * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
 * TS��������
 *
 * Revision 1.4  2006/04/06 01:14:07  guojc
 * �����˴򿪹رյ����bug,ȷ����̬���ܹ���ȷ�˳�
 *
 * Revision 1.3  2006/03/29 08:38:13  guojc
 * �����˹�������ϵ�ָ�������
 *
 * Revision 1.1  2006/03/04 09:25:07  guojc
 * �״��ύICE Server��̬��Ĵ���
 *
 * Revision 1.0  2006/01/12 01:10:14  guojc
 * �����ļ���ʵ��RSP_TcpServer��Ļ����ӿڡ�
 */

/**
 * @file 	memory.cpp
 * @brief
 *	<li>���ܣ���ʼ���ڴ������� </li>
 * @author 	������
 * @date 	2006-02-20
 * <p>���ţ�ϵͳ��
 */

/**************************** ���ò��� *********************************/
#include "memory.h"

/*************************** ǰ���������� ******************************/

/**************************** ���岿�� *********************************/

QList<RSP_Memory*> RSP_MemoryManager::m_freeList;		//������а�����
T_WORD RSP_MemoryManager::m_waterLevel = NULL;				//��ǰˮλֵ 
TMutex RSP_MemoryManager::m_mutex;						//�������
	
/**************************** ʵ�ֲ��� *********************************/

RSP_Memory::RSP_Memory()
{
}

RSP_Memory::~RSP_Memory()
{
	//��ʱû��ʲô��Դ�����ͷ�
}

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<memory manager>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

RSP_MemoryManager::RSP_MemoryManager()
{

}

RSP_MemoryManager::~RSP_MemoryManager()
{
	//�ͷ�������Դ
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

//�õ����������ܹ�����Ļ������Ŀ��С
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
* ����:����һ�����е��ڴ��
* @return �ɹ����ظ��ڴ�飬ʧ�ܷ���NULL
*/
RSP_Memory* RSP_MemoryManager::Alloc()
{
	TMutexLocker locker(&m_mutex);
	
	while(true)
	{
		if (m_freeList.size() > 0)
		{	//����������������ˮλһ���������
			RSP_Memory* pPacket = m_freeList.first();
			m_freeList.removeFirst();
			return pPacket;
		}

		if(m_waterLevel < HIGH_LEVEL) 
		{	//���ڸ�ˮλ�����ռ�
			RSP_Memory* pack = new RSP_Memory();
			++m_waterLevel;
			return pack;
		}
		if (m_waterLevel >= HIGH_LEVEL)
		{
			return NULL;
			//m_bufferNotFull.wait(locker.mutex());	//�ȴ�
		}
	}
	return NULL;
}

/**
* ����:�ͷ�һ���ڴ��
* @param[in] tpMem Ҫ�ͷŵ��ڴ��
* @return �ɹ�����true��ʧ�ܷ���false
*/
T_BOOL RSP_MemoryManager::Free(RSP_Memory* tpPack) {
	TMutexLocker locker(&m_mutex);
	if(m_freeList.isEmpty()) 
	{	//�յ�
		if (m_waterLevel >= LOW_LEVEL)
		{	//������ڵ�ˮλ
			if(m_waterLevel >= HIGH_LEVEL)
			{	//���ڸ�ˮλ
				if (LOW_LEVEL != HIGH_LEVEL)
				{//�ߵ�ˮλ�����
					--m_waterLevel;	//����ˮλ
					delete tpPack;
					tpPack = NULL;
				}
				else//���ߵ�ˮλ��ȵ�ʱ��,�����а���ɾ��,ֱ�ӷ����������
				{
					m_freeList.append(tpPack);
				}
			//	m_bufferNotFull.wakeOne();	//������������
			}
			else 
			{	//ɾ���߳���ˮλ�����ݰ�
				--m_waterLevel;	//����ˮλ
				delete tpPack;	
				tpPack = NULL;
			}
		}
		else
		{
			m_freeList.append(tpPack);	//��ӵ���������
		}
	}
	else
	{	
		m_freeList.append(tpPack);	//��ӵ���������
	}

	return TRUE;

}

