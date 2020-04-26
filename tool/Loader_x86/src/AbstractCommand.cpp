/************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2007 CoreTek Systems Inc All Rights Reserved
************************************************************************/

/*
 * �޸���ʷ��
 * 2010-8-18  ��Ԫ־�������������ɼ������޹�˾
 *                   �������ļ���
 */

/**
* @file  AbstractCommand.cpp
* @brief
*       ���ܣ�
*       <li> �����������</li>
*/
/************************���ò���******************************/
#include "AbstractCommand.h"
#include "Loader.h"
#include "Util.h"

/************************���岿��*****************************/

/************************ʵ�ֲ���******************************/
namespace com_coretek_tools_loader_command
{	
	/**
	 * @Funcname  : GetErrorNumber
	 * @brief	  : ����ErrprStr��ȡErrprMum,��������ʽ����
	 * @para[IN]  : str�����ַ���
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	UINT32 AbstractCommand::GetErrorNumber(string str)
	{
		return Util::GetDefault()->GetErrorNumber(str);
	}

	/**
	 * @Funcname  : NormalOutput
	 * @brief	  : һ����Ϣ���
	 * @para[IN]  : outStr����ַ���
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	void AbstractCommand::NormalOutput(string outStr)
	{
		Util::GetDefault()->NormalOutput(outStr);
	}

	/**
	 * @Funcname  : ErrorMessageOutput
	 * @brief	  : ���������Ϣ
	 * @para[IN]  : errorNmber:�����
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	void AbstractCommand::ErrorMessageOutput(UINT32 errorNmber)
	{
		Util::GetDefault()->ErrorMessageOutput(errorNmber);
	}

	/**
	 * @Funcname  : DoneMessageOutput
	 * @brief	  :  Done��Ϣ���
	 * @para[IN]  : errorNmber:�����
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	void AbstractCommand::DoneMessageOutput(void)
	{
		Util::GetDefault()->DoneMessageOutput();
	}

	/**
	 * @Funcname  : ErrorMessageOutput
	 * @brief	  :  ������Ϣ���
	 * @para[IN]  : errorNmber:�����
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	void AbstractCommand::InformationProgressMessageOutput(string msg,UINT32 rate,UINT32 sendsize,UINT32 totalsize)
	{
		Util::GetDefault()->InformationProgressMessageOutput( msg, rate, sendsize, totalsize);
	}
}