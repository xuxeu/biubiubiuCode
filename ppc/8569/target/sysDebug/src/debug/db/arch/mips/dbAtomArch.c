/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2011-09-23         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file:dbAtomArch.c
 * @brief:
 *             <li>ta���Թ�����ϵ�ṹ��ص�ԭ�Ӳ���</li>
 */

/************************ͷ �� ��*****************************/
#include "taErrorDefine.h"
#include "dbContext.h"
#include "taUtil.h"
#include "dbAtom.h"
#include "dbSignal.h"
#include "dbVector.h"
#include "ta.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/* �ϵ��쳣���� */
extern void taBpExceptionStub(void);

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
    /* ��װ����ϵ��쳣 */
	taInstallVectorHandle(IV_BP_VEC, taBpExceptionStub);

    return (TRUE);
}

/*
 * @Brief
 *     vector��ת���ɱ�׼�źű�ʾ��ʽ
 * @param[in] vector:������
 * @param[in] context:�쳣������
 * @return
 *     ���������Ŷ�Ӧ���ź�
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context)
{
    UINT32 signal = SIGNAL_TERM;
    
    /* ����vector��ת���ɱ�׼POSIX�ź� */
    switch (vector)
    {
        /* 0 ���жϡ����ж� */
        case IV_LOW_VEC:
            signal = SIGNAL_INT;
            break;      

		/* 6 ���жϡ���ָ������쳣 */
		case IV_IBUS_VEC:
			signal = SIGNAL_BUS;
			break;

		/* 7 ���жϡ������ݴ����쳣 */
		case IV_DBUS_VEC:
			signal = SIGNAL_BUS;
			break;

	 	/* 9 ���жϡ����ϵ��쳣*/
        case IV_BP_VEC:
            signal = SIGNAL_TRAP;
            break;

		/* 13 ���жϡ���TRAP */
		case IV_TRAP_VEC:
			signal = SIGNAL_TRAP;
			break;

		/* 13 ���жϡ��������쳣 */
		case IV_FPE_VEC:
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
 * @param[in] tpContext:�쳣������
 * @param[in] uwRegisterNum:�Ĵ�����
 * @param[in] ubInBuf:����buffer
 * @param[in] uwInLen:�������ݳ���(��λ�ֽ�)
 * @return
 *     TA_OK:�����ɹ�
 *     TA_FAIL:����ʧ��
 *     TA_INVALID_PARAM:�Ƿ��Ĳ���
 */
T_TA_ReturnCode taDebugWriteSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, const UINT8 *inBuf)
{
	UINT64 value = 0;

    /* ������Ч�Լ�� */
    if (registerNum >= REGISTER_SUM)
    {
        /* ���������Ч,����TA_INVALID_PARAM */
        return (TA_INVALID_PARAM);
    }

    /* �Ĵ�������ڸ���Ĵ�����Χ��,�����±�Ŵ�38��ʼ����֧�ָ���Ĵ����޸� */
    if(registerNum > GPR_SUM + SPR_SUM)
    {
    	return (TA_INVALID_PARAM);
    }

    /* �Ĵ�����Ų��ڸ���Ĵ�����Χ��,�Ƚ��ַ���ת��Ϊ���� */
    if (0 != hex2int64(&inBuf, &value) )
    {	
		/* ��ת�����ֵ���浽ͨ�üĴ����� */
		context->registers[registerNum] = swap64(value);
		
		return (TA_OK);
    }

    /* ת��ʧ��,����TA_FAIL */
    return (TA_FAIL);
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
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext* context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen)
{
    /* ������Ч���ж� */
    if ((registerNum >= REGISTER_SUM))
    {
        return (TA_INVALID_PARAM);
    }
    
    mem2hex((UINT8*)&context->registers[registerNum],outBuf,DB_REGISTER_UNIT_LEN);
    
	*outBufferLen = DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN;

    return (TA_OK);
}

/*
 * @brief:
 *     MIPS��֧�ֵ���λ����
 * @param[in]:context: �����쳣������
 * @return:
 *     TA_OK: �����ɹ�
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context)
{
    return (TA_OK);
}

/*
 * @brief:
 *     MIPS��֧�ֵ���λ����
 * @param[in]:context:�����쳣������
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext* context)
{
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
