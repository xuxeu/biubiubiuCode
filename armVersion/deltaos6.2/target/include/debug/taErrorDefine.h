/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file: taErrorDefine.h
 *@brief:
 *       <li>����Ŷ���</li>
 */

#ifndef _TAERRORDEFINE_H
#define _TAERRORDEFINE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************ͷ�ļ�********************************/

/************************�궨��********************************/

/************************���Ͷ���******************************/

/* ����API�ķ���ֵ���Ͷ��� */
typedef enum
{
    /*GDB����*/
    GDB_OK = 0,
    GDB_EXEC_COMMAND = 1,             /* ִ������ʱ���� */
    GDB_INVALID_PARAM = 2,            /* ��ΪGDB���Ĳ������� */
    GDB_INVALID_VECTOR = 3,           /* �Ƿ���vector */
    GDB_READ_SIZE_ERROR = 4,          /* ���ڴ��С���� */

    /* ���ݶ���,TA�Ĵ���Ŵ�500��ʼ */
    TA_OK = 500,                      /* ��ʾ���óɹ� */
    TA_FAIL = 501,                    /* ��ʾ����ʧ�� */
    TA_INVALID_INPUT  = 502,          /* ��Ч��������� */
    TA_INVALID_NET_CFG = 503,         /* ��Ч�������� */
    TA_COMMANDTYPE_ERROR = 504,       /* �Ƿ����������� */
    TA_OUT_OF_BUFFER = 505,           /* TAһ������bufferװ�������� */
    TA_INVALID_ID = 506,              /* ��Ч��ͨ��ID�� */
    TA_INVALID_PROTOCOL = 507,        /* Э��û�а�װ */
    TA_INVALID_ALIGN_SIZE = 508,      /* ��Ч�ķ��ʿ�� */
    TA_COMMAND_NOTFOUND = 509,        /* �����Ҳ��� */
    TA_GET_DATA_ERROR = 510,          /* ��ȡ����ʧ�� */
    TA_GET_PACKET_DATA_FAIL = 512,    /* ��ȡ������ʧ�� */
    TA_GET_CTRL_PACKET_FAIL = 513,    /* ��ȡȷ�ϰ�ʧ�� */
    TA_PACKET_CHECK_SUM_FAIL = 514,   /* ��֤У���ʧ�� */
    TA_CTRL_PACKET = 515,             /* ȷ�ϰ� */
    TA_HANDLED_PACKET = 516,          /* ������İ� */
    TA_INVALID_PARAM = 517,           /* �Ƿ��Ĳ��� */
    TA_IPI_NO_INIT = 518,             /* IPI δ��ʼ�� */
    TA_IPI_EMIT_FAILED = 519,         /* Ͷ�� IPI �ж�ʧ�� */
    TA_ILLEGAL_OPERATION = 520,       /* �Ƿ����� */
    TA_FUNCTION_IS_NULL = 521,        /* ����ָ��Ϊ�� */
    TA_AGENT_NO_START = 522,          /* ����δ����  */
    TA_ERR_MEM_ACCESS = 523,          /* �ڴ����ʧ��  */

    /* DB��ش���Ŷ��� */
    TA_DB_BREAKPOINT_FULL = 600,      /* �ϵ������*/
    TA_DB_BREAKPOINT_EMPTY = 601,     /* û�������κζϵ� */
    TA_DB_BREAKPOINT_NOTFOUND = 602,  /* û�ҵ��ϵ� */
    TA_DB_BREAKPOINT_DIFFER_ID = 603, /* �ҵ���ַ��ͬ,��sessionID,taskID��ͬ�Ķϵ� */
    TA_DB_BREAKPOINT_EXIST = 604,     /* ������ͬaddr,sessionID,taskID�Ķϵ� */
    TA_DB_NEED_IRET = 605,            /* ��Ҫ���ж�/�쳣 ���� */
    TA_DB_COMMAND_REGISTERED = 606,   /* �����Ѿ�ע��  */

    /* RTP������ش���� */
    TA_INVALID_CONTEXT = 700,         /* ��Ч������ */
    TA_INVALID_RTP_NAME = 701,        /* ��Чʵʱ������ */
    TA_INVALID_RTP_ID = 702,          /* ��Чʵʱ����ID */
    TA_ALLOCATE_SESSION_FAIL = 703,   /* ������ԻỰʧ�� */
    TA_SESSION_ALREADY_ATTACH = 704,  /* ���ԻỰ�Ѿ��� */
    TA_SESSION_FULL = 705,            /* ���ԻỰ�Ѿ�������� */
    TA_TASK_NO_ATTACH = 706,          /* ����δ�� */
    TA_ALLOC_MEM_FAIL = 707,          /* �����ڴ�ʧ�� */
    TA_ERR_NO_RT_PROC = 708,          /* ����ʱ�ӿ�δ��װ */
    TA_TASK_ALREADY_ATTACH = 709,     /* �����Ѿ��� */
    TA_INVALID_DEBUG_MODE = 710,      /* ��Ч������ */
    TA_INVALID_TASK_NAME = 711,       /* ��Чʵ������ */
    TA_ATTACH_TASK_FAIL = 712,        /* ������ʧ�� */
    TA_INVALID_TASK_ID = 713,         /* ��Чʵ����ID */
    TA_PAUSE_TASK_FAIL = 714,         /* ��ͣ����ʧ�� */

    /* ��̬ģ����ش����  */
    TA_UNLOAD_MODULE_FAIL = 900,      /* ж��ģ��ʧ�� */
    TA_ALLOC_MODULE_MEM_FAIL = 901,   /* ����ģ���ڴ�ʧ�� */
    TA_LOAD_MODULE_FAIL = 902,        /* ����ģ��ʧ�� */
    TA_ADD_SYMBOL_FAIL = 903,          /* ����ģ��ʧ�� */

    /* RSE����� */
    TA_INVALID_OPERATE_ID = 1000,      /* ��Ч�Ĳ���ID */
    TA_INVALID_MODULE_ID = 10001       /* ��Ч��ģ��ID */
}T_TA_ReturnCode;

/************************�ӿ�����******************************/

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TAERRORDEFINE_H */
