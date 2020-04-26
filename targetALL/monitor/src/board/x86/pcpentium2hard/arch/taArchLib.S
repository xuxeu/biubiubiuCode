/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taArchLib.c
 * @brief:
 *             <li>��ȡIDT������ַ</li>
 */

/************************ͷ �� ��******************************/
#define ASM_USE

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
* @brief
*       ��ȡIDT������ַ
* @param[out]: table: idt������ַ
* @param[out]: limit: idt����С
* @return
*       none
*/
.text
.p2align  4
.global taGetIdtBase
taGetIdtBase:
	movl	4(%esp), %ecx	    /* table��ַ*/
	movl	8(%esp), %edx	    /* limit��ַ*/
	subl	$6, %esp	        /* �ճ��ռ�Ϊ48λ��IDTR */
	sidt	(%esp)	            /* ��ȡIDT�Ĵ�������*/
	movl	2(%esp), %eax	    /* IDT������ַ*/
	movl	%eax, (%ecx)
	movzwl	(%esp), %eax	    /* IDT����С*/
	movl	%eax, (%edx)
	addl	$6, %esp		   
	ret
    
/*
* @brief
*       ͨ���쳣��������
* @return
*       none
*/	
.text
.p2align  4
.global taGeneralExpAsmHandler
taGeneralExpAsmHandler:
   	cli
	pushal  /* �Ĵ�����ͨ�üĴ��������ΰ�eax->ecx->edx->ebx->esp->ebp->esi->ediѹ��ջ�� */

    movl    %esp, %eax
    pushl   %eax 
    
    call    taExceptionPrintk /* C�����쳣�������� */
        
    addl	$0x4,%esp  /* ջָ���4����Ϊ����taExceptionPrintk����ʱѹջ��1������ */
    popal			   /* �ָ�����ͨ�üĴ��� */

    addl    $8,%esp     /* ��������ź�vector */
	iret		        /* �쳣���� */