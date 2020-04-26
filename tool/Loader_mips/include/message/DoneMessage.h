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
 * @file  DoneMessage.h
 * @brief
 *       <li>功能：回复^done</li>
 */
#ifndef _INCLUDE_DONEMESSAGE_H_
#define _INCLUDE_DONEMESSAGE_H_

/************************引用部分******************************/
#include "message.h"

/************************类型定义******************************/
namespace com_coretek_tools_loader_message
{
	class DoneMessage :public Message
	{
		private:
			
			/* 状态,回复时为status="" */
			string m_status;
		public:
			
			/* 仅返回^done */
			DoneMessage(void):m_status(NULL_STRING){}

			/* 返回^done,status="" */
			DoneMessage(string status):m_status(status){}

			/* 构造函数 */
			virtual ~DoneMessage(void){}
			
			/* ^done字符串 */
			virtual string tostring(void)
			{
				return m_status != NULL_STRING ? string("^done,status=\"") + m_status + string("\"") : string("^done");
			}
	};
}
#endif
