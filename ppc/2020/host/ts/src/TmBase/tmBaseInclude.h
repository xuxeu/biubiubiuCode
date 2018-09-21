/*
* 更改历史：
* 2008-1-9 tangxp  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  tmBaseInclude.h
* @brief
*       功能：
*       <li>用于包含tmBase中的常用宏定义</li>
*/

#ifndef _TMBASEINCLUDE_H
#define _TMBASEINCLUDE_H

/************************头文件********************************/

#include "util.h"

/************************宏定义********************************/




#define UC unsigned char
#define US unsigned short
#define UI unsigned int
#define UL unsigned long

#define DIDLEN sizeof(US)             //对话ID的字节长度

const char CONTROL_FLAG[] = ",";

#define CONTROL_FLAG_LEN strlen(CONTROL_FLAG)

/************************类型定义******************************/

/************************接口声明******************************/

#endif
