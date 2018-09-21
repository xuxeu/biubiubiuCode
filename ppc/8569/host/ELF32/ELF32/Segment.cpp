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
 *@file    Segment.cpp
 *@brief
 *<li>提供段的数据操作接口</li>
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
/**
* @brief      段表管理类构造函数
* @param[in]	无
* @return      无
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
* @brief      段表管理类构造函数
* @param[in]	无
* @return      无
**/
Segment::~Segment(void)
{
}

/**
* @brief     获取段类型
* @param	 无
* @return    段类型
**/
INT32 Segment::GetType()
{
	return p_type;
}

/**
* @brief    获取段在elf文件中的偏移位置
* @param	无
* @return   段偏移位置
**/
UINT32 Segment::GetOffset()
{
	return p_offset;
}

/**
* @brief	获取该段在内存中的首字节地址
* @param	无
* @return   该段在内存中的首字节地址
**/
UINT32 Segment::GetVirtualAddr()
{
	return p_vaddr;
}

/**
* @brief	获取段的物理地址定位
* @param	无
* @return	段的物理地址定位
**/
UINT32 Segment::GetPhysicalAddr()
{
	return p_paddr;
}

/**
* @brief	获取段内容
* @param[out]	buf: 存放段内容的数据空间
* @param[in]	len: buf容量
* @return	段内容
**/
BOOL Segment::GetContents(INT8* buf, UINT32 len)
{
	UINT32 rc = 0;
	INT8* segContents = buf;
	INT8 realLen = 0;
	char* str = NULL;


	if (buf == NULL)
	{
		printf("获取段内容错误：存放内容的buf为空\n");
		return FALSE;
	}

	if (len == 0)
	{
		printf("获取段内容错误：buf长度不能为0\n");
		return FALSE;
	}

	rc = fseek(m_fp, p_offset, SEEK_SET);
	if (rc != 0)
	{
		printf("获取段内容错误：读取elf文件不正确\n");
		return FALSE;
	}

	rc = (UINT32)fread(segContents, p_filesz, 1, m_fp);
	if (rc != 1)
	{
		printf("获取段内容错误：读取elf文件不正确\n");
		return FALSE;
	}

//	p_filesz > len ? realLen = len : realLen = p_filesz;

//	memcpy(buf, segContents, realLen);


	return realLen;
}

/**
* @brief   获取段文件空间
* @param	无
* @return 段文件空间
**/
UINT32 Segment::GetFileSize()
{
	return p_filesz;
}

/**
* @brief	获取段内存空间大小
* @param	无
* @return	段内存空间大小
**/
UINT32 Segment::GetMemSize()
{
	return p_memsz;
}

/**
* @brief	获取段对齐字节
* @param	无
* @return	段对齐字节
**/
UINT32 Segment::GetAlign()
{
	return p_align;
}

/**
* @brief	判断
* @param	无
* @return	该段属性
**/
INT32 Segment::GetFlags()
{
	return p_flags;
}
