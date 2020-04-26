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
 * @file  LoadCommand.h
 * @brief
 *       <li>数据加载</li>
 */
#ifndef _INCLUDE_LOADCOMMAND_H_
#define _INCLUDE_LOADCOMMAND_H_

/************************引用部分******************************/
#include "command.h"
#include "elf.h"

/************************定 义部分******************************/
namespace com_coretek_tools_loader_command
{
class LoadCommand :public Command
{
	protected:

		/* 看是否参数完整无误 */
		virtual BOOL IsParamOK(void);
		
		/* 加载段数据 */
		BOOL LoadData(UINT32 address, char *content, UINT32 contentSize, UINT32 packSize,UINT32 said);
	public:
		
		/* 构造函数 */
		LoadCommand(vector<Parameter*> &param):m_LoadFileSize(0),m_CurrentLoadSize(0),Command(param){}

		/* 析构函数 */
		virtual ~LoadCommand(void){}     

		/* 命令执行体 */	
		virtual BOOL Exec(UINT32 said);
	private:
	
		/* 加载文件 */
		BOOL LoadCommand::LoadFile(UINT32 said);

		/* 获取需要下载的总大小 */
		UINT32 m_LoadFileSize;

		/* 当前加载大小 */
		UINT32 m_CurrentLoadSize;

		/* 判断该段是否需要加载 */
		BOOL IsSegmentNeedLoad(Segment *segment);
		
		/* 加载段的总大小 */
		UINT32 GetLoadSegmentSize(ELF32 * elf32);
	};
}
#endif
