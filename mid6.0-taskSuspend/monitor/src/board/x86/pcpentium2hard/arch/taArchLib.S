/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taArchLib.c
 * @brief:
 *             <li>获取IDT表基地址</li>
 */

/************************头 文 件******************************/
#define ASM_USE

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
* @brief
*       获取IDT表基地址
* @param[out]: table: idt表基地址
* @param[out]: limit: idt表大小
* @return
*       none
*/
.text
.p2align  4
.global taGetIdtBase
taGetIdtBase:
	movl	4(%esp), %ecx	    /* table地址*/
	movl	8(%esp), %edx	    /* limit地址*/
	subl	$6, %esp	        /* 空出空间为48位的IDTR */
	sidt	(%esp)	            /* 获取IDT寄存器内容*/
	movl	2(%esp), %eax	    /* IDT表基地址*/
	movl	%eax, (%ecx)
	movzwl	(%esp), %eax	    /* IDT表大小*/
	movl	%eax, (%edx)
	addl	$6, %esp		   
	ret
    
/*
* @brief
*       通用异常处理程序
* @return
*       none
*/	
.text
.p2align  4
.global taGeneralExpAsmHandler
taGeneralExpAsmHandler:
   	cli
	pushal  /* 寄存所有通用寄存器，依次把eax->ecx->edx->ebx->esp->ebp->esi->edi压入栈中 */

    movl    %esp, %eax
    pushl   %eax 
    
    call    taExceptionPrintk /* C代码异常处理函数 */
        
    addl	$0x4,%esp  /* 栈指针加4，因为调用taExceptionPrintk函数时压栈了1个参数 */
    popal			   /* 恢复所有通用寄存器 */

    addl    $8,%esp     /* 跳过错误号和vector */
	iret		        /* 异常返回 */