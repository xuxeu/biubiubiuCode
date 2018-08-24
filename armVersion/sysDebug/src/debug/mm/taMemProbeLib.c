/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * �޸���ʷ:
 * 2013-11-08         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taMemProbeLib.c
 * @brief:
 *             <li>��ģ���ṩ��ϵ�ṹ�޹ص��ڴ�̽����ʽӿ�</li>
 */

/************************ͷ �� ��*****************************/
#include "taMemProbeLib.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* �ڴ�̽���ʼ����־ */
static BOOL taMemProbeInitialized = FALSE;

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/**
 * @brief:
 *     �ڴ�̽���ʼ��
 * @param[in]:_func_excBaseHook: �쳣HOOKָ�룬�쳣����ʱ����ϵͳ���ø�HOOK
 * @return:
 * 	   OK����ʼ���ɹ�
 * 	   ERROR����ʼ��ʧ��
 */
STATUS taMemProbeInit(FUNCPTR *_func_excBaseHook)
{
    STATUS status = OK;

    if (taMemProbeInitialized == FALSE)
    {
        status = taMemArchProbeInit(_func_excBaseHook);
        taMemProbeInitialized = (status == OK)?TRUE:FALSE;
    }

    return (status);
}

/*
 * @brief:
 *     �ڴ�̽�⣬̽���ڴ��Ƿ�ɶ�д��������ɶ�д���������ߴ����������ģʽΪTA_READ�����ݽ�
 *     ������pVal��ַ�����������ģʽΪTA_WRITE������pValָ���ֵд��pAdrs��ַ��
 * @param[in]:pAdrs: �ڴ��ַ
 * @param[in]:mode:����ģʽ��TA_READ���� TA_WRITE
 * @param[out]:length: �ڴ���ʳ���
 * @param[in]:pVal: TA_WRITEģʽpValΪԴ��ַ��TA_READģʽΪ pValΪĿ�ĵ�ַ
 *     OK����д�ڴ�ɹ�
 *     ERROR�������Ƿ���ַ�����쳣
 */
#ifdef __X86__
STATUS taMemCommonProbe(char *pAdrs, int  mode, int length, char *pVal)
{
    /* ��֤�Ƿ��ʼ�� */
    if (taMemProbeInitialized == FALSE)
    {
        return (OK);
    }

    /* �����ڴ���� */
    if (mode == TA_READ)
    {
        return (taMemProbeSup(length, pAdrs, pVal));
    }
    else
    {
        return (taMemProbeSup(length, pVal, pAdrs));
    }
}
#endif
/*
 * @brief:
 *     �ڴ�̽�⣬̽���ڴ��Ƿ�ɶ�д��������ɶ�д���������ߴ����������ģʽΪTA_READ�����ݽ�
 *     ������pVal��ַ�����������ģʽΪTA_WRITE������pValָ���ֵд��pAdrs��ַ��
 * @param[in]:pAdrs: ���ʵ��ڴ��ַ
 * @param[in]:mode: �ڴ����ģʽ��TA_READ���� TA_WRITE
 * @param[out]:length: �ڴ���ʳ���
 * @param[in]:pVal: TA_WRITEģʽpValΪԴ��ַ��TA_READģʽΪ pValΪĿ�ĵ�ַ
 *     OK���ڴ�ɷ���
 *     ERROR���ڴ治�ɷ���
 */
STATUS taMemProbe(char *pAdrs, int  mode, int length, char *pVal)
{
#ifdef __X86__
#ifdef CONFIG_TA_LP64

    return taMemArchProbe((void *)pAdrs, mode, length, (void *)pVal);
#else
    return taMemCommonProbe((void *)pAdrs, mode, length, (void *)pVal);
#endif
#endif
#ifdef __ARM__
    return taMemArchProbe((void *)pAdrs, mode, length, (void *)pVal);
    
    //return (OK);
#endif
}

/*
 * @brief:
 *     ����һ���ڴ淶Χ��ַ�Ƿ�ɷ���
 * @param[in]:pAdrs: ���ʵ��ڴ��ַ
 * @param[out]:size: �ڴ���ʳ���
 * @param[in]:mode: �ڴ����ģʽ��TA_READ���� TA_WRITE
 * @param[in]:width: ���ʿ��
 *     TA_OK���ڴ�ɷ���
 *     TA_INVALID_INPUT���ڴ���ʷ�Χ���
 *     TA_ERR_MEM_ACCESS���ڴ����ʧ��
 */
UINT32 taMemTest(char *addr, UINT32 size, int mode, UINT32 width)
{
    UINT32 dummy = 0;
    char *memAddr = 0;
    BOOL preserve = FALSE;


    if (size == 0)
    {
    	return (TA_OK);
    }

    if (width == 0)
    {
    	width = 1;
    }

    /* ����ڴ���ʷ�Χ�Ƿ���� */
    if (addr > addr + size)
    {
    	return (TA_INVALID_INPUT);
    }

    /* �ڴ�д̽�⣬��Ҫ����ԭ�������� */
    preserve = (mode == TA_WRITE)?TRUE:FALSE;

    /* ̽������ʵ��ڴ淶Χ�Ƿ񶼿ɷ��� */
	for (memAddr = addr; memAddr < addr + size; memAddr += width)
	{
		if ((preserve && (taMemProbe(memAddr, TA_READ, width,
			    		   (char *)&dummy) != OK)) || 
			(taMemProbe(memAddr, mode, width, (char *)&dummy) != OK))
	    {
			return (TA_ERR_MEM_ACCESS);
	    }
	}

    return (TA_OK);
}
