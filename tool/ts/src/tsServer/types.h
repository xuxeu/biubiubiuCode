/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	types.h
 * @brief	�������Ͷ���
 * @author 	����F
 * @date 	2007��12��17��
 * @History
 */

#define SUCC (0)
#define FAIL (-1)

#define TRUE (1)
#define FALSE (0)

typedef enum 
{
	LM_IMG_OS = 0,	  //OSӳ��
	LM_IMG_OS_APP,      //����OS��Ӧ��ӳ��
	LM_IMG_OS_DLIB,      //����os��ģ��ӳ��
	LM_IMG_MSL_MODULE,       //����MSL��ģ��ӳ��
} T_LM_IMG_TYPE;

#define LM_IMG_UDEFINED 0xffffffff

