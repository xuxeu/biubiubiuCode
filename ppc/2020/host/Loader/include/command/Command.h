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
 * @file  Command.h
 * @brief
 *       <li>功能：所有命令的抽象类</li>
 */
#ifndef _INCLUDE_COMMAND_H_
#define _INCLUDE_COMMAND_H_

/************************引用部分******************************/
#include "Parameter.h"
#include "Target.h"
#include "AbstractCommand.h"

/************************类型定义******************************/
namespace com_coretek_tools_loader_command
{
class Command:public AbstractCommand
{
	private:
		
		/* 主机端传递的参数 */
		vector<Parameter*> m_params;

		/* 目标机用于收发数据 */
		Target *m_target;
	protected:	

		/* 根据参数名称获取参数的值 */
		string GetParameterValue(string paramName);

		/* 看是否参数完整无误 */
		virtual BOOL IsParamOK(void) = 0;

	public:
		
		/* 构造函数 */
		Command(vector<Parameter*> &param);

		/* 析构函数 */
		virtual ~Command(void);

		/* 创建Target */
		Target* CreateTarget(void);

		/* 获得Target */
		Target *GetTarget(void);

		/* 获取超时时间 */
		UINT32 GetTimeOut();

		/* 命令执行体 */	
		virtual BOOL Exec(UINT32 said);
	};
}

#endif
