#include "ta.h"

void taOsExceptionHandler(unsigned int vector,ESF0 *info,unsigned int *regs,BOOL error,EXC_INFO *pExcInfo);

void ExceptMain()
{
	printk("you can use except to set except take in 0-32. for example: except 0\n");

}


void exceptAll()
{
	int i=0;
	for(i=0; i<=32;i++)
	{
		if((i != 1) || (i != 3) )
		{
			excConnect(i, taOsExceptionHandler);
		}
	}

	printk("excNum has been seted all about 0-32.\n");

}


int except(int excNum)
{
	if (excNum < 0 && excNum > 32)
	{
		printk("excNum must in 0-32.\n");
		return 0;
	}

	excConnect(excNum, taOsExceptionHandler);
	printk("excNum has been seted as  %d.\n",excNum);
	return 1;
}
