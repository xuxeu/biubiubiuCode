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
 * @file  mdlLib.h
 * @brief
 *       ���ܣ�
 *       <li>��̬����ģ��������Ͷ���ͽӿ�����</li>
 */
#ifndef MDL_LIB_H_
#define MDL_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************ͷ �� ��******************************/
#include <vxWorks.h>
#include "moduleLib.h"
#include "ta.h"

/************************�� �� ��******************************/

/* ģ��Ķθ��� */
#define	MDL_MOD_NUM_SEGS  3

/* ����ģ��ID�� */
#define MDL_MAX_MODULE_ID_NUM  1024

#ifdef __X86__

/*��ź�������buffer*/
#define STRING_BUFF_LEN  (1024)
#endif
/* Ĭ������ջ��С */
#define MDL_TASK_STACK_SIZE 0x10000
/************************���Ͷ���*****************************/

/* ����Ϣ�ṹ��  */
typedef struct
{
	UINT32 type;	 /* ������ */
	UINT32 address;  /* �ε�ַ */
	UINT32 size;	 /* �δ�С */
	UINT32 flags;	 /* �α�־ */
} MDL_SEG_INFO;

/* ģ����Ϣ */
typedef struct
{
	char *name;	  /* ģ���� */
	UINT32 flags; /* ���ر�־ */
    MDL_SEG_INFO segment[MDL_MOD_NUM_SEGS];	/* ����Ϣ */
} MDL_MOD_INFO;

/* ƥ����Ϣ */
typedef struct
{
	MODULE_ID moduleId;
	SYMBOL **moduleSymbol;
}MDL_MATCH_INFO;

/* �������� */
typedef enum
{
    MDL_CRT_TASK	    = 0, /* ���� */
    MDL_CRT_RTP		    = 1, /* RTP */
    MDL_CRT_TYPE_NUM
} MDL_CREATE_TYPE;

/* ���������� */
typedef struct
{
	MDL_CREATE_TYPE contextType; /* �������� */
	char name[PATH_MAX];/* ���� */
	UINT32	redirIn;	/* �ض�������I/O */
	UINT32	redirOut;	/* �ض������I/O */
	UINT32	redirErr;	/* �ض������I/O */
	UINT32	baseAddr;	/* �������ַ */
	UINT32  priority;	/* �������ȼ�  */
	UINT32  stackSize;	/* ����ջ��С  */
	UINT32  options;	/* �������� */

	union
	{
		/* ���񴴽���Ϣ */
		struct
		{
			UINT32 entry;        /* ������ڵ� */
			UINT32 numArgs;	     /* �������� */
			UINT32 arg[MAX_ARGS];/* �����б� */
		} task;

		/* RTP������Ϣ */
		struct
		{
			UINT32	options;	/* RTP���� */
			UINT32	numArgv;	/* �������� */
			char**	argv;		/* RTP���� */
			UINT32	numEnvp;	/* ������������ */
			char**  envp;		/* �������� */
		 } rtp;
	}crtSpecific;
} MDL_CREATE_DESC;

/************************�ӿ�����*****************************/

/* ���ģ�����ID */
extern SYMBOL **mdlModuleSymbol;

/*
 * @brief
 *    ����ģ��
 * @param[in]: pModInfo: ģ����Ϣ
 * @param[out]: pModuleId: ģ��ID
 * @return
 *    ERROR������ģ��ʧ��
 *    OK������ģ��ɹ�
 */
STATUS mdlLoadModule(MDL_MOD_INFO *pModInfo, MODULE_ID *pModuleId);

/*
 * @brief
 *    ж��ģ��
 * @param[in]: moduleId: ģ��ID
 * @return
 *    ERROR��ж��ģ��ʧ��
 *    OK��ж��ģ��ɹ�
 */
STATUS mdlUnloadModule(MODULE_ID moduleId);

/*
 * @brief
 *    �ͷ�ģ���ڴ�
 * @param[in]: address: ģ���ڴ��ַ
 * @return
 *    ��
 */
void mdlFreeModuleMem(void *address);

/*
 * @brief
 *    ����ģ���ڴ�
 * @param[in]: alignment: �����ڴ�����С
 * @param[in]: size: ������ڴ��С
 * @return
 *    NULL������ģ���ڴ�ʧ��
 *    �ڴ��ַ������ģ���ڴ�ɹ�
 */
void* mdlAllocateModuleMem(UINT32 alignment,UINT32 size);

/*
 * @brief
 *    ���ģ���Ƿ����
 * @param[in]: moduleId: ģ��ID
 * @return
 *    NULL��ģ�鲻����
 *    ģ��ID��ģ�����
 */
MODULE_ID mdlFigureModuleId(MODULE_ID moduleId);

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
SYMBOL_ID mdlRegisterModuleSym(char *symbolName, char *symbolValue,
							SYM_TYPE symbolType, UINT16 groupId, UINT32 symRef);

/*
 * @brief
 *    ��ȡ���ķ��Ÿ���
 * @return
 *    ���ķ��Ÿ���
 */
UINT32 mdlGetSymbolsNum(void);

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
BOOL mdlGetModuleSymId(char *name, int val, SYM_TYPE type, MDL_MATCH_INFO *info, UINT16 group, SYMBOL *pSymbol);

/*
 * @brief
 *    ��ȡģ�����ID�б�
 * @param[in]: moduleId: ģ��ID
 * @return
 *    ģ�������
 */
UINT32 mdlGetModuleSymIdList(MODULE_ID moduleId);

/*
 * @brief
 *    ��ȡģ����Ϣ
 * @param[in]: moduleId: ģ��ID
 * @param[out]: pModuleInfo: �洢ģ����Ϣ
 * @return
 *    ERROR��ģ��ID��Ч
 *    OK����ȡģ����Ϣ�ɹ�
 */
STATUS mdlGetModuleInfo(MODULE_ID moduleId, MODULE_INFO *pModuleInfo);

/*
 * @brief
 *    ����ģ���C++���캯��
 * @param[in]: moduleId: ģ��ID
 * @return
 *    ERROR������ʧ��
 *    OK�����سɹ�
 */
STATUS mdlCplusCallCtors(MODULE_ID moduleId);

/*
 * @brief
 *    ��������MDL_CREATE_DESC�ṹ��
 * @param[in]: inbuf: ��������
 * @param[out]: desc: ����������
 * @return
 *    TRUE�������ɹ�
 *    FALSE������ʧ��
 */
BOOL mdlCtxCreateDesc(const UINT8 *inbuf, MDL_CREATE_DESC *desc);

/*
 * @brief
 *    ��������ִ�к������ߴ�������ִ��RTP
 * @param[in]: pCtxCreate: ����������
 * @return
 *    ERROR����������ִ�к���ʧ��
 *    tid����������ִ�к����ɹ�,��������ID
 */
INT32 mdlCreateTask(MDL_CREATE_DESC *pTaskCreate);

/*
 * @brief
 *    ��������ִ��RTP
 * @param[in]: pCtxCreate: ����������
 * @return
 *    ERROR����������ִ��RTPʧ��
 *    pid����������ִ��RTP�ɹ�,����RTP ID
 */
INT32 mdlCreateRtp(MDL_CREATE_DESC *pCtxCreate, UINT32 *textStart);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MDL_LIB_H_ */
