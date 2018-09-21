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
 * @file  InformationProgressMessage.cpp
 * @brief
 *       <li>������Ϣ(һ����Ϣ��ʾ)</li>
 */

/************************���ò���******************************/
#include "InformationProgressMessage.h"
#include <sstream>

/************************�� �岿��******************************/

/************************ʵ�ֲ���*******************************/
namespace com_coretek_tools_loader_message
{
	/**
	 * @Funcname  : InformationProgressMessage
	 * @brief	  : ���캯��
	 * @para[IN]  : msg����Ϣ rate���ٷֱ�
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	InformationProgressMessage::InformationProgressMessage(string msg,UINT32 rate):ProgressMessage(msg,rate)
	{
		m_sendSize = 0;
		m_totalSize = 0;
	}

	/**
	 * @Funcname  : InformationProgressMessage
	 * @brief	  : ���캯��
	 * @para[IN]  : msg����Ϣ rate���ٷֱ� sendsize�����ʹ�С totalsize���ܴ�С
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	InformationProgressMessage::InformationProgressMessage(string msg,UINT32 rate,UINT32 sendsize,UINT32 totalsize):ProgressMessage(msg,rate)
	{
		m_sendSize = sendsize;
		m_totalSize = totalsize;
	}

	/**
	 * @Funcname  : ~InformationProgressMessage
	 * @brief	  : ��������
	 * @para[IN]  : ��
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	InformationProgressMessage::~InformationProgressMessage(void)
	{
	}

	/**
	 * @Funcname  : tostring
	 * @brief	  : ��ȡ��Ϣ�ַ�������ʽΪ+infomation,{msg="��ʾ��Ϣ",sendsize="�ѷ��ʹ�С",totalsize="�ܴ�С",rate="��ǰ�������Ȱٷֱ�"}
	 * @para[IN]  : ��
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	string InformationProgressMessage::tostring(void)
	{
		ostringstream format_message;

		format_message << "+done,infomation={msg=\"" << m_message << "\",sendsize=\"" << m_sendSize 
		<< "\",totalsize=\"" << m_totalSize << "\",rate=\"" << m_rate << "\"}";

		return format_message.str();
	}
}
