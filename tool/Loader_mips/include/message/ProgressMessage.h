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
 * @file  ProgressMessage.h
 * @brief
 *       <li>�ظ�����(+��ͷ���ַ���)</li>
 */
#ifndef _INCLUDE_PROGRESSMESSAGE_H_
#define _INCLUDE_PROGRESSMESSAGE_H_

/************************���ò���******************************/
#include "message.h"

/************************�ඨ��********************************/
namespace com_coretek_tools_loader_message
{
	class ProgressMessage : public Message
	{
		protected:
			
			/* ������Ϣ */
			string m_message;

			/* ����ɰٷֱ� */
			UINT32 m_rate;
		public:

			/* ���캯�� */
			ProgressMessage(string msg, UINT32 rate):m_message(msg),m_rate(rate){}
			
			/* �������� */
			virtual ~ProgressMessage(void){}
	};
}

#endif
