/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlEncoding.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   XmlEncoding.h
 * @brief
 *    <li>解析XML文件的编码方式</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */
#ifndef _XMLENCODING_H
#define _XMLENCODING_H

/*编码类型*/
enum XmlEncoding
{
	XML_ENCODING_UNKNOWN,
	XML_ENCODING_UTF8,
	XML_ENCODING_LEGACY
};

#endif
