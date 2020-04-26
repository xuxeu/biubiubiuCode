/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  dbHdBp.c
 * @brief:
 *             <li>Ӳ���ϵ���롢ɾ�������ӿ�ʵ��</li>
 */
/************************ͷ�ļ�******************************/
#include "ta.h"
#include "dbDR.h"
#include "taErrorDefine.h"
#include "dbBreakpoint.h"

/************************�궨��******************************/

/************************���Ͷ���****************************/

/************************ȫ�ֱ���****************************/

/************************ǰ������****************************/

/************************ģ�����****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *     ����Ӳ���ϵ㣬Ӳ���ϵ��Ϊ���ݶϵ��ָ��ϵ�
 * @param[in]: debugInfo: ���ԻỰ��Ϣ
 * @param[in]: addr: �ϵ��ַ
 * @param[in]: type: �ϵ�����
 * @param[in]: size: �ϵ���
 * @return:
 *    TA_OK: ����ɹ�
 *    TA_DB_BREAKPOINT_FULL:����ʧ��(�ϵ������)
 */
T_TA_ReturnCode taDebugInsertHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
    UINT32 idx = DR_ERROR;
    UINT32 drsize = 0;
    UINT32 drtype = DR_WRITE;

    /* ��ѯӲ���ϵ��Ƿ���� */
    if ((idx = taDebugLookupDR()) == DR_ERROR)
    {
        /* Ӳ�ϵ��Ѿ����� */
        return (TA_DB_BREAKPOINT_FULL);
    }

    /* ת������ΪX86�ϵ��׼ */
    drtype = taDebugTypeConvert(type);

    /* ת�����ΪX86��׼ */
    drsize = taDebugSizeConvert(size);

    /* ��������Ķϵ����Ͳ���һ��Ӳ���ϵ� */
    taDebugInsertDR(idx, addr, drtype, drsize);

    /* ʹ��ָ���Ķϵ� */
    taDebugEnableDR(idx);

    return (TA_OK);
}

/*
 * @brief:
 *     ɾ��Ӳ���ϵ�
 * @param[in]: debugInfo: ���ԻỰ��Ϣ
 * @param[in]: addr: �ϵ��߼���ַ
 * @param[in]: type: �ϵ�����
 * @param[in]: size: �ϵ���
 * @return:
 *    TA_OK:ɾ���ɹ�
 *    TA_DB_BREAKPOINT_NOTFOUND:�ϵ��Ҳ���
 */
T_TA_ReturnCode taDebugRemoveHardBp(T_DB_Info *debugInfo, UINT32 addr, T_DB_BpType type, UINT32 size)
{
    UINT32 drsize = 0;
    UINT32 drtype = DR_WRITE;
    UINT32 idx = DR_ERROR;

    /* ת������ΪX86�ϵ��׼ */
    drtype = taDebugTypeConvert(type);

    /* ת�����ΪX86��׼ */
    drsize = taDebugSizeConvert(size);

    /* �鿴�Ƿ���ƥ��Ķϵ� */
    idx = taDebugMatchDR(addr, drtype, drsize);
    if (DR_ERROR != idx)
    {
        /* ����ƥ��Ķϵ�,�����taDebugDisableDR��ֹ�ϵ� */
        taDebugDisableDR(idx);

        /* ���ز����ɹ� */
        return (TA_OK);
    }

    /* ������ƥ��Ķϵ�,�򷵻�DB_BREAKPOINT_NOTFOUND */
    return (TA_DB_BREAKPOINT_NOTFOUND);
}
