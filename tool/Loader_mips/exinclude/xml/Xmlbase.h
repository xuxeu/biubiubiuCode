/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlBase.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file   XmlBase.h
 * @brief
 *    <li>����:XmlBase�ඨ��,XmlBase��һ��������,Xml�����඼�̳�����.
	  XmlBase���ṩʵ���˴�ӡ���ܺ�һЩͨ�ú���</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _XMLBASE_H
#define _XMLBASE_H

/************************���ò���*****************************/
#include "XmlUtils.h"
#include "xml.h"
#include "XmlEncoding.h"
#include "XmlParsingData.h"

/************************���岿��*****************************/

class XmlBase
{
	friend class XmlNode;
	friend class XmlElement;
	friend class XmlDocument;

public:
	/*���캯��*/
	XmlBase()	: userData(0) {}
	
	/*��������*/
	virtual ~XmlBase() {}

	/*���е�TinyXml�඼�ܴ�ӡ�����Լ���һ���ļ���*/
	virtual void Print( FILE* cfile, int depth ) const = 0;

	/*�����Ƿ������ո�*/
	static void SetCondenseWhiteSpace( bool condense ) { condenseWhiteSpace = condense;}

	/*���ص�ǰ�ո�����*/
	static bool IsWhiteSpaceCondensed()	{ return condenseWhiteSpace;}

	/*��ýڵ����������Դ�ļ���λ��*/
	int Row() const	{ return location.row + 1;}
	
	int Column() const { return location.col + 1;}
	
	/*���ý������û�����*/
	void  SetUserData(void* user)	{ userData = user; }
	
	void *GetUserData()	{ return userData; }
	
	const void *GetUserData() const { return userData; }

	static const int utf8ByteTable[256];

	/*�����û�����*/
	virtual const char* Parse(const char* p,
							 XmlParsingData* data,
							 XmlEncoding encoding) = 0;

	static void EncodeString( const XML_STRING& str, XML_STRING* out );

protected:

	/*���������еĿո�*/
	static const char *SkipWhiteSpace(const char *p, XmlEncoding encoding);
	
	/*�ж��Ƿ��ǿո�*/
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

	/*���ڶ�ȡ���Ե�����*/
	static const char *ReadName( const char* p, XML_STRING* name, XmlEncoding encoding );
	
	/*���ڶ�ȡ���Ե�ֵ*/
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

	/*�ж��ַ����Ƿ����*/
	static bool StringEqual(const char* p, const char* endTag, bool ignoreCase, XmlEncoding encoding);

	/*������Ϣ�б�*/
	static const char* errorString[XML_ERROR_STRING_COUNT];

	XmlCursor location;

	void *userData;

	/*�ж��ַ��Ƿ�����ĸ*/
	static int IsAlpha(unsigned char anyByte, XmlEncoding encoding);
	
	/*�ж��ַ��Ƿ�����ĸ������*/
	static int IsAlphaNum(unsigned char anyByte, XmlEncoding encoding);
	
	/*��д��ĸת��ΪСд��ĸ*/
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

	/*�ַ���ת��*/
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


