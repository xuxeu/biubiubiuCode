/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * �޸���ʷ��
 * 2010-8-18  ��Ԫ־�������������ɼ������޹�˾
 *                   �������ļ���
 */

/**
* @file  Util.cpp
* @brief
*       ���ܣ�
*       <li> ��������</li>
*/
/************************ͷ �� ��******************************/
#include <iostream>
#include <windows.h>
#include <Tlhelp32.h>
#include "Util.h"
#include "loader.h"
#include "ErrorMessage.h"
#include "InformationProgressMessage.h"
#include "DoneMessage.h"

using namespace com_coretek_tools_loader_message;

/************************���岿��*****************************/
Util* Util::m_instance = NULL;

/************************ʵ�ֲ���******************************/
/**
 * @Funcname  : GetDefault
 * @brief	  : ��ȡʵ��
 * @para[IN]  : ��
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
Util* Util::GetDefault()
{
	if (NULL == m_instance)
	{
		m_instance = new Util();
	}

	return m_instance;
}

/**
 * @Funcname  : LoadErrorFile
 * @brief	  : ���ش������ļ�
 * @para[IN]  : errorFileName���������ļ�
 * @reture	  : TRUE:��ȷ���� FALDE:����ʱ����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
BOOL Util::LoadErrorFile(string errorFileName)
{
	return m_errorTable.LoadFile(errorFileName);
}

/**
 * @Funcname  : GetErrorNumber
 * @brief	  : ����ErrprStr��ȡErrprMum,��������ʽ����
 * @para[IN]  : ErrorStr�������ַ���
 * @reture	  : ������ʽ�Ĵ����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
UINT32 Util::GetErrorNumber(string str)
{
	return m_errorTable.GetErrorNumber(str);
}

/**
 * @Funcname  : InformationProgressMessageOutput
 * @brief	  : ������Ϣ���
 * @para[IN]  : msg����Ϣ rate�����ؽ��� sendsize����ǰ���ش�С totalsize�������ļ��ܴ�С
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
void Util::InformationProgressMessageOutput(string msg, UINT32 rate, UINT32 sendsize, UINT32 totalsize)
{
	InformationProgressMessage *progressmessage = new  InformationProgressMessage(msg, rate, sendsize, totalsize);
	NormalOutput(progressmessage->tostring());
	delete progressmessage;
	progressmessage = NULL;
}

/**
 * @Funcname  : ErrorMessageOutput
 * @brief	  : ������Ϣ���
 * @para[IN]  : errorNumber�������
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
void Util::ErrorMessageOutput(UINT32 errorNumber)
{
	ErrorMessage *errorMessage = new ErrorMessage(errorNumber);
	NormalOutput(errorMessage->tostring());
	delete errorMessage;
	errorMessage = NULL;
}

/**
 * @Funcname  : DoneMessageOutput
 * @brief	  : done��Ϣ���
 * @para[IN]  : ��
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
void Util::DoneMessageOutput(void)
{
	DoneMessage *doneMessage = new DoneMessage();
	NormalOutput(doneMessage->tostring());
	delete doneMessage;
	doneMessage = NULL;
}

/**
 * @Funcname  : NormalOutput
 * @brief	  : һ����Ϣ���
 * @para[IN]  : outStr�������Ϣ
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
void Util::NormalOutput(string outStr)
{
	cout << outStr << endl;
} 

/**
 * @Funcname  : GetCurrentDirectory
 * @brief	  : �õ�ִ�г�������Ŀ¼
 * @para[IN]  : ��
 * @reture	  : ִ�г���·��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
string Util::GetCurrentDirectory(void)
{
	if (m_exePath == "")
	{
		TCHAR Buffer[MAX_PATH];

	#ifdef _UNICODE
			wmemset(Buffer,0,MAX_PATH);
	#else
			memset(Buffer,0,MAX_PATH);
	#endif

		GetModuleFileName(NULL, Buffer, sizeof(Buffer));
	#ifdef _UNICODE
			*(wcsrchr(Buffer, '\\')) = '\0';
	#else
			*(strrchr(Buffer, '\\')) = '\0';
	#endif
			m_exePath = Buffer;
	}

	return m_exePath;
}

/**
 * @Funcname  : FindProcess
 * @brief	  : ��ѯ�����Ƿ�����
 * @para[IN]  : ��
 * @reture	  : processName��������
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
BOOL Util::FindProcess(const char* processName)
{
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32) ;

    //�������վ��
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE == hSnapshot)
	{
		return FALSE;
	}

    //������ϵͳ�е�һ�����̵���Ϣ
    Process32First(hSnapshot, &pe);
    
    //�����ϵͳ�е����н��̽���ö��
    do
    {
        //�����ִ�Сд
        if (strnicmp(pe.szExeFile, processName, strlen(pe.szExeFile)) == 0)
        {
			//�رտ��վ��
			CloseHandle(hSnapshot); 

			return TRUE;
        }
    }while (Process32Next(hSnapshot, &pe));
    
    //�رտ��վ��
    CloseHandle(hSnapshot); 

	return FALSE;
}

/**
 *@brief
 *  ��С��ת��
 *@param[in] cnt:�ֽ���
 *@param[in|out] ptr:Ҫת�����ַ���
 *@Return
 *   ��
 */
void Util::big2little(int cnt, void *ptr)
{
    char    *ByteData;
    char    TempByte;
    
    /* Get data pointer */
    ByteData = ( char * )ptr;
    
    switch(cnt)
    {
        case 2:
        {
            TempByte = ByteData[0];
            ByteData[0] = ByteData[1];
            ByteData[1] = TempByte;
            break;
        }
        
        case 4:
        {
            TempByte = ByteData[0];
            ByteData[0] = ByteData[3];
            ByteData[3] = TempByte;
            TempByte = ByteData[1];
            ByteData[1] = ByteData[2];
            ByteData[2] = TempByte;
			break;
        }
        case 8:
        {
            TempByte = ByteData[0];
            ByteData[0] = ByteData[7];
            ByteData[7] = TempByte;
            TempByte = ByteData[1];
            ByteData[1] = ByteData[6];
            ByteData[6] = TempByte;
			TempByte = ByteData[2];
            ByteData[2] = ByteData[5];
            ByteData[5] = TempByte; 
			TempByte = ByteData[3];
            ByteData[3] = ByteData[4];
            ByteData[4] = TempByte; 
			break;
        }
		default:
		{
            TempByte = ByteData[0];
            ByteData[0] = ByteData[3];
            ByteData[3] = TempByte;
            TempByte = ByteData[1];
            ByteData[1] = ByteData[2];
            ByteData[2] = TempByte;
			break;
		}
    }
}
