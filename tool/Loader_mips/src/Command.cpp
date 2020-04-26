/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
* 更改历史：
* 2004-12-6 彭元志  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  Command.cpp
* @brief
*       功能：
*       <li>基本的命令类</li>
*/
/************************引用部分******************************/
#include "Command.h"

/************************定 义部分******************************/

/************************实现部分*******************************/
namespace com_coretek_tools_loader_command
{
	/**
	 * @Funcname  : Command
	 * @brief	  : 构造函数
	 * @para[IN]  : Parameter：输入参数
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	Command::Command(vector<Parameter*> &param)
	{
		m_target = NULL;
		for(UINT32 i = 0; i < (UINT32)param.size(); i++)
		{
			m_params.push_back(param[i]);
		}
	}

	/**
	 * @Funcname  : ~Command
	 * @brief	  : 析构函数
	 * @para[IN]  : 无
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	Command::~Command(void)
	{
		if(NULL != m_target)
		{
			delete m_target;
		}
	}

	/**
	 * @Funcname  :GetTimeOut
	 * @brief	  : 得到超时时间
	 * @para[IN]  : 无
	 * @reture	  : 超时时间
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	UINT32 Command::GetTimeOut()
	{
		string timeout = GetParameterValue("-timeout");
		
		return atoi(timeout.c_str());
	}

	/**
	 * @Funcname  :CreateTarget
	 * @brief	  : 创建目标机
	 * @para[IN]  : 无
	 * @reture	  : 目标机对象
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	Target* Command::CreateTarget(void)
	{	
		string tsIP = NULL_STRING;
		string tgtName = NULL_STRING;
		UINT32 tsPort = 0;

		if(NULL == m_target)
		{
			/* 获取IP地址 */
			tsIP = GetParameterValue("-tsip");

			/* 获取端口 */
			tsPort = atoi(GetParameterValue("-tsport").c_str());

			/* 获取目标机名字 */
			tgtName = GetParameterValue("-targetname");

			/* 创建Taeget */
			m_target = new Target(tsIP, tgtName, tsPort);
		}

		return m_target;
	}

	/**
	 * @Funcname  :GetTarget
	 * @brief	  : 获得Taeget
	 * @para[IN]  : 无
	 * @reture	  : 目标机对象
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	Target* Command::GetTarget(void)
	{
		return m_target;
	}

	/**
	 * @Funcname  :IsParamOK
	 * @brief	  : 看是否参数完整无误
	 * @para[IN]  : 无
	 * @reture	  :TRUE:参数正确 FALSE:参数有错   
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	BOOL Command::IsParamOK(void)
	{
		if(GetParameterValue("-file") == NULL_STRING)
		{
			return FALSE;
		}
		
		if(GetParameterValue("-psize") == NULL_STRING)
		{
			return FALSE;
		}
		
		if(GetParameterValue("-tsip") == NULL_STRING)
		{
			return FALSE;
		}
		
		if(GetParameterValue("-tsport") == NULL_STRING)
		{
			return FALSE;
		}

		if(GetParameterValue("-targetname") == NULL_STRING)
		{
			return FALSE;
		}

		if(GetParameterValue("-timeout") == NULL_STRING)
		{
			return FALSE;
		}
		
		return TRUE;
	}

	/**
	 * @Funcname  :GetParameterValue
	 * @brief	  : 根据参数名称获取参数的值
	 * @para[IN]  : paramName:参数名称
	 * @reture	  :返回参数的值
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	string Command::GetParameterValue(string paramName)
	{
		UINT32 i = 0;

		/* 根据参数名称获取参数的值 */
		for(i = 0; i < (UINT32)m_params.size(); i++)
		{
			if(m_params[i]->name == paramName)
			{
				return m_params[i]->value;
			}
		}
		
		return NULL_STRING;
	}

	/**
	 * @Funcname  : Exec
	 * @brief	  : 执行体
	 * @para[IN]  : said:代理ID
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
	 *	 
	**/
	BOOL Command::Exec(UINT32 said)
	{
		return TRUE;
	}
}
