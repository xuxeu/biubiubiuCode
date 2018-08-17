/* $id:lmain.c  V1.0 2005/03/16 */

/******************************************************************************
 *  This source code has been made available to you by CORETEK  on
 *  AS-IS.Anyone receiving this source is licensed under
 *  CORETEK copyrights to use it in any way he or she deems fit,including
 *  copying it,modifying it,compiling it,and redistributing it either with
 *  or without modifictions.
 *
 *
 *  Any person who transfers this source code or any derivative work must
 *  include the CORETEK copyright notice, this paragraph,and the preceding
 *  two paragraphs in the transferred software.
 *
 *
 *       COPYRIGHT CORETEK CORPORATION 2001
 *     LICENSED MATERIAL - PROGRAM PROPERTY OF CORETEK
 *****************************************************************************/

/******************************************************************************
 *
 *  FILE: lmain.c
 *
 *  MODULE: bsp
 *
 *  PURPOSE: 完成BSS段的清零.
 *
 *  AUTHOR(S):Lemon
 *
 *  GROUP:SYSTEM DEPT.
 *
 *  DATE CREATED:2005/03/16
 *
 *  REFERENCE DOCUMENT ID:
 *
 *  MODIFICATIONS:
 *  Date          user Name       Description
 *  2005/03/16    Lemon           Create this file
 *
 *********************************************************************************/
#include <tastring.h>

extern void sys_init();
extern void CPU_FlushCache(unsigned int addr);
void CPU_FlushCaches(unsigned int addr, unsigned int len);

extern char _bss_start[];
extern char _bss_size[];
extern char _crtl_heap_start[];
extern char _crtl_heap_size[];
extern char _vectors_start[];
extern char _rom_vectors_start[];

extern char _stack_top[];

extern void *_vectors_table_start();
extern void *_vectors_table_end();
extern void *__except_general();

extern void *set_except_vector(int n, void *addr);
extern void except_general_default();
extern void _ISR_Handler( void );

unsigned int *stack_address = (unsigned int*)&_stack_top;
unsigned char saved_general_vector[0x80];

/**
 * @brief
 * 		在GCC编译器中编译PPC体系时需要该函数。
 *
 * @param[in]	无。
 *
 * @return		无。
 */
//void __init(){}
void restore_general_vector()
{
	tamemcpy((char *)0x80000180, saved_general_vector, 0x80);
}


void lmain(void)
{

	unsigned int *bstart,bsize,i;

	bstart=(unsigned int*)&_bss_start;
	bsize=(unsigned int)&_bss_size;
	for (i = 0;i < bsize;i+=4)
	{
		*bstart++ = 0;
	}

bstart = (int *)__except_general;
	/* save the general exception vector of PMON */
	tamemcpy(saved_general_vector, (char *)0x80000180, 0x80);

	
	#if 1 /* for pmon debug */
	/* install first vectors */
	tamemcpy((char *)0x80000180,/*  0x80000000 */
		(char *)__except_general,/* _vectors_table_start */
		(char *)_vectors_table_end - (char *)__except_general);

	i = (unsigned int)((char *)_vectors_table_end - (char *)__except_general);

	CPU_FlushCaches(0x80000180, i);
		
	#endif

	sys_init();



	
}

void CPU_FlushCaches(unsigned int addr, unsigned int len)
{
	unsigned int startAddr = 0;
	unsigned int endAddr = 0;

	startAddr = (addr  & (~(32 - 1) ));
	endAddr = ((addr + len + 32) & (~(32 -1 )));

	do
	{	
  		CPU_FlushCache(startAddr);
		startAddr+= 32;
	}while (startAddr < endAddr);

}
