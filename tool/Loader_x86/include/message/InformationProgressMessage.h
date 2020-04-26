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
 * @file  InformationProgressMessage.h
 * @brief
 *       <li>进度信息(一般信息显示)</li>
 */
#ifndef _INCLUDE_INFORMATIONPROGRESSMESSAGE_H_
#define _INCLUDE_INFORMATIONPROGRESSMESSAGE_H_

/************************引用部分******************************/
#include "progressmessage.h"

/************************类定义********************************/
namespace com_coretek_tools_loader_message
{
	class InformationProgressMessage :public ProgressMessage
	{
		private:

			/* 发送的大小 */
			UINT32 m_sendSize;

			 /* 总大小 */
			UINT32 m_totalSize;
		public:

			/* 构造函数 */
			InformationProgressMessage(string msg,UINT32 rate);

			/* 构造函数 */
			InformationProgressMessage(string msg,UINT32 rate,UINT32 sendsize,UINT32 totalsize);
			
			/* 析构函数 */
			virtual ~InformationProgressMessage(void);
			
			/* 获取进度信息 */
			virtual string tostring(void);
	};
}

#endif

