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
 * @file  ErrorProgressMessage.h
 * @brief
 *       <li>��ʾ������Ϣ���ַ���<</li>
 */
#ifndef _INCLUDE_ERRORPROGRESSMESSAGE_H_
#define _INCLUDE_ERRORPROGRESSMESSAGE_H_

/************************���ò���******************************/
#include "progressmessage.h"
#include <sstream>

/************************�ඨ��********************************/
namespace com_coretek_tools_loader_message
{
	class ErrorProgressMessage :public ProgressMessage
	{
		public:

			/* ���캯�� */
			ErrorProgressMessage(string msg,UINT32 rate):ProgressMessage(msg,rate){}

			/* �������� */
			virtual ~ErrorProgressMessage(void){}
			
			/* ��ȡ���������Ϣ,��ʽΪ+error,{msg="m_message",reta="m_rata"} */
			virtual string tostring(void)
			{
				ostringstream format_message;

				format_message << "+error,{msg=\"" << m_message << "\",rate=\"" << rate << "\"}";

				return format_message.str();
			}
	};
}
#endif
