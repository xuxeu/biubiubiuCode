/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * �޸���ʷ:
 * 2013-06-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbDRALib.S
 * @brief:
 *             <li>����X86Ӳ���ϵ�Ĵ�����ز���</li>
 */

/************************ͷ �� ��*****************************/

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*    DR7
+---+-+-+---+-+-+---+-+-+---+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|LEN|R|W|LEN|R|W|LEN|R|W|LEN|R|W|0|0|G|0|0|0|G|L|G|L|G|L|G|L|G|L|
| 3 |3|3| 2 |2|2| 1 |1|1| 0 |0|0| | |D| | | |E|e|3|3|2|2|1|1|0|0|
+---+-+-+---+-+-+---+-+-+---+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|31                           16|15                            0|
*/

/*
* @brief
 *   ��ȡָ��DR������
 *@return        
 *   DR������
 */
.text
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
 *    ����ָ��DR������ *
 * @param[in] content: ����
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