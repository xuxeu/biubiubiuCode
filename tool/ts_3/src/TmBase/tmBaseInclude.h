/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  tmBaseInclude.h
 * @Version        :  1.0.0
 * @Brief           :  用于包含tmBase中的常用宏定义
 *   
 * @Author        :  tangxp
 * @CreatedDate:  2008年1月9日
 * @History        : 
 *   
 */
#ifndef _TMBASEINCLUDE_H
#define _TMBASEINCLUDE_H

/******************************* 引用部分 ************************************/

#include "util.h"

/******************************* 声明部分 ************************************/


/******************************* 定义部分 ************************************/

#define UC unsigned char
#define US unsigned short
#define UI unsigned int
#define UL unsigned long

#define DIDLEN sizeof(US)             //对话ID的字节长度

const char CONTROL_FLAG[] = ",";

#define CONTROL_FLAG_LEN strlen(CONTROL_FLAG)

#endif
