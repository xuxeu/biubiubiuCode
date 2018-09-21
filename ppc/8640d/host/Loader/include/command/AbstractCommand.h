/************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2007 CoreTek Systems Inc All Rights Reserved
************************************************************************/

/*
 * �޸���ʷ��
 * 2010-8-18  ��Ԫ־�������������ɼ������޹�˾
 *                   �������ļ���
 */

/*
 * @file  AbstractCommand.h
 * @brief
 *       <li>���ܣ������������</li>
 */
#ifndef _INCLUDE_ABSTRACTCOMMAND_H
#define _INCLUDE_ABSTRACTCOMMAND_H

/************************ ���ò��� ****************************/
#include "Loader.h"

/************************�� �� ��******************************/
namespace com_coretek_tools_loader_command
{
	class AbstractCommand
	{
		protected:
			
			/* һ����Ϣ��� */
			void NormalOutput(string outStr);

			/* ������Ϣ��� */
			void ErrorMessageOutput(UINT32 errorNmber);

			/* Done��Ϣ��� */
			void DoneMessageOutput(void);

			/* ������Ϣ��� */
			void InformationProgressMessageOutput(string msg,UINT32 rate,UINT32 sendsize,UINT32 totalsize);

			/* ����ErrprStr��ȡErrprMum,��������ʽ���� */
			UINT32 GetErrorNumber(string str);
		public:
			
			/* ���캯�� */
			AbstractCommand(void){}

			/* �������� */
			virtual ~AbstractCommand(void){}
	};
}

#endif
