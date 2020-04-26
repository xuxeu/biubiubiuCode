/**********************************************************************************
*                     北京科银京成技术有限公司 版权所有
*     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
*修改历史:
2010-9-15 9:57:23       文思惟     北京科银京成技术有限公司
修改及方案:创建本文件
*/

/**
*@file   SymbolTable.cpp
*@brief
<li>符号表管理，并提供获取指定的符号操作接口</li>
*@Note
 */
/************************头 文 件******************************/
#include "StdAfx.h"
#include "elf.h"
/************************宏 定 义******************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/
/************************实   现*******************************/

SymbolTable::SymbolTable(void)
{
}

SymbolTable::~SymbolTable(void)
{
}

/**
* @brief
*	 获取符号个数
* @param	无
* @return	符号个数。
*/
UINT32 SymbolTable::GetSymCount()
{
	UINT32 size = (UINT32)(m_symTbl.size());
	return size;
}

/**
* @brief
*	 获取符号序号
* @param	symName 符号名称
* @return	successful:  符号序号
*			fail：   -1
*/
INT32 SymbolTable::GetSymIndex(string symName)
{
	INT32 cnt = 0;
	Symbol *sym = NULL;

	while (cnt < m_symTbl.size())
	{
		sym = (Symbol*)(m_symTbl.at(cnt));
		if (sym->nameStr == symName)
		{
			return cnt;
		}
		cnt++;
	}
	return FAIL;
}

/**
* @brief
*	 查找符号值和给定值相同的符号序号
* @param[in]	symValue 符号值
* @return	successful:  符号序号
*			fail：   -1
*/
INT32 SymbolTable::GetSymIndex(UINT32 symValue)
{
	INT32 cnt = 0;
	Symbol *sym = NULL;

	while (cnt < m_symTbl.size())
	{
		sym = (Symbol*)(m_symTbl.at(cnt));
		if (sym->st_value == symValue)
		{
			return cnt;
		}
		cnt++;
	}
	return FAIL;
}

/**
* @brief
*	 获取符号
* @param	index 符号序号
* @return   指定序号的符号Object指针。
*/
Symbol* SymbolTable::GetSymbol(UINT32 index)
{
	UINT32 size = m_symTbl.size();
	UINT32 idx = index;
	if (index > size)
	{
		return NULL;
	}

	return (Symbol*)m_symTbl.at(idx);
	
}