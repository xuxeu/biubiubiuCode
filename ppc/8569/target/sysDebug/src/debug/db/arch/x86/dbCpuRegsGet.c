/*
 * dbCpuRegsGet.c
 *
 *  Created on: 2014-7-2
 *      Author: Administrator
 */

#ifndef _KERNEL_DEBUG_

/*
 * @brief:
 *     保存核间中断上下文，操作系统核间中断处理会调用此函数。
 * @param[in]:pRegSet:核间中断上下文
 * @return:
 *     无
 */
void taIntVxdbgCpuRegsGet(void *pRegSet)
{
	/* 任务寄存器转换为调试上下文 */
	taDebugRegsConvert(pRegSet, taSDAExpContextPointerGet(taGetCpuID()));

	/* 核间中断处理函数 */
	sdaIpiHandler();
}

#endif
