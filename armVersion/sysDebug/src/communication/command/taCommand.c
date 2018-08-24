/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * �޸���ʷ:
 * 2013-06-29         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ��� 
 */

/*
 * @file:taCommand.c
 * @brief:
 *             <li>TA��Ϣ�ӿڱ����ʵ��</li>
 */

/************************ͷ �� ��******************************/
#include <string.h>
#include "taCommand.h"
#include "taManager.h"
#include "db.h"
#include "ta.h"

/************************�� �� ��*******************************/

/************************���Ͷ���******************************/

/************************�ⲿ����******************************/

/* ��ѯ����� */
extern T_TA_CommandTable taManagerCommandTable[TA_MANAGER_COMMANDNUM];

/************************ǰ������******************************/

/************************ģ�����******************************/

/************************ȫ�ֱ���******************************/

/************************����ʵ��******************************/

/*
 * @brief:
 *      �������������ָ������
 * @param[in]:cmdTable: Ҫ�����������
 * @param[in]:cmdStr: �����ַ���
 * @param[in]:tableLength: �������
 * @return:
 *     NULL: δ�ҵ���Ӧ����
 *     �ҵ���Ӧ������������ݽṹָ��
 */
static T_TA_CommandTable* taSearchCommand(T_TA_CommandTable *cmdTable,
                                   const UINT8 *cmdStr,
                                   UINT32 tableLength)
{
    INT32 idx = 0;
    INT32 cmdIdx = -1;

    /* ���в����Ϸ��Լ�  */
    if (( NULL == cmdTable ) || (NULL == cmdStr ) || (0 == cmdStr[0]))
    {
        return (NULL);
    }

    /* ���α����������ƥ������ */
    while ((idx < (INT32)tableLength) && (NULL != cmdTable[idx].ubCmdStr))
    {
        /* ��Ҫ��ȫƥ��ؼ��� */
		if (0 == strncmp((const char *)cmdStr, (const char*)(cmdTable[idx].ubCmdStr), strlen(cmdTable[idx].ubCmdStr)))
		{
            /* ��¼��ѯ��Ԫ�ص������� */
            cmdIdx = idx;
			break;
		}
        idx++;
    }

    return ((cmdIdx == -1) ? NULL : &(cmdTable[cmdIdx]));
}

/*
 * @brief:
 *     ����ִ�е���������
 * @param[in]:   debugInfo: ���Դ���Ļ�����Ϣ
 * @param[in]:   inbuf:����Ĵ�����İ��壬�ް�ͷ
 * @param[in]:   inSize: ���Ĵ�С
 * @param[in]:   commandType: ��������
 * @param[in]:   cmdTable: ������� cmdTable �в�ѯ����
 * @param[out]:  outbuf: ������������İ��壬�ް�ͷ
 * @param[out]:  outSize: �������ظ����ݵ��ֽڳ���
 * @param[out]:  isNeedReply: �Ƿ���Ҫ�ظ�
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL: ����ִ��ʧ��
 *     TA_DB_NEED_IRET����Ҫ���쳣�з���
 *     TA_COMMAND_NOTFOUND: ����û�ҵ�
 */
T_TA_ReturnCode taProcessCommand(T_DB_Info *debugInfo,
                                 UINT8 *inbuf,
                                 UINT32 inSize,
                                 UINT8 *outbuf,
                                 UINT32 *outSize,
                                 BOOL *isNeedReply,
                                 T_TA_CommandType commandType,
                                 T_TA_CommandTable *cmdTable,
                                 UINT32 tableLength)
{
	UINT32 ret = TA_OK;
	T_TA_CommandTable *cmdHandle = NULL;

    /* ��������cmdTableΪ�գ�����Ĭ��������в�ѯ */
    if (NULL == cmdTable)
    {
        switch (commandType)
        {
			/* ����ǲ�ѯ�����Ӳ�ѯ���в���*/
			case COMMANDTYPE_QUERY:
			{
				/* ����Ϣ��ѯ�����������ָ������ */
				cmdHandle = taSearchCommand(taManagerCommandTable, (const UINT8 *)inbuf, TA_MANAGER_COMMANDNUM);
			}
            break;
			case COMMANDTYPE_DEBUG:
			{
				/* ��RSP�����������ָ������ */
				cmdHandle = taSearchCommand(taRSPCmdTable, (const UINT8 *)inbuf, TA_RSP_TABLE_LEN);
			}
			break;
        default:
            break;
        }
    }
    else
    {
        /* ���ṩ�������������в�ѯ */
        cmdHandle = taSearchCommand(cmdTable, (const UINT8 *)inbuf,tableLength);
    }

    /* δ�ҵ���Ӧ���� */
    if (NULL == cmdHandle)
    {
        *isNeedReply = TA_NEED_REPLY;

        return (TA_COMMAND_NOTFOUND);
    }

    /* ������������������������ */

    ret = cmdHandle->cmdProc(debugInfo, inbuf, inSize, outbuf, outSize);

    /* ���ûظ���־ */
    *isNeedReply = cmdHandle->ubNeedReply;

    return (ret);
}
