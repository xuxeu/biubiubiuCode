/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * 修改历史：
 * 2010-9-15  彭元志，北京科银京成技术有限公司
 *                   创建该文件。
 */

/*
 * @file  Parameter.h
 * @brief
 *       <li>表示一个命令行参数</li>
 */
#ifndef _INCLUDE_PARAMETER_H_
#define _INCLUDE_PARAMETER_H_

/************************引用部分******************************/
#include "Loader.h"

/************************类定义********************************/
namespace com_coretek_tools_loader_command
{
	class Parameter
	{	
		public:
			/* 名字 */
			string name;
			
			/* 参数值 */
			string value;
			
			/* 构造函数 */
			Parameter(void):name(NULL_STRING),value(NULL_STRING){}

			/* 构造函数 */
			Parameter(string name,string value):name(name),value(value){}
			
			/* 析构函数 */
			~Parameter(void){}
	};
}
#endif
