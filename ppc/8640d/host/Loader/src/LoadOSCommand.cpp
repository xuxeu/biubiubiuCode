/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * �޸���ʷ��
 * 2010-8-18  ��Ԫ־�������������ɼ������޹�˾
 *                   �������ļ���
 */

/*
 * @file  LoadOSCommand.cpp
 * @brief
 *       <li>����APP</li>
 */
/************************���ò���******************************/
#include "LoadOSCommand.h"
using namespace com_coretek_tools_loader_message;

/************************���岿��******************************/

/************************ʵ�ֲ���******************************/
namespace com_coretek_tools_loader_command
{
	/**
	 * @Funcname  :run
	 * @brief	  : ����ִ����
	 * @para[IN]  : ��
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	BOOL LoadOSCommand::Exec(UINT32 said)
	{
		UINT32 cmdCount = 0;
		BOOL status = FALSE;

		/* ��鴫�ݵĲ����Ƿ���ȷ */
		status = IsParamOK();
		if(TRUE != status)
		{	
			status = GetErrorNumber(LOADER_ERROR_PARAMETER);
			ErrorMessageOutput(status);
			return FALSE;
		}
		
		/* ִ�м��� */
		return LoadCommand::Exec(said);
	}

	/**
	 * @Funcname  :IsParamOK
	 * @brief	  : �����������Ƿ���ȫ
	 * @para[IN]  : ��
	 * @reture	  : �ɹ�����TRUE ʧ�ܷ���FALSE
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	BOOL LoadOSCommand::IsParamOK(void)
	{
		return Command::IsParamOK();
	}
}
