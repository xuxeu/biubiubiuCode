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
 * @file  ErrorProgressMessage.h
 * @brief
 *       <li>表示进度信息的字符串<</li>
 */
#ifndef _INCLUDE_ERRORPROGRESSMESSAGE_H_
#define _INCLUDE_ERRORPROGRESSMESSAGE_H_

/************************引用部分******************************/
#include "progressmessage.h"
#include <sstream>

/************************类定义********************************/
namespace com_coretek_tools_loader_message
{
	class ErrorProgressMessage :public ProgressMessage
	{
		public:

			/* 构造函数 */
			ErrorProgressMessage(string msg,UINT32 rate):ProgressMessage(msg,rate){}

			/* 析构函数 */
			virtual ~ErrorProgressMessage(void){}
			
			/* 获取错误进度信息,格式为+error,{msg="m_message",reta="m_rata"} */
			virtual string tostring(void)
			{
				ostringstream format_message;

				format_message << "+error,{msg=\"" << m_message << "\",rate=\"" << rate << "\"}";

				return format_message.str();
			}
	};
}
#endif
