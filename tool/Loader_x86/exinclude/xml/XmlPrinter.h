/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlPrinter.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file  XmlPrinter.h
 * @brief
 *    <li>��ӡ�ڴ�����ô�ӡ��ʽ</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _XMLPRINTER_H
#define _XMLPRINTER_H

#include "XmlVisitor.h"

class XmlPrinter : public XmlVisitor
{
public:
	XmlPrinter() : depth(0), simpleTextPrint(false),
					 buffer(), indent( "    " ), lineBreak( "\n" ) {}

	virtual bool VisitEnter(const XmlDocument& doc);
	
	virtual bool VisitExit(const XmlDocument& doc);
	
	virtual bool VisitEnter(const XmlElement& element, const XmlAttribute* firstAttribute);
	
	virtual bool VisitExit( const XmlElement& element );

	virtual bool Visit(const XmlDeclaration& declaration);
	
	virtual bool Visit(const XmlText& text);
	
	virtual bool Visit(const XmlComment& comment);
	
	virtual bool Visit(const XmlUnknown& unknown);

	void SetIndent( const char *_indent ) { indent = _indent ? _indent : "" ; }
	
	const char *Indent() { return indent.c_str(); }
	
	/*���÷ָ���Ĭ��Ϊ��\n��*/
	void SetLineBreak( const char *_lineBreak )	{ lineBreak = _lineBreak ? _lineBreak : ""; }
	
	const char* LineBreak()	{ return lineBreak.c_str(); }

	/*��������ӡ��ʽ*/
	void SetStreamPrinting()						
	{ 
		indent = "";
		lineBreak = "";											
	}	

	const char *CStr() { return buffer.c_str(); }
	
	size_t Size() { return buffer.size(); }

private:
	void DoIndent()	
	{
		for(int i = 0; i < depth; ++i)
		{
			buffer += indent;
		}
	}
	void DoLineBreak() 
	{
		buffer += lineBreak;
	}

	int depth;
	bool simpleTextPrint;
	XML_STRING buffer;
	XML_STRING indent;
	XML_STRING lineBreak;
};


#endif
