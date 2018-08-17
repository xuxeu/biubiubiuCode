/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                         �������ļ���
 */

/*
 * @file:lmain.c
 * @brief:
 *             <li>taģ���ʼ��</li>
 */
 
/************************ͷ �� ��******************************/

/************************�� �� ��********************************/

/************************���Ͷ���*******************************/

/************************�ⲿ����*******************************/
extern char _bss_start[];
extern char _bss_size[];
extern void *_vectors_table_end();
extern void taGeneralExpAsmHandler(void);
extern void CPU_FlushCacheL1Data(void);
extern void CPU_FlushCacheL2(void);
extern void sys_init(void);

/************************ǰ������*******************************/

/************************ģ�����*******************************/

/************************ȫ�ֱ���*******************************/

/************************����ʵ��*******************************/

 /*
 * @brief:
 *      �ϵ���һ��C���,���BSS�Σ�������������ʼ��TA
 * @return:
 *      ��
 */
void lmain(void)
{
    unsigned int *bstart,bsize,i;

    bstart = (unsigned int*)&_bss_start;
    bsize = (unsigned int)&_bss_size;
    for (i = 0;i < bsize;i+=4)
    {
        *bstart++ = 0;
    }

    /* ���������� */
    memcpy((char *)0x80000180,
        (char *)taGeneralExpAsmHandler,
        (char *)_vectors_table_end - (char *)taGeneralExpAsmHandler);

    CPU_FlushCacheL2();
    CPU_FlushCacheL1Data();
        
    sys_init();
}
