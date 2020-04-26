/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * �޸���ʷ��
 * 2010-8-18  ��Ԫ־�������������ɼ������޹�˾
 *                   �������ļ���
 */

/*
 * @file  ErrorDefine.h
 * @brief
 *       <li>�������ļ�</li>
 */
#ifndef _INCLUDE_ERRORDEFINE_H_
#define _INCLUDE_ERRORDEFINE_H_

/************************���ò���******************************/
#include "Loader.h"

/************************�ඨ��******************************/
namespace com_coretek_tools_loader_error
{
	class ErrorDefine
	{
		public:
			
			/* ����� */
			UINT32 errorNum;

			/* �����ַ��� */
			string errorStr;

			/* ���߸��û��Ĵ���ԭ�� */
			string userStr;

			/* ���ܵĽ������ */
			string userSolution;

			/* �ڲ�������Ϣ */
			string debugStr;

			/* ���캯�� */
			ErrorDefine():errorNum(0),errorStr(NULL_STRING),userStr(NULL_STRING),userSolution(NULL_STRING),debugStr(NULL_STRING){}
			
			/* �������� */
			~ErrorDefine(){}
	};
}

#endif

