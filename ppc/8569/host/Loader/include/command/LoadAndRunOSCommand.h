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
 * @file  LoadAndRunOSCommand.h
 * @brief
 *       <li>加载运行OS</li>
 */
#ifndef _INCLUDE_LOADANDRUNOSCOMMAND_H_
#define _INCLUDE_LOADANDRUNOSCOMMAND_H_

/************************引用部分******************************/
#include "LoadOSCommand.h"

/************************类定义********************************/
namespace com_coretek_tools_loader_command
{
	class LoadAndRunOSCommand :public LoadOSCommand
	{
		public:

			/* 构造函数 */
			LoadAndRunOSCommand(vector<Parameter*> &param):LoadOSCommand(param){};
			
			/* 命令执行体 */	
			virtual BOOL Exec(UINT32 said);

			/* 析构函数 */
			virtual ~LoadAndRunOSCommand(void){}
	};
}
#endif
