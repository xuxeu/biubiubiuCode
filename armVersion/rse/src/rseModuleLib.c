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
 * @file  rseModuleLib.c
 * @brief
 *       ���ܣ�
 *       <li>�ں�ģ����Ϣ��ѯ</li>
 */

/************************ͷ �� ��******************************/
#include "rseConfig.h"
#include "moduleLib.h"
#include "rseCmdLib.h"
#include "rseInitLib.h"

/************************�� �� ��******************************/

/* ģ�����Ը��� */
#define RSE_MODULE_INFO_ATTR_NUM 7

/* ���ģ���� */
#define RSE_MAX_MODULE_NUM 1000

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/*
 * @brief
 *      �ṩģ����Ϣ����Ϣ����
 * @param[in] packet: ���յ������ݰ�
 * @return
 *      None
 */
static T_TA_ReturnCode rseModuleHandler(T_TA_Packet* packet);

/*
 * @brief
 *      ��ȡģ����Ϣ
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
static void rseGetAllModuleInfo(T_TA_Packet* packet, UINT32* outSize);

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/**
  * @brief
  *     ģ����Ϣ��ʼ������
  *@return
  *      None
  */
void rseModuleLibInit(void)
{
	rseRegisterModuleHandler(RSE_TYPE_IMAGE_MODULE, rseModuleHandler);
}

/*
 * @brief
 *      �ṩģ����Ϣ����Ϣ����
 * @param[in] packet: ���յ������ݰ�
 * @return
 *      TA_OK:�����ɹ�
 */
static T_TA_ReturnCode rseModuleHandler(T_TA_Packet* packet)
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
        	rseGetAllModuleInfo(packet, &outSize);
            break;
        default:
        	outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d", TA_INVALID_OPERATE_ID);
            break;
    }

    /* �������ݰ� */
    taCOMMSendPacket(packet, outSize);

    return TA_OK;
}

/*
 * @brief
 *      ��ȡģ����Ϣ
 * @param[in] packet: ���յ������ݰ�
 * @param[out] outSize: д�����ݰ����ֽ���
 * @return
 *      None
 */
static void rseGetAllModuleInfo(T_TA_Packet* packet, UINT32* outSize)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT8* ptr = NULL;
    UINT32 len = 0;
	MODULE_INFO moduleInfo;
	MODULE_ID moduleId;
	static UINT32 count = 0;
	static UINT32 idx = 0;
	static MODULE_ID moduleIdList[RSE_MAX_MODULE_NUM] = {0};

    /* �Ӱ��л�ȡʵ������ */
	inbuf = taGetPktUsrData(packet);

    /* �����������outbufΪinbuf */
	ptr = outbuf = inbuf;

    /* ��ȡģ��ID�б� */
    if(count == 0)
    {
    	count = moduleIdListGet(moduleIdList, RSE_MAX_MODULE_NUM);
    }

    /* ������ݰ� */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    while (idx < count)
    {
    	/* �ȼ��ʣ������Ƿ���Դ洢ģ����Ϣ */
    	RSE_CHECK_OVER_PKTSIZE(ptr,outbuf, RSE_MODULE_MAX_INFO_LEN);

        /* ���ûظ���� */
        len = sprintf((INT8 *)ptr,"%s","OK");
        ptr += (len+2);

        /* ȡ��ģ��ID */
    	moduleId = moduleIdList[idx++];

		/* д������ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_IMAGE_MODULE, ptr);

		/* ��ȡģ����Ϣ */
		moduleInfoGet(moduleId, &moduleInfo);

		/* д�����ID */
		RSE_PUTPKT_LONG((ulong_t)moduleId, ptr);

		/* д���־λ */
		*ptr = 0;
		ptr += 1;

		/* д����ʾ��ǩ */
		len = sprintf((INT8 *)ptr,"%s", moduleId->name);
		ptr += (len+1);

		/* д��������� */
		*ptr = RSE_NULL_OPER;
		ptr += 1;

		/* д�����Ը��� */
		*ptr = RSE_MODULE_INFO_ATTR_NUM;
		ptr += 1;

		/* д�����Ա�,����Ϊģ��ID��ģ������ģ���С��ģ����ʼ��ַ��ģ��״̬��ģ��TOSID��ģ�����͡�ģ��·�� */
		RSE_PUTPKT_SHORT(RSE_ATTR_ID, ptr);
		len = sprintf((INT8 *)ptr,"%x", moduleId->group);
		ptr += (len+1);

		/* д��ģ�������� */
		RSE_PUTPKT_SHORT(RSE_ATTR_NAME,ptr);
		len = sprintf((INT8 *)ptr,"%s", moduleId->name);
		ptr += (len+1);

		/* д��ģ���С���� */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_SIZE, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", (moduleInfo.segInfo.textSize + moduleInfo.segInfo.dataSize + moduleInfo.segInfo.bssSize));
	#else
		len = sprintf((INT8 *)ptr,"%x", (moduleInfo.segInfo.textSize + moduleInfo.segInfo.dataSize + moduleInfo.segInfo.bssSize));
	#endif
		ptr += (len+1);

		/* д��ģ����ʼ��ַ���� */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_START_ADDR, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", moduleInfo.segInfo.textAddr);
	#else
		len = sprintf((INT8 *)ptr,"%x", moduleInfo.segInfo.textAddr);
	#endif
		ptr += (len+1);

		/* д��ģ��״̬���� */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_STATE, ptr);
		len = sprintf((INT8 *)ptr,"%s", "module loaded on target - symbols loaded");
		ptr += (len+1);

		/* д��ģ��TOSID���� */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_TOSID, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", (ulong_t)moduleId);
	#else
		len = sprintf((INT8 *)ptr,"%x", (ulong_t)moduleId);
	#endif
		ptr += (len+1);

		/* д��ģ���������� */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_TYPE, ptr);
		len = sprintf((INT8 *)ptr,"%s", "Module");
		ptr += (len+1);

		/* д��ģ��·������ */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_PATH, ptr);
		len = sprintf((INT8 *)ptr,"%s", moduleId->path);
		ptr += (len+1);

		/* д��ģ��������ʼ��ַ���� */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_TEXT_ADDR, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", moduleInfo.segInfo.textAddr);
	#else
		len = sprintf((INT8 *)ptr,"%x", moduleInfo.segInfo.textAddr);
	#endif
		ptr += (len+1);
    }

    /* �����������Ϊ "$",��ʾ������Ϣ������� */
    if(idx == count)
    {
    	*ptr = RSE_END_STRING;
    	ptr += 1;
    	idx = count = 0;
    }

    /* ��������������ݴ�С */
    *outSize = ptr - outbuf;
}
