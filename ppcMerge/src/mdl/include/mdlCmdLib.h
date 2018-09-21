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
 * @file  mdlCmdLib.h
 * @brief
 *       ���ܣ�
 *       <li>��̬���������ģ���������</li>
 */
#ifndef MDL_CMDLIB_H_
#define MDL_CMDLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************ͷ �� ��******************************/

/************************�� �� ��******************************/

/* �����ֽ������� */
#define MDL_ROUND_UP(x, align)	(((int) (x) + (align - 1)) & ~(align - 1))

/* 16�ֽڶ��� */
#define	MDL_ALLOC_ALIGN_SIZE 16

/* ģ���ڴ��ַ2^12���� */
//#define	MDL_ALLOC_ALIGN_ADDRESS 4096
#define	MDL_ALLOC_ALIGN_ADDRESS 65536

/* ��̬����ģ��֧�ֵ�������� */
#define MDL_MODULE_COMMAND_NUM  10

/* ģ����Ϣ��󳤶� */
#define MDL_MODULE_MAX_INFO_LEN  350

/* ������Ϣ��󳤶� */
#define MDL_SYMBOL_MAX_INFO_LEN  300

/************************���Ͷ���*****************************/

/************************�ӿ�����*****************************/

/*
 * @brief:
 *     ��װ��̬������Ϣ������
 * @return:
 *     ��
 */
void mdlModuleCommandInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MDL_CMDLIB_H_ */
