
/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taCpuLib.c
 * @brief:
 *             <li>CPU��س�ʼ��</li>
 */

/************************ͷ �� ��******************************/
#include "taTypes.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/
#define CONFIG_INTERRUPT_NUMBER_OF_VECTORS 255

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/
void* taVectorTable[256];

/************************����ʵ��*****************************/

/*
 * @brief:
 *      �쳣����պ���
 * @param[in]: vector: �쳣��
 * @return:
 *     ��
 */
void taNullRawHandler(UINT32 vector)
{
	while(1);
}

/*
 * @brief:
 *     ��װ�쳣��������IDT����
 * @param[in] vector :�쳣��
 * @param[in] vectorHandle :�쳣������
 * @return:
 *     ��
 */
void taInstallVectorHandle(int vector, void *vectorHandle)
{
    if(vector > CONFIG_INTERRUPT_NUMBER_OF_VECTORS)
    {
        return;
    }
    
	/* ������������<vector>ָ���Ĵ������ */
    taVectorTable[vector] = vectorHandle;
}

/*
 * @brief: 
 *    ��ʼ��������ʹ���е�������ȱʡ����������nullRawHandler
 * @return: 
 *    ��
 */
void taInitVectorTable(void)
{
    UINT32 vector = 0;
    
    for (vector = 0; vector < 256; vector++)
    {
        /* ������������<vector>ָ���Ĵ������ΪnullRawHandler */
        taVectorTable[vector] = taNullRawHandler;
    }
}

/*
 * @brief:
 *      Arch��س�ʼ��
 * @return:
 *      ��
 */
void taArchInit(void)
{
	/* ��ʼ�������� */
	taInitVectorTable();
}
