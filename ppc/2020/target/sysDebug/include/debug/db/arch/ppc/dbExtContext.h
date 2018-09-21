/*
 *变更历史: 
 * 2010-11-9  唐兴培       创建该文件。
 */


/**
 * @file  dbExtContext.h
 * @brief 
 *       功能: 
 *       <li>上下文定义</li>
 */


#ifndef DB_EXT_CONTEXT_H
#define DB_EXT_CONTEXT_H


/*头文件*/


#include "dbContext.h"

#ifdef __cplusplus
extern "C" {
#endif


/*宏定义*/

/* 断点指令 */
#define DB_ARCH_BREAK_INST        0x7d821008

/* 断点指令长度 */
#define DB_ARCH_BREAK_INST_LEN    4

/* 普通32位寄存器每个元素长度为4个字节 */
#define DB_REGISTER_UNIT_LEN   4

/* 将内存数据转换为字符流长度扩展2倍  */
#define DB_MEM_TO_CHAR_LEN     2  

 /* 普通64位寄存器每个元素长度为8个字节 */
#define DB_FLOAT_REGISTER_UNIT_LEN 8   

 /* MSR寄存器的SE位,用于单步位使能 */
#define DB_PPC_MSR_SE    0x400     

/* PPC MSR寄存器的IR位 */
#define DB_PPC_MSR_IR    0x20

 /* PPC MSR寄存器的DR位 */
#define DB_PPC_MSR_DR    0x10

/* 通用寄存器长度 */
#define DB_REGISTER_SIZE  (DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

/* 浮点寄存器长度 */
#define DB_FLOAT_REGISTER_SIZE  (DB_FLOAT_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN)

/* 状态寄存器中的单步位 */
#define DB_ARCH_PS_TRAP_BIT DB_PPC_MSR_SE

/*类型定义*/


/*接口声明*/


#ifdef __cplusplus
}
#endif

#endif/*DB_EXT_CONTEXT_H*/
