/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: xml.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   xml.h
 * @brief
 *    <li>头文件包含</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */


#ifndef XML_INCLUDED
#define XML_INCLUDED

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <assert.h>
#include "XmlUtils.h"
#include "XmlNodeType.h"
#include "XmlEncoding.h"

#if defined( _DEBUG ) && !defined( DEBUG )
#define DEBUG
#endif

/*版本编译控制*/
#define XML_SAFE

#ifdef XML_SAFE
	#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
		#define XML_SNPRINTF _snprintf_s
		#define XML_SNSCANF  _snscanf_s
		#define XML_SSCANF   sscanf_s
	#elif defined(_MSC_VER) && (_MSC_VER >= 1200 )
		#define XML_SNPRINTF _snprintf
		#define XML_SNSCANF  _snscanf
		#define XML_SSCANF   sscanf
	#elif defined(__GNUC__) && (__GNUC__ >= 3 )
		#define XML_SNPRINTF snprintf
		#define XML_SNSCANF  snscanf
		#define XML_SSCANF   sscanf
	#else
		#define XML_SSCANF   sscanf
	#endif
#endif	

const int XML_MAJOR_VERSION = 2;
const int XML_MINOR_VERSION = 5;
const int XML_PATCH_VERSION = 3;

const XmlEncoding XML_DEFAULT_ENCODING = XML_ENCODING_UNKNOWN;

extern const unsigned char XML_UTF_LEAD_0;
extern const unsigned char XML_UTF_LEAD_1;
extern const unsigned char XML_UTF_LEAD_2;

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif

