#include <tastdio.h>

void usr_app_test()
{
	printk("enter usr app");
	while(1);	
}
/*
 * vector default handlers
 */

void except_tlb_refill_default()
{
	printk("\nTLB refill exception occured!\n");
	while(1);
}

void except_xtlb_refill_default()
{
	printk("\nXTLB refill exception occured!\n");
	while(1);
}


void except_cache_error_default()
{
	printk("\nCache error exception occured!\n");
	while(1);
}

void except_general_default()
{
	printk("\nGeneral(not interrupt) exception occured!\n");
	while(1);
}


int cacheLsn2ePipeFlush (void)
{
    sysWbFlush();
    return (0);
}

