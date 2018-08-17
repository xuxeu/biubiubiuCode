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
 * @file:dbAtomic.S
 * @brief:
 *             <li>X86���ԭ�Ӳ����ӿ�</li>
 */
#ifdef CONFIG_CORE_SMP

#define ASM_USE

/************************ͷ �� ��*****************************/
#include "dbAsm.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/
.text

/*
* @brief 
*       ��targetָ���ֵ��oldValue�Աȣ������ȣ�����targetָ���ֵΪnewValue��
*		������TRUE���������ȣ���ֱ�ӷ���FALSE��
* @@param target: ��Ҫ�Ա�ֵ�ĵ�ַ��
* @param[in]oldValue: ��Ҫ�Աȵ�ֵ��
* @param[in]newValue: ��Ҫ���õ�ֵ��
* @return
*	TURE:targetָ���ֵ��oldValue���
*	FALSE:targetָ���ֵ��oldValue�����
*/
/* BOOL _atomic_32_cas(T_UWORD * target,T_UWORD oldValue,T_UWORD newValue) */
.align 
.global _atomic_32_cas
_atomic_32_cas:
    movl    SP_ARG1(esp),edx /* get address */
    movl    SP_ARG2(esp),eax /* get old_value to cmp */
    movl    SP_ARG3(esp),ecx /* get new_value to set */
    lock                       /* lock the Bus during the next inst */
    cmpxchg ecx,(edx)        /* if (eax == (edx) */
                               /*   {ZF = 1; (edx) = ecx;} */
                               /* else              */
                               /*   {ZF = 0; eax = (edx);} */
    jne     cas1
    movl    $1, eax           /* set status to TRUE */
    ret

cas1:
    movl    $0, eax           /* set status to FALSE */
    ret
	
/*
* @brief 
*       ��targetָ���ֵ����value��������targetԭ�е�ֵ��
* @param  target: ��Ҫ����ֵ�ĵ�ַ��
* @param[in]  value: ��Ҫ���ӵ�ֵ��
* @return
*	targetԭ��ָ���ֵ
*/
/* T_UWORD _atomic_32_add(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_add
_atomic_32_add:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  SP_ARG2(esp),eax  /*��ȡ��Ҫ���ӵ�ֵ */

	lock			                       /* lock the Bus during the next inst */
	xadd  eax,(edx)                /* TMP = eax + (edx) */
				                       /* eax = (edx) */
				                       /* (edx) = TMP */
	ret

/*
* @brief 
*       ��targetָ���ֵ��ȥvalue��������targetԭ�е�ֵ��
* @param  target: ��Ҫ��ȥֵ�ĵ�ַ��
* @param[in]  value: ��Ҫ��ȥ��ֵ��
* @return
*	targetԭ��ָ���ֵ
*/
/* T_UWORD _atomic_32_sub(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_sub
_atomic_32_sub:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  SP_ARG2(esp),eax  /*��ȡ��Ҫ���ӵ�ֵ */

	negl  eax		  /* Negate eax... */

	lock		          /* lock the Bus during the next inst */
	xadd  eax,(edx)         /* TMP = eax + (edx) */
				  /* eax = (edx) */
				  /* (edx) = TMP */
	ret
	
/*
* @brief 
*       ��targetָ���ֵ����1��������targetԭ�е�ֵ��
* @param  target: ��Ҫ����ֵ�ĵ�ַ��
* @return
*	targetԭ��ָ���ֵ
*/
/* T_UWORD _atomic_32_inc(T_UWORD * target) */
.align 
.global _atomic_32_inc
_atomic_32_inc:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  $0x1,eax           /* inc by 1 */

	lock                      /* lock the Bus during the next inst */
	xadd  eax,(edx)         /* TMP = eax + (edx) */
				  /* eax = (edx) */
				  /* (edx) = TMP */
	ret

/*
* @brief 
*       ��targetָ���ֵ��ȥ1��������targetԭ�е�ֵ��
* @param  target: ��Ҫ��ȥֵ�ĵ�ַ��
* @return
*	targetԭ��ָ���ֵ
*/
/* T_UWORD _atomic_32_dec(T_UWORD * target) */
.align 
.global _atomic_32_dec
_atomic_32_dec:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  $0x1,eax           /* dec by 1 */
	negl  eax		  /* Negate eax... */

	lock                      /* lock the Bus during the next inst */
	xadd  eax,(edx)         /* TMP = eax + (edx) */
				  /* eax = (edx) */
				  /* (edx) = TMP */
	ret

/*
* @brief 
*       ��ȡtargetָ���ֵ��
* @param  target: ��Ҫ��ȡֵ�ĵ�ַ��
* @return
*	��ȡtargetָ���ֵ
*/
/* T_UWORD _atomic_32_get(T_UWORD * target) */
.align 
.global _atomic_32_get
_atomic_32_get:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl (edx), eax
	ret

/*
* @brief 
*       ��targetָ���ֵ����Ϊvalue��������targetԭ�е�ֵ��
* @param  target: ��Ҫ����ֵ�ĵ�ַ��
* @param[in]  value: ��Ҫ���õ�ֵ��
* @return
*	targetԭ��ָ���ֵ
*/
/* T_UWORD _atomic_32_set(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_set
_atomic_32_set:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  SP_ARG2(esp),eax  /*��ȡ��Ҫ���õ�ֵ */

	/* lock */		  /* xchg always locks, lock not needed */
	xchg  eax,(edx)         /* set value with bus-lock */

	ret

/*
* @brief 
*       ���targetָ���ֵ��������targetԭ�е�ֵ��
* @param  target: ��Ҫ���ֵ�ĵ�ַ��
* @return
*	targetԭ��ָ���ֵ
*/
/* T_UWORD _atomic_32_clear(T_UWORD * target) */
.align 
.global _atomic_32_clear
_atomic_32_clear:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  $0x0,eax           /* 'clear' value to set */
	/* lock */                /* xchg always locks, lock not needed */
	xchg  eax,(edx)         /* swap 'clear' value with bus-lock */

	ret

/*
* @brief 
*       ��targetָ���ֵ��valueλ�򣬲�����targetԭ�е�ֵ��
* @param  target: ��Ҫλ��ֵ�ĵ�ַ��
* @param[in]  value: ��Ҫλ���ֵ��
* @return
*	targetԭ��ָ���ֵ
*/
/* T_UWORD _atomic_32_or(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_or
_atomic_32_or:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  (edx),eax         /* get old value */

atomicOr_retry:

	movl  SP_ARG2(esp),ecx  /* get value to set */
	orl   eax, ecx
	lock			  /* lock the Bus during the next inst */
	cmpxchg  ecx,(edx)      /* if (eax == (edx)) */
				  /*   {ZF = 1 ;  (edx) = ecx;} */
				  /* else                */
				  /*   {ZF = 0 ;  eax = (edx);} */
	jnz atomicOr_retry
	ret
	
/*
* @brief 
*       ��targetָ���ֵ��valueλ��򣬲�����targetԭ�е�ֵ��
* @param  target: ��Ҫλ���ֵ�ĵ�ַ��
* @param[in]  value: ��Ҫλ����ֵ��
* @return
*	targetԭ��ָ���ֵ
*/
/* T_UWORD _atomic_32_xor(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_xor
_atomic_32_xor:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  (edx),eax         /* get old value */

atomicXor_retry:

	movl  SP_ARG2(esp),ecx  /* get value to set */
	xorl  eax, ecx
	lock			  /* lock the Bus during the next inst */
	cmpxchg  ecx,(edx)      /* if (eax == (edx)) */
				  /*   {ZF = 1 ;  (edx) = ecx;} */
				  /* else                */
				  /*   {ZF = 0 ;  eax = (edx);} */
	jnz atomicXor_retry
	ret

/*
* @brief 
*       ��targetָ���ֵ��valueλ�룬������targetԭ�е�ֵ��
* @param  target: ��Ҫλ��ֵ�ĵ�ַ��
* @param[in]  value: ��Ҫλ���ֵ��
* @return
*	targetԭ��ָ���ֵ
* @implements  DC.32.1.4
*/
/* T_UWORD _atomic_32_and(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_and
_atomic_32_and:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  (edx),eax         /* get old value */

atomicAnd_retry:

	movl  SP_ARG2(esp),ecx  /* get value to set */
	andl  eax, ecx
	lock			  /* lock the Bus during the next inst */
	cmpxchg  ecx,(edx)      /* if (eax == (edx)) */
				  /*   {ZF = 1 ;  (edx) = ecx;} */
				  /* else                */
				  /*   {ZF = 0 ;  eax = (edx);} */
	jnz atomicAnd_retry
	ret

/*
* @brief 
*       ��targetָ���ֵ��valueλnand��������targetԭ�е�ֵ��
* @param  target: ��Ҫλnandֵ�ĵ�ַ��
* @param[in]  value: ��Ҫλnand��ֵ��
* @return
*	targetԭ��ָ���ֵ
*/
/* T_UWORD _atomic_32_nand(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_nand
_atomic_32_nand:
	movl  SP_ARG1(esp),edx  /* ��ȡ��ַ*/
	movl  (edx),eax         /* get old value */

atomicNand_retry:

	movl  SP_ARG2(esp),ecx  /* get value to nand with old value */

	andl  eax, ecx
    not   ecx

	lock                      /* lock the Bus during the next inst */
	cmpxchg  ecx,(edx)      /* if (eax == (edx)) */
				  /*   {ZF = 1 ;  (edx) = ecx;} */
				  /* else                */
				  /*   {ZF = 0 ;  eax = (edx);} */
	jnz atomicNand_retry
	ret
	
/*
* @brief 
*       ��targetָ���64λֵ����Ϊ64λvalue��
* @param  target: ��Ҫ����ֵ�ĵ�ַ��
* @param[in]  value: ��Ҫ���õ�ֵ��
* @return
*	��
*/
/* T_VOID _atomic_64_write(T_UDWORD * target,T_UDWORD value) */
.align 
.global _atomic_64_write
_atomic_64_write:
   pushl ebx
   pushl	edi
   movl  SP_ARG4(esp),edi
   movl  0(edi), ebx
   movl  4(edi), ecx
   movl  SP_ARG3(esp),edi

	/* current 64 bit value in location */
   movl  0(edi), eax
   movl  4(edi), edx

atomicQuadWrite_retry:
   lock
   cmpxchg8b   (edi) /* If (edi) == edx:eax write ecx:ebx value to (edi) */
   jnz        atomicQuadWrite_retry /* keep trying until success */
   popl        edi
   popl        ebx
   ret	
   
/*
* @brief 
*       ��valueָ���64λֵ����Ϊtargetָ���64λֵ��
* @param[in]   target: ��Ҫ��ȡֵ�ĵ�ַ��
* @param[in]  value: ��Ҫ����ֵ�ĵ�ַ��
* @return
*	��
*/
/* T_VOID _atomic_64_read(T_UDWORD * target,T_UDWORD * value) */
.align 
.global _atomic_64_read
_atomic_64_read:
   pushl ebx
   pushl edi
   movl  SP_ARG3(esp), edi

   movl  eax, ebx 
   movl  edx, ecx
   lock
   cmpxchg8b   (edi)
	
   movl  SP_ARG4(esp), edi
   movl  eax, 0(edi)
   movl  edx, 4(edi)
   popl  edi
   popl  ebx
   ret

/*
 * taTas - C-callable atomic test-and-set primitive
 *
 * This routine provides a C-callable interface to the test-and-set
 * instruction.  The LOCK-BTS instruction is executed on the specified
 * address.
 *
 * RETURNS:
 * TRUE if value had been not set, but is now,
 * FALSE if the value was set already.
 *
 * BOOL taTas (address)
 *     char *address             /@ address to be tested @/
 */
.balign 16,0x90
.global taTas
taTas:
	movl    SP_ARG1(esp),edx /* get address */
	xorl    eax,eax   /* set status to FALSE */
	lock    /* lock the Bus during the nexinst */
	bts     $0,(edx)          /* set MSB with bus-lock */
	jc      taTas1
	incl    eax               /* set status to TRUE */
taTas1:
	ret

#endif