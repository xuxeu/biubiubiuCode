/**********************************************************************************
*                     北京科银京成技术有限公司 版权所有
*     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *修改历史:
 *2010-9-15 9:57:23       文思惟     北京科银京成技术有限公司
 *修改及方案:创建本文件
 */

/**
 *@file   ELF32.cpp
 *@brief
 *<li>实现ELF数据管理，并提供操作接口</li>
 *@Note
 */

/************************头 文 件******************************/
#include "stdafx.h"
#include "elf.h"
/************************宏 定 义******************************/

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved

					   )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/
/************************实   现*******************************/

/**
* @brief      ELF类构造函数
* @param[in]	无
* @return      无
**/
ELF32::ELF32(void)
{
	parseDone = false;
	m_symCtl = NULL;
}

/**
* @brief      ELF类析构函数
* @param[in]	无
* @return      无
**/
ELF32::~ELF32(void)
{
	if (m_symCtl != NULL)
	{
		free(m_symCtl);
	}	
}

/**
* @brief      获取指定index的段头
* @param[in]	index: 段序号
* @return       指定index的段头指针
**/
Segment* ELF32::GetSegment(UINT32 index)
{
	if (!(e_phnum > index))
	{
		return NULL;
	}
	return m_sgmtTbl.at(index);

}

/**
* @brief      获取指定index的节区头
* @param[in]	index: 节区序号
* @return       指定index的节区头指针
**/
Section* ELF32::GetSection(UINT32 index)
{

	if (!(e_shnum > index))
	{
		return NULL;
	}
	return m_secTbl.at(index);

}

/**
* @brief      获取elf类型
* @param	无
* @return      elf类型
**/
elf_type ELF32::GetFileType()
{
	return e_type;
}

/**
* @brief      获取elf入口地址
* @param	无
* @return      入口地址
**/
UINT32 ELF32::GetStartAddr()
{
	return e_entry;
}

/**
* @brief      获取elf大小端
* @param	无
* @return      大小端
**/
UINT32 ELF32::GetEndian()
{
	return e_ident[5];
}

/**
* @brief      获取elf体系结构
* @param	无
* @return      大小端
**/
elf_Machine ELF32::GetCpuType()
{
	return  e_machine;
}

/**
* @brief      获取elf节表个数
* @param	无
* @return      大小端
**/
INT16 ELF32::GetSectionCount()
{
	return e_shnum;
}

/**
* @brief      获取elf段表个数
* @param	无
* @return      大小端
**/
INT16 ELF32::GetSegmentCount()
{
	return e_phnum;
}

/**
* @brief	根据名称获取节区序号
* @param	无
* @return	successful: 节区序号
*			fail :-1
**/
UINT16 ELF32::GetSectionIndex(char* name)
{
	UINT16 index = 0;
	UINT16 secCount = (UINT16)m_secTbl.size();
	char buf[NAME_LEN];
	Section *sec;

	while(index < secCount)
	{
		sec = m_secTbl.at(index);
		sec->GetName(buf);
	
		if(!(strcmp(name, buf)))
		{
			return index;
		}
		index++;
	}

	return FAIL;
}

/**
* @brief	获取符号表
* @param	无
* @return	符号表Object，如果返回值为NULL，表示没有符号表
**/
SymbolTable* ELF32::GetSymTable()
{
	return m_symCtl;
}
