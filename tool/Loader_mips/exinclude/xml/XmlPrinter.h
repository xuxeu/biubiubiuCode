/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlPrinter.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file  XmlPrinter.h
 * @brief
 *    <li>打印内存和设置打印格式</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
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
	
	/*设置分割线默认为‘\n’*/
	void SetLineBreak( const char *_lineBreak )	{ lineBreak = _lineBreak ? _lineBreak : ""; }
	
	const char* LineBreak()	{ return lineBreak.c_str(); }

	/*设置流打印格式*/
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
