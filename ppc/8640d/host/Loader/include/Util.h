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
 * @file  Util.h
 * @brief
 *       <li>��������</li>
 */
#ifndef _INCLUDE_UTIL_H_
#define _INCLUDE_UTIL_H_

/************************���ò���******************************/
#include "Message.h"
#include "ErrorTableDefine.h"
#include <wchar.h>

using namespace com_coretek_tools_loader_error;

/************************�ඨ��******************************/
class Util
{
	private:
		string m_exePath;

		/* ��ʵ�� */
		static Util *m_instance;

		/* ������� */
		ErrorTableDefine m_errorTable;

		Util(void):m_exePath(NULL_STRING){}
	public:
		~Util(void){};

		static Util *GetDefault();

		/* ���ش������ļ� */
		BOOL LoadErrorFile(string errorFileName);

		/* ����ErrprStr��ȡErrprMum,��������ʽ���� */
		UINT32 GetErrorNumber(string str);

		/* һ����Ϣ��� */
		void NormalOutput(string outStr);

		/* ������Ϣ��� */
		void ErrorMessageOutput(UINT32 errorNumber);

		/* done��Ϣ��� */
		void DoneMessageOutput(void);

		/* ������Ϣ��� */
		void InformationProgressMessageOutput(string msg, UINT32 rate, UINT32 sendsize, UINT32 totalsize);

		/* �õ���������Ŀ¼ */
		string GetCurrentDirectory(void);

		/* ��ѯ�����Ƿ����� */
		BOOL Util::FindProcess(const char* processName);

		/*��С��ת��*/
		void big2little(INT32 cnt, void *ptr);


};

#endif
