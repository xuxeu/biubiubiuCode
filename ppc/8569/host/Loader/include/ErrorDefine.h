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
 * @file  ErrorDefine.h
 * @brief
 *       <li>错误定义文件</li>
 */
#ifndef _INCLUDE_ERRORDEFINE_H_
#define _INCLUDE_ERRORDEFINE_H_

/************************引用部分******************************/
#include "Loader.h"

/************************类定义******************************/
namespace com_coretek_tools_loader_error
{
	class ErrorDefine
	{
		public:
			
			/* 错误号 */
			UINT32 errorNum;

			/* 错误字符串 */
			string errorStr;

			/* 告诉给用户的错误原因 */
			string userStr;

			/* 可能的解决方案 */
			string userSolution;

			/* 内部出错信息 */
			string debugStr;

			/* 构造函数 */
			ErrorDefine():errorNum(0),errorStr(NULL_STRING),userStr(NULL_STRING),userSolution(NULL_STRING),debugStr(NULL_STRING){}
			
			/* 析构函数 */
			~ErrorDefine(){}
	};
}

#endif

