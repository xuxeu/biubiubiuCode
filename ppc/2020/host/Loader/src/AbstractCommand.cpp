/************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2007 CoreTek Systems Inc All Rights Reserved
************************************************************************/

/*
 * 修改历史：
 * 2010-8-18  彭元志，北京科银京成技术有限公司
 *                   创建该文件。
 */

/**
* @file  AbstractCommand.cpp
* @brief
*       功能：
*       <li> 抽象的命令类</li>
*/
/************************引用部分******************************/
#include "AbstractCommand.h"
#include "Loader.h"
#include "Util.h"

/************************定义部分*****************************/

/************************实现部分******************************/
namespace com_coretek_tools_loader_command
{	
	/**
	 * @Funcname  : GetErrorNumber
	 * @brief	  : 根据ErrprStr获取ErrprMum,以数字形式返回
	 * @para[IN]  : str错误字符串
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	UINT32 AbstractCommand::GetErrorNumber(string str)
	{
		return Util::GetDefault()->GetErrorNumber(str);
	}

	/**
	 * @Funcname  : NormalOutput
	 * @brief	  : 一般信息输出
	 * @para[IN]  : outStr输出字符串
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	void AbstractCommand::NormalOutput(string outStr)
	{
		Util::GetDefault()->NormalOutput(outStr);
	}

	/**
	 * @Funcname  : ErrorMessageOutput
	 * @brief	  : 输出错误信息
	 * @para[IN]  : errorNmber:错误号
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	void AbstractCommand::ErrorMessageOutput(UINT32 errorNmber)
	{
		Util::GetDefault()->ErrorMessageOutput(errorNmber);
	}

	/**
	 * @Funcname  : DoneMessageOutput
	 * @brief	  :  Done信息输出
	 * @para[IN]  : errorNmber:错误号
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	void AbstractCommand::DoneMessageOutput(void)
	{
		Util::GetDefault()->DoneMessageOutput();
	}

	/**
	 * @Funcname  : ErrorMessageOutput
	 * @brief	  :  进度信息输出
	 * @para[IN]  : errorNmber:错误号
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	void AbstractCommand::InformationProgressMessageOutput(string msg,UINT32 rate,UINT32 sendsize,UINT32 totalsize)
	{
		Util::GetDefault()->InformationProgressMessageOutput( msg, rate, sendsize, totalsize);
	}
}