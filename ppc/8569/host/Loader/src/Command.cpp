/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
* ������ʷ��
* 2004-12-6 ��Ԫ־  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  Command.cpp
* @brief
*       ���ܣ�
*       <li>������������</li>
*/
/************************���ò���******************************/
#include "Command.h"

/************************�� �岿��******************************/

/************************ʵ�ֲ���*******************************/
namespace com_coretek_tools_loader_command
{
	/**
	 * @Funcname  : Command
	 * @brief	  : ���캯��
	 * @para[IN]  : Parameter���������
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
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
	 * @brief	  : ��������
	 * @para[IN]  : ��
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
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
	 * @brief	  : �õ���ʱʱ��
	 * @para[IN]  : ��
	 * @reture	  : ��ʱʱ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	UINT32 Command::GetTimeOut()
	{
		string timeout = GetParameterValue("-timeout");
		
		return atoi(timeout.c_str());
	}

	/**
	 * @Funcname  :CreateTarget
	 * @brief	  : ����Ŀ���
	 * @para[IN]  : ��
	 * @reture	  : Ŀ�������
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	Target* Command::CreateTarget(void)
	{	
		string tsIP = NULL_STRING;
		string tgtName = NULL_STRING;
		UINT32 tsPort = 0;

		if(NULL == m_target)
		{
			/* ��ȡIP��ַ */
			tsIP = GetParameterValue("-tsip");

			/* ��ȡ�˿� */
			tsPort = atoi(GetParameterValue("-tsport").c_str());

			/* ��ȡĿ������� */
			tgtName = GetParameterValue("-targetname");

			/* ����Taeget */
			m_target = new Target(tsIP, tgtName, tsPort);
		}

		return m_target;
	}

	/**
	 * @Funcname  :GetTarget
	 * @brief	  : ���Taeget
	 * @para[IN]  : ��
	 * @reture	  : Ŀ�������
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	Target* Command::GetTarget(void)
	{
		return m_target;
	}

	/**
	 * @Funcname  :IsParamOK
	 * @brief	  : ���Ƿ������������
	 * @para[IN]  : ��
	 * @reture	  :TRUE:������ȷ FALSE:�����д�   
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
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
	 * @brief	  : ���ݲ������ƻ�ȡ������ֵ
	 * @para[IN]  : paramName:��������
	 * @reture	  :���ز�����ֵ
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	string Command::GetParameterValue(string paramName)
	{
		UINT32 i = 0;

		/* ���ݲ������ƻ�ȡ������ֵ */
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
	 * @brief	  : ִ����
	 * @para[IN]  : said:����ID
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	BOOL Command::Exec(UINT32 said)
	{
		return TRUE;
	}
}
