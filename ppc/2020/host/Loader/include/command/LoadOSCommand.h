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
 * @file  LoadOSCommand.h
 * @brief
 *       <li>加载OS</li>
 */
#ifndef _INCLUDE_LOADOSCOMMAND_H_
#define _INCLUDE_LOADOSCOMMAND_H_

/************************引用部分******************************/
#include "LoadCommand.h"

/************************定 义部分******************************/
namespace com_coretek_tools_loader_command
{
	class LoadOSCommand :public LoadCommand
	{
		protected:

			/* 是否参数完整无误 */
			virtual BOOL IsParamOK(void);
		public:
			
			/* 构造函数 */
			LoadOSCommand(vector<Parameter*> &param):LoadCommand(param){}
			
			/* 命令执行体 */	
			virtual BOOL Exec(UINT32 said);

			/* 析构函数 */
			virtual ~LoadOSCommand(void){}
	};
}
#endif
