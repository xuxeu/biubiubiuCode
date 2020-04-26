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
*@file   Section.cpp
*@brief
<li>提供节区的数据操作接口</li>
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
* @brief      节表管理类构造函数
* @param[in]	无
* @return      无
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
* @brief      节表管理类析构函数
* @param[in]	无
* @return      无
**/
Section::~Section(void)
{
}


/**
* @brief	获取节区类型
* @param	无
* @return	节区类型
**/
UINT32 Section::GetType()
{
	return sh_type;
}

/**
* @brief	获取节区内容
* @param[in]	buf: 存放节区内容的数据空间
* @param[out]	len: buf容量
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
		printf("获取节表内容错误：存放内容的buf为空\n");
		return FALSE;
	}

	if (len == 0)
	{
		printf("获取节表内容错误：buf长度不能为0\n");
		return FALSE;
	}

	rc = fseek(m_fp, sh_offset, SEEK_SET);
	if (rc != 0)
	{
		printf("获取节表内容错误：读取elf文件不正确\n");
		return FALSE;
	}

	rc = (UINT32)fread(secContents, sh_size, 1, m_fp);
	if (rc != 1)
	{
		printf("获取节表内容错误：读取elf文件不正确\n");
		return FALSE;
	}

	sh_size > len ? realLen = len : realLen = sh_size;

	memcpy(buf, secContents, realLen);

	return realLen;
}

/**
* @brief	获取节区名称
* @param	无
* @return	节区名称
**/
BOOL Section::GetName(char* nameBuf)
{
	const char* secName = nameStr.c_str();

	strcpy(nameBuf, secName);
	
	return TRUE;
}

/**
* @brief	获取节区flags
* @param	无
* @return	节区flags
**/
UINT32 Section::GetFlags()
{
	return sh_flags;
}

/**
* @brief	获取节区在内存中的地址
* @param	无
* @return	节区在内存中的地址
**/
UINT32 Section::GetAddr()
{
	return sh_addr;
}

/**
* @brief	获取节区在文件中的偏移量
* @param	无
* @return	节区在文件中的偏移量
**/
UINT32 Section::GetOffset()
{
	return sh_offset;
}

/**
* @brief	获取节区大小
* @param	无
* @return	节区大小
**/
UINT32 Section::GetSize()
{
	return sh_size;
}

/**
* @brief	获取节区表的索引连接
* @param	无
* @return	节区表的索引连接
**/
UINT32 Section::GetLink()
{
	return sh_link;
}

/**
* @brief	获取节区额外信息
* @param	无
* @return	节区额外信息
**/
UINT32 Section::GetInfo()
{
	return sh_info;
}

/**
* @brief	获取节区对齐属性
* @param	无
* @return	节区对齐属性
**/
UINT32 Section::GetAlign()
{
	return sh_addralign;
}

/**
* @brief	获取节区存放的表的固定大小
* @param	无
* @return	节区存放的表的固定大小
**/
UINT32 Section::GetEntSize()
{
	return sh_entsize;
}
