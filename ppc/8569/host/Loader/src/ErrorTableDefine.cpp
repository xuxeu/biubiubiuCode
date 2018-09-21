/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *�޸���ʷ:
       2010-9-16 13:06:58        ��Ԫ־      �����������ɼ������޹�˾
       �޸ļ�����:�������ļ�
 */

/**
 *@file   ErrorTableDefine.cpp
 *@brief
       <li>������Ϣ��</li>
 *@Note     
 */

/************************���ò���******************************/
#include "ErrorTableDefine.h"
#include "XmlElement.h"
#include "XmlNode.h"
#include "XmlDocument.h"
using namespace std;

/************************���岿��******************************/

/************************ʵ�ֲ���*******************************/
namespace com_coretek_tools_loader_error
{

/**
 * @Funcname  : ErrorTableDefine
 * @brief	  : ���캯��
 * @para[IN]  : ��
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
ErrorTableDefine::ErrorTableDefine(void)
{
}

/**
 * @Funcname  : ~ErrorTableDefine
 * @brief	  : ��������
 * @para[IN]  : ��
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
ErrorTableDefine::~ErrorTableDefine(void)
{
}

/**
 * @Funcname  : LoadFile
 * @brief	  : ���ؽ����������ļ�
 * @para[IN]  : string�������XML�ļ�
 * @reture	  : TRUE:���سɹ� FALSE:����ʧ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
BOOL ErrorTableDefine::LoadFile(string fileName)
{
	ErrorDefine errorInformation;
	XmlElement *RootElement = NULL;
	XmlElement *LoaderElement = NULL;
	XmlElement *ErrorCodeElement = NULL; 
	XmlDocument mDocument;
	
	try
	{
		/* ���������ļ� */
		bool loadOkay = mDocument.LoadFile(fileName.c_str());
		if (!loadOkay)
		{
			throw "���������ļ�ʧ�ܣ��鿴�����ļ��Ƿ���ȷ";
			return FALSE;
		}

		/* ��õ�һ��Loader��ǩ */
		RootElement = mDocument.RootElement();
		LoaderElement = RootElement->FirstChildElement();

		/* ����ÿ���ڵ� */
		for(ErrorCodeElement = LoaderElement->FirstChildElement(); ErrorCodeElement; ErrorCodeElement = ErrorCodeElement->NextSiblingElement())
		{
			errorInformation.errorNum = atoi(ErrorCodeElement->Attribute("ErrorNum"));
			errorInformation.errorStr = ErrorCodeElement->Attribute("ErrorStr");
			errorInformation.userStr= ErrorCodeElement->Attribute("ErrorUser");
			errorInformation.userSolution = ErrorCodeElement->Attribute("ErrorSolution");
			errorInformation.debugStr = ErrorCodeElement->Attribute("ErrorDebug");
			m_ErrorDefines.push_back(errorInformation);
		}

		return TRUE;
	}
	catch(...)
	{
		throw "�����������ļ�ʧ��";
	}
}

/**
 * @Funcname  : getErrorNumber
 * @brief	  : ���ݴ����ַ�����ȡ�����
 * @para[IN]  : str:�����ַ���
 * @reture	  : �����
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
UINT32 ErrorTableDefine::GetErrorNumber(const string str)
{
	UINT32 count = 0;
	
	/*��ô����*/
	for(count = 0; count < m_ErrorDefines.size(); count++)
	{
		if (str == m_ErrorDefines[count].errorStr)
		{
			return m_ErrorDefines[count].errorNum;
		}
	}

	return LOADER_INVALID_ID;
}

}