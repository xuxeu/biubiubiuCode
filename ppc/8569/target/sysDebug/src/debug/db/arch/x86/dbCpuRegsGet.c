/*
 * dbCpuRegsGet.c
 *
 *  Created on: 2014-7-2
 *      Author: Administrator
 */

#ifndef _KERNEL_DEBUG_

/*
 * @brief:
 *     ����˼��ж������ģ�����ϵͳ�˼��жϴ������ô˺�����
 * @param[in]:pRegSet:�˼��ж�������
 * @return:
 *     ��
 */
void taIntVxdbgCpuRegsGet(void *pRegSet)
{
	/* ����Ĵ���ת��Ϊ���������� */
	taDebugRegsConvert(pRegSet, taSDAExpContextPointerGet(taGetCpuID()));

	/* �˼��жϴ����� */
	sdaIpiHandler();
}

#endif
