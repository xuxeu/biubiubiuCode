/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-12-17         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taskSchedLockLib.c
 * @brief:
 *             <li>�����������ؽӿ�</li>
 */

/************************ͷ �� ��*****************************/
#include "taskSchedLockLib.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

#ifdef	CONFIG_CORE_SMP
/* ������������� */
static volatile atomic_t taTaskSchedInvoked = 0;
#endif

/************************ȫ�ֱ���*****************************/

/************************ʵ   ��********************************/

#ifdef	CONFIG_CORE_SMP
/*
 * @brief:
 *     CPC�жϾ�����ú�������ֹͣ��ǰCPU
 * @return:
 *     ��
 */
void taTaskSchedLockHandler(void)
{
    while (TA_MULTIOS_ATOMIC_GET((atomic_t *)&taTaskSchedInvoked) != 0)
	;
}
#endif

/*
 * @brief:
 *     ���������,��ֹ������������
 * @return:
 *     ��
 */
void taTaskSchedLock(void)
{
#ifdef	CONFIG_CORE_SMP

	/* �ڶ��ϵͳ�У������ϵ��쳣��Ҫ�����ȣ�ֱ������ */
    while (!(taTas((void *)&taTaskSchedInvoked)))
	;

    /* Ͷ��IPI�ж� */
    sdaIpiEmit(taTaskSchedLockHandler, TA_NO_WAIT);
#endif	/* CONFIG_CORE_SMP */
}

/*
 * @brief:
 *     ������Ƚ���,����������������
 * @return:
 *     ��
 */
void taTaskSchedUnlock(void)
{
#ifdef	CONFIG_CORE_SMP

	/* �ж��Ƿ��Ѿ����� */
	if (TA_MULTIOS_ATOMIC_GET ((atomic_t *)&taTaskSchedInvoked) == 0)
	return;

    /* �ͷ�����CPU���������������� */
	TA_MULTIOS_ATOMIC_CLEAR((atomic_t *)&taTaskSchedInvoked);

#endif	/* CONFIG_CORE_SMP */
}
