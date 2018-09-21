
#include "dbAsm.h"

#define __ASM__
#define _ASMLANGUAGE

/*.text*/
.section ".start","ax"
.global _start

_start:

    /* load r0 with 0. This register will always be 0 */
    addis    r0,0,0

    ori      r3,r0,0x1002    /* Set ME,RI flags */
    //ori      r3,r0,0x0002    /* Set ME,RI flags */
    ori      r3,r3,0x2000    /* Set FP flag */


    mtmsr    r3
    isync
    sync

    /* Need SRR1 to match MSR */
    mtsrr1   r3
    
     /* Disable Cache */
	
	   lwi r3,0x0000
	   sync

	   mtspr 1008,r3
	   isync
	
	   mtspr 1017 ,r3  /* disable L2 Cache  */
	   isync

        mtfsfi	cr7,0
        mtfsfi	cr6,0
        mtfsfi	cr5,0
        mtfsfi	cr4,0
        mtfsfi	cr3,0
        mtfsfi	cr2,0
        mtfsfi	cr1,0
        mtfsfi	cr0,0



    /*Enable ICache Complete*/
    /* Clear BSS Section */

    lwi     r3, _bss_start
    lwi     r4, _bss_size
    add     r5, r3, r4
    xor     r6, r6, r6
bss_clear_loop:
    cmpw    r3, r5
    beq     build_stack
    stw     r6, 0(r3)
    addi    r3, r3, 4
    b       bss_clear_loop

build_stack:

    lwi     r1, _init_stack
    addi    r1, r1, SYS_INIT_STACK_SIZE 
    lwi     r3, _lmain
    mtlr    r3
    blrl
    
    /* build c calling frame  */
    subi    r1,r1,8
    stw     r1,0(r1)


    nop
    nop
    nop
    nop
.extern lmain
    lwi   r3, lmain
    mtlr  r3
    blrl
    /*  loop forever,if return */
    b       .

 /* 不可达路径，用于将向量表链接到映像中(__reset的起始地址为0x100) */
.extern __reset
    lwi r3, __reset
    mtlr r3
    blrl
    
_lmain:
    /* 重定位数据段  */
    lwi    r3,  _data_start     /* 目标地址 */
    lwi    r4, _rom_data_start  /* 源地址 */
    lwi    r5, _data_size       /* 数据段长度 */
    add    r5, r5, r4

.data_relocate_begin:
    lwz    r0, 0(r4)
    stw    r0, 0(r3)
    addi    r3, r3, 4
    addi    r4, r4, 4
    cmpw    r4, r5
    blt    .data_relocate_begin

    /* 重定位向量表 */
    lwi    r3,  _vectors_start     /* 目标地址 */
    lwi     r4, _rom_vectors_start  /* 源地址 */
    lwi     r5, _vectors_size       /* 向量段长度 */
    add     r5, r5, r4

.vectors_relocate_begin:
    lwz     r0, 0(r4)
    stw     r0, 0(r3)
    addi    r3, r3, 4
    addi    r4, r4, 4
    cmpw    r4, r5
    blt    .vectors_relocate_begin    

    /* 重定位代码段 */
    lwi     r3,  _code_start        /* 目标地址 */
    lwi     r4, _rom_code_start     /* 源地址 */
    lwi     r5, _code_size          /* 代码段长度 */
    add     r5, r5, r4
    
.code_relocate_begin:
    lwz     r0, 0(r4)
    stw     r0, 0(r3)
    addi    r3, r3, 4
    addi    r4, r4, 4
    cmpw    r4, r5
    blt    .code_relocate_begin

    blr
.end

