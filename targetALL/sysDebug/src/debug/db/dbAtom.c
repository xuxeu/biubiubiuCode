/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-07-01         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file: dbAtom.c
 * @brief:
 *             <li>����ԭ�Ӳ�����������ȡ���мĴ������޸����мĴ�������ȡ��ǰ�ϵ��ַ����</li>
 */
/************************ͷ �� ��******************************/
#include "taErrorDefine.h"
#include "dbAtom.h"
#include "taUtil.h"

/************************�� �� ��******************************/

/************************���Ͷ���*****************************/

/************************�ⲿ����*****************************/

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief
 *     �����е��û��Ĵ���
 * @param[in] tpContext:�쳣������
 * @param[out] ubOutBuf:���buffer
 * @param[in] uwOutBufferLen:���buffer�ĳ���
 * @return
 *     TA_OK:�����ɹ�
 *     TA_OUT_OF_BUFFER:������峤�Ȳ���
 */
T_TA_ReturnCode taDebugReadAllRegisters(T_DB_ExceptionContext* context, UINT8 *outBuf, UINT32 *outBufferLen)
{
    /*  �ж�������峤���Ƿ��㹻(��˫���ȸ��㴦��,����Ĵ���ռ64λ)  */
    if ((*outBufferLen) < DB_ARCH_REGISTER_SUM_SIZE)
    {
        return (TA_OUT_OF_BUFFER);
    }
#ifdef __PPC85XX__
    //����ͨ�üĴ���
    mem2hex((const UINT8*)(context->registers), outBuf, GPR_SUM*DB_REGISTER_SIZE);
    //����������Ĵ���
    //��������Ĵ���
    mem2hex((const UINT8*)(&(context->registers[PC])), outBuf+GPR_SUM*DB_REGISTER_SIZE, SPR_SUM*DB_REGISTER_SIZE);
    /* ������ݳ��� */
    (*outBufferLen) = (GPR_SUM+SPR_SUM)*DB_REGISTER_UNIT_LEN*DB_MEM_TO_CHAR_LEN;

#else
    /* ��GDB������,�ȱ���Ĵ���*/
    mem2hex((const UINT8*)(context->registers), outBuf, DB_REGISTER_SUM_SIZE);

    /* ������ݳ��� */
    (*outBufferLen) = DB_ARCH_REGISTER_SUM_SIZE;
#endif
    return (TA_OK);
}

/*
 * @brief
 *     д�����û��Ĵ���
 * @param[in] tpContext:�쳣������
 * @param[in] ubInBuf:����buffer
 * @param[in] uwInLen:�������ݳ���
 * @return
 *     TA_OK:�����ɹ�
 *     TA_OUT_OF_BUFFER:���뻺�峤�ȷǷ�
 */
T_TA_ReturnCode taDebugWriteAllRegisters(T_DB_ExceptionContext *context, const UINT8 *inBuf, UINT32 inLen)
{
    /*  �����ֽ��������Ƿ񳬳���Χ */
    if (inLen != DB_ARCH_REGISTER_SUM_SIZE)
    {
        /*  ������Χ�򷵻�DB_INVALID_PARAM_LENGTH  */
        return (TA_OUT_OF_BUFFER);
    }
    UINT8* saveBuf = inBuf;
#ifdef __PPC85XX__
    //����ͨ�üĴ���
    //mem2hex((const UINT8*)(context->registers), outBuf, GPR_SUM*DB_REGISTER_SIZE);
    hex2memEx(inBuf, (UINT8*)(context->registers), GPR_SUM*DB_REGISTER_SIZE);
    //����������Ĵ���
    inBuf += GPR_SUM*DB_REGISTER_SIZE;//����32�����㣬������32λ��
    //��������Ĵ���
    hex2memEx(inBuf, (const UINT8*)(&(context->registers[PC])), FPSCR*DB_REGISTER_SIZE);
    //mem2hex((const UINT8*)(&(context->registers[PC])), outBuf, FPSCR*DB_REGISTER_SIZE);
#else
    /* ��Ĵ���ֵд���������� */
    hex2memEx(inBuf, (UINT8*)(context->registers), DB_REGISTER_SUM_SIZE);
#endif
    return (TA_OK);
}

/*
 * @brief:
 *    ��ȡ�����Գ���ǰ�ϵ㴥���ĵ�ַ���������Գ��򴥷��ϵ��쳣��,PC�Ĵ�����ֵ��ָ��ǰ�ϵ�ĵ�ַ
 * @param[in]:  context: �����쳣������
 * @param[out]: address:�洢�����ϵ�ָ��ĵ�ַ
 * @return:
 *     TA_OK:�����ɹ�
 */
T_TA_ReturnCode taDebugGetCurrentBpAddress(T_DB_ExceptionContext *context, UINT32 *address)
{
    /* ��ȡ�����Գ���ǰ�ϵ㴥���ĵ�ַ  */
    *address = context->registers[PC];

    return (TA_OK);
}
