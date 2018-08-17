/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-05-26         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  mdlCmdLib.c
 * @brief
 *       ���ܣ�
 *       <li>��̬����ģ�����</li>
 */

/************************ͷ �� ��******************************/
#include "mdlInitLib.h"
#include "mdlCmdLib.h"
#include "mdlLib.h"
#include "loadLib.h"
#include "symLib.h"
#include "sysSymTbl.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/*
 * @brief:
 *      ����ģ��,�����ʽ"ml:<modulePath>:textAdd:textSize:dataAdd:dataSize:bssAdd:bssSize"
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode LoadModule(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);
/*
 * @brief:
 *      ж��ģ��,�����ʽ"mu:moduleID"
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode UnloadModule(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      ����ģ���ڴ�,�����ʽ"ma:fileSize",������ͨ���������ȡģ���ŵ�ַ��
 *      Ȼ����ݸõ�ַ���ӳɾ��Զ�λ��ELF
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode AllocateModuleMemory(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);
/*
 * @brief:
 *      �ͷ�ģ���ڴ�,�����ʽ"mf:address",һ�����ڷ���ģ���ڴ��
 *      �����������ӳ�ELF�����г����������Ҫ���͸������ͷ�Ŀ����˷�����ڴ�
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode FreeModuleMemory(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      ���ģ����ŵ�ϵͳ���ű���,�����ʽΪ"ms:<symbolName>:symbolValue:symbolType:groupId:moduleId"
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode AddModuleSymbol(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      ͬ��ģ�����,�����ʽΪ"mg:moduleId",���ģ��IDΪ0��ʾ��ȡ�ں�ӳ��ģ����ŷ���ΪDKM��Ŀģ�����
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode SyncModuleSymbol(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      ��ȡ����ģ����Ϣ,�����ʽΪ"mi"�����ģ����ͨ������ӷ���ʧ�Դ洢���ʼ��أ�����ʱIDE��Ҫͨ��ģ��
 *      ��Ϣ���ӳɾ�̬��ELF�ļ�
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode GetAllModuleInfo(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      ִ��ģ�����,�����ʽΪ"mc:funcAdd:priority:size:options:arg...."
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode CallModuleFunc(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      ִ��ģ��C++�Ĺ��캯��,�����ʽΪ"mp:moduleId"
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode CallCplusplusCtors(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/************************ģ�����*****************************/

/* ��ѯ����� */
static T_TA_CommandTable mdlModuleCommandTable[MDL_MODULE_COMMAND_NUM] =
{
	{"ml",  LoadModule,            TA_NEED_REPLY},/* ����ģ�� */
    {"mu",  UnloadModule,          TA_NEED_REPLY},/* ж��ģ�� */
    {"mf",  FreeModuleMemory,      TA_NEED_REPLY},/* ģ���ڴ��ͷ� */
    {"ma",  AllocateModuleMemory,  TA_NEED_REPLY},/* ����ģ���ڴ� */
    {"ms",  AddModuleSymbol,       TA_NEED_REPLY},/* ���ģ����� */
    {"mg",  SyncModuleSymbol,      TA_NEED_REPLY},/* ͬ��ģ����� */
    {"mi",  GetAllModuleInfo,      TA_NEED_REPLY},/* ��ȡ����ģ����Ϣ */
    {"mc",  CallModuleFunc,        TA_NEED_REPLY},/* ģ�麯��ִ�� */
    {"mp",  CallCplusplusCtors,    TA_NEED_REPLY},/* ģ��Ĺ��캯��ִ�� */
    {NULL,  NULL,                  TA_NO_REPLY  }
};

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 * 		��̬���ش�����Ϣ��������
 * @param[in]:packet: Ҫ�������Ϣ��
 * @return:
 *     TA_OK: ����ִ�гɹ�
 *     TA_FAIL������ִ��ʧ��
 */
static T_TA_ReturnCode mdlModuleCommandProc(T_TA_Packet *packet)
{
    /* ����taAgentEntry�����յ�����Ϣ�� */
    return (taAgentEntry(packet,NULL,COMMANDTYPE_NONE,mdlModuleCommandTable,
    		MDL_MODULE_COMMAND_NUM,NULL,NULL));
}

/*
 * @brief:
 *     ��װ��̬������Ϣ������
 * @return:
 *     ��
 */
void mdlModuleCommandInit(void)
{
	/* ��װ��̬������Ϣ������ */
	taInstallMsgProc(TA_DYNAMIC_LOADER, mdlModuleCommandProc);
}

/*
 * @brief:
 *      ж��ģ��,�����ʽ"mu:moduleID"
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode UnloadModule(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    ulong_t moduleId = 0;
    STATUS status = OK;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[2];

#ifdef CONFIG_TA_LP64
    /* ��"mu:moduleID"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int64(&ptr,&moduleId))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#else
    /* ��"mu:moduleID"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&moduleId))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#endif


    /* ж��ģ�� */
    status = mdlUnloadModule((MODULE_ID)moduleId);
    if (OK != status)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_UNLOAD_MODULE_FAIL);
        return (TA_FAIL);
    }

    /* �����������Ϊ "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return(TA_OK);
}

/*
 * @brief:
 *      ִ��ģ��C++�Ĺ��캯��,�����ʽΪ"mp:moduleId"
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode CallCplusplusCtors(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    ulong_t moduleId = 0;
    STATUS status = OK;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[2];

#ifdef CONFIG_TA_LP64
    /* ��"mp:moduleID"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int64(&ptr,&moduleId))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#else
    /* ��"mp:moduleID"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&moduleId))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#endif


    /* ���ģ���Ƿ���Ч */
    if (NULL == mdlFigureModuleId((MODULE_ID)moduleId))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_MODULE_ID);
        return (TA_FAIL);
    }

    /* ж��ģ�� */
    status = mdlCplusCallCtors((MODULE_ID)moduleId);
    if (OK != status)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* �����������Ϊ "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      ����ģ���ڴ�,�����ʽ"ma:fileSize",������ͨ���������ȡģ���ŵ�ַ��
 *      Ȼ����ݸõ�ַ���ӳɾ��Զ�λ��ELF
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode AllocateModuleMemory(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 allocSize = 0;
    void *allocAddr = NULL;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[2];

    /* ��"ma:fileSize"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&allocSize))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* ����ģ���ڴ� */
    allocAddr = mdlAllocateModuleMem(MDL_ALLOC_ALIGN_SIZE, allocSize);
    if (NULL == allocAddr)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_ALLOC_MODULE_MEM_FAIL);
        return (TA_FAIL);
    }

    /* �����������Ϊ "OK:<allocAddr>" */
#ifdef CONFIG_TA_LP64
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%lx",(ulong_t)allocAddr);
#else
	*outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%x",(ulong_t)allocAddr);
#endif

    return (TA_OK);
}

/*
 * @brief:
 *      �ͷ�ģ���ڴ�,�����ʽ"mf:address",һ�����ڷ���ģ���ڴ��
 *      �����������ӳ�ELF�����г����������Ҫ���͸������ͷ�Ŀ����˷�����ڴ�
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode FreeModuleMemory(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    ulong_t address = 0;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[2];

#ifdef CONFIG_TA_LP64
    /* ��"mf:address"��ʽ����������� */
    if (!((*(ptr++) == ':') && (0 != hex2int64(&ptr,&address))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#else
	/* ��"mf:address"��ʽ����������� */
	if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&address))))
	{
		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
		return (TA_FAIL);
	}

#endif

    /* �ͷ�ģ���ڴ� */
    mdlFreeModuleMem((void *)address);

    /* �����������Ϊ "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      ����ģ��,�����ʽ"ml:<modulePath>:textAdd:textSize:dataAdd:dataSize:bssAdd:bssSize"
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode LoadModule(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    ulong_t textAdd = 0;
	ulong_t textSize = 0;
    ulong_t dataAdd = 0;
    ulong_t dataSize = 0;
    ulong_t bssAdd = 0;
    ulong_t bssSize = 0;
    char modulePath[PATH_MAX] = {0};
    char *pModulePath = modulePath;
    MODULE_ID moduleId;
    STATUS status = OK;
    MDL_MOD_INFO moduleInfo;
    const UINT8 *ptr = NULL;

    /* ������������������� */
    ptr = &inbuf[2];

    /* ��"ml:<modulePath>:textAdd:textSize:dataAdd:dataSize:bssAdd:bssSize"��ʽ����������� */
#ifdef CONFIG_TA_LP64
    if (!((*(ptr++) == ':')
    	&& (0 != hex2str(&ptr, (UINT8 **)&pModulePath, PATH_MAX))
    	&& (*(ptr++) == ':')
    	&& (0 != hex2int64(&ptr,&textAdd))
        && (*(ptr++) == ':' )
        && (0 != hex2int64(&ptr, &textSize))
        && (*(ptr++) == ':' )
    	&& (0 != hex2int64(&ptr,&dataAdd))
        && (*(ptr++) == ':' )
        && (0 != hex2int64(&ptr, &dataSize))
        && (*(ptr++) == ':' )
    	&& (0 != hex2int64(&ptr,&bssAdd))
        && (*(ptr++) == ':' )
        && (0 != hex2int64(&ptr, &bssSize))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#else
	if (!((*(ptr++) == ':')
		&& (0 != hex2str(&ptr, (UINT8 **)&pModulePath, PATH_MAX))
		&& (*(ptr++) == ':')
		&& (0 != hex2int(&ptr,&textAdd))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr, &textSize))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr,&dataAdd))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr, &dataSize))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr,&bssAdd))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr, &bssSize))))
	{
		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
		return (TA_FAIL);
	}
#endif

    /* ����ģ����Ϣ */

    moduleInfo.name = modulePath;
    moduleInfo.flags = LOAD_GLOBAL_SYMBOLS;

    /* ���ô������Ϣ��SEG_FREE_MEMORY��ʾɾ����ʱ�ͷŸö��ڴ�  */
    moduleInfo.segment[0].address = textAdd;
    moduleInfo.segment[0].size = textSize;
    moduleInfo.segment[0].type = SEGMENT_TEXT;
    moduleInfo.segment[0].flags = SEG_FREE_MEMORY;

    /* �������ݶ���Ϣ��SEG_FREE_MEMORY��ʾɾ����ʱ�ͷŸö��ڴ� */
    moduleInfo.segment[1].address = dataAdd;
    moduleInfo.segment[1].size = dataSize;
    moduleInfo.segment[1].type = SEGMENT_DATA;
    moduleInfo.segment[1].flags = 0;
	/*flags������Ҫ��Ϊ0���������ΪSEG_FREE_MEMORY����ôϵͳ���ͷ��ڴ�ʱ�����ͷ�һ�Σ�Ȼ��ϵͳ�ڷ����ڴ�ʱ��ͳһ��seg��data��bss�η����ڴ�ģ�����ֻ��Ҫ�ͷ�һ�ξ�����*/

    /* ����BSS����Ϣ��SEG_FREE_MEMORY��ʾɾ����ʱ�ͷŸö��ڴ� */
    moduleInfo.segment[2].address = bssAdd;
    moduleInfo.segment[2].size = bssSize;
    moduleInfo.segment[2].type = SEGMENT_BSS;
    moduleInfo.segment[2].flags = 0;

    /* ����ģ�� */
    status = mdlLoadModule(&moduleInfo, &moduleId);
    if (OK != status)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_LOAD_MODULE_FAIL);
        return (TA_FAIL);
    }

    /* ���ûظ���ϢΪ"OK:moduleId:groupId" */
#ifdef CONFIG_TA_LP64
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%lx:%x",(ulong_t)moduleId, moduleId->group);
#else
	*outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%x:%x",(ulong_t)moduleId, moduleId->group);
#endif


    return (TA_OK);
}

/*
 * @brief:
 *      ���ģ����ŵ�ϵͳ���ű���,�����ʽΪ"ms:<symbolName>:symbolValue:symbolType:groupId:moduleId"
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode AddModuleSymbol(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
	ulong_t symbolValue = 0;
	UINT32 symbolType = 0;
	UINT32 groupId = 0;
	ulong_t moduleId = 0;
	SYMBOL_ID pSymbol = NULL;
	const UINT8 *ptr = NULL;
    char symbolName[PATH_MAX] = {0};
    char *pSymbolName = symbolName;

	/* ������������������� */
	ptr = &inbuf[2];

	/* ��"ms:<symbolName>:symbolValue:symbolType:groupId:moduleId"��ʽ����������� */
#ifdef CONFIG_TA_LP64
	if ( !((*(ptr++) == ':')
		&& (0 != hex2str(&ptr, (UINT8 **)&pSymbolName, PATH_MAX))
		&& (*(ptr++) == ':')
		&& (0 != hex2int64(&ptr,&symbolValue))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr, &symbolType))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr,&groupId))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int64(&ptr,&moduleId))))
	{
		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
		return (TA_FAIL);
	}
#else
	if ( !((*(ptr++) == ':')
		&& (0 != hex2str(&ptr, (UINT8 **)&pSymbolName, PATH_MAX))
		&& (*(ptr++) == ':')
		&& (0 != hex2int(&ptr,&symbolValue))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr, &symbolType))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr,&groupId))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr,&moduleId))))
	{
		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
		return (TA_FAIL);
	}

#endif

	/* ע��ģ����ŵ�ϵͳ���ű� */
	pSymbol = mdlRegisterModuleSym(symbolName, (char*)symbolValue, (SYM_TYPE)symbolType, (UINT16)groupId, moduleId);
    if (NULL == pSymbol)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_ADD_SYMBOL_FAIL);
        return (TA_FAIL);
    }

    /* �����������Ϊ "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return(TA_OK);
}

/*
 * @brief:
 *      ͬ��ģ�����,�����ʽΪ"mg:moduleId",���ģ��IDΪ0��ʾ��ȡ�ں�ģ�����з��ŷ���ΪDKM��Ŀģ����ţ�
 *      ����ͨ���������ȡĿ����˷��ţ����ڷ��ŵ��Ժ�����DKMģ�飬��ΪDKMģ����ܴ�������ⲿ���ӷ��ţ�
 *      ���Ŷ�λ�÷��ŵ�ַ��
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode SyncModuleSymbol(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    ulong_t moduleId = 0;
	UINT32 len = 0;
    const UINT8 *ptr = NULL;
    UINT8 *pTemp = outbuf;
    SYMBOL *pSymbol = NULL;
    static UINT32 count = 0;
	static UINT32 index = 0;

    /* ������������������� */
    ptr = &inbuf[2];
#ifdef CONFIG_TA_LP64
    /* ��"mg:moduleId"��ʽ����������� */
    if (!((*(ptr++) == ':') && 0 != hex2int64(&ptr,&moduleId)))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#else
	/* ��"mg:moduleId"��ʽ����������� */
    if (!((*(ptr++) == ':') && 0 != hex2int(&ptr,&moduleId)))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }
#endif

    /* ���ģ���Ƿ����,���ģ��IDΪ0��ʾ��ȡ�ں�ӳ��ģ�����,������ģ���Ƿ���� */
    if ((NULL == mdlFigureModuleId((MODULE_ID)moduleId)) && (moduleId != 0) )
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* ���countΪ0��ʾ��ȡ���з���ID */
    if (count == 0)
    {
    	count = mdlGetModuleSymIdList((MODULE_ID)moduleId);
    }

    /* ���ûظ���� */
    len = sprintf((INT8*)pTemp,"%s","OK");
    pTemp += len;

    while (index < count)
    {
    	if (((pTemp) - (outbuf) + MDL_SYMBOL_MAX_INFO_LEN) > TA_PKT_DATA_SIZE)
    	{
    		break;
    	}

    	/* ��ȡ������Ϣ */
    	pSymbol = mdlModuleSymbol[index++];

    	/* ���ûظ���ϢΪ"OK,name,val,type,group" */
	#ifdef CONFIG_TA_LP64
    	len = (UINT32)sprintf((INT8*)pTemp, ":%s,%lx,%x,%x", pSymbol->name, pSymbol->value, pSymbol->type, pSymbol->group);
	#else
		len = (UINT32)sprintf((INT8*)pTemp, ":%s,%x,%x,%x", pSymbol->name, pSymbol->value, pSymbol->type, pSymbol->group);
	#endif

    	pTemp += len;
    }

    /* �����������Ϊ "$",��ʾ������Ϣ������� */
    if (index == count)
    {
    	*pTemp = '$';
    	pTemp += 1;
    	index = count = 0;
    	mdlFreeModuleMem(mdlModuleSymbol);
    }

    /* ��������������ݴ�С */
    *outSize = pTemp - outbuf;

    return (TA_OK);
}

/*
 * @brief:
 *      ��ȡ����ģ����Ϣ,�����ʽΪ"mi"
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode GetAllModuleInfo(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
	static MODULE_ID moduleIdList[MDL_MAX_MODULE_ID_NUM] = {0};
	static UINT32 count = 0;
	static UINT32 index = 0;
	UINT8* ptr = outbuf;
	UINT32 len = 0;
	MODULE_ID moduleId = 0;
	MODULE_INFO moduleInfo;

    /* ��ȡģ��ID�б�  */
    if (count == 0)
    {
    	/* ��ȡ����ģ��ID */
    	count = moduleIdListGet(moduleIdList, MDL_MAX_MODULE_ID_NUM);
    }

    /* ���ûظ���� */
    len = sprintf((INT8*)ptr,"%s","OK");
    ptr += len;

    while (index < count)
    {
    	if (((ptr) - (outbuf) + MDL_MODULE_MAX_INFO_LEN) > TA_PKT_DATA_SIZE)
    	{
    		break;
    	}

		/* ����ģ��ID */
		moduleId = moduleIdList[index++];

		/* ��ȡģ����Ϣ */
		mdlGetModuleInfo(moduleId, &moduleInfo);

		/* ����ģ����ϢΪ"OK:moduleID,groupID,modulePath,textAdd,dataAdd,bssAddr,textSize,dataSize,bbsSize" */
	#ifdef CONFIG_TA_LP64
		len = (UINT32)sprintf((INT8*)ptr, ";%lx,%x,%s,%s,%lx,%lx,%lx,%lx,%lx,%lx",(ulong_t)moduleId, moduleId->group,moduleId->name,moduleId->path,
						(ulong_t)moduleInfo.segInfo.textAddr,(ulong_t)moduleInfo.segInfo.dataAddr,(ulong_t)moduleInfo.segInfo.bssAddr,
						(ulong_t)moduleInfo.segInfo.textSize,(ulong_t)moduleInfo.segInfo.dataSize,(ulong_t)moduleInfo.segInfo.bssSize);
	#else
		len = (UINT32)sprintf((INT8*)ptr, ";%x,%x,%s,%s,%x,%x,%x,%x,%x,%x",(ulong_t)moduleId, moduleId->group,moduleId->name,moduleId->path,
						moduleInfo.segInfo.textAddr, moduleInfo.segInfo.dataAddr, moduleInfo.segInfo.bssAddr,
						moduleInfo.segInfo.textSize,moduleInfo.segInfo.dataSize,moduleInfo.segInfo.bssSize);
	#endif

		ptr += len;
    }

    /* �����������Ϊ "$",��ʾģ����Ϣ������� */
    if (index == count)
    {
    	*ptr = '$';
    	ptr += 1;
    	index = count = 0;
    }

    /* ��������������ݴ�С */
    *outSize = ptr - outbuf;

    return (TA_OK);
}

/*
 * @brief:
 *      ִ��ģ�����,�����ʽΪ"mc:funcAdd:priority:size:options:arg...."
 * @param[in]:debugInfo:���ԻỰ��Ϣ
 * @param[in]:inbuf:����������������ݻ���
 * @param[in]:inSize:�����������ݳ���
 * @param[out]:outbuf:�洢������������ݻ���
 * @param[out]:outSize:�洢����������ݳ���
 * @return:
 *      TA_OK: �����ɹ�
 *      TA_FAIL:ִ��ʧ��
 */
static T_TA_ReturnCode CallModuleFunc(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
	MDL_CREATE_DESC mdlCreateDesc;
	ulong_t textStart = 0;
	ulong_t status = OK;

	/* ���������� */
    if (!mdlCtxCreateDesc(inbuf, &mdlCreateDesc))
    {
    	 *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
    	 return (TA_FAIL);
    }

    /* �������͵��ò�ͬ�Ĵ������� */
    if (MDL_CRT_TASK == mdlCreateDesc.contextType)
    {
    	status = mdlCreateTask(&mdlCreateDesc);
    }
    else if (MDL_CRT_RTP == mdlCreateDesc.contextType)
    {
    	status = mdlCreateRtp(&mdlCreateDesc,&textStart);
    }
    else
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
    	return (TA_FAIL);
    }

    /* ִ��ģ�麯�����ߴ���RTPʧ�� */
    if (ERROR == status)
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
    	return (TA_FAIL);
    }

    /* ����������� */
#ifdef CONFIG_TA_LP64
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%lx:%lx",textStart,status);
#else
	*outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%x:%x",textStart,status);
#endif

    return (TA_OK);
}
