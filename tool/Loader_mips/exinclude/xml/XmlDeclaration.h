/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlDeclaration.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file   XmlDeclaration.h
 * @brief
 *    <li>XmlDeclaration�ඨ�壬������XML�ļ��Ŀ�ʼ���֣�
 *	   ����������������version, encoding, and standalone</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _XMLDECLARATION_H
#define _XMLDECLARATION_H

#include "XmlNodeType.h"
#include "XmlNode.h"
#include "XmlVisitor.h"

class XmlDeclaration : public XmlNode
{
public:
	/*���캯��*/
	XmlDeclaration() : XmlNode(DECLARATION) {}

	XmlDeclaration(const char* _version,
					const char* _encoding,
					const char* _standalone);

	XmlDeclaration( const XmlDeclaration& copy );
	
	void operator=( const XmlDeclaration& copy );

	/*����������*/
	virtual ~XmlDeclaration()	{}
	
	/*��ð汾�ַ���*/
	const char *Version() const	{ return version.c_str (); }
	
	/*��ñ����ַ���*/
	const char *Encoding() const { return encoding.c_str (); }
	
	/*���standalone�ַ���*/
	const char *Standalone() const		{ return standalone.c_str (); }

	virtual XmlNode* Clone() const;
	
	/*дע�͵��ļ���*/
	virtual void Print( FILE* cfile, int depth, XML_STRING* str ) const;
	
	virtual void Print( FILE* cfile, int depth ) const 
	{
		Print( cfile, depth, 0 );
	}
	
	/*���������ڵ�*/
	virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

	virtual const XmlDeclaration* ToDeclaration() const { return this; }
	
	virtual XmlDeclaration*  ToDeclaration()       { return this; }

	virtual bool Accept( XmlVisitor* visitor ) const;

protected:
	
	void CopyTo( XmlDeclaration* target ) const;

private:

	XML_STRING version;
	XML_STRING encoding;
	XML_STRING standalone;
};


#endif
