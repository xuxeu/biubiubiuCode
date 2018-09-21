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
 * @file  LoadCommand.h
 * @brief
 *       <li>���ݼ���</li>
 */
#ifndef _INCLUDE_LOADCOMMAND_H_
#define _INCLUDE_LOADCOMMAND_H_

/************************���ò���******************************/
#include "command.h"
#include "elf.h"

/************************�� �岿��******************************/
namespace com_coretek_tools_loader_command
{
class LoadCommand :public Command
{
	protected:

		/* ���Ƿ������������ */
		virtual BOOL IsParamOK(void);
		
		/* ���ض����� */
		BOOL LoadData(UINT32 address, char *content, UINT32 contentSize, UINT32 packSize,UINT32 said);
	public:
		
		/* ���캯�� */
		LoadCommand(vector<Parameter*> &param):m_LoadFileSize(0),m_CurrentLoadSize(0),Command(param){}

		/* �������� */
		virtual ~LoadCommand(void){}     

		/* ����ִ���� */	
		virtual BOOL Exec(UINT32 said);
	private:
	
		/* �����ļ� */
		BOOL LoadCommand::LoadFile(UINT32 said);

		/* ��ȡ��Ҫ���ص��ܴ�С */
		UINT32 m_LoadFileSize;

		/* ��ǰ���ش�С */
		UINT32 m_CurrentLoadSize;

		/* �жϸö��Ƿ���Ҫ���� */
		BOOL IsSegmentNeedLoad(Segment *segment);
		
		/* ���ضε��ܴ�С */
		UINT32 GetLoadSegmentSize(ELF32 * elf32);
	};
}
#endif
