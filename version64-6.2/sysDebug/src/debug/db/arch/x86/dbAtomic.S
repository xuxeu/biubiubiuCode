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
 * @file:dbAtomic.S
 * @brief:
 *             <li>X86相关原子操作接口</li>
 */
#ifdef CONFIG_CORE_SMP

#define ASM_USE

/************************头 文 件*****************************/
#include "dbAsm.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/
#ifdef CONFIG_TA_LP64

.text
.code64
/*
* @brief 
*       将target指向的值与oldValue对比，如果相等，设置target指向的值为newValue，
*		并返回TRUE；如果不相等，则直接返回FALSE。
* @@param target: 需要对比值的地址。
* @param[in]oldValue: 需要对比的值。
* @param[in]newValue: 需要设置的值。
* @return
*	TURE:target指向的值与oldValue相等
*	FALSE:target指向的值与oldValue不相等
*/
/* BOOL _atomic_32_cas(T_UWORD * target,T_UWORD oldValue,T_UWORD newValue) */

.balign 16,0x90
.global _atomic_64_cas
_atomic_64_cas:
	movq     %rsi, %rax          /* get old_value to cmp */
	lock                         /* lock the Bus during the next inst */
	cmpxchgq %rdx,(%rdi)         /* if (%rax == (%rdi) */
                                 /*   {ZF = 1; (%rdi) = %rdx;} */
                                 /* else              */
                                 /*   {ZF = 0; %rax = (%rdi);} */
	setz     al                 /* if success, set swapped to TRUE else FALSE */
	movzb    al, %rax           /* zero extend swapped */
    ret


/*
* @brief 
*       获取target指向的值。
* @param  target: 需要获取值的地址。
* @return
*	获取target指向的值
*/
/* T_UWORD _atomic_32_get(T_UWORD * target) */
.balign 16,0x90 
.global _atomic_64_get
_atomic_64_get:
		movq (%rdi), %rax
		ret


/*
* @brief 
*       将target指向的值设置为value，并返回target原有的值。
* @param  target: 需要设置值的地址。
* @param[in]  value: 需要设置的值。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_set(T_UWORD * target,T_UWORD value) */
.balign 16,0x90 
.global _atomic_64_set
_atomic_64_set:
	/* lock */		   /* xchg always locks, lock not needed */
	xchgq %rsi,(%rdi)          /* set value with bus-lock */
    movq  %rsi, %rax           /*  RETURNS: The old value of *target */

	ret

/*
* @brief 
*       清除target指向的值，并返回target原有的值。
* @param  target: 需要清除值的地址。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_clear(T_UWORD * target) */
.balign 16,0x90 
.global _atomic_64_clear
_atomic_64_clear:
	movq  $0x0,%rax            /* 'clear' value to set */
	/* lock */                 /* xchg always locks, lock not needed */
	xchgq %rax,(%rdi)          /* swap 'clear' value with bus-lock */

	ret

/*
* @brief 
*       将target指向的值与value位或，并返回target原有的值。
* @param  target: 需要位或值的地址。
* @param[in]  value: 需要位或的值。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_or(T_UWORD * target,T_UWORD value) */
.balign 16,0x90 
.global _atomic_64_or
_atomic_64_or:
	movq  (%rdi),%rax          /* get old value */

atomic64Or_retry:

	movq  %rsi, %rdx
	orq   %rax, %rdx
	lock			   /* lock the Bus during the next inst */
	cmpxchgq %rdx,(%rdi)       /* if (%rax == (%rdi)) */
				   /*   {ZF = 1 ;  (%rdi) = %rdx;} */
				   /* else                */
				   /*   {ZF = 0 ;  %rax = (%rdi);} */
	jnz atomic64Or_retry
	ret


/*
* @brief 
*       将target指向的值与value位与，并返回target原有的值。
* @param  target: 需要位与值的地址。
* @param[in]  value: 需要位与的值。
* @return
*	target原有指向的值
* @implements  DC.32.1.4
*/
/* T_UWORD _atomic_32_and(T_UWORD * target,T_UWORD value) */
.balign 16,0x90 
.global _atomic_64_and
_atomic_64_and:
	movq  (%rdi),%rax          /* get old value */

atomic64And_retry:

	movq  %rsi,%rdx
	andq  %rax, %rdx
	lock			   /* lock the Bus during the next inst */
	cmpxchgq %rdx,(%rdi)       /* if (%rax == (%rdi)) */
				   /*   {ZF = 1 ;  (%rdi) = %rdx;} */
				   /* else                */
				   /*   {ZF = 0 ;  %eax = (%rdi);} */
	jnz atomic64And_retry
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
	movq    %rdi,%rdx /* get address */
	xorq    %rax,%rax   /* set status to FALSE */
	lock    /* lock the Bus during the nexinst */
	bts     $0,(%rdx)          /* set MSB with bus-lock */
	jc      taTas1
	inc    %rax               /* set status to TRUE */
taTas1:
	ret


#else



.text
.code32
/*
* @brief 
*       将target指向的值与oldValue对比，如果相等，设置target指向的值为newValue，
*		并返回TRUE；如果不相等，则直接返回FALSE。
* @@param target: 需要对比值的地址。
* @param[in]oldValue: 需要对比的值。
* @param[in]newValue: 需要设置的值。
* @return
*	TURE:target指向的值与oldValue相等
*	FALSE:target指向的值与oldValue不相等
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
*       将target指向的值增加value，并返回target原有的值。
* @param  target: 需要增加值的地址。
* @param[in]  value: 需要增加的值。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_add(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_add
_atomic_32_add:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
	movl  SP_ARG2(esp),eax  /*获取需要增加的值 */

	lock			                       /* lock the Bus during the next inst */
	xadd  eax,(edx)                /* TMP = eax + (edx) */
				                       /* eax = (edx) */
				                       /* (edx) = TMP */
	ret

/*
* @brief 
*       将target指向的值减去value，并返回target原有的值。
* @param  target: 需要减去值的地址。
* @param[in]  value: 需要减去的值。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_sub(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_sub
_atomic_32_sub:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
	movl  SP_ARG2(esp),eax  /*获取需要增加的值 */

	negl  eax		  /* Negate eax... */

	lock		          /* lock the Bus during the next inst */
	xadd  eax,(edx)         /* TMP = eax + (edx) */
				  /* eax = (edx) */
				  /* (edx) = TMP */
	ret
	
/*
* @brief 
*       将target指向的值增加1，并返回target原有的值。
* @param  target: 需要增加值的地址。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_inc(T_UWORD * target) */
.align 
.global _atomic_32_inc
_atomic_32_inc:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
	movl  $0x1,eax           /* inc by 1 */

	lock                      /* lock the Bus during the next inst */
	xadd  eax,(edx)         /* TMP = eax + (edx) */
				  /* eax = (edx) */
				  /* (edx) = TMP */
	ret

/*
* @brief 
*       将target指向的值减去1，并返回target原有的值。
* @param  target: 需要减去值的地址。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_dec(T_UWORD * target) */
.align 
.global _atomic_32_dec
_atomic_32_dec:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
	movl  $0x1,eax           /* dec by 1 */
	negl  eax		  /* Negate eax... */

	lock                      /* lock the Bus during the next inst */
	xadd  eax,(edx)         /* TMP = eax + (edx) */
				  /* eax = (edx) */
				  /* (edx) = TMP */
	ret

/*
* @brief 
*       获取target指向的值。
* @param  target: 需要获取值的地址。
* @return
*	获取target指向的值
*/
/* T_UWORD _atomic_32_get(T_UWORD * target) */
.align 
.global _atomic_32_get
_atomic_32_get:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
	movl (edx), eax
	ret

/*
* @brief 
*       将target指向的值设置为value，并返回target原有的值。
* @param  target: 需要设置值的地址。
* @param[in]  value: 需要设置的值。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_set(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_set
_atomic_32_set:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
	movl  SP_ARG2(esp),eax  /*获取需要设置的值 */

	/* lock */		  /* xchg always locks, lock not needed */
	xchg  eax,(edx)         /* set value with bus-lock */

	ret

/*
* @brief 
*       清除target指向的值，并返回target原有的值。
* @param  target: 需要清除值的地址。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_clear(T_UWORD * target) */
.align 
.global _atomic_32_clear
_atomic_32_clear:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
	movl  $0x0,eax           /* 'clear' value to set */
	/* lock */                /* xchg always locks, lock not needed */
	xchg  eax,(edx)         /* swap 'clear' value with bus-lock */

	ret

/*
* @brief 
*       将target指向的值与value位或，并返回target原有的值。
* @param  target: 需要位或值的地址。
* @param[in]  value: 需要位或的值。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_or(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_or
_atomic_32_or:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
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
*       将target指向的值与value位异或，并返回target原有的值。
* @param  target: 需要位异或值的地址。
* @param[in]  value: 需要位异或的值。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_xor(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_xor
_atomic_32_xor:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
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
*       将target指向的值与value位与，并返回target原有的值。
* @param  target: 需要位与值的地址。
* @param[in]  value: 需要位与的值。
* @return
*	target原有指向的值
* @implements  DC.32.1.4
*/
/* T_UWORD _atomic_32_and(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_and
_atomic_32_and:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
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
*       将target指向的值与value位nand，并返回target原有的值。
* @param  target: 需要位nand值的地址。
* @param[in]  value: 需要位nand的值。
* @return
*	target原有指向的值
*/
/* T_UWORD _atomic_32_nand(T_UWORD * target,T_UWORD value) */
.align 
.global _atomic_32_nand
_atomic_32_nand:
	movl  SP_ARG1(esp),edx  /* 获取地址*/
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
*       将target指向的64位值设置为64位value。
* @param  target: 需要设置值的地址。
* @param[in]  value: 需要设置的值。
* @return
*	无
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
*       将value指向的64位值设置为target指向的64位值。
* @param[in]   target: 需要获取值的地址。
* @param[in]  value: 需要设置值的地址。
* @return
*	无
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
#endif
