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
*@file   Section.cpp
*@brief
<li>�ṩ���������ݲ����ӿ�</li>
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
/**
* @brief      �ڱ�����๹�캯��
* @param[in]	��
* @return      ��
**/
Section::Section(void)
{
	sh_name = 0;
	sh_type = 0;
	sh_flags = 0;
	sh_addr = 0;
	sh_offset = 0;
	sh_size = 0;
	sh_link = 0;
	sh_info = 0;
	sh_addralign = 0;
	sh_entsize = 0;
	m_fp = NULL;
}

/**
* @brief      �ڱ��������������
* @param[in]	��
* @return      ��
**/
Section::~Section(void)
{
}


/**
* @brief	��ȡ��������
* @param	��
* @return	��������
**/
UINT32 Section::GetType()
{
	return sh_type;
}

/**
* @brief	��ȡ��������
* @param[in]	buf: ��Ž������ݵ����ݿռ�
* @param[out]	len: buf����
* @return	
**/
INT32 Section::GetContents(INT8* buf, UINT32 len)
{
	UINT32 rc = 0;
	INT8* secContents = NULL;
	INT8 realLen = 0;
	char* str = NULL;


	if (buf == NULL)
	{
		printf("��ȡ�ڱ����ݴ��󣺴�����ݵ�bufΪ��\n");
		return FALSE;
	}

	if (len == 0)
	{
		printf("��ȡ�ڱ����ݴ���buf���Ȳ���Ϊ0\n");
		return FALSE;
	}

	rc = fseek(m_fp, sh_offset, SEEK_SET);
	if (rc != 0)
	{
		printf("��ȡ�ڱ����ݴ��󣺶�ȡelf�ļ�����ȷ\n");
		return FALSE;
	}

	rc = (UINT32)fread(secContents, sh_size, 1, m_fp);
	if (rc != 1)
	{
		printf("��ȡ�ڱ����ݴ��󣺶�ȡelf�ļ�����ȷ\n");
		return FALSE;
	}

	sh_size > len ? realLen = len : realLen = sh_size;

	memcpy(buf, secContents, realLen);

	return realLen;
}

/**
* @brief	��ȡ��������
* @param	��
* @return	��������
**/
BOOL Section::GetName(char* nameBuf)
{
	const char* secName = nameStr.c_str();

	strcpy(nameBuf, secName);
	
	return TRUE;
}

/**
* @brief	��ȡ����flags
* @param	��
* @return	����flags
**/
UINT32 Section::GetFlags()
{
	return sh_flags;
}

/**
* @brief	��ȡ�������ڴ��еĵ�ַ
* @param	��
* @return	�������ڴ��еĵ�ַ
**/
UINT32 Section::GetAddr()
{
	return sh_addr;
}

/**
* @brief	��ȡ�������ļ��е�ƫ����
* @param	��
* @return	�������ļ��е�ƫ����
**/
UINT32 Section::GetOffset()
{
	return sh_offset;
}

/**
* @brief	��ȡ������С
* @param	��
* @return	������С
**/
UINT32 Section::GetSize()
{
	return sh_size;
}

/**
* @brief	��ȡ���������������
* @param	��
* @return	���������������
**/
UINT32 Section::GetLink()
{
	return sh_link;
}

/**
* @brief	��ȡ����������Ϣ
* @param	��
* @return	����������Ϣ
**/
UINT32 Section::GetInfo()
{
	return sh_info;
}

/**
* @brief	��ȡ������������
* @param	��
* @return	������������
**/
UINT32 Section::GetAlign()
{
	return sh_addralign;
}

/**
* @brief	��ȡ������ŵı�Ĺ̶���С
* @param	��
* @return	������ŵı�Ĺ̶���С
**/
UINT32 Section::GetEntSize()
{
	return sh_entsize;
}
