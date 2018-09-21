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
 * @file  ErrorTableDefine.h
 * @brief
 *       <li>��ȡ�������ļ�����Ĵ�����Ϣ</li>
 */
#ifndef _INCLUDE_ERRORTABLEDEFINE_H_
#define _INCLUDE_ERRORTABLEDEFINE_H_

/************************���ò���******************************/
#include "Loader.h"
#include "ErrorDefine.h"
#include "xml.h"

/************************�ඨ��********************************/
namespace com_coretek_tools_loader_error
{
	class ErrorTableDefine
	{
		private:

			/* ������vector */
			vector<ErrorDefine> m_ErrorDefines;
		public:
			
			/* ���캯�� */
			ErrorTableDefine(void);
			
			/* �������� */
			~ErrorTableDefine(void);

			/* �����ļ��������XML�ļ� */
			BOOL LoadFile(string fileName);

			/* ���ݴ����ַ�����ȡ����� */
			UINT32 GetErrorNumber(const string str);	 
	};
}
#endif

