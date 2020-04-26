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

		UINT32 startAddr = elf32->GetStartAddr();

		Util::GetDefault()->big2little(4,&startAddr);
				
		ELF32_Parse::close(elf32);

		format_message << "P8=" << hex << startAddr;

        /* 发送运行命令 */
		target->ExecCommand(format_message.str().c_str(), format_message.str().size());
		
		/* 添加运行命令 */
		command = "c";

		/* 发送运行命令 */
		target->ExecCommand(command.c_str(), command.size());

		return TRUE;
	}
}
