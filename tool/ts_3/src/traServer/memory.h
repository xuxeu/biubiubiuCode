/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: memory.h,v $
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
  * Revision 1.3  2006/10/30 08:42:15  zhangym
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
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * �����ļ�;�����ļ��Ļ����ӿ�
  */

/**
* @file 	memory.h
* @brief 	
*     <li>���ܣ� �ڴ�������</li>
* @author 	������
* @date 	2006-02-20
* <p>���ţ�ϵͳ�� 
*/

#ifndef _MEMORY_H
#define _MEMORY_H


/************************************���ò���******************************************/
//#include <QMutexLocker>
//#include <QWaitCondition>
#include <QList>
#include "sysTypes.h"		///�����ض���
#include "common.h"
/************************************��������******************************************/


/************************************���岿��******************************************/

/**
* @brief
*	RSP_Memory�Ǳ��ػ���Ŀ������ڴ���Ϣ�Ļ����࣬ÿ������ӵ��һ������ַ�͸û���ַ��Ӧ���ڴ��\n
* @author 	������
* @version 1.0
* @date 	2006-02-20
* <p>���ţ�ϵͳ�� 
*/
class RSP_Memory
{
public:
	T_MODULE T_CONST T_WORD MEMORY_BLOCK_BIT = 6;    ///�ڴ���С����λ
	T_MODULE T_CONST T_WORD MEMORY_BLOCK_SIZE = 1 << MEMORY_BLOCK_BIT;   ///ϵͳ���屾���ڴ�黺���С
	T_MODULE T_CONST T_WORD MEMORY_BLOCK_MASK = (~0) - MEMORY_BLOCK_SIZE + 1;  //�����ڴ����ַ������

	/**
	* @brief 
	*	 ����: ���캯��
	*/
	RSP_Memory();

	/**
	* @brief 
	*	 ����: ��������
	*/
	~RSP_Memory();

	/**
	* @brief 
	*	 ����: �����ڴ��Ļ���ַ
	* @param[in] wAddr ����ַ
	*/
	T_VOID SetBaseAddr(T_WORD wAddr)
	{
		m_addr = wAddr;
	}

	/**
	* @brief 
	*	 ����: �õ��ڴ��Ļ���ַ
	* @return ����ַ
	*/
	T_WORD GetBaseAddr()
	{
		return m_addr;
	}

	/**
	* @brief 
	*	 ����: �õ��ڴ�鱾�ػ��������ָ��
	* @return ����ַ
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
*	RSP_MemoryManager����������ڴ滺�������Ӷ�������Ƶ���ش������ͷŶ���\n
* @author 	������
* @version 1.0
* @date 	2006-02-20
* <p>���ţ�ϵͳ�� 
*/
class RSP_MemoryManager {
public:
	RSP_MemoryManager();

	~RSP_MemoryManager();
	
	/**
	* ����:����һ�����е��ڴ��
	* @return �ɹ����ظ��ڴ�飬ʧ�ܷ���NULL
	*/
	RSP_Memory* Alloc() ;

	/**
	* ����:�ͷ�һ���ڴ��
	* @param[in] tpMem Ҫ�ͷŵ��ڴ��
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	T_BOOL Free(RSP_Memory* tpMem) ;

	T_WORD RSP_MemoryManager::LeftPackets();
private:
	T_MODULE QList<RSP_Memory*> m_freeList;		//������а�����
	T_MODULE T_WORD m_waterLevel;				//��ǰˮλֵ 
	T_MODULE TMutex m_mutex;						//�������
	waitCondition m_bufferNotFull;		//��������,�����ж��Ƿ�ﵽ��ˮλ�ж�

	T_CONST T_MODULE T_WORD LOW_LEVEL = 32;	//���ˮλ��־
	T_CONST T_MODULE T_WORD HIGH_LEVEL = 12800;	//���λ��־
};
#endif