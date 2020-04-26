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
 * @file  LoadOSCommand.h
 * @brief
 *       <li>����OS</li>
 */
#ifndef _INCLUDE_LOADOSCOMMAND_H_
#define _INCLUDE_LOADOSCOMMAND_H_

/************************���ò���******************************/
#include "LoadCommand.h"

/************************�� �岿��******************************/
namespace com_coretek_tools_loader_command
{
	class LoadOSCommand :public LoadCommand
	{
		protected:

			/* �Ƿ������������ */
			virtual BOOL IsParamOK(void);
		public:
			
			/* ���캯�� */
			LoadOSCommand(vector<Parameter*> &param):LoadCommand(param){}
			
			/* ����ִ���� */	
			virtual BOOL Exec(UINT32 said);

			/* �������� */
			virtual ~LoadOSCommand(void){}
	};
}
#endif
