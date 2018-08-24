/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:taMsgDispatch.c
 * @brief:
 *             <li>ͨ��ģ����Ϣ�ɷ�ʵ��</li>
 */

/************************ͷ �� ��******************************/
#include "taMsgDispatch.h"
#include "taPacketComm.h"

ulong_t taTaskIdSelf(void);


/************************�� �� ��*******************************/

/************************���Ͷ���******************************/

/* ������Ϣ�ɷ���ṹ */
typedef struct
{
    TaMsgProc proc;  /* ��Ϣ������ */
}TA_MSG_DISPATCH_TAB;

/************************�ⲿ����******************************/

/************************ǰ������******************************/

/************************ģ�����******************************/

/* ��ѭ�հ���־ */
static BOOL taDispatchFlag  = FALSE;

/* ��Ϣ��ģ����� */
static T_TA_Packet taPacket;

/* ��Ϣ�ɷ��� */
static TA_MSG_DISPATCH_TAB taMessageDispatchTable[TA_MAX_AGENT_NUM];

/************************ȫ�ֱ���******************************/

/************************����ʵ��******************************/

/*
 * @brief:
 *  	����ѭ��������Ϣ
 * @return:
 *      ��
 */
void taStartLoopDispatch(void)
{
    taDispatchFlag = TRUE;
}

/*
 * @brief:
 *      ֹͣѭ��������Ϣ
 * @return:
 *      ��
 */
void taStopLoopDispatch(void)
{
    taDispatchFlag = FALSE;
}


/*
 * @brief:
 *      ��װͨ��ͨ����Ϣ������
 * @param[in]:aid: ͨ����
 * @param[in]:proc: ��Ϣ������ָ��
 * @return:
 *     TA_OK: ��װ�ɹ�
 *     TA_INVALID_ID: �����ͨ����,��ȷ��ͨ��ID�ŷ�ΧӦ����0~MAX_AGENT_NUM
 */
T_TA_ReturnCode taInstallMsgProc(UINT32 aid, TaMsgProc proc)
{
    if (aid >= TA_MAX_AGENT_NUM)
    {
        return (TA_INVALID_ID);
    }

    /* ����ͨ���ź�ͨ����Ϣ������֮���ӳ���ϵ */
    taMessageDispatchTable[aid].proc = proc;

    return (TA_OK);
}

/**
 * @brief:
 *      ��Ϣ�����ɷ��������յ���Ϣ�������ݽ���ͨ��ID���ö�Ӧ��װ����Ϣ����
 *      ����������Ϣ�����д���
 * @return:
 *      TA_OK:�հ��ɹ�
 *      TA_FAIL:�հ�ʧ��
 */
T_TA_ReturnCode taMessageDispatch(void)
{
    UINT32 desSAid = 0;
    T_TA_ReturnCode ret = TA_OK;

    do
    {
        /* ����taCOMMGetPkt()�������ݰ� */
        ret = taCOMMGetPkt(&taPacket);
        if (TA_OK == ret)
        {
            /* ��ȡĿ��ͨ��ID */
            desSAid = taGetPktHeadRecvChannel(&(taPacket.header));

            /* ���Ŀ��ͨ��ID������ķ�Χ�ڣ�������ע����Ϣ������ */
            if ((desSAid < TA_MAX_AGENT_NUM) && (NULL != taMessageDispatchTable[desSAid].proc))
            {
                /* ��Ŀ��ͨ���ɷ����ݰ� */
                taMessageDispatchTable[desSAid].proc(&taPacket);
            }
        }
    } while (TRUE == taDispatchFlag);

    return (ret);
}
