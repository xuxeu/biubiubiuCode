/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * 修改历史:
 * 2013-06-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbDRALib.S
 * @brief:
 *             <li>定义X86硬件断点寄存器相关操作</li>
 */

/************************头 文 件*****************************/

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*    DR7
+---+-+-+---+-+-+---+-+-+---+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|LEN|R|W|LEN|R|W|LEN|R|W|LEN|R|W|0|0|G|0|0|0|G|L|G|L|G|L|G|L|G|L|
| 3 |3|3| 2 |2|2| 1 |1|1| 0 |0|0| | |D| | | |E|e|3|3|2|2|1|1|0|0|
+---+-+-+---+-+-+---+-+-+---+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|31                           16|15                            0|
*/

/*
* @brief
 *   读取指定DR的内容
 *@return        
 *   DR的内容
 */
 #ifdef CONFIG_TA_LP64
.text
.code64
.global taDebugGetDR0
taDebugGetDR0:
	push %rbp
	mov  %rsp,%rbp
	mov  %dr0,%rax
	leave
	ret

.text
.global taDebugGetDR1
taDebugGetDR1:
	push %rbp
	mov  %rsp,%rbp
	mov  %dr1,%rax
	leave
	ret
	
.text
.global taDebugGetDR2
taDebugGetDR2:
	push %rbp
	mov  %rsp,%rbp
	mov  %dr2,%rax
	leave
	ret

.text
.global taDebugGetDR3
taDebugGetDR3:
	push %rbp
	mov  %rsp,%rbp
	mov  %dr3,%rax
	leave
	ret
	
.text
.global taDebugGetDR4
taDebugGetDR4:
	push %rbp
	mov  %rsp,%rbp
	mov  %dr4,%rax
	leave
	ret
	
.text
.global taDebugGetDR5
taDebugGetDR5:
	push %rbp
	mov  %rsp,%rbp
	mov  %dr5,%rax
	leave
	ret
	
.text
.global taDebugGetDR6
taDebugGetDR6:
	push %rbp
	mov  %rsp,%rbp
	mov  %dr6,%rax
	leave
	ret
	
.text
.global taDebugGetDR7
taDebugGetDR7:
	push %rbp
	mov  %rsp,%rbp
	mov  %dr7,%rax
	leave
	ret

/*
* @brief
 *    设置指定DR的内容 *
 * @param[in] content: 内容
 * @return        
 *      none
 */
.text
.global taDebugSetDR0
taDebugSetDR0:
	push %rbp
	mov  %rsp,%rbp
	mov  %rdi,%dr0
	leave
	ret

.text
.global taDebugSetDR1
taDebugSetDR1:
	push %rbp
	mov  %rsp,%rbp
	mov  %rdi,%dr1
	leave
	ret

.text
.global taDebugSetDR2
taDebugSetDR2:
	push %rbp
	mov  %rsp,%rbp
	mov  %rdi,%dr2
	leave
	ret
	
.text
.global taDebugSetDR3
taDebugSetDR3:
	push %rbp
	mov  %rsp,%rbp
	mov  %rdi,%dr3
	leave
	ret
	
.text
.global taDebugSetDR4
taDebugSetDR4:
	push %rbp
	mov  %rsp,%rbp
	mov  %rdi,%dr4
	leave
	ret

.text
.global taDebugSetDR5
taDebugSetDR5:
	push %rbp
	mov  %rsp,%rbp
	mov  %rdi,%dr5
	leave
	ret

.text
.global taDebugSetDR6
taDebugSetDR6:
	push %rbp
	mov  %rsp,%rbp
	mov  %rdi,%dr6
	leave
	ret

.text
.global taDebugSetDR7
taDebugSetDR7:
	push %rbp
	mov  %rsp,%rbp
	mov  %rdi,%dr7
	leave
	ret

#else

.text
.code32
.global taDebugGetDR0
taDebugGetDR0:
	pushl %ebp
	movl  %esp,%ebp
	movl  %dr0,%eax
	leave
	ret

.text
.global taDebugGetDR1
taDebugGetDR1:
	pushl %ebp
	movl  %esp,%ebp
	movl  %dr1,%eax
	leave
	ret
	
.text
.global taDebugGetDR2
taDebugGetDR2:
	pushl %ebp
	movl  %esp,%ebp
	movl  %dr2,%eax
	leave
	ret

.text
.global taDebugGetDR3
taDebugGetDR3:
	pushl %ebp
	movl  %esp,%ebp
	movl  %dr3,%eax
	leave
	ret
	
.text
.global taDebugGetDR4
taDebugGetDR4:
	pushl %ebp
	movl  %esp,%ebp
	movl  %dr4,%eax
	leave
	ret
	
.text
.global taDebugGetDR5
taDebugGetDR5:
	pushl %ebp
	movl  %esp,%ebp
	movl  %dr5,%eax
	leave
	ret
	
.text
.global taDebugGetDR6
taDebugGetDR6:
	pushl %ebp
	movl  %esp,%ebp
	movl  %dr6,%eax
	leave
	ret
	
.text
.global taDebugGetDR7
taDebugGetDR7:
	pushl %ebp
	movl  %esp,%ebp
	movl  %dr7,%eax
	leave
	ret

/*
* @brief
 *    设置指定DR的内容 *
 * @param[in] content: 内容
 * @return        
 *      none
 */
.text
.global taDebugSetDR0
taDebugSetDR0:
	pushl %ebp
	movl  %esp,%ebp
	movl  8(%ebp),%eax
	movl  %eax,%dr0
	leave
	ret

.text
.global taDebugSetDR1
taDebugSetDR1:
	pushl %ebp
	movl  %esp,%ebp
	movl  8(%ebp),%eax
	movl  %eax,%dr1
	leave
	ret

.text
.global taDebugSetDR2
taDebugSetDR2:
	pushl %ebp
	movl  %esp,%ebp
	movl  8(%ebp),%eax
	movl  %eax,%dr2
	leave
	ret
	
.text
.global taDebugSetDR3
taDebugSetDR3:
	pushl %ebp
	movl  %esp,%ebp
	movl  8(%ebp),%eax
	movl  %eax,%dr3
	leave
	ret
	
.text
.global taDebugSetDR4
taDebugSetDR4:
	pushl %ebp
	movl  %esp,%ebp
	movl  8(%ebp),%eax
	movl  %eax,%dr4
	leave
	ret

.text
.global taDebugSetDR5
taDebugSetDR5:
	pushl %ebp
	movl  %esp,%ebp
	movl  8(%ebp),%eax
	movl  %eax,%dr5
	leave
	ret

.text
.global taDebugSetDR6
taDebugSetDR6:
	pushl %ebp
	movl  %esp,%ebp
	movl  8(%ebp),%eax
	movl  %eax,%dr6
	leave
	ret

.text
.global taDebugSetDR7
taDebugSetDR7:
	pushl %ebp
	movl  %esp,%ebp
	movl  8(%ebp),%eax
	movl  %eax,%dr7
	leave
	ret
#endif
