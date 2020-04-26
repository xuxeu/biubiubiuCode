/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * $Log: packet.cpp,v $
 * Revision 1.1  2008/04/02 03:36:57  guojc
 * Bug ���  	0003005
 * Bug ���� 	ICE����ʧ��
 *
 * ���룺������
 * ���ӣ�������
 *
 * Revision 1.1  2006/11/13 03:40:55  guojc
 * �ύts3.1����Դ��,��VC8�±���
 *
 * Revision 1.2  2006/07/17 09:30:01  tanjw
 * 1.����arm,x86 rsp��־�򿪺͹رչ���
 * 2.����arm,x86 rsp�ڴ��ȡ�Ż��㷨�򿪺͹رչ���
 *
 * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
 * no message
 *
 * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
 * TS��������
 *
 * Revision 1.2  2006/03/08 06:18:56  guojc
 * ��ע�ͽ����������Ͳ���
 *
 * Revision 1.1  2006/03/04 09:25:08  guojc
 * �״��ύICE Server��̬��Ĵ���
 *
 * Revision 1.0  2006/01/12 01:10:14  guojc
 * �����ļ���ʵ��RSP_Packet��Ļ����ӿڡ�
 */

/**
 * @file 	packet.cpp
 * @brief
 *	<li>���ܣ������������� </li>
 * @author 	������
 * @date 	2006-02-20
 * <p>���ţ�ϵͳ��
 */

/**************************** ���ò��� *********************************/
#include "packet.h"

/*************************** ǰ���������� ******************************/

/**************************** ���岿�� *********************************/

RSP_PacketManager* RSP_PacketManager::m_pm = NULL;
T_WORD RSP_PacketManager::m_waterLevel = 0;
QList<RSP_Packet*> RSP_PacketManager::m_freeList;
QMutex RSP_PacketManager::m_mutex;
/**************************** ʵ�ֲ��� *********************************/

RSP_Packet::RSP_Packet()
{
	m_desAid = -1;
	m_desSaid = -1;
	m_srcSaid = 0; //Ĭ����ID 0
}

RSP_Packet::~RSP_Packet()
{

}

/**
* ����:����һ�������
* @return �ɹ����ػ������ʧ�ܷ���NULL
*/
RSP_Packet* RSP_PacketManager::Alloc() 
{
	QMutexLocker locker(&m_mutex);
	
	while(true)
	{
		if (m_freeList.size() > 0) 
		{	//����������������ˮλһ���������
			RSP_Packet* pRSP_Packet = m_freeList.first();
            memset(pRSP_Packet, 0, sizeof(RSP_Packet));
			m_freeList.removeFirst();
			return pRSP_Packet;
		}

		if(m_waterLevel < HIGH_LEVEL) 
		{	//���ڸ�ˮλ�����ռ�
			RSP_Packet* pack = new RSP_Packet();
			++m_waterLevel;
			return pack;
		}
		if (m_waterLevel >= HIGH_LEVEL)
			m_bufferNotFull.wait(locker.mutex());	//�ȴ�
	}
	return NULL;

 }

/**
* ����:�ͷ�һ�������
* @param pack һ��RSP_Packet����
* @return �ɹ�����true��ʧ�ܷ���false
*/
T_BOOL RSP_PacketManager::Free(RSP_Packet* pack) 
{
	QMutexLocker locker(&m_mutex);
	//pack->SetDesAid( -1);
	//pack->SetDesSaid( -1);
	//pack->SetSrcSaid(0); //Ĭ����ID 0

	if(m_freeList.isEmpty()) 
	{	//�յ�
		if (m_waterLevel >= LOW_LEVEL) 
		{	//������ڵ�ˮλ
			if(m_waterLevel >= HIGH_LEVEL)
			{	//���ڸ�ˮλ
				if (LOW_LEVEL != HIGH_LEVEL) 
				{//�ߵ�ˮλ�����
					--m_waterLevel;	//����ˮλ
					delete pack;
					pack = NULL;
				}
				else//���ߵ�ˮλ��ȵ�ʱ��,�����а���ɾ��,ֱ�ӷ����������
				{
					m_freeList.append(pack);
				}

				m_bufferNotFull.wakeOne();	//������������
			} 
			else 
			{	//ɾ���߳���ˮλ�����ݰ�
				--m_waterLevel;	//����ˮλ
				delete pack;	
				pack = NULL;
			}
		} 
		else
		{ 
			m_freeList.append(pack);	//��ӵ���������
		}
	}
	else 
	{	
		m_freeList.append(pack);	//��ӵ���������
	}
	return true;

}
