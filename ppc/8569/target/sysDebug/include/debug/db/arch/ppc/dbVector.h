/*
* 变更历史：
* 2013-01-22  冯彪        将双斜杠的注释方式修改为斜杠星的注释方式。
* 2012-05-25  邱学强      针对静态代码检查结果对宏值增加括号。(BUG6540)
* 2012-04-21  邱学强      增加存放临时保存中断异常部分上下文的空间起始地址宏。(BUG6549)
* 2010-08-30  尹立孟      创建文件。
*/

/*
* @file 	vectorPpc.h
* @brief
*	   功能：
*      <li>定义ppcCPU相关的宏、数据结构、函数接口等</li>
*/

#ifndef _VECTORPPC_H
#define _VECTORPPC_H

#ifdef __cplusplus
 extern "C" {
#endif

/*宏定义*/
/*异常号和x86匹配*/
/* #define GENERAL_ZDIV_INT      0 */ /*整数除0异常*/
#define GENERAL_DB_INT          (1) /*调试单步异常*/
#define GENERAL_NMI_INT         (2) /*非屏蔽中断,ppc异常都是非屏蔽中断*/
#define GENERAL_TRAP_INT        (3) /* 软件断点异常，0700:  trap program exception */
/* #define GENERAL_OVERF_INT     4 */ /*运算溢出*/
/* #define GENERAL_BOUND_INT     5 */ /*Bound指令范围越界*/
#define GENERAL_ILLINS_INT 	    (6)/*非法操作码异常*/
/* #define GENERAL_DEV_INT 		 7 */ /*设备不可用异常*/
/* #define GENERAL_DF_INT 		 8 */ /*双故障异常*/
/* #define GENERAL_CSO_INT 		 9 */ /*协处理器断超出*/
/* #define GENERAL_TSS_INT 		 10 */ /*非法TSS(Task State Segment)异常*/
/* #define GENERAL_SEGMENT_INT	 11 */ /*段不存在异常*/
/* #define GENERAL_SF_INT 		 12 */ /*栈故障异常*/
/* #define GENERAL_GP_INT 		 13 */ /*一般保护异常*/
#define GENERAL_PAGE_INT  		(14)/*页故障异常*/
#define GENERAL_IPAGE_INT  		(15)/*获取指令造成的页故障异常*/
#define GENERAL_MF_INT			(16)/*浮点异常 0700:  IEEE floating-point enabled program exception*/
#define GENERAL_AE_INT          (17)/*对齐检验异常 0600:  ALIGNMENT错误 */
#define GENERAL_MC_INT          (18)/*Machine Check异常 0200:  机器检查*/
#define GENERAL_SIMD_INT 		(19)/*SIMD 浮点异常 */

/* ppc产生的异常 */
#define GENERAL_RST_INT         (20) /* 0100:  系统复位*/
#define GENERAL_DAE_INT         (21) /* 0300:  DSI异常，对于PPC750更新HASH表是特殊处理。 */
#define GENERAL_PRIVI_INT       (22) /* 0700:  privileged instruction program exception */
#define GENERAL_IAE_INT         (23) /* 0400:  ISI异常，对于PPC750更新HASH表是特殊处理。 */
#define GENERAL_UF_INT          (24) /* 0800:  浮点不可用异常 */
#define GENERAL_DEC_INT         (25) /* 0900:  DEC中断，DEC一般用于计时，系统对于该中断直接返回。 */
#define GENERAL_PM_INT          (26) /* 0F00:  性能监控 */
#define GENERAL_HBP_INT         (27) /* 0300:  硬件数据断点异常 */

/*个别ppc会产生的异常*/
#define GENERAL_ITM_INT        (28) /* 1000:  指令转换丢失，对于PPC603e更新TLB表是特殊处理。*/
#define GENERAL_DLTM_INT       (29) /* 1100:  数据加载转化丢失，对于PPC603e更新TLB表是特殊处理。*/
#define GENERAL_DSTM_INT       (30) /* 1200:  数据存储转换丢失   ，对于PPC603e更新TLB表是特殊处理。*/
#define GENERAL_IAB_INT        (31) /* 1300:  硬件指令断点异常 */

#define GENERAL_SM_INT         (0x83) /* 1400:  系统管理中断 */
#define GENERAL_TH_INT         (0x84) /* 1700:  温度管理中断 */
/*个别ppc会产生的异常*/

#define GENERAL_SC_INT          (0x80) /* 0C00:  系统调用中断*/
#define GENERAL_CALLLIB_INT		(0x81) /* 0C00:  CALLLIB调用中断*/
#define GENERAL_CALLLIB_ERROR_INT		(0x82) /* 0C00:  CALLLIB调用失败中断*/
#define GENERAL_ILLSC_INT       (0x85)    /* 0C00:  系统态下非正常的系统调用中断*/

#define GENERAL_STD_IRQ_LAST   (32) /* 通用异常号的上限和x86统一，个别ppc产生异常不算在内 */

/*使用此地址保存MSL映像和OS映像都需要使用的异常临时栈空间*/
#define EXP_TMP_STACK_BASE (0xBA0)

/**************PPC异常定义*********************************/
 //#define Reserved 0x00000
 #define EXC_SYSTEM_RESET 						0x00100
 #define EXC_MACHINE_CHECK 						0x00200
 #define EXC_DSI 								0x00300
 #define EXC_ISI 								0x00400
 #define EXC_EXTERNAL_INTERRUPT 				0x00500
 #define EXC_ALIGNMENT 							0x00600
 #define EXC_PROGRAM 							0x00700
 #define EXC_FLOATING-POINT_UNAVAILABLE 		0x00800
 #define EXC_DECREMENTER 						0x00900
 //#define RESERVED 0x00A00–00BFF
 #define EXC_SYSTEM_CALL 						0x00C00
 #define EXC_TRACE 								0x00D00
 #define EXC_PERFORMANCE_MONITOR 				0x00F00
 #define EXC_ALTIVEC_UNAVAILABLE 				0x00F20
 #define EXC_ITLB_MISS 							0x01000
 #define EXC_DTLB_MISS_ON_LOAD 					0x01100
 #define EXC_DTLB_MISS_ON_STORE 				0x01200
 #define EXC_INSTRUCTION_ADDRESS_BREAKPOINT 	0x01300
 #define EXC_SYSTEM_MANAGEMENT_INTERRUPT 		0x01400
 #define EXC_ALTIVEC_ASSIST 					0x01600
/*
  * 因为EContext_Control结构中的成员fpscr为double型，所以结构大小必须为8字节对齐
  */
#define EXCEPTION_FRAME_SIZE  (0xB0)
#ifdef __cplusplus
}
#endif


#endif


