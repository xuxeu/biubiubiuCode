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
 * @file  WarnningProgressMessage.h
 * @brief
 *       <li>���������Ϣ</li>
 */
#ifndef _INCLUDE_WARNNINGPROGRESSMESSAGE_H_
#define _INCLUDE_WARNNINGPROGRESSMESSAGE_H_

/************************���ò���******************************/
#include "progressmessage.h"
#include <sstream>

/************************�ඨ��*******************************/
namespace com_coretek_tools_loader_message
{
	class WarnningProgressMessage :public ProgressMessage
	{
		public:

			/* ���캯�� */
			WarnningProgressMessage(string msg,UINT32 rate):ProgressMessage(msg,rate){}
		
			/* �������� */
			virtual ~WarnningProgressMessage(void){}
		
			/* ��þ��������Ϣ�ַ���,��ʽΪ+warnning,{msg="m_message",reta="m_rata"} */
			virtual string tostring(void)
			{
				ostringstream format_message;

				format_message << "+warnning,{msg=\"" << m_message << "\",rate=\"" << m_rate << "\"}";
		
				return format_message.str();
			}
	};
}

#endif
