/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-07-2         ��Ԫ־�������������ɼ������޹�˾
 *                           �������ļ���
 */

/**
 * @file  mdlLib.c
 * @brief
 *       ���ܣ�
 *       <li>��̬����ģ�����</li>
 */

/************************ͷ �� ��******************************/
#include "mdlInitLib.h"
#include "mdlLib.h"
#include "mdlCmdLib.h"
#include "unldLib.h"
#include "loadLib.h"
#include "sysSymTbl.h"
#include "moduleLib.h "
#include "symLibP.h"
#include "loadShareLibP.h"
#include "rtpLibCommon.h "
#include "rtpLibP.h"
#include "cplusLoadP.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* ģ������� */
static int mdlSymbolNumber = 0;

/* ���ģ�����ID */
SYMBOL **mdlModuleSymbol = NULL;
/************************ȫ�ֱ���*****************************/

/* ��ʼ������ID�����ڵ���ʱ���г�ʼ������ */
int taInitTaskId = 0;

/************************����ʵ��*****************************/

/*
 * @brief
 *    ����ģ��
 * @param[in]: pModInfo: ģ����Ϣ
 * @param[out]: pModuleId: ģ��ID
 * @return
 *    ERROR������ģ��ʧ��
 *    OK������ģ��ɹ�
 */
STATUS mdlLoadModule(MDL_MOD_INFO *pModInfo, MODULE_ID *pModuleId)
{
	MODULE_ID moduleId;
	STATUS status = OK;
	int i = 0;

	/* ����ģ�� */
	moduleId = moduleCreate((char*)pModInfo->name, MODULE_ELF, pModInfo->flags);
	if (NULL == moduleId)
	{
		return (ERROR);
	}

	/* ���ģ�����Ϣ */
    for (i = 0; i < MDL_MOD_NUM_SEGS; i++)
    {
        status = moduleSegAdd(moduleId, pModInfo->segment[i].type,
    						(void *)pModInfo->segment[i].address,
    						pModInfo->segment[i].size, pModInfo->segment[i].flags);
    	if (OK != status)
    	{
    		return (ERROR);
    	}
    }

    /* ��¼ģ��ID */
    *pModuleId = moduleId;

    return (OK);
}

/*
 * @brief
 *    ж��ģ��
 * @param[in]: moduleId: ģ��ID
 * @return
 *    ERROR��ж��ģ��ʧ��
 *    OK��ж��ģ��ɹ�
 */
STATUS mdlUnloadModule(MODULE_ID moduleId)
{
    /* ����loader���ģ��ж�ؽӿ�ж��ģ�� */
    return unldByModuleId(moduleId, UNLD_FORCE | UNLD_CPLUS_XTOR_AUTO);
}

/*
 * @brief
 *    �ͷ�ģ���ڴ�
 * @param[in]: address: ģ���ڴ��ַ
 * @return
 *    ��
 */
void mdlFreeModuleMem(void *address)
{
	/* �ͷ��ڴ� */
	free(address);
}

/*
 * @brief
 *    ����ģ���ڴ�
 * @param[in]: alignment: �����ڴ�����С
 * @param[in]: size: ������ڴ��С
 * @return
 *    NULL������ģ���ڴ�ʧ��
 *    �ڴ��ַ������ģ���ڴ�ɹ�
 */
void* mdlAllocateModuleMem(UINT32 alignment,UINT32 size)
{
	/* ����ϵͳ���ڴ����ӿڷ����ڴ� */
	void * addr = loadShareTgtMemAlign(MDL_ALLOC_ALIGN_ADDRESS,MDL_ROUND_UP(size, alignment));

	/* ����ڴ� */
	loadShareTgtMemSet(addr, 0, MDL_ROUND_UP(size, alignment));

	return (addr);
}

/*
 * @brief
 *    ���ģ���Ƿ����
 * @param[in]: moduleId: ģ��ID
 * @return
 *    NULL��ģ�鲻����
 *    ģ��ID��ģ�����
 */
MODULE_ID mdlFigureModuleId(MODULE_ID moduleId)
{
	/* ���ģ���Ƿ���� */
	return moduleIdFigure(moduleId);
}

/*
 * @brief
 *    ע��ģ����ŵ�ϵͳ���ű�
 * @param[in]: symbolName: ������
 * @param[out]: symbolValue: ����ֵ
 * @param[out]: symbolType: ��������
 * @param[out]: groupId: ��ID
 * @param[out]: symRef: �������ã�һ��Ϊģ��ID
 * @return
 *    ERROR������ע��ʧ��
 *    OK������ע��ɹ�
 */
SYMBOL_ID mdlRegisterModuleSym(char *symbolName, char *symbolValue, SYM_TYPE symbolType, UINT16 groupId, UINT32 symRef)
{
	return symRegister(sysSymTbl, symbolName, symbolValue, symbolType, groupId, symRef, FALSE);
}

/*
 * @brief
 *    ��ȡ���ķ��Ÿ���
 * @return
 *    ���ķ��Ÿ���
 */
UINT32 mdlGetSymbolsNum(void)
{
	return sysSymTbl->nsymbols;
}

/*
 * @brief
 *    ��ȡģ�����ID
 * @param[in]: name: ������
 * @param[out]: val: ����ֵ
 * @param[out]: type: ��������
 * @param[out]: info: ƥ����Ϣ
 * @param[out]: group: ��ID
 * @param[out]: pSymbol: ���Žڵ�ID
 * @return
 *    TRUE������ִ��
 *    FALSE��������ִ��
 */
BOOL mdlGetModuleSymId(char *name, int val, SYM_TYPE type, MDL_MATCH_INFO *info, UINT16 group, SYMBOL *pSymbol)
{
	/* ���Ų����ڸ�ģ�� */
    if (pSymbol->symRef != (UINT32)info->moduleId)
	{
    	return (TRUE);
	}

    /* ���洢ģ�����ID���ڴ��ַ�Ƿ�ΪNULL */
    if (NULL == info->moduleSymbol)
    {
    	return (FALSE);
    }

    /* ���÷���ID */
    (info->moduleSymbol)[mdlSymbolNumber++] = pSymbol;

    return (TRUE);
}

/*
 * @brief
 *    ��ȡģ�����ID�б�
 * @param[in]: moduleId: ģ��ID
 * @return
 *    ģ�������
 */
UINT32 mdlGetModuleSymIdList(MODULE_ID moduleId)
{
    MDL_MATCH_INFO matchInfo;
    UINT32 symbolNum = 0;

    /* ����ģ�������Ϊ0 */
    mdlSymbolNumber = 0;

	/* ����ģ��ID */
	matchInfo.moduleId = moduleId;

	/* ��ȡ����ģ��ķ��Ÿ��� */
	symbolNum = mdlGetSymbolsNum();

	/* �����ڴ棬��ŷ���ID  */
	mdlModuleSymbol = (SYMBOL **)mdlAllocateModuleMem(MDL_ALLOC_ALIGN_SIZE, symbolNum * sizeof(SYMBOL*));

	/* ���ô�ŷ��ŵ�ַ�ĵ�ַ */
	matchInfo.moduleSymbol = mdlModuleSymbol;

	/* ��ȡ���з���ID  */
	symEach(sysSymTbl, (FUNCPTR)mdlGetModuleSymId, (int)&matchInfo);

	return (mdlSymbolNumber);
}

/*
 * @brief
 *    ��ȡģ����Ϣ
 * @param[in]: moduleId: ģ��ID
 * @param[out]: pModuleInfo: �洢ģ����Ϣ
 * @return
 *    ERROR��ģ��ID��Ч
 *    OK����ȡģ����Ϣ�ɹ�
 */
STATUS mdlGetModuleInfo(MODULE_ID moduleId, MODULE_INFO *pModuleInfo)
{
	/* ��ȡģ����Ϣ */
	return moduleInfoGet(moduleId, pModuleInfo);
}

/*
 * @brief
 *    ����ģ���C++���캯��
 * @param[in]: moduleId: ģ��ID
 * @return
 *    ERROR������ʧ��
 *    OK�����سɹ�
 */
STATUS mdlCplusCallCtors(MODULE_ID moduleId)
{
	return cplusLoadFixup(moduleId, sysSymTbl, XTOR_CALL);
}

/*
 * @brief
 *    ��������MDL_CREATE_DESC�ṹ��
 * @param[in]: inbuf: ��������
 * @param[out]: desc: ����������
 * @return
 *    TRUE�������ɹ�
 *    FALSE������ʧ��
 */
BOOL mdlCtxCreateDesc(const UINT8 *inbuf, MDL_CREATE_DESC *desc)
{
	const UINT8 *ptr = NULL;
	char *pTmpStr = NULL;
	char *pTmpStr1 = NULL;
	char *pTmpStr2 = NULL;
	char **rtpArgs = NULL;
	int count = 0;
    char *pName = desc->name;
    int i = 0;
#ifdef __X86__
    char *pArgBuf = NULL; //ָ��������bufferָ��
    int  offset = 0; //ƫ����
#endif
	/* ������������������� */
    ptr = &inbuf[2];

    /* �������� */
    if (!((*(ptr++) == ':')
    	&& (0 != hex2int(&ptr,&desc->contextType))
    	&& (*(ptr++) == ':')
    	&& (0 != hex2str(&ptr, (UINT8 **)&pName, PATH_MAX))
    	&& (*(ptr++) == ':')
    	&& (0 != hex2int(&ptr,&desc->redirIn))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr, &desc->redirOut))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&desc->redirErr))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&desc->priority))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&desc->stackSize))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&desc->options))))
    {
        return (FALSE);
    }

    /* �������ͽ������� */
    switch (desc->contextType)
    {
    	case MDL_CRT_TASK:
#ifdef __MIPS__
            if (!((*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.task.entry)))
    	    	&& (*(ptr++) == ':' )
    	    	&& (0 != hex2int(&ptr, &(desc->crtSpecific.task.numArgs)))))
    	    {
    	        return (FALSE);
    	    }

    	    /* ������������ */
    	    for (i = 0; i < desc->crtSpecific.task.numArgs; i++)
    	    {
    	    	if (!((*(ptr++) == ':') && hex2int(&ptr,&(desc->crtSpecific.task.arg[i]))))
    	    	{
    	    		 return (FALSE);
    	    	}
    	    }
#else
    		pArgBuf = (char*)malloc(sizeof(char)*STRING_BUFF_LEN);
    		UINT32 tmp = 0;
    		memset(pArgBuf,0,STRING_BUFF_LEN);

    	    if (!((*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.task.entry)))
    	    	&& (*(ptr++) == ':' )
    	    	&& (0 != hex2int(&ptr, &(desc->crtSpecific.task.numArgs)))))
    	    {
    	        return (FALSE);
    	    }

    	    /* ������������ */
    	    for (i = 0; i < desc->crtSpecific.task.numArgs; i++)
    	    {

			    desc->crtSpecific.task.arg[i] = (int)(pArgBuf+offset);

			    if (*ptr == ':')
			    {
			      ptr++;
				}
				else if(*ptr == '\0')
				{
						  break;
				}

				while((*ptr != ':')&&(*ptr != '\0'))
				{
					*(pArgBuf+offset++) = *ptr++;
				}

			    *(pArgBuf+offset++) = '\0';//����������š�
    	    }

    	    //ת������
    	    for (i = 0; i < desc->crtSpecific.task.numArgs; i++)
    	    {
    	    	char *ptr = (char*)(desc->crtSpecific.task.arg[i]);
    	    	char *ptrSave = ptr;
                if(*ptr != '"')
                {
                	//������0x��
                	if((*ptr == '0')&&((*(ptr+1) == 'x')||(*(ptr+1) == 'X')))
                	{
                        ptr = ptr+2;
                        ptrSave = ptr;
                	}
                	if(hex2int(&ptr,&tmp) == strlen(ptrSave)) 
                	{
                		desc->crtSpecific.task.arg[i] = tmp;
                	}
                }
                else
                {
                	//ȥ��˫����
                	desc->crtSpecific.task.arg[i] = (int)(ptr+1);
                	*(pArgBuf+strlen(ptr)-1) = '\0';
                }
    	    }
#endif
    		break;
    	case MDL_CRT_RTP:
    		/* ����RTP���� */
    	    if (!((*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.rtp.options)))
    	    	&& (*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.rtp.numArgv)))
    	    	&& (*(ptr++) == ':')))
    	    {
    	        return (FALSE);
    	    }

    	    /* ���ò������� */
    	    count = desc->crtSpecific.rtp.numArgv;

    	    /* �����ڴ� */
    	    rtpArgs = (char **)mdlAllocateModuleMem(sizeof(char*), ((count+1) * sizeof(char*)));

    	    /* ���ò�����ַ */
    	    desc->crtSpecific.rtp.argv = rtpArgs;

    	    /* ���Ҳ����ַ��� */
    	    pTmpStr1 = strchr((const char*)ptr, '<');
    	    pTmpStr2 = strchr((const char*)ptr, '>');
    	    if ((pTmpStr1 == NULL) || (pTmpStr2 == NULL))
    	    {
    	    	return (FALSE);
    	    }

    	    pTmpStr2[0] = EOS;
    	    pTmpStr1++;

    	    /* �������� */
    	    for (i = 0; i < count; i++)
    	    {
    	    	rtpArgs[i] = pTmpStr1;
    	    	pTmpStr = strchr(pTmpStr1, ' ');
    	    	if(pTmpStr == NULL || (pTmpStr > pTmpStr2))
    	    	{
        	    	break;
    	    	}

    	    	pTmpStr[0] = EOS;
    	    	pTmpStr1 = ++pTmpStr;
    	    }

    	    /* �����Կս��� */
    	    rtpArgs[count] = NULL;

    	    ptr = (const UINT8 *)(++pTmpStr2);

    	    /* ������������ */
    	    if (!((*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.rtp.numEnvp)))
    	    	&& (*(ptr++) == ':')))
    	    {
    	        return (FALSE);
    	    }

    	    /* ���û����������� */
    	    count = desc->crtSpecific.rtp.numEnvp;

    	    /* ���价�������ڴ� */
    	    rtpArgs = (char **)mdlAllocateModuleMem(sizeof(char*), ((count+1) * sizeof(char*)));

    	    /* �����ڴ� */
    	    desc->crtSpecific.rtp.envp = rtpArgs;

    	    /* ���Ҳ����ַ��� */
    	    pTmpStr1 = strchr((const char*)ptr, '<');
    	    pTmpStr2 = strchr((const char*)ptr, '>');
    	    if ((pTmpStr1 == NULL) || (pTmpStr2 == NULL))
    	    {
    	    	return (FALSE);
    	    }

    	    pTmpStr2[0] = EOS;
    	    pTmpStr1++;

    	    /* ������������ */
    	    for (i = 0; i < count; i++)
    	    {
    	    	rtpArgs[i] = pTmpStr1;
    	    	pTmpStr = strchr(pTmpStr1, ' ');
    	    	if (pTmpStr == NULL || (pTmpStr > pTmpStr2))
    	    	{
        	    	break;
    	    	}

    	    	pTmpStr[0] = EOS;
    	    	pTmpStr1 = ++pTmpStr;
    	    }

    	    /* ���������Կս��� */
    	    rtpArgs[count] = NULL;

    		break;
    	default:
    	    break;
    }

	return (TRUE);
}

/*
 * @brief
 *    ��������ִ�к������ߴ�������ִ��RTP
 * @param[in]: pCtxCreate: ����������
 * @return
 *    ERROR����������ִ�к���ʧ��
 *    tid����������ִ�к����ɹ�,��������ID
 */
INT32 mdlCreateTask(MDL_CREATE_DESC *pCtxCreate)
{
	UINT32 i = 0;
	UINT32 stackSize = 0;

	/* ����������� */
    if (pCtxCreate->crtSpecific.task.numArgs > MAX_ARGS)
    {
    	return ERROR;
    }

    /* �����������Ϊ0 */
    for (i = pCtxCreate->crtSpecific.task.numArgs; i < MAX_ARGS; i++)
    {
    	pCtxCreate->crtSpecific.task.arg[i] = 0;
    }

    /* ��������ջ��С */
    if ((stackSize = pCtxCreate->stackSize) == 0)
    {
    	stackSize = MDL_TASK_STACK_SIZE;
    }


    /* ����ǵ������������taskCreate������������ */
    if ((pCtxCreate->options & VX_TASK_NOACTIVATE) != 0)
    {
		/* ��������ִ�к��� */
    	taInitTaskId = taskCreate(NULL,pCtxCreate->priority,
					pCtxCreate->options,stackSize,
					(FUNCPTR)pCtxCreate->crtSpecific.task.entry,
					pCtxCreate->crtSpecific.task.arg[0], pCtxCreate->crtSpecific.task.arg[1],
					pCtxCreate->crtSpecific.task.arg[2], pCtxCreate->crtSpecific.task.arg[3],
					pCtxCreate->crtSpecific.task.arg[4], pCtxCreate->crtSpecific.task.arg[5],
					pCtxCreate->crtSpecific.task.arg[6], pCtxCreate->crtSpecific.task.arg[7],
					pCtxCreate->crtSpecific.task.arg[8], pCtxCreate->crtSpecific.task.arg[9]);
    	return (taInitTaskId);
    }
    else
    {
		/* ��������ִ�к��� */
		return taskSpawn(NULL,pCtxCreate->priority,
					pCtxCreate->options,stackSize,
					(FUNCPTR)pCtxCreate->crtSpecific.task.entry,
					pCtxCreate->crtSpecific.task.arg[0], pCtxCreate->crtSpecific.task.arg[1],
					pCtxCreate->crtSpecific.task.arg[2], pCtxCreate->crtSpecific.task.arg[3],
					pCtxCreate->crtSpecific.task.arg[4], pCtxCreate->crtSpecific.task.arg[5],
					pCtxCreate->crtSpecific.task.arg[6], pCtxCreate->crtSpecific.task.arg[7],
					pCtxCreate->crtSpecific.task.arg[8], pCtxCreate->crtSpecific.task.arg[9]);
    }
}

/*
 * @brief
 *    ��������ִ��RTP
 * @param[in]: pCtxCreate: ����������
 * @return
 *    ERROR����������ִ��RTPʧ��
 *    pid����������ִ��RTP�ɹ�,����RTP ID
 */
INT32 mdlCreateRtp(MDL_CREATE_DESC *pCtxCreate, UINT32 *textStart)
{
	RTP_ID rtpId = 0;
    UINT32 stackSize = 0;

    /* ��������ջ��С */
    if ((stackSize = pCtxCreate->stackSize) == 0)
    {
    	stackSize = MDL_TASK_STACK_SIZE;
    }

    /* ����RTP */
    rtpId = rtpSpawn(pCtxCreate->name,
    		    (const char **)pCtxCreate->crtSpecific.rtp.argv,
    		    (const char **)pCtxCreate->crtSpecific.rtp.envp,
    			pCtxCreate->priority,stackSize,
    			pCtxCreate->crtSpecific.rtp.options,
    			pCtxCreate->options);
    if ((RTP_ID)ERROR == rtpId)
    {
    	return (ERROR);
    }

    /* ����RTP��ʼ������ */
    taInitTaskId = (((WIND_RTP *)rtpId)->initTaskId);

    /* ���ô������ʼ��ַ */
    *textStart = (UINT32)((WIND_RTP *)rtpId)->binaryInfo.textStart;

    return ((INT32)rtpId);
}
