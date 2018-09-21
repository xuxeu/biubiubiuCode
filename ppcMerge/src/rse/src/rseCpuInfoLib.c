/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-07-08         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  rseCpuInfoLib.c
 * @brief
 *       ���ܣ�
 *       <li>��ȡCPU��Ϣ</li>
 */

/************************ͷ �� ��******************************/
#include "rseConfig.h"
#include "rseInitLib.h"
#include "rseCmdLib.h"
#include "taDebugMode.h"
#include "vxCpuLib.h"

/************************�� �� ��******************************/

/* CPU��Ϣ���Ը��� */
#define RSE_CPU_INFO_ATTR_NUM 4


/* CPU�ַ��� */
#if defined( __MIPS__)
#define RSE_CPU_STRING "loongson"
#elif defined(__PPC__)
#define RSE_CPU_STRING "PowerPC"
#else
#define RSE_CPU_STRING "pentium4"
#endif


/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/*
 * @brief
 *      �ṩCPU��Ϣ����Ϣ����
 * @param[in] packet: ���յ������ݰ�
 * @return
 *      None
 */
static T_TA_ReturnCode rseCpuInfoHandler(T_TA_Packet* packet);

/*
 * @brief
 *      ��ȡϵͳCPU����Ϣ��
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
static void rseGetCpuInfo(T_TA_Packet* packet,UINT32* outSize);

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/**
  * @brief
  *     CPU��Ϣ��ʼ������
  *@return
  *      None
  */
void rseCpuInfoLibInit(void)
{
	rseRegisterModuleHandler(RSE_TYPE_CPU_MODULE, rseCpuInfoHandler);
}

/*
 * @brief
 *      �ṩCPU��Ϣ����Ϣ����
 * @param[in] packet: ���յ������ݰ�
 * @return
 *      TA_OK:�����ɹ�
 */
static T_TA_ReturnCode rseCpuInfoHandler(T_TA_Packet* packet)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT32 outSize = 0;
    UINT8 operateId = 0;

    /* �Ӱ��л�ȡʵ������ */
    inbuf = taGetPktUsrData(packet);

    /* �����������outbufΪinbuf */
    outbuf = inbuf;

    /* ��ȡ����ID */
    operateId = inbuf[2];
    switch (operateId)
    {
        case RSE_OPR_QUERY:
        	rseGetCpuInfo(packet,&outSize);
            break;
        default:
        	outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d",TA_INVALID_OPERATE_ID);
            break;
    }

    /* �������ݰ� */
    taCOMMSendPacket(packet, outSize);

    return TA_OK;
}

/*
 * @brief
 *      ��ȡϵͳCPU����Ϣ��
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
static void rseGetCpuInfo(T_TA_Packet* packet,UINT32* outSize)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT8* ptr = NULL;
    UINT32 len = 0;

    /* �Ӱ��л�ȡʵ������ */
    inbuf = taGetPktUsrData(packet);

    /* �����������outbufΪinbuf */
    ptr = outbuf = inbuf;

    /* ������ݰ� */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* ���ûظ���Ϣ */
    len = sprintf((INT8 *)ptr,"%s","OK");
    ptr += (len+2);

    /* д������ID */
    RSE_PUTPKT_SHORT(RSE_TYPE_CPU, ptr);

    /* д�����ID */
    RSE_PUTPKT_LONG(RSE_NULL_OBJECTID, ptr);

    /* д���־λ */
    *ptr = 0;
    ptr += 1;

    /* д��Ҫ��ʾ��ǩ */
    len = sprintf((INT8 *)ptr,"%s", RSE_CPU_STRING);
    ptr += (len+1);

    /* д��������� */
    *ptr = RSE_NULL_OPER;
    ptr += 1;

    /* д�����Ը��� */
    *ptr = RSE_CPU_INFO_ATTR_NUM;
    ptr += 1;

    /* д�����Ա� */
    RSE_PUTPKT_SHORT(RSE_ATTR_NAME, ptr);
    len = sprintf((INT8 *)ptr,"%s", RSE_CPU_STRING);
    ptr += (len+1);

    /* д�뵱ǰϵͳ�ĵ���ģʽ*/
    RSE_PUTPKT_SHORT(RSE_ATTR_DEBUG_MODE, ptr);
    if(DEBUGMODE_SYSTEM == taDebugModeGet())
    {
        len = sprintf((INT8 *)ptr,"%s", "system");
    }
    else
    {
    	len = sprintf((INT8 *)ptr,"%s", "task");
    }
    ptr += (len+1);

    /* д�뵱ǰϵͳʹ�ܵ�CPU���� */
    RSE_PUTPKT_SHORT(RSE_ATTR_ENABLED_CPU, ptr);
    len = sprintf((INT8 *)ptr,"%d", vxCpuConfiguredGet());
    ptr += (len+1);

    /* д�뵱ǰCPU״̬  */
    RSE_PUTPKT_SHORT(RSE_ATTR_STATE, ptr);
    len = sprintf((INT8 *)ptr,"%s", "running");
    ptr += (len+1);

    /* ��������������ݴ�С */
    *outSize = ptr - outbuf;
}
