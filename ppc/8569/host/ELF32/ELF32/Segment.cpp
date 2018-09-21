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
 *@file    Segment.cpp
 *@brief
 *<li>�ṩ�ε����ݲ����ӿ�</li>
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
* @brief      �α�����๹�캯��
* @param[in]	��
* @return      ��
**/
Segment::Segment(void)
{
	p_type = 0;
	p_offset = 0;
	p_vaddr = 0;
	p_paddr = 0;
	p_filesz = 0;
	p_memsz = 0;
	p_flags = 0;
	p_align = 0;	
	m_fp = NULL;
}

/**
* @brief      �α�����๹�캯��
* @param[in]	��
* @return      ��
**/
Segment::~Segment(void)
{
}

/**
* @brief     ��ȡ������
* @param	 ��
* @return    ������
**/
INT32 Segment::GetType()
{
	return p_type;
}

/**
* @brief    ��ȡ����elf�ļ��е�ƫ��λ��
* @param	��
* @return   ��ƫ��λ��
**/
UINT32 Segment::GetOffset()
{
	return p_offset;
}

/**
* @brief	��ȡ�ö����ڴ��е����ֽڵ�ַ
* @param	��
* @return   �ö����ڴ��е����ֽڵ�ַ
**/
UINT32 Segment::GetVirtualAddr()
{
	return p_vaddr;
}

/**
* @brief	��ȡ�ε������ַ��λ
* @param	��
* @return	�ε������ַ��λ
**/
UINT32 Segment::GetPhysicalAddr()
{
	return p_paddr;
}

/**
* @brief	��ȡ������
* @param[out]	buf: ��Ŷ����ݵ����ݿռ�
* @param[in]	len: buf����
* @return	������
**/
BOOL Segment::GetContents(INT8* buf, UINT32 len)
{
	UINT32 rc = 0;
	INT8* segContents = buf;
	INT8 realLen = 0;
	char* str = NULL;


	if (buf == NULL)
	{
		printf("��ȡ�����ݴ��󣺴�����ݵ�bufΪ��\n");
		return FALSE;
	}

	if (len == 0)
	{
		printf("��ȡ�����ݴ���buf���Ȳ���Ϊ0\n");
		return FALSE;
	}

	rc = fseek(m_fp, p_offset, SEEK_SET);
	if (rc != 0)
	{
		printf("��ȡ�����ݴ��󣺶�ȡelf�ļ�����ȷ\n");
		return FALSE;
	}

	rc = (UINT32)fread(segContents, p_filesz, 1, m_fp);
	if (rc != 1)
	{
		printf("��ȡ�����ݴ��󣺶�ȡelf�ļ�����ȷ\n");
		return FALSE;
	}

//	p_filesz > len ? realLen = len : realLen = p_filesz;

//	memcpy(buf, segContents, realLen);


	return realLen;
}

/**
* @brief   ��ȡ���ļ��ռ�
* @param	��
* @return ���ļ��ռ�
**/
UINT32 Segment::GetFileSize()
{
	return p_filesz;
}

/**
* @brief	��ȡ���ڴ�ռ��С
* @param	��
* @return	���ڴ�ռ��С
**/
UINT32 Segment::GetMemSize()
{
	return p_memsz;
}

/**
* @brief	��ȡ�ζ����ֽ�
* @param	��
* @return	�ζ����ֽ�
**/
UINT32 Segment::GetAlign()
{
	return p_align;
}

/**
* @brief	�ж�
* @param	��
* @return	�ö�����
**/
INT32 Segment::GetFlags()
{
	return p_flags;
}
