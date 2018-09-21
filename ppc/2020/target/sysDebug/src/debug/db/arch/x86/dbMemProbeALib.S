/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * �޸���ʷ:
 * 2013-11-07         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbMemProbeALib.S
 * @brief:
 *             <li>�ڴ�̽�������֧��</li>
 */

/************************ͷ �� ��*****************************/
#define ASM_USE

#include <dbAsm.h>
/************************�� �� ��******************************/

/************************���Ͷ���*****************************/
#define ARG1	8
#define ARG2	12
#define ARG3	16

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/
.globl GTEXT(taMemProbeSup)
.globl GTEXT(taMemProbeByteRead)
.globl GTEXT(taMemProbeByteWrite)
.globl GTEXT(taMemProbeWordRead)
.globl GTEXT(taMemProbeWordWrite)
.globl GTEXT(taMemProbeLongRead)
.globl GTEXT(taMemProbeLongWrite)
.globl GTEXT(taMemProbeErrorRtn)

/************************����ʵ��*****************************/

.text
.balign 16

/*
 * �ú�����ͼ��д�ڴ棬���һ���쳣�����ڴ�̽���쳣��������taMemProbeTrapHandle��
 * ��PC����ΪtaMemProbeErrorRtn��ʹ��taMemProbeSup��������һ������ţ���ʾ�ڴ��д
 * ʧ�ܣ����ڴ��ַ�޷���д������ԭ�����£�
 * STATUS taMemProbeSup(length, src, dest)
 *     int length: length of cell to test (1, 2, 4)
 *     char *src : address to read
 *     char *dest: address to write
 */
FUNC_LABEL(taMemProbeSup)
	pushl	ebp
	movl	esp,ebp

	pushl	esi  /* �������ʧ�Ĵ��� */
	pushl	edi

	movl	ARG2(ebp),esi	/* ��ȡsrc���� */
	movl	ARG3(ebp),edi	/* ��ȡdest���� */

	xorl	eax,eax	        /* ���õ�ǰ״̬ΪOK */

	movl	ARG1(ebp),edx   /* ��ȡlength���� */

	cmpl	$1,edx
    je      taMemProbeByteRead
	cmpl	$2,edx
    je      taMemProbeWordRead
	cmpl	$4,edx
    je      taMemProbeLongRead

	jmp     taMemProbeErrorRtn

	.balign 16,0x90
FUNC_LABEL(taMemProbeByteRead)
	movb	(esi),cl
FUNC_LABEL(taMemProbeByteWrite)
	movb	cl,(edi)

	jmp	tapRtn

	.balign 16,0x90
FUNC_LABEL(taMemProbeWordRead)
	movw	(esi),cx
FUNC_LABEL(taMemProbeWordWrite)
	movw	cx,(edi)

	jmp	tapRtn

	.balign 16,0x90
FUNC_LABEL(taMemProbeLongRead)
	movl	(esi),ecx
FUNC_LABEL(taMemProbeLongWrite)
	movl	ecx,(edi)

	jmp	tapRtn

	.balign 16,0x90
FUNC_LABEL(taMemProbeErrorRtn)
	movl	$-1,eax	 /* ���ô����ΪERROR */

tapRtn:
	popl	edi	  /* �ָ�����ʧ�Ĵ��� */
	popl	esi

    leave
	ret