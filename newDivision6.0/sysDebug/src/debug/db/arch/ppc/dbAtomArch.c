/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2011-02-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbAtomArch.c
 * @brief:
 *             <li>ta���Թ�����ϵ�ṹ��ص�ԭ�Ӳ���</li>
 */

/************************ͷ �� ��*****************************/
#include "taErrorDefine.h"
#include "taUtil.h"
#include "dbAtom.h"
#include "dbSignal.h"

/************************�� �� ��******************************/
#define GENERAL_DB_INT          1  /* �����쳣������*/
#define GENERAL_TRAP_INT        3  /* �ϵ��쳣 0700:  trap program exception */
#define GENERAL_SM_INT          4  /* 1400:  ϵͳ�����ж� */
#define GENERAL_TH_INT          5  /* 1700:  �¶ȹ����ж� */
#define GENERAL_IAE_INT         6  /* �Ƿ��������쳣*/
#define GENERAL_PAGE_INT        14 /* ҳ�����쳣*/
#define GENERAL_MF_INT          16 /* �����쳣 0700:  IEEE floating-point enabled program exception*/
#define GENERAL_AE_INT          17 /* ��������쳣 0600:  ALIGNMENT���� */
#define GENERAL_MC_INT          18 /* Machine Check�쳣 0200:  �������*/
#define GENERAL_RST_INT         20 /* 0100:  ϵͳ��λ*/
#define GENERAL_DAE_INT         21 /* 0300:  DSI�쳣�����ڸ���HASH�������⴦�� */
#define GENERAL_PRIVI_INT       22 /* 0700:  privileged instruction program exception */
#define GENERAL_ILLINS_INT      23 /* 0700:  illegal instruction program exception */
#define GENERAL_UF_INT          24 /* 0800:  ���㲻�����쳣 */
#define GENERAL_DEC_INT         25 /* 0900:  DEC�жϣ�DECһ�����ڼ�ʱ��ϵͳ���ڸ��ж�ֱ�ӷ��ء� */
#define GENERAL_PM_INT          26 /* 0F00:  ���ܼ�� */
#define GENERAL_HBP_INT         27 /* 0300:  Ӳ���ϵ��쳣 */
#define GENERAL_IAB_INT         31 /* 1300:  ָ���ַ�ϵ� */

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *      ��ϵ�ṹ��ʼ����Ŀǰ��Ҫ�����ϵ�ṹ���RSP�����
 * @return:
 *     TRUE: ��ʼ���ɹ�
 *     FALSE: ��ʼ��ʧ��
 */
BOOL taDebugArchInit(void)
{
    return (TRUE);
}

/*
 * @brief:
 *     ���쳣������ת���ɱ�׼POSIX�źű�ʾ��ʽ
 * @param[in]:vector:�쳣������
 * @param[in]:context:�����쳣������
 * @return:
 *     ���������Ŷ�Ӧ��POSIX�ź�
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context)
{
    UINT32 signal = SIGNAL_TERM;

    /* �ж����������� */
    switch(vector)
    {
        case GENERAL_MC_INT:
            signal = SIGNAL_SEGV;
            break;

        case GENERAL_DAE_INT:
            signal = SIGNAL_SEGV;
            break;

        case GENERAL_PAGE_INT:
            signal = SIGNAL_SEGV;
            break;

        case GENERAL_AE_INT:
            signal = SIGNAL_BUS;
            break;

        case GENERAL_TRAP_INT:
           signal = SIGNAL_TRAP;
           break;

        case GENERAL_MF_INT:
           signal = SIGNAL_FPE;
           break;

        case GENERAL_ILLINS_INT:
           signal = SIGNAL_ILL;
           break;

        case GENERAL_UF_INT:
            signal = SIGNAL_FPE;
            break;

        case GENERAL_DB_INT:
            signal = SIGNAL_TRAP;
            break;

        case GENERAL_IAB_INT:
            signal = SIGNAL_TRAP;
            break;

        default:
            break;
    }

    return (signal);
}

/*
 * @brief
 *     ���������û��Ĵ���
 * @param[in] context:�쳣������
 * @param[in] registerNum:�Ĵ�����
 * @param[out] outBuf:���buffer
 * @param[out] outBufferLen::�������ĳ���
 * @return
 *     TA_OK:�����ɹ�
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 *     TA_OUT_OF_BUFFER:������峤�Ȳ���
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext* context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen)
{
    /* �Ĵ����Ų���֧�ֵķ�Χ�� */
    if (registerNum >= REGISTER_SUM)
    {
        return (TA_INVALID_PARAM);
    }

    /* ��ȡ�ļĴ���Ϊͨ�üĴ��� */
    if ( registerNum < GPR_SUM )
    {
        /* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
        mem2hex( (UINT8*)&(context->registers[registerNum]), outBuf, DB_REGISTER_UNIT_LEN );

        *outBufferLen = DB_REGISTER_SIZE;

        return (TA_OK);
    }

    /* ��ȡ�ļĴ���Ϊ����Ĵ��� */
    if ( (registerNum >= GPR_SUM) && (registerNum < GPR_SUM+FGPR_SUM) )
    {
        /* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
        mem2hex( (UINT8*)&(context->registers[registerNum*2-GPR_SUM]), outBuf, DB_FLOAT_REGISTER_UNIT_LEN);

        *outBufferLen = DB_FLOAT_REGISTER_SIZE;

        return (TA_OK);
    }

    /* ���õļĴ���Ϊ����Ĵ���,�����±��64��69 */
    if ( registerNum >= GPR_SUM+FGPR_SUM )
    {
    	/* ���ݼĴ����Ŵ�tpContext�ж����ݵ�ubOutBuf������ */
         mem2hex( (UINT8*)&(context->registers[registerNum+FGPR_SUM]), outBuf, DB_REGISTER_UNIT_LEN );

        *outBufferLen = DB_REGISTER_SIZE;

        return (TA_OK);
    }

    return (TA_FAIL);
}

/*
 * @brief:
 *     д������Ӧ���������е����Ĵ�����ֵ
 * @param[in]:context: �����쳣������
 * @param[in]:registerNum:�Ĵ����ţ��Ĵ����ŷ�Χ��0~REGISTER_SUM  0~GPR_SUMΪͨ�����ͼĴ��� GPR_SUM~(GPR_SUM+FGPR_SUM)Ϊ����Ĵ��� (GPR_SUM+FGPR_SUM)~REGISTER_SUMΪ����Ĵ���
 * @param[in]:inBuf:�������ݻ��壬��������Ϊд��Ĵ�����ֵ(ʮ����ֵ�ַ�����ʾ)
 * @return:
 *     TA_OK:�����ɹ�
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 *     TA_INVALID_PARAM_LENGTH:���뻺�峤�ȷǷ�
 *     TA_FAIL:ת��ʧ��
 */
T_TA_ReturnCode  taDebugWriteSingleRegister(T_DB_ExceptionContext* context, UINT32 registerNum, const UINT8 *inBuf)
{
    UINT32 value = 0;
    UINT64 doubleValue = 0;

    /* ������Ч�Լ�� */
    if (registerNum >= REGISTER_SUM)
    {
        return (TA_INVALID_PARAM);
    }

    /* �Ĵ�������ڸ���Ĵ�����Χ��,�����±�Ŵ�32��63 */
    if ( (registerNum >= GPR_SUM) && (registerNum < GPR_SUM+FGPR_SUM) )
    {
        if (0 != hex2int64(&inBuf, &doubleValue) && '\0' == (*inBuf))
        {
            /* �������ַ���ת��Ϊ64λֵ,��д����Ӧ�ĸ���Ĵ���  */
            context->registers[registerNum*2 - GPR_SUM] = ((DOUBLE)doubleValue);

            return (TA_OK);
        }

        return (TA_FAIL);
    }

    /* �Ĵ�����Ų��ڸ���Ĵ�����Χ��,�Ƚ��ַ���ת��Ϊ���� */
    if ( 0 != hex2int(&inBuf, &value) && '\0' == (*inBuf))
    {
    	/* �Ĵ��������ͨ�üĴ�����Χ��,�����±��0��31  */
        if (registerNum < GPR_SUM)
        {
            context->registers[registerNum] = value;

            return (TA_OK);
        }

        /* �Ĵ������������Ĵ�����Χ��,�����±��64��69 */
        if (registerNum >= GPR_SUM + FGPR_SUM)
        {
        	/* ��ת�����ֵ���浽����Ĵ����� */
            context->registers[registerNum+FGPR_SUM] = value;

            return (TA_OK);
        }
    }

    return (TA_FAIL);
}

/*
 * @brief:
 *     ���������Ӧ����������PS�Ĵ����ĵ�����־λSE��ȷ�����ص�������Ӧ�ú������������
 * @param[in]:context: �����쳣������
 * @return:
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context)
{
    /* ����PS�Ĵ����ĵ�����־λSEΪ0 */
    context->registers[PS] &= (~DB_ARCH_PS_TRAP_BIT);

    return (TA_OK);
}

/*
 * @brief:
 *     ���ñ�����Ӧ����������PS�Ĵ����ĵ�����־λSEΪ1,ȷ�����ص�������Ӧ������ִ��һ��ָ���Ժ󴥷������쳣
 * @param[in]:context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext* context)
{
    /* ����PS�Ĵ�����SE��־λΪ1 */
    context->registers[PS] |= DB_ARCH_PS_TRAP_BIT;

    return (TA_OK);
}

/*
 * @brief:
 *     ���������쳣ָ��
 * @param[in]:context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugJumpOverTrap(T_DB_ExceptionContext* context)
{
    /* �����ϵ㳤�� */
    context->registers[PC] += DB_ARCH_BREAK_INST_LEN;

    return (TA_OK);
}
