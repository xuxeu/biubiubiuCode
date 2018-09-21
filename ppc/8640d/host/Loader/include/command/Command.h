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
 * @file  Command.h
 * @brief
 *       <li>���ܣ���������ĳ�����</li>
 */
#ifndef _INCLUDE_COMMAND_H_
#define _INCLUDE_COMMAND_H_

/************************���ò���******************************/
#include "Parameter.h"
#include "Target.h"
#include "AbstractCommand.h"

/************************���Ͷ���******************************/
namespace com_coretek_tools_loader_command
{
class Command:public AbstractCommand
{
	private:
		
		/* �����˴��ݵĲ��� */
		vector<Parameter*> m_params;

		/* Ŀ��������շ����� */
		Target *m_target;
	protected:	

		/* ���ݲ������ƻ�ȡ������ֵ */
		string GetParameterValue(string paramName);

		/* ���Ƿ������������ */
		virtual BOOL IsParamOK(void) = 0;

	public:
		
		/* ���캯�� */
		Command(vector<Parameter*> &param);

		/* �������� */
		virtual ~Command(void);

		/* ����Target */
		Target* CreateTarget(void);

		/* ���Target */
		Target *GetTarget(void);

		/* ��ȡ��ʱʱ�� */
		UINT32 GetTimeOut();

		/* ����ִ���� */	
		virtual BOOL Exec(UINT32 said);
	};
}

#endif
