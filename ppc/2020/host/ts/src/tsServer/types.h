/************************************************************************
*				北京科银京成技术有限公司 版权所有
*    Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	types.h
 * @brief	基本类型定义
 * @author 	张宇旻
 * @date 	2007年12月17日
 * @History
 */

#define SUCC (0)
#define FAIL (-1)

#define TRUE (1)
#define FALSE (0)

typedef enum 
{
	LM_IMG_OS = 0,	  //OS映像
	LM_IMG_OS_APP,      //基于OS的应用映像
	LM_IMG_OS_DLIB,      //基于os的模块映像
	LM_IMG_MSL_MODULE,       //基于MSL的模块映像
} T_LM_IMG_TYPE;

#define LM_IMG_UDEFINED 0xffffffff

