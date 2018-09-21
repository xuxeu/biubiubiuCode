/* vxdbgCpuArchLib.c - VxDBG CPU control architecture specific library */


/* includes */
#ifndef _KERNEL_DEBUG_
#include <vxWorks.h>
#include <vxdbgLibP.h>
/*******************************************************************************
*
* vxdbgCpuCtrlEnable - enable CPU control
*
* NOMANUAL
*/
VXDBG_STATUS taDbgCpuCtrlEnable (void)
    {
	/*x86 ע�ᴦ������GDT���˺���Ϊ��*/
    return (VXDBG_OK);
    }

/*******************************************************************************
*
* vxdbgCpuCtrlDisable - disable VxDBG CPU control
*
* NOMANUAL
*/

VXDBG_STATUS taDbgCpuCtrlDisable (void)
    {
	/*x86 ע�ᴦ������GDT���˺���Ϊ��*/
    return (VXDBG_OK);
    }
#endif
