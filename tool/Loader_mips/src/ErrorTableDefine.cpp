/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *修改历史:
       2010-9-16 13:06:58        彭元志      北京科银京成技术有限公司
       修改及方案:创建本文件
 */

/**
 *@file   ErrorTableDefine.cpp
 *@brief
       <li>错误信息表</li>
 *@Note     
 */

/************************引用部分******************************/
#include "ErrorTableDefine.h"
#include "XmlElement.h"
#include "XmlNode.h"
#include "XmlDocument.h"
using namespace std;

/************************定义部分******************************/

/************************实现部分*******************************/
namespace com_coretek_tools_loader_error
{

/**
 * @Funcname  : ErrorTableDefine
 * @brief	  : 构造函数
 * @para[IN]  : 无
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
ErrorTableDefine::ErrorTableDefine(void)
{
}

/**
 * @Funcname  : ~ErrorTableDefine
 * @brief	  : 析构函数
 * @para[IN]  : 无
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
ErrorTableDefine::~ErrorTableDefine(void)
{
}

/**
 * @Funcname  : LoadFile
 * @brief	  : 加载解析错误定义文件
 * @para[IN]  : string错误定义的XML文件
 * @reture	  : TRUE:加载成功 FALSE:加载失败
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
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
		/* 加载配置文件 */
		bool loadOkay = mDocument.LoadFile(fileName.c_str());
		if (!loadOkay)
		{
			throw "解析配置文件失败，查看配置文件是否正确";
			return FALSE;
		}

		/* 获得第一个Loader标签 */
		RootElement = mDocument.RootElement();
		LoaderElement = RootElement->FirstChildElement();

		/* 解析每个节点 */
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
		throw "解析错误定义文件失败";
	}
}

/**
 * @Funcname  : getErrorNumber
 * @brief	  : 根据错误字符串获取错误号
 * @para[IN]  : str:错误字符串
 * @reture	  : 错误号
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
UINT32 ErrorTableDefine::GetErrorNumber(const string str)
{
	UINT32 count = 0;
	
	/*获得错误号*/
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