/**********************************************************************************
*                     �����������ɼ������޹�˾ ��Ȩ����
*     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
*�޸���ʷ:
2010-9-15 9:57:23       ��˼Ω     �����������ɼ������޹�˾
�޸ļ�����:�������ļ�
*/

/**
*@file   SymbolTable.cpp
*@brief
<li>���ű�������ṩ��ȡָ���ķ��Ų����ӿ�</li>
*@Note
 */
/************************ͷ �� ��******************************/
#include "StdAfx.h"
#include "elf.h"
/************************�� �� ��******************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/
/************************ʵ   ��*******************************/

SymbolTable::SymbolTable(void)
{
}

SymbolTable::~SymbolTable(void)
{
}

/**
* @brief
*	 ��ȡ���Ÿ���
* @param	��
* @return	���Ÿ�����
*/
UINT32 SymbolTable::GetSymCount()
{
	UINT32 size = (UINT32)(m_symTbl.size());
	return size;
}

/**
* @brief
*	 ��ȡ�������
* @param	symName ��������
* @return	successful:  �������
*			fail��   -1
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
*	 ���ҷ���ֵ�͸���ֵ��ͬ�ķ������
* @param[in]	symValue ����ֵ
* @return	successful:  �������
*			fail��   -1
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
*	 ��ȡ����
* @param	index �������
* @return   ָ����ŵķ���Objectָ�롣
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