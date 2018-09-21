/**********************************************************************************
*                     �����������ɼ������޹�˾ ��Ȩ����
*     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *�޸���ʷ:
 *2010-9-15 9:57:23       ��˼Ω     �����������ɼ������޹�˾
 *�޸ļ�����:�������ļ�
 */

/**
 *@file   ELF32.cpp
 *@brief
 *<li>ʵ��ELF���ݹ������ṩ�����ӿ�</li>
 *@Note
 */

/************************ͷ �� ��******************************/
#include "stdafx.h"
#include "elf.h"
/************************�� �� ��******************************/

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
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/
/************************ʵ   ��*******************************/

/**
* @brief      ELF�๹�캯��
* @param[in]	��
* @return      ��
**/
ELF32::ELF32(void)
{
	parseDone = false;
	m_symCtl = NULL;
}

/**
* @brief      ELF����������
* @param[in]	��
* @return      ��
**/
ELF32::~ELF32(void)
{
	if (m_symCtl != NULL)
	{
		free(m_symCtl);
	}	
}

/**
* @brief      ��ȡָ��index�Ķ�ͷ
* @param[in]	index: �����
* @return       ָ��index�Ķ�ͷָ��
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
* @brief      ��ȡָ��index�Ľ���ͷ
* @param[in]	index: �������
* @return       ָ��index�Ľ���ͷָ��
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
* @brief      ��ȡelf����
* @param	��
* @return      elf����
**/
elf_type ELF32::GetFileType()
{
	return e_type;
}

/**
* @brief      ��ȡelf��ڵ�ַ
* @param	��
* @return      ��ڵ�ַ
**/
UINT32 ELF32::GetStartAddr()
{
	return e_entry;
}

/**
* @brief      ��ȡelf��С��
* @param	��
* @return      ��С��
**/
UINT32 ELF32::GetEndian()
{
	return e_ident[5];
}

/**
* @brief      ��ȡelf��ϵ�ṹ
* @param	��
* @return      ��С��
**/
elf_Machine ELF32::GetCpuType()
{
	return  e_machine;
}

/**
* @brief      ��ȡelf�ڱ����
* @param	��
* @return      ��С��
**/
INT16 ELF32::GetSectionCount()
{
	return e_shnum;
}

/**
* @brief      ��ȡelf�α����
* @param	��
* @return      ��С��
**/
INT16 ELF32::GetSegmentCount()
{
	return e_phnum;
}

/**
* @brief	�������ƻ�ȡ�������
* @param	��
* @return	successful: �������
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
* @brief	��ȡ���ű�
* @param	��
* @return	���ű�Object���������ֵΪNULL����ʾû�з��ű�
**/
SymbolTable* ELF32::GetSymTable()
{
	return m_symCtl;
}
