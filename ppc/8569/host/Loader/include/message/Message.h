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
 * @file  Message.h
 * @brief
 *       <li>���лظ���Ϣ�Ļ���</li>
 */
#ifndef _INCLUDE_MESSAGE_H_
#define _INCLUDE_MESSAGE_H_

/************************���ò���******************************/
#include "Loader.h"
#include "AbstractCommand.h"
using namespace com_coretek_tools_loader_command;

/************************�ඨ��******************************/
namespace com_coretek_tools_loader_message
{
	class Message
	{
		public:	
			/* ���캯�� */
			Message(void){}
			
			/* �������� */
			virtual ~Message(void){}
			
			/* ����ַ��� */
			virtual string tostring(void) = 0;
	};
}
#endif
