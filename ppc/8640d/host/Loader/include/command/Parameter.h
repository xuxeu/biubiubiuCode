/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * �޸���ʷ��
 * 2010-9-15  ��Ԫ־�������������ɼ������޹�˾
 *                   �������ļ���
 */

/*
 * @file  Parameter.h
 * @brief
 *       <li>��ʾһ�������в���</li>
 */
#ifndef _INCLUDE_PARAMETER_H_
#define _INCLUDE_PARAMETER_H_

/************************���ò���******************************/
#include "Loader.h"

/************************�ඨ��********************************/
namespace com_coretek_tools_loader_command
{
	class Parameter
	{	
		public:
			/* ���� */
			string name;
			
			/* ����ֵ */
			string value;
			
			/* ���캯�� */
			Parameter(void):name(NULL_STRING),value(NULL_STRING){}

			/* ���캯�� */
			Parameter(string name,string value):name(name),value(value){}
			
			/* �������� */
			~Parameter(void){}
	};
}
#endif
