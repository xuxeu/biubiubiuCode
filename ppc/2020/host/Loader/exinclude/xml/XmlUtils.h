/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: Utils.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   Utils.h
 * @brief
 *    <li>公共声明</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */
#ifndef _XMLUTILS_H
#define _XMLUTILS_H

#include "XMLString.h"
#define XML_STRING		String

/*QueryDoubleValue函数返回类型*/
enum 
{ 
	XML_SUCCESS,
	XML_NO_ATTRIBUTE,
	XML_WRONG_TYPE
};

/*系统错误类型*/
enum
{
	XML_NO_ERROR = 0,
	XML_ERROR_ERROR,
	XML_ERROR_OPENING_FILE,
	XML_ERROR_OUT_OF_MEMORY,
	XML_ERROR_PARSING_ELEMENT,
	XML_ERROR_FAILED_TO_READ_ELEMENT_NAME,
	XML_ERROR_READING_ELEMENT_VALUE,
	XML_ERROR_READING_ATTRIBUTES,
	XML_ERROR_PARSING_EMPTY,
	XML_ERROR_READING_END_TAG,
	XML_ERROR_PARSING_UNKNOWN,
	XML_ERROR_PARSING_COMMENT,
	XML_ERROR_PARSING_DECLARATION,
	XML_ERROR_DOCUMENT_EMPTY,
	XML_ERROR_EMBEDDED_NULL,
	XML_ERROR_PARSING_CDATA,
	XML_ERROR_DOCUMENT_TOP_ONLY,
	XML_ERROR_STRING_COUNT
	};

#endif
