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
 * @file  LoadAndRunOSCommand.h
 * @brief
 *       <li>��������OS</li>
 */
#ifndef _INCLUDE_LOADANDRUNOSCOMMAND_H_
#define _INCLUDE_LOADANDRUNOSCOMMAND_H_

/************************���ò���******************************/
#include "LoadOSCommand.h"

/************************�ඨ��********************************/
namespace com_coretek_tools_loader_command
{
	class LoadAndRunOSCommand :public LoadOSCommand
	{
		public:

			/* ���캯�� */
			LoadAndRunOSCommand(vector<Parameter*> &param):LoadOSCommand(param){};
			
			/* ����ִ���� */	
			virtual BOOL Exec(UINT32 said);

			/* �������� */
			virtual ~LoadAndRunOSCommand(void){}
	};
}
#endif
