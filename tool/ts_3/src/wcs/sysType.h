/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  sysType.h
 * @Version        :  1.0.0
 * @Brief           :  系统自定义数据类型
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:28:07
 * @History        : 
 *   
 */

#ifndef _SYSTYPE_H_
#define _SYSTYPE_H_

/************************引用部分*****************************/

/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/

typedef unsigned char        UINT8;
typedef unsigned int        UINT32;
typedef	signed char						T_BYTE;     /*8-bit signed integer */
typedef  unsigned char					T_UBYTE;    /*8-bit unsigned integer */

typedef	signed short					T_HWORD;    /*16-bit signed integer */
typedef	unsigned short					T_UHWORD;   /*16-bit unsigned integer */

typedef	signed int						T_WORD;     /*32-bit signed integer */
typedef  unsigned int					T_UWORD;    /*32-bit unsigned integer */
typedef char                            T_CHAR;
#define T_VOID void

#define FAIL                          -1
#define SUCC                         1

const char SUCC_ACK[] = "OK";
const char FAIL_ACK[] = "E03";
const char CONTROL_FLAG[] = ",";
#define SUCC_ACK_LEN              2
#define FAIL_ACK_LEN                3
#define CONTROL_FLAG_LEN     1


/*the listen port*/
#define WCS_LIS_PORT             54321

#define BIG_EDIAN                    1
#define LITTLE_EDIAN                0


/*define the command char */
#define     RSP_CMD_LEN                              1

#define     CRT_SESSION                               'f'
#define     GET_ENDIAN                                 "i -e"

#define     RSP_QUERY_TARGET_STATE      'q'
#define     RSP_SET_ALL_REGISTER            'G'
#define     RSP_GET_ALL_REGISTER            'g'
#define     RSP_READ_MEMORY                    'x'
#define     RSP_WRITE_MEMORY                  'X'
#define     RSP_STOP_TARGET                      '!'
#define     RSP_RUN_TARGET                        'c'
#define     RSP_RESET_TARGET                     'R'
#define     RSP_SET_BREAKPOINT                'Z'
#define     RSP_DELETE_BREAKPOINT          'z'
#define     RSP_GET_TARGET_CPUCODE      'i'
#define     RSP_SET_CPP_REGISTER            'D'
#define     RSP_GET_CPP_REGISTER            'd'
#define     RSP_STEP_TARGET                       's'


#define     ARM_REG_NUM                              38

/*定义断点类型*/
#define     SOFT_BP                                         0
#define     HARD_BP                                         1
#define     WRITEWATCH_BP                           2
#define     READWATCH_BP                             3
#define     ACCESS_BP                                     4

/*定义ICE 中的目标机状态*/
#define     JTAG_CPU_NORMAL                         0
#define     JTAG_CPU_DEBUG_HANGUP           1
#define     JTAG_CPU_BKP_HANGUP                2
#define     JTAG_CPU_BKP_EXCEPTION           3
#define     JTAG_CPU_UNKONW_STATUS        4

///处理器的处理模式
#define ARM_USER_MODE                  16
#define ARM_FIQ_MODE                    17
#define ARM_IRQ_MODE                    18
#define ARM_SUPER_MODE               19
#define ARM_ABORT_MODE               23
#define ARM_UNDEFINED_MODE       27
#define ARM_SYSTEM_MODE             31


//ARM37个寄存器
const UINT32 JTAG_R0	= 0;
const UINT32 JTAG_R1	= 1;
const UINT32 JTAG_R2	= 2;
const UINT32 JTAG_R3	= 3;
const UINT32 JTAG_R4	= 4;
const UINT32 JTAG_R5	= 5;
const UINT32 JTAG_R6	= 6;
const UINT32 JTAG_R7	= 7;
const UINT32 JTAG_R8	= 8;
const UINT32 JTAG_R9	= 9;
const UINT32 JTAG_R10 = 10;
const UINT32 JTAG_R11 = 11;
const UINT32 JTAG_R12 = 12;
const UINT32 JTAG_R13 = 13;				/* use this for the stack pointer */
const UINT32 JTAG_R14 = 14;				/* use this for link pointer	*/
const UINT32 JTAG_R15 = 15;				/* use this for the program counter */
const UINT32 JTAG_FIQ_R8 = 16;
const UINT32 JTAG_FIQ_R9 = 17;
const UINT32 JTAG_FIQ_R10 = 18;
const UINT32 JTAG_FIQ_R11 = 19;
const UINT32 JTAG_FIQ_R12	= 20;
const UINT32 JTAG_FIQ_R13	=	21;
const UINT32 JTAG_FIQ_R14	=	22;
const UINT32 JTAG_IRQ_R13	=	23;
const UINT32 JTAG_IRQ_R14	=	24;
const UINT32 JTAG_ABT_R13	=	25;
const UINT32 JTAG_ABT_R14	=	26;
const UINT32 JTAG_SVC_R13	=	27;		/* same as user mode */
const UINT32 JTAG_SVC_R14	=	28;
const UINT32 JTAG_UND_R13	=	29;
const UINT32 JTAG_UND_R14	=	30;
const UINT32 JTAG_CPSR		=	31;
const UINT32 JTAG_FIQ_SPSR	=	32;
const UINT32 JTAG_IRQ_SPSR	=	33;
const UINT32 JTAG_ABT_SPSR	=	34;
const UINT32 JTAG_SVC_SPSR	=	35;
const UINT32 JTAG_UND_SPSR	=	36;

#endif /*_SYSTYPE_H_*/
