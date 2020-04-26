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
 *@file   symbol.cpp
 *@brief
 *<li>提供获取单一符号数据的操作接口</li>
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
* @brief    符号管理类构造函数
* @param[in]	无
* @return      无
**/
Symbol::Symbol(void)
{
	st_name = 0;	
	st_value = 0;		
	st_size = 0;		
	st_info = 0;		
	st_other = 0;		
	st_shndx = 0;	
}

/**
* @brief      符号管理类析构函数
* @param[in]	无
* @return      无
**/
Symbol::~Symbol(void)
{
}


/**
* @brief
*	 获取符号名称
* @param[out]	buf: 存储符号名称的buf指针。
* @return	无。
*/
BOOL Symbol::GetName(char* buf)
{
	const char* name = nameStr.c_str();
	strcpy(buf, name);
	return TRUE;
}

/**
* @brief
*	 获取符号值
* @param	无	
* @return	符号名称。
*/
INT32 Symbol::GetValue()
{
	return st_value;
}

/**
* @brief
*	 获取符号大小
* @param	无	
* @return	符号大小。
*/
UINT32 Symbol::GetSize()
{
	return st_size;
}

/**
* @brief
*	 获取符号属性
* @param	无	
* @return	符号属性。
*/
INT8 Symbol::GetInfo()
{
	return st_info;
}

/**
* @brief
*	 获取相关联的符号
* @param	无	
* @return	相关联的符号。
*/
INT32 Symbol::GetAssociatedSection()
{
	return st_shndx;
}