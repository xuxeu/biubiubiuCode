/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *�޸���ʷ:
       2010-9-15 15:11:08        ��Ԫ־      �����������ɼ������޹�˾
       �޸ļ�����:�������ļ�
 */

/*
 * @file  Target.h
 * @brief
 *       <li>�����Ŀ���������֮��Ľ�������</li>
 */
#ifndef _INCLUDE_TARGET_H_
#define _INCLUDE_TARGET_H_

/************************���ò���******************************/
#include "Loader.h"
#include "TClientAPI.h"
#include "Util.h"

/************************�ඨ��******************************/
class Target
{
	private:
		
		/* IP��ַ */
		string m_tsIP;
		
		/* Ŀ������������� */
		string m_tgtName ;
		
		/* Ŀ�������˿ں� */
		UINT32 m_tsPort;
		
		/* ���Ӿ�� */
		UINT16 m_tsFd;

		/* Ŀ���ID */
		UINT16 m_desAid;

		/* ��ǰҪ�����Ĵ���� */
		UINT16 m_currentDesSaid;
	public:
		
		/* ���캯�� */
		Target(string tsip, string tgtname, UINT32 tsport);

		/* �������� */
		virtual ~Target(void){}

		/* ���������ָ���ظ� */
		UINT32 ExecCommand(const char *cmd, UINT32 cmdCount, UINT32 timeout);

		/* ��������Ȼظ�,���ظ��ŵ�ָ����buffer�� */
		UINT32 ExecCommand(const char *cmd, UINT32 cmdCount, char *result, UINT32 resultCount, UINT32 timeout);

		/* ��������Ȼظ� */
		UINT32 ExecCommand(const char *cmd, UINT32 cmdCount);

		/* ���ӵ�ָ����Ŀ��������� */
		UINT32 Connect(void);

		/* �Ͽ����� */
		UINT32 DisConnect(void);

		/* ���ö�������� */
		void SetDesSAid(UINT16 desSAid);

		/* �������� */
		UINT32 ReceiveData(char *result, UINT32 resultCount, UINT32 timeout, INT32 *receSize);
	protected:
		
		/* ����ErrprStr��ȡErrprMum,��������ʽ���� */
		UINT32 GetErrorNumber(string str);
};
#endif
