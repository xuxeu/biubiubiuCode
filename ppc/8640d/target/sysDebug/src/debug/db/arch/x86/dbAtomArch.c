/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  dbAtomArch.c
 * @brief:
 *             <li>�����û�����Ĵ�����صĽӿ�</li>
 */

/************************ͷ�ļ�******************************/
#include "taErrorDefine.h"
#include "dbVector.h"
#include "dbAtom.h"
#include "dbSignal.h"
#include "taUtil.h"
#include "ta.h"

/************************�궨��******************************/

/************************���Ͷ���****************************/

/************************ȫ�ֱ���****************************/
/************************�ⲿ����*****************************/

/* �ϵ��쳣���� */
extern void taBpExceptionStub(void);
/************************ǰ������****************************/

/************************ģ�����****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *      ��ϵ�ṹ��ʼ��
 * @return:
 *     TRUE: ��ʼ���ɹ�
 */
BOOL taDebugArchInit(void)
{
    /* ��װ�����쳣 */
	taInstallVectorHandle(GENERAL_DB_INT, taTraceExceptionStub);

    /* ��װ����ϵ��쳣 */
	taInstallVectorHandle(GENERAL_TRAP_INT, taBpExceptionStub);

    return (TRUE);
}

/*
 * @Brief
 *     vector��ת���ɱ�׼�źű�ʾ��ʽ
 * @param[in]: vector:������
 * @param[in]: context:�쳣������
 * @return
 *     ���������Ŷ�Ӧ��POSIX�ź�
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context)
{
    UINT32 signal = SIGNAL_TERM;
    
    /* ����vector��ת���ɱ�׼POSIX�ź� */
    switch (vector)
    {
        /* 0 ���жϡ����������쳣 */
        case GENERAL_ZDIV_INT:
            signal = SIGNAL_FPE;
            break;

        /* 1 ���жϡ��������쳣*/
        case GENERAL_DB_INT:
            signal = SIGNAL_TRAP;
            break;

        /* 2 ���жϡ����������ж�*/
        case GENERAL_NMI_INT:
            signal = SIGNAL_INT;
            break;

        /* 3 ���жϡ����ϵ��쳣��#BP��*/
        case GENERAL_TRAP_INT:
            signal = SIGNAL_TRAP;
            break; 

        /* 4 ���жϡ������������#DE��*/
        case GENERAL_OVERF_INT:
            signal = SIGNAL_NON;
            break;  

        /* 5 ���жϡ���Boundָ�ΧԽ��*/
        case GENERAL_BOUND_INT:
            signal = SIGNAL_NON;
            break;  

        /* 6 ���жϡ����Ƿ��������쳣*/
        case GENERAL_ILLINS_INT:
            signal = SIGNAL_ILL;
            break;

        /* 7 ���жϡ���Э�������������쳣*/
        case GENERAL_DEV_INT:
            signal = SIGNAL_FPE;
            break;

        /* 8 ���жϡ���˫�����쳣*/
        case GENERAL_DF_INT:
            signal = SIGNAL_EMT;
            break;

        /* 9 ���жϡ���Э��������Խ��*/
        case GENERAL_CSO_INT:
            signal = SIGNAL_SEGV;
            break;
        
        /* 10 ���жϡ����Ƿ�TSS(Task State Segment)�쳣*/
        case GENERAL_TSS_INT:
            signal = SIGNAL_SEGV;
            break;

        /* 11 ���жϡ����β�����*/
        case GENERAL_SEGMENT_INT:
            signal = SIGNAL_SEGV;
            break;

        /* 12 ���жϡ���ջ�����쳣*/
        case GENERAL_SF_INT:
            signal = SIGNAL_SEGV;
            break;

        /* 13 ���жϡ���һ�㱣���쳣*/
        case GENERAL_GP_INT:
            signal = SIGNAL_SEGV;
            break;
            
        /* 14 ���жϡ���ҳ�����쳣*/
        case GENERAL_PAGE_INT:
            signal = SIGNAL_SEGV;
            break;

        /* 16 ���жϡ���������������쳣*/
        case GENERAL_MF_INT:
            signal = SIGNAL_EMT;
            break;

        /* 17 ���жϡ�����������쳣*/
        case GENERAL_AE_INT:
            signal = SIGNAL_BUS;
            break;
            
        /* 18 ���жϡ���Machine Check�쳣*/
        case GENERAL_MC_INT:
            signal = SIGNAL_EMT;
            break;

        /* 19 ���жϡ���SIMD�����쳣*/
        case GENERAL_SIMD_INT:
            signal = SIGNAL_FPE;
            break;
         
        /*  ��������SIGNAL_EMT */
        default:
            signal = SIGNAL_EMT;
            break;
    }

    return (signal); 
} 

/*
 * @brief
 *     д�������û��Ĵ���
 * @param[in]: context:�쳣������
 * @param[in]: registerNum:�Ĵ�����
 * @param[in]: inBuf:����buffer
 * @return
 *     TA_OK:�����ɹ�
 *     TA_FAIL:����ʧ��
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
T_TA_ReturnCode taDebugWriteSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, const UINT8 *inBuf)
{
    UINT32 value = 0;

    /* ������Ч���ж� */
    if ((registerNum >= DB_X86_REGISTER_SUM))
    {
        /* ����в���Ϊ��,����TA_INVALID_PARAM */
        return (TA_INVALID_PARAM);
    }

    /* �������ַ���ת��Ϊ������ */
    if (hex2int(&inBuf, &value) > 0 )
    {
        /* дָ����ֵ��context��ָ���Ĵ��� */
        context->registers[registerNum] = swap(value);

        return (TA_OK);
    }

    /* �����ַ���ת��ʧ�� */
    return (TA_FAIL);
}

/*
 * @brief
 *     ���������û��Ĵ���
 * @param[in]: context:�쳣������
 * @param[in]: registerNum:�Ĵ�����
 * @param[out]: outBuf:���buffer
 * @param[out]: outBufferLen::�������ĳ���
 * @return
 *     TA_OK:�����ɹ�
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen)
{
    /* ������Ч���ж� */
    if ((registerNum >= DB_X86_REGISTER_SUM))
    {
        /* ����в���Ϊ��,����TA_INVALID_PARAM */
        return (TA_INVALID_PARAM);
    }
    
    mem2hex((UINT8*)&context->registers[registerNum],outBuf,DB_REGISTER_UNIT_LEN);
    
	*outBufferLen = DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN;

    return (TA_OK);
}

/*
 * @brief:
 *     ���������Ӧ����������PS�Ĵ����ĵ�����־λ��ȷ�����ص�������Ӧ�ú������������
 * @param[in]: context: �����쳣������
 * @return:
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context)
{
    /* ���PS�Ĵ����ĵ�����־λ */
    context->registers[PS] &= (~DB_ARCH_PS_TRAP_BIT);

    return (TA_OK);
}

/*
 * @brief:
 *     ���ñ�����Ӧ����������PS�Ĵ����ĵ�����־λ,ȷ�����ص�������Ӧ������ִ��һ��ָ���Ժ󴥷������쳣
 * @param[in]: context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext* context)
{
    /* ����PS�Ĵ����ĵ�����־λ */
    context->registers[PS] |= DB_ARCH_PS_TRAP_BIT;

    return (TA_OK);
}

/*
 * @brief:
 *     ���������쳣ָ��
 * @param[in]: context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugJumpOverTrap(T_DB_ExceptionContext* context)
{
    /* �����ϵ㳤�� */
    context->registers[PC] += DB_ARCH_BREAK_INST_LEN;

    return (TA_OK);
}
