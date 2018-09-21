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
 * @file  Message.h
 * @brief
 *       <li>所有回复消息的基类</li>
 */
#ifndef _INCLUDE_MESSAGE_H_
#define _INCLUDE_MESSAGE_H_

/************************引用部分******************************/
#include "Loader.h"
#include "AbstractCommand.h"
using namespace com_coretek_tools_loader_command;

/************************类定义******************************/
namespace com_coretek_tools_loader_message
{
	class Message
	{
		public:	
			/* 构造函数 */
			Message(void){}
			
			/* 析构函数 */
			virtual ~Message(void){}
			
			/* 获得字符串 */
			virtual string tostring(void) = 0;
	};
}
#endif
