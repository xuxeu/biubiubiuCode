/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlBase.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   XmlBase.h
 * @brief
 *    <li>功能:XmlBase类定义,XmlBase是一个基础类,Xml其他类都继承于它.
	  XmlBase类提供实现了打印功能和一些通用函数</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _XMLBASE_H
#define _XMLBASE_H

/************************引用部分*****************************/
#include "XmlUtils.h"
#include "xml.h"
#include "XmlEncoding.h"
#include "XmlParsingData.h"

/************************定义部分*****************************/

class XmlBase
{
	friend class XmlNode;
	friend class XmlElement;
	friend class XmlDocument;

public:
	/*构造函数*/
	XmlBase()	: userData(0) {}
	
	/*析构函数*/
	virtual ~XmlBase() {}

	/*所有的TinyXml类都能打印它们自己到一个文件流*/
	virtual void Print( FILE* cfile, int depth ) const = 0;

	/*设置是否跳过空格*/
	static void SetCondenseWhiteSpace( bool condense ) { condenseWhiteSpace = condense;}

	/*返回当前空格设置*/
	static bool IsWhiteSpaceCondensed()	{ return condenseWhiteSpace;}

	/*获得节点或者属性在源文件的位置*/
	int Row() const	{ return location.row + 1;}
	
	int Column() const { return location.col + 1;}
	
	/*设置解析的用户数据*/
	void  SetUserData(void* user)	{ userData = user; }
	
	void *GetUserData()	{ return userData; }
	
	const void *GetUserData() const { return userData; }

	static const int utf8ByteTable[256];

	/*解析用户数据*/
	virtual const char* Parse(const char* p,
							 XmlParsingData* data,
							 XmlEncoding encoding) = 0;

	static void EncodeString( const XML_STRING& str, XML_STRING* out );

protected:

	/*跳过数据中的空格*/
	static const char *SkipWhiteSpace(const char *p, XmlEncoding encoding);
	
	/*判断是否是空格*/
	inline static bool IsWhiteSpace(char c)
	{
		return (isspace((unsigned char)c) || c == '\n' || c == '\r');
	}
	    
	inline static bool IsWhiteSpace(int c)
	{
		if (c < 256)
			return IsWhiteSpace((char)c);
		return false;
	}

	/*用于读取属性的名字*/
	static const char *ReadName( const char* p, XML_STRING* name, XmlEncoding encoding );
	
	/*用于读取属性的值*/
	static const char *ReadText(const char* in,			
								XML_STRING* text,			
								bool ignoreWhiteSpace,		
								const char* endTag,			
								bool ignoreCase,		
								XmlEncoding encoding);

	static const char* GetEntity(const char *in, char *value, int *length, XmlEncoding encoding );


	inline static const char *GetChar(const char *p, char *_value, int *length, XmlEncoding encoding)
	{
		assert(p);
		if(encoding == XML_ENCODING_UTF8)
		{
			*length = utf8ByteTable[*((const unsigned char*)p)];
			assert(*length >= 0 && *length < 5);
		}
		else
		{
			*length = 1;
		}

		if (*length == 1)
		{
			if(*p == '&')
			{
				return GetEntity(p, _value, length, encoding);
			}
			else
			{
				*_value = *p;
				return p+1;
			}
		}
		else if(*length)
		{
			for(int i = 0; p[i] && i < *length; ++i) 
			{
				_value[i] = p[i];
			}
			return p + (*length);
		}
		else
		{
			return 0;
		}
	}

	/*判断字符串是否相等*/
	static bool StringEqual(const char* p, const char* endTag, bool ignoreCase, XmlEncoding encoding);

	/*错误信息列表*/
	static const char* errorString[XML_ERROR_STRING_COUNT];

	XmlCursor location;

	void *userData;

	/*判断字符是否是字母*/
	static int IsAlpha(unsigned char anyByte, XmlEncoding encoding);
	
	/*判断字符是否是字母和数组*/
	static int IsAlphaNum(unsigned char anyByte, XmlEncoding encoding);
	
	/*大写字母转化为小写字母*/
	inline static int ToLower( int v, XmlEncoding encoding )
	{
		if (encoding == XML_ENCODING_UTF8)
		{
			if (v < 128) 
			{
				return tolower(v);
			}
			return v;
		}
		else
		{
			return tolower(v);
		}
	}

	/*字符集转换*/
	static void ConvertUTF32ToUTF8( unsigned long input, char* output, int* length );

private:

	XmlBase( const XmlBase& );
	void operator=( const XmlBase& base );
	struct Entity
	{
		const char*  str;
		unsigned int strLength;
		char chr;
	};
	enum
	{
		NUM_ENTITY = 5,
		MAX_ENTITY_LENGTH = 6

	};
	static Entity entity[NUM_ENTITY];
	static bool condenseWhiteSpace;
};

#endif


