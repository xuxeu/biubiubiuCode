#include "tatypes.h"
#include "dbContext.h"
#include "tastdio.h"

typedef T_VOID (*MONITOR_EXCEPTION_HANDLER)(T_UWORD vector);
T_EXTERN DB_ExceptionContext g_sdaExpContext;
T_EXTERN T_VOID Handler_GeneralExc(T_UWORD vector);

T_MODULE MONITOR_EXCEPTION_HANDLER exceptionHandler[256];


T_MODULE T_BYTE* exceptionName[32] = 
{
	"Interrupt",/*0*/
	"TLB modification exception", /*1*/
	"TLB exception (load or instruction fetch)", /*2*/
	"TLB exception (store)", /*3*/
	"Address error exception (load or instruction fetch)", /*4*/
	"Address error exception (store)", /*5*/
	"Bus error exception(instruction fetch)", /*6*/
	"Bus error exception(data referencd:load or store)", /*7*/
	"Syscall exception",/*8*/
	"Breakpoint exception", /*9*/
	"Reserved instruction exception",/*10*/
	"Coprocessor Unusable exception", /*11*/
	"Arithmetic Overflow exception", /*12*/
	"Trap exception",/*13*/
	"Virtual Coherency Exception instruction", /*14*/
	"Floating-Point exception", /*15*/
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reference to WatchHi/WatchLo address",/*23*/
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Virtual Coherency Exception data",/*31*/
};

T_UWORD monitorCallHandler(T_UWORD vector)
{
	/*FIXME:目前中断直接派发到OS，在添加了处理Monitor的中断后，如果是
	 * Monitor的中断，则不派发到OS*/
	if (vector == 0)
	{
		return 1;
	}
	/*FIXME:任务及调试的异常派发*/
	
	if ((vector == 9) && (*((T_UWORD*)0x81FFFFE0) == 0x55AAFEFE))
	{
		return 1;	
	}
	
	(exceptionHandler[vector])(vector);
	return 0;
}

T_VOID monitorSetupRawVector(T_UWORD vector, T_VOID *handler, T_UWORD *oldHandler )
{
	exceptionHandler[vector] = (MONITOR_EXCEPTION_HANDLER)handler;
}

/*FIXME中断映射*/
T_UWORD monitorGetLogicalVector(T_UWORD vector)
{
	
}

T_VOID monitorDefaultHandler(T_UWORD vector)
{
	printk("\n*********************Monitor Exception***************************\n");
	printk("%s, Vector = 0x%x\n",exceptionName[vector], vector);
	printk("Cause = 0x%x,Status = 0x%x,Bad = 0x%x,EPC = 0x%x\n", g_sdaExpContext.registers[CAUSEREG],
										g_sdaExpContext.registers[STATUSREG],
										g_sdaExpContext.registers[BADVADDRREG],
										g_sdaExpContext.registers[PC]);
	while (1)
	{
		;	
	}	
}

T_VOID monitorInitRawVector()
{
	UINT32 i;
	
	for (i = 0; i < 32; i++)
	{
		monitorSetupRawVector(i, monitorDefaultHandler,NULL);
	}
	
	*((T_UWORD*)0x81fffff0) = 0x11223344;
    
  *((T_UWORD*)0x81fffff4) = Handler_GeneralExc;
    
}
