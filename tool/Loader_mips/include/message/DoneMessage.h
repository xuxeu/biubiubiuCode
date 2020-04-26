/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * �޸���ʷ��
 * 2010-8-18  ��Ԫ־�������������ɼ������޹�˾
 *                   �������ļ���
 */

/*
 * @file  DoneMessage.h
 * @brief
 *       <li>���ܣ��ظ�^done</li>
 */
#ifndef _INCLUDE_DONEMESSAGE_H_
#define _INCLUDE_DONEMESSAGE_H_

/************************���ò���******************************/
#include "message.h"

/************************���Ͷ���******************************/
namespace com_coretek_tools_loader_message
{
	class DoneMessage :public Message
	{
		private:
			
			/* ״̬,�ظ�ʱΪstatus="" */
			string m_status;
		public:
			
			/* ������^done */
			DoneMessage(void):m_status(NULL_STRING){}

			/* ����^done,status="" */
			DoneMessage(string status):m_status(status){}

			/* ���캯�� */
			virtual ~DoneMessage(void){}
			
			/* ^done�ַ��� */
			virtual string tostring(void)
			{
				return m_status != NULL_STRING ? string("^done,status=\"") + m_status + string("\"") : string("^done");
			}
	};
}
#endif
