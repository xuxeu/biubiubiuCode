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
 * @file  LoadOSCommand.cpp
 * @brief
 *       <li>加载APP</li>
 */
/************************引用部分******************************/
#include "LoadOSCommand.h"
using namespace com_coretek_tools_loader_message;

/************************定义部分******************************/

/************************实现部分******************************/
namespace com_coretek_tools_loader_command
{
	/**
	 * @Funcname  :run
	 * @brief	  : 命令执行体
	 * @para[IN]  : 无
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	BOOL LoadOSCommand::Exec(UINT32 said)
	{
		UINT32 cmdCount = 0;
		BOOL status = FALSE;

		/* 检查传递的参数是否正确 */
		status = IsParamOK();
		if(TRUE != status)
		{	
			status = GetErrorNumber(LOADER_ERROR_PARAMETER);
			ErrorMessageOutput(status);
			return FALSE;
		}
		
		/* 执行加载 */
		return LoadCommand::Exec(said);
	}

	/**
	 * @Funcname  :IsParamOK
	 * @brief	  : 检查命令参数是否齐全
	 * @para[IN]  : 无
	 * @reture	  : 成功返回TRUE 失败返回FALSE
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	BOOL LoadOSCommand::IsParamOK(void)
	{
		return Command::IsParamOK();
	}
}
