#/************************************************************************
*               �����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C)  2011 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/*
 * �޸���ʷ:
 * 2011-09-23 ��ѩ�ף������������ɼ������޹�˾
 *                  �������ļ���
 */

/*
* @file: monitorBSP.h
* @brief:
*       <li>����Monitor�Ĳ���</li>
* @implements: DTA
*/

#ifndef _MONITORBSP_H_
#define _MONITORBSP_H_

#include <taTypes.h>
/************************ͷ �� ��******************************/

/************************�� �� ��******************************/

/************************���Ͷ���******************************/
/************************�ⲿ����******************************/

/************************ǰ������******************************/

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
