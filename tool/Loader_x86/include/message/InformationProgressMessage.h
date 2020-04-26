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
 * @file  InformationProgressMessage.h
 * @brief
 *       <li>������Ϣ(һ����Ϣ��ʾ)</li>
 */
#ifndef _INCLUDE_INFORMATIONPROGRESSMESSAGE_H_
#define _INCLUDE_INFORMATIONPROGRESSMESSAGE_H_

/************************���ò���******************************/
#include "progressmessage.h"

/************************�ඨ��********************************/
namespace com_coretek_tools_loader_message
{
	class InformationProgressMessage :public ProgressMessage
	{
		private:

			/* ���͵Ĵ�С */
			UINT32 m_sendSize;

			 /* �ܴ�С */
			UINT32 m_totalSize;
		public:

			/* ���캯�� */
			InformationProgressMessage(string msg,UINT32 rate);

			/* ���캯�� */
			InformationProgressMessage(string msg,UINT32 rate,UINT32 sendsize,UINT32 totalsize);
			
			/* �������� */
			virtual ~InformationProgressMessage(void);
			
			/* ��ȡ������Ϣ */
			virtual string tostring(void);
	};
}

#endif

