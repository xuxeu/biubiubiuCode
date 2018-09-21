/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * 修改历史：
 * 2010-8-18  彭元志，北京科银京成技术有限公司
 *                   创建该文件。
 */

/*
 * @file  InformationProgressMessage.cpp
 * @brief
 *       <li>进度信息(一般信息显示)</li>
 */

/************************引用部分******************************/
#include "InformationProgressMessage.h"
#include <sstream>

/************************定 义部分******************************/

/************************实现部分*******************************/
namespace com_coretek_tools_loader_message
{
	/**
	 * @Funcname  : InformationProgressMessage
	 * @brief	  : 构造函数
	 * @para[IN]  : msg：信息 rate：百分比
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	InformationProgressMessage::InformationProgressMessage(string msg,UINT32 rate):ProgressMessage(msg,rate)
	{
		m_sendSize = 0;
		m_totalSize = 0;
	}

	/**
	 * @Funcname  : InformationProgressMessage
	 * @brief	  : 构造函数
	 * @para[IN]  : msg：信息 rate：百分比 sendsize：发送大小 totalsize：总大小
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	InformationProgressMessage::InformationProgressMessage(string msg,UINT32 rate,UINT32 sendsize,UINT32 totalsize):ProgressMessage(msg,rate)
	{
		m_sendSize = sendsize;
		m_totalSize = totalsize;
	}

	/**
	 * @Funcname  : ~InformationProgressMessage
	 * @brief	  : 析构函数
	 * @para[IN]  : 无
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	InformationProgressMessage::~InformationProgressMessage(void)
	{
	}

	/**
	 * @Funcname  : tostring
	 * @brief	  : 获取信息字符串，格式为+infomation,{msg="提示信息",sendsize="已发送大小",totalsize="总大小",rate="当前操作进度百分比"}
	 * @para[IN]  : 无
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
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
