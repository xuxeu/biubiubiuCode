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
 * @file  WarnningProgressMessage.h
 * @brief
 *       <li>警告进度信息</li>
 */
#ifndef _INCLUDE_WARNNINGPROGRESSMESSAGE_H_
#define _INCLUDE_WARNNINGPROGRESSMESSAGE_H_

/************************引用部分******************************/
#include "progressmessage.h"
#include <sstream>

/************************类定义*******************************/
namespace com_coretek_tools_loader_message
{
	class WarnningProgressMessage :public ProgressMessage
	{
		public:

			/* 构造函数 */
			WarnningProgressMessage(string msg,UINT32 rate):ProgressMessage(msg,rate){}
		
			/* 析构函数 */
			virtual ~WarnningProgressMessage(void){}
		
			/* 获得警告进度信息字符串,格式为+warnning,{msg="m_message",reta="m_rata"} */
			virtual string tostring(void)
			{
				ostringstream format_message;

				format_message << "+warnning,{msg=\"" << m_message << "\",rate=\"" << m_rate << "\"}";
		
				return format_message.str();
			}
	};
}

#endif
