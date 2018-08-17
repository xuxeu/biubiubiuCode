/* $id: crt1.S V3.0 2002/11/21 */
/*****************************************************************************
 *    This source code has been made available to you by CoreTek on an AS-IS
 *    basis. Anyone receiving this source is licensed under CoreTek
 *    copyrights to use it in any way he or she deems fit, including
 *    copying it, modifying it, compiling it, and redistributing it either
 *    with or without modifications.
 *
 *    Any person who transfers this source code or any derivative work
 *    must include the CoreTek copyright notice, this paragraph, and the
 *    preceding two paragraphs in the transferred software.
 *
 *    COPYRIGHT   CoreTek  CORPORATION 2001
 *    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
 *****************************************************************************/

/*****************************************************************************
 * FILE: crt1.S
 * MODULE: Initializing code for PC386 machine.
 * PURPOSE: Build gdt and idt ,and enter protect mode.
 *
 * AUTHOR(S): Lemon
 * GROUP: System Department
 * DATE CREATED: 2002/11/21
 * REFERENCE DOCUMENT ID: no.
 * MODIFICATIONS:
 *    Date          userName        Description
 *    2002/11/21    Lemon           Create this file
 *****************************************************************************/

#define ASM_USE
#include "dbAsm.h"

.macro gdt_desc dlim_0_15, dbas_0_15, dbas_16_23, daccess, dgran, dbas_24_31
 .word \dlim_0_15
 .word \dbas_0_15
 .byte \dbas_16_23
 .byte \daccess
 .byte \dgran
 .byte \dbas_24_31
.endm

.macro idt_desc dIDT_off_0_15, dIDT_selector, dIDT_access, dIDT_off_16_31
.word \dIDT_off_0_15
.word \dIDT_selector
.word \dIDT_access
.word \dIDT_off_16_31
.endm

BEGIN_PROGRAM

BEGIN_BSS
PUBLIC(_idt_base)
_idt_base:
	.rept  IDT_ENTRYS
	.word 0,0,0,0
	.endr
END_BSS

BEGIN_DATA
PUBLIC(_gdt_base)
_gdt_base:

/* define the gdt table,at least 6 entry exist*/

gdt_desc 0,0,0,0,0,0                    /* the empty entry */
gdt_desc 0,0,0,0,0,0                    /* the gdt self entry */
gdt_desc 0,0,0,0,0,0                    /* the idt entry */
gdt_desc 0xffff,0,0x0,0x9a,0xCF,0x00    /* the flat code entry */
gdt_desc 0xffff,0,0x0,0x92,0xCF,0x00    /* the flat data entry */
gdt_desc 0xffff,0,0x0,0xfa,0xCF,0x00    /* the user code entry */
gdt_desc 0xffff,0,0x0,0xf2,0xCF,0x00    /* the user data entry */
gdt_desc 0xffff,0,0x0,0x89,0x8f,0x00    /* the task status entry */
gdt_desc 0,0,0,0,0,0                    /* the more entry */

.rept GDT_ENTRYS - 9
gdt_desc 0,0,0,0,0,0
.endr

_gdt_register:
.word GDT_SIZE
.int _gdt_base

_idt_register:
.word IDT_SIZE
.int _idt_base

END_DATA

BEGIN_CODE

PUBLIC(_start)
EXTERN(boot_card)
EXTERN(_stack_top)
EXTERN(_bss_end)
EXTERN(_bss_start)

_start:

  	nop
  	cli
  	cld
  	
	/*πÿ“≥”≥…‰*/
	mov   cr0,eax
	andl  $0x7fffffff,eax
    mov   eax,cr0
        
    /*πÿcache*/
    /*mov   cr0,eax
    orl  $0x40000000,eax
    mov   eax,cr0
    wbinvd*/
  	
  	lgdt _gdt_register
    smsw ax
  	orb  $0x01,al
  	andw $0xff01,ax
  	lmsw ax
  	jmp  FLUSH
 FLUSH:
  	nop
  	nop 
  
  	ljmpl $0x18,$PROTECT_MODE /* flat code segment is 0x18 */
  
 PROTECT_MODE:
  	movw $0x20,ax  /* make all data segment be 0x20 */
  	movw ax,ss
  	movw ax,ds
  	movw ax,es
  	movw ax,fs
  	movw ax,gs
  
  	movl   $_stack_top,esp /*establish the initial stack */
  	movl   esp,ebp
 
  	lidt _idt_register /* install idt table */
 	
 	/* clear eflag */
  	xorl   eax,eax
  	pushl  eax
  	popfl
  
  	call  sys_init /* never return in normal state */
  	nop
  	nop
  	nop
  	nop
  	cli
  	hlt
END_CODE
END_PROGRAM

