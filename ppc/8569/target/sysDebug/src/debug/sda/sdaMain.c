/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * @file:sdaMain.c
 * @brief:
 *             <li>���Թ���ϵͳ�����Գ�ʼ������Ϣ����</li>
 */

/************************ͷ �� ��******************************/
#include <string.h>
#include "taCommand.h"
#include "taPacketComm.h"
#include "taMsgDispatch.h"
#include "taDebugMode.h"
#include "sdaCpuLib.h"
#include "taExceptionReport.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/* �쳣�����ĵ�ַ */
UINT32 taSDAExpContext[MAX_CPU_NUM];

/*ʸ�������Ƿ��ʼ����־*/
UINT32 taAltiVecInitFlag = FALSE;

/* �쳣������ */
T_DB_ExceptionContext taSDASmpExpContext[MAX_CPU_NUM];

#ifdef TA_ALTIVEC_SUPPORT_DEBUG
/* �쳣ʸ�������ĵ�ַ */
UINT32 taSDAExpVectorContext[MAX_CPU_NUM];
/* �쳣ʸ�������� */
T_DB_ExceptionVectorContext taSDASmpExpVectorContext[MAX_CPU_NUM];

#endif

/* ϵͳ�����ԻỰ  */
T_TA_Session  taSDASessInfo;
/************************����ʵ��******************************/

/*
 * @brief:
 *      SDA��Ϣ����ӿ�
 * @param[in]:packet: �յ�����Ϣ��
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL: ����ִ��ʧ�� 
 */
static T_TA_ReturnCode taSDAMessageProc(T_TA_Packet *packet)
{
    T_DB_Info* debugInfo = NULL;
    UINT32 desSAid = (UINT32)(TA_SYS_DEBUGER);
    T_TA_ReturnCode ret = TA_OK;

    /* ��ȡͨ���� */
    desSAid = (UINT32)taGetPktHeadRecvChannel(&(packet->header));

    /* ��ȡͨ����ΪdesSAid�ĵ�����Ϣ */
    debugInfo = taSDADebugInfoGet();

    /* ����GDB��ͷ */
    taSaveGdbPacketHead(taGetPktUsrData(packet),&(packet->header),&(taSDASessInfo.gdbPacketHead));

    /* ����taAgentEntryͨ����Ϣ����ӿ�,����ϵͳ��������Ϣ�� */
    ret = taAgentEntry(packet,debugInfo,COMMANDTYPE_DEBUG,NULL,0,NULL,NULL);

    return (ret);
}

/*
 * @brief:
 *         ��ȡ������Ϣ
 * @return:
 *       ϵͳ���ĵ�����Ϣ
 */
T_DB_Info* taSDADebugInfoGet(void)
{
    return (&(taSDASessInfo.sessInfo));
}

/*
 * @brief:
 *     ��ȡ�쳣������ָ��
 * @return:
 * 		�쳣������ָ��
 */
T_DB_ExceptionContext* taSDAExpContextPointerGet(UINT32 cpuIndex)
{
    return (&taSDASmpExpContext[cpuIndex]);
}

#ifdef TA_ALTIVEC_SUPPORT_DEBUG
/*
 * @brief:
 *     ��ȡ�쳣ʸ��������ָ��
 * @return:
 * 		�쳣ʸ��������ָ��
 */
T_DB_ExceptionVectorContext* taSDAExpVectorContextPointerGet(UINT32 cpuIndex)
{
	 return taSDAExpVectorContext[cpuIndex];
}
#endif

#ifdef TA_ALTIVEC_SUPPORT_DEBUG
/*
 * @brief:
 *     ��ʼ��ʸ������֧��
 * @return:
 * 		None
 */
void taAltiVecInit()
{
	int i = 0;
	//��ʼ����־
	taAltiVecInitFlag = TRUE;

	//����洢�ռ�
	for (i = 0; i < (INT32)(MAX_CPU_NUM); i++)
	{
#ifndef _KERNEL_DEBUG_
		taSDAExpVectorContext[i] = (UINT32)malloc(sizeof(T_DB_ExceptionVectorContext));
#else
		taSDAExpVectorContext[i] = (UINT32)&(taSDASmpExpVectorContext[i]);
#endif
	}
}

/*
 * @brief:
 *     ��ȡʸ�������Ƿ��ʼ����־
 * @return:
 * 		��ʼ����־
 */
int taAltiVecIsInit()
{
	return taAltiVecInitFlag;
}
#endif

/*
 * @brief:
 *      SDA��ʼ��,�����ʼ��ϵͳ�����ԻỰ��������Ϣ�������İ�װ
 * @return:
 *        TA_OK: ��ʼ���ɹ�
 */
T_TA_ReturnCode taSDAInit(void)
{
    UINT32 i = 0;

    /* ��ʼ�����ݰ�ͷ */
    taZeroPktHead(&(taSDASessInfo.gdbPacketHead));

    /* ��װ��Ϣ������  */
    taInstallMsgProc(TA_SYS_DEBUGER, taSDAMessageProc);

    /* ����쳣������  */
    ZeroMemory((void *)taSDAExpContextPointerGet(0), sizeof(T_DB_ExceptionContext) * MAX_CPU_NUM);

    /* �����쳣�����ĵ�ַ  */
    for (i = 0; i < (INT32)(MAX_CPU_NUM); i++)
    {
        /* �����쳣�����ĵ�ַ */
    	taSDAExpContext[i] = (UINT32)&taSDASmpExpContext[i];
    }

    /* ���õ��ԻỰ */
    taSDASessInfo.sessInfo.sessionID = TA_SYS_DEBUGER_SESSION;
    taSDASessInfo.sessInfo.objID = 0;
    taSDASessInfo.sessInfo.curThreadId = 0;
    taSDASessInfo.sessInfo.status = DEBUG_STATUS_CONTINUE;
    taSDASessInfo.sessInfo.attachStatus = DEBUG_STATUS_DETACH;
    taSDASessInfo.sessInfo.ctxType = TA_CTX_SYSTEM;

    /* ���õ�ǰ���ԻỰ������ָ�� */
    taSDASessInfo.sessInfo.context = taSDAExpContextPointerGet(GET_CPU_INDEX());

    /* ���õ�ǰ����ģʽΪϵͳ������ģʽ */
    taDebugModeSet(DEBUGMODE_SYSTEM);

    /* ����ϵͳ���쳣������ */
    _func_taSysExceptionHandler = taSDAExceptionHandler;

    /* �������ݰ����û�������ʼλ�� */
    taSetPktOffset(&(taSDASessInfo.packet),TA_PROTOCOL_HEAD_SIZE+TA_PROTOCOL_CHECKSUM_SIZE);

    return (TA_OK);
}
