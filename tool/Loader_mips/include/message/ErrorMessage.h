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
 * @file  ErrorMessage.h
 * @brief
 *       <li>定义^error返回<</li>
 */
#ifndef _INCLUDE_ERRORMESSAGE_H_
#define _INCLUDE_ERRORMESSAGE_H_

/************************引用部分******************************/
#include "message.h"
#include <sstream>

/************************类定义*******************************/
namespace com_coretek_tools_loader_message
{
	class ErrorMessage :public Message
	{
		private:
			
			/* 错误号 */
			UINT32 m_errorNum;
		public:
			
			/* 错误信息 */
			ErrorMessage(UINT32 errorNum):m_errorNum(errorNum){}
			
			/* 析构函数 */
			virtual ~ErrorMessage(void){}
			
			/* 获取错误信息字符串^error,errorcode="errorNum"*/
			virtual string tostring(void)
			{
				ostringstream format_message;

				format_message << "^error,errorcode=\"" << m_errorNum << "\"";

				return format_message.str();
			}
	};
}
#endif
