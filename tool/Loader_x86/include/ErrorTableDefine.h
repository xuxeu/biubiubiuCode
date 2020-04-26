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
 * @file  ErrorTableDefine.h
 * @brief
 *       <li>获取错误定义文件定义的错误信息</li>
 */
#ifndef _INCLUDE_ERRORTABLEDEFINE_H_
#define _INCLUDE_ERRORTABLEDEFINE_H_

/************************引用部分******************************/
#include "Loader.h"
#include "ErrorDefine.h"
#include "xml.h"

/************************类定义********************************/
namespace com_coretek_tools_loader_error
{
	class ErrorTableDefine
	{
		private:

			/* 错误类vector */
			vector<ErrorDefine> m_ErrorDefines;
		public:
			
			/* 构造函数 */
			ErrorTableDefine(void);
			
			/* 析构函数 */
			~ErrorTableDefine(void);

			/* 加载文件错误定义的XML文件 */
			BOOL LoadFile(string fileName);

			/* 根据错误字符串获取错误号 */
			UINT32 GetErrorNumber(const string str);	 
	};
}
#endif

