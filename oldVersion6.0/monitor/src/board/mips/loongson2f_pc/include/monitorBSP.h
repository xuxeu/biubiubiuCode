#/************************************************************************
*               北京科银京成技术有限公司 版权所有
*    Copyright (C)  2011 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * 修改历史:
 * 2011-09-23 胡雪雷，北京科银京成技术有限公司
 *                  创建该文件。
 */

/*
* @file: monitorBSP.h
* @brief:
*       <li>关于Monitor的操作</li>
* @implements: DTA
*/

#ifndef _MONITORBSP_H_
#define _MONITORBSP_H_

#include <taTypes.h>
/************************头 文 件******************************/

/************************宏 定 义******************************/

/************************类型定义******************************/
/************************外部声明******************************/

/************************前向声明******************************/

#define MONITOR_GLOBALINT_DISABLE(level) \
do\
{\
	__asm__ __volatile__("mfc0 $8, $12\n\t" \
											 "move %0, $8\n\t" \
											 "ori $8, $8, 0x1\n\t" \
											 "xori $8, $8, 0x1\n\t" \
											 "mtc0 $8, $12\n\t" \
												::"r"(level));\
}while(0)

#define MONITOR_GLOBALINT_ENABLE(level) \
do\
{\
	__asm__ __volatile__("mtc0 %0, $12"::"r"(level));\
}while(0)

#define MONITOR_LOONGSON2F_CACHE_SIZE 32

#endif
