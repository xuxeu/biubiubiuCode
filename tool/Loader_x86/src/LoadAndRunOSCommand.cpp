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
 * @file  LoadAndRunOSCommand.cpp
 * @brief
 *       <li>���ز�����OS</li>
 */
/************************���ò���******************************/
#include "LoadAndRunOSCommand.h"
#include <sstream>
using namespace com_coretek_tools_loader_message;

/************************�� �岿��******************************/

/************************ʵ�ֲ���*******************************/
namespace com_coretek_tools_loader_command
{
	/**
	 * @Funcname  : run
	 * @brief	  : ִ������
	 * @para[IN]  : ��
	 * @reture	  : ��
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010��11��24�� 
	 *	 
	**/
	BOOL LoadAndRunOSCommand::Exec(UINT32 said)
	{
		UINT32 status = 0;
		string command(NULL_STRING);
		Target *target = NULL;
		ostringstream format_message;
		ELF32 *elf32 = NULL;
		string elfpath = NULL_STRING;

		/* ִ�м��ز��� */
		status = LoadOSCommand::Exec(said);
		if(!status)
		{
			return FALSE;
		}

		/* ���Ŀ��� */
		target = GetTarget();

		/* ���elf�ļ�·�� */
		elfpath = GetParameterValue("-file");	  
		
		/* ��elf�ļ���ȡ�ļ������� */
		elf32 = ELF32_Parse::open(elfpath.c_str()); 
		if(NULL == elf32)
		{
			status = GetErrorNumber(LOADER_ERROR_OPENELF_FAIL);
			ErrorMessageOutput(status);
			return FALSE;
		}

		UINT32 startAddr = elf32->GetStartAddr();

		Util::GetDefault()->big2little(4,&startAddr);
				
		ELF32_Parse::close(elf32);

		format_message << "P8=" << hex << startAddr;

        /* ������������ */
		target->ExecCommand(format_message.str().c_str(), format_message.str().size());
		
		/* ����������� */
		command = "c";

		/* ������������ */
		target->ExecCommand(command.c_str(), command.size());

		return TRUE;
	}
}
