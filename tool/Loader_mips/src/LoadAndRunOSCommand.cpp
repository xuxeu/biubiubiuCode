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
 * @file  LoadAndRunOSCommand.cpp
 * @brief
 *       <li>加载并运行OS</li>
 */
/************************引用部分******************************/
#include "LoadAndRunOSCommand.h"
#include <sstream>
using namespace com_coretek_tools_loader_message;

/************************定 义部分******************************/

/************************实现部分*******************************/
namespace com_coretek_tools_loader_command
{
	/**
	 * @Funcname  : run
	 * @brief	  : 执行命令
	 * @para[IN]  : 无
	 * @reture	  : 无
	 * @Author	  : pengyz
	 * @History: 1.  Created this function on 2010年11月24日 
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

		/* 执行加载操作 */
		status = LoadOSCommand::Exec(said);
		if(!status)
		{
			return FALSE;
		}

		/* 获得目标机 */
		target = GetTarget();

		/* 获得elf文件路径 */
		elfpath = GetParameterValue("-file");	  
		
		/* 打开elf文件获取文件的属性 */
		elf32 = ELF32_Parse::open(elfpath.c_str()); 
		if(NULL == elf32)
		{
			status = GetErrorNumber(LOADER_ERROR_OPENELF_FAIL);
			ErrorMessageOutput(status);
			return FALSE;
		}
		
		/* 获取启动地址 */
		UINT64 startAddr = elf32->GetStartAddr();
		
		/*获取CPU 类型*/
		cpuType = elf32->GetCpuType();

		/* 修改目标机指令指针寄存器*/
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
			/* 龙芯2f为64位地址，高32位为全f, GetStartAddr接口需要修改为64位，当前暂时加上0xffffffff00000000代替 */
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

        /* 发送运行命令 */
		target->ExecCommand(format_message.str().c_str(), format_message.str().size());
		
		/* 添加运行命令 */
		command = "c";

		/* 发送运行命令 */
		target->ExecCommand(command.c_str(), command.size());

		return TRUE;
	}
}
