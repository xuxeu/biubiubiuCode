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
		UINT32 cpuType = 0;
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
		
		/* ��ȡ������ַ */
		UINT64 startAddr = elf32->GetStartAddr();
		
		/*��ȡCPU ����*/
		cpuType = elf32->GetCpuType();

		/* �޸�Ŀ���ָ��ָ��Ĵ���*/
		if(cpuType == EM_386)
		{
			UINT32 startAddress = (UINT32)startAddr;
			Util::GetDefault()->big2little(4,&startAddress);
			format_message << "P8=" << hex << startAddress;
		}
		else if(cpuType == EM_POWERPC)
		{
			format_message << "P40=" << hex << (UINT32)startAddr;
		}
		else if(cpuType == EM_MIPS)
		{
			/* ��о2fΪ64λ��ַ����32λΪȫf, GetStartAddr�ӿ���Ҫ�޸�Ϊ64λ����ǰ��ʱ����0xffffffff00000000���� */
			startAddr = startAddr + 0xffffffff00000000;
			Util::GetDefault()->big2little(8, &startAddr);
			format_message << "P25=" << hex << startAddr;
		}
		else
		{
			status = GetErrorNumber(LOADER_ERROR_OPENELF_FAIL);
			ErrorMessageOutput(status);
			return FALSE;
		}
				
		ELF32_Parse::close(elf32);

        /* ������������ */
		target->ExecCommand(format_message.str().c_str(), format_message.str().size());
		
		/* ����������� */
		command = "c";

		/* ������������ */
		target->ExecCommand(command.c_str(), command.size());

		return TRUE;
	}
}
