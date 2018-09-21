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
 * @file  ProgressMessage.h
 * @brief
 *       <li>回复进度(+开头的字符串)</li>
 */
#ifndef _INCLUDE_PROGRESSMESSAGE_H_
#define _INCLUDE_PROGRESSMESSAGE_H_

/************************引用部分******************************/
#include "message.h"

/************************类定义********************************/
namespace com_coretek_tools_loader_message
{
	class ProgressMessage : public Message
	{
		protected:
			
			/* 进度信息 */
			string m_message;

			/* 已完成百分比 */
			UINT32 m_rate;
		public:

			/* 构造函数 */
			ProgressMessage(string msg, UINT32 rate):m_message(msg),m_rate(rate){}
			
			/* 析构函数 */
			virtual ~ProgressMessage(void){}
	};
}

#endif
