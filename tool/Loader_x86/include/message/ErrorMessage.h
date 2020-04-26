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
 * @file  ErrorMessage.h
 * @brief
 *       <li>����^error����<</li>
 */
#ifndef _INCLUDE_ERRORMESSAGE_H_
#define _INCLUDE_ERRORMESSAGE_H_

/************************���ò���******************************/
#include "message.h"
#include <sstream>

/************************�ඨ��*******************************/
namespace com_coretek_tools_loader_message
{
	class ErrorMessage :public Message
	{
		private:
			
			/* ����� */
			UINT32 m_errorNum;
		public:
			
			/* ������Ϣ */
			ErrorMessage(UINT32 errorNum):m_errorNum(errorNum){}
			
			/* �������� */
			virtual ~ErrorMessage(void){}
			
			/* ��ȡ������Ϣ�ַ���^error,errorcode="errorNum"*/
			virtual string tostring(void)
			{
				ostringstream format_message;

				format_message << "^error,errorcode=\"" << m_errorNum << "\"";

				return format_message.str();
			}
	};
}
#endif
