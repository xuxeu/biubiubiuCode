/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlDocument.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file  XmlDocument.h
 * @brief
 *    <li>Document类定义</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */
#ifndef _XMLDOCUMENT_H
#define _XMLDOCUMENT_H

#include "XmlNode.h"
#include "XmlEncoding.h"

class XmlDocument : public XmlNode
{
public:
	XmlDocument();

	XmlDocument( const char * documentName );

	XmlDocument( const XmlDocument& copy );
	
	void operator=( const XmlDocument& copy );

	virtual ~XmlDocument() {}

	/*载入XML一个文件*/
	bool LoadFile(XmlEncoding encoding = XML_DEFAULT_ENCODING);
	
	/*保存一个XML文件*/
	bool SaveFile() const;
	
	bool LoadFile( const char * filename, XmlEncoding encoding = XML_DEFAULT_ENCODING );
	
	bool SaveFile( const char * filename ) const;

	bool LoadFile( FILE*, XmlEncoding encoding = XML_DEFAULT_ENCODING );
	
	bool SaveFile( FILE* ) const;

	/*解析XML文件*/
	virtual const char* Parse( const char* p, XmlParsingData* data = 0, XmlEncoding encoding = XML_DEFAULT_ENCODING );

	/*获得根元素*/
	const XmlElement* RootElement() const		{ return FirstChildElement(); }
	
	XmlElement* RootElement()	{ return FirstChildElement();}

	/*如果出现错误error将被设置为true*/
	bool Error() const	{ return error; }

	/*获得错误字符串*/
	const char * ErrorDesc() const	{ return errorDesc.c_str (); }

	/*获得错误ID*/
	int ErrorId() const { return errorId; }

	/*获得发生错误的位置*/
	int ErrorRow() const { return errorLocation.row+1; }
	
	int ErrorCol() const { return errorLocation.col+1; }

	void SetTabSize(int _tabsize) { tabsize = _tabsize; }

	int TabSize() const { return tabsize; }

	/*清除错误状态*/
	void ClearError()
	{	
		error = false;
		errorId = 0;
		errorDesc = "";
		errorLocation.row = errorLocation.col = 0;
	}

	/*打印XML文件到控制台*/
	void Print() const	{ Print(stdout, 0 ); }

	virtual void Print(FILE* cfile, int depth = 0) const;

	/*设置出错信息*/
	void SetError(int err, const char* errorLocation, XmlParsingData* prevData, XmlEncoding encoding );

	virtual const XmlDocument *ToDocument() const { return this; }
	
	virtual XmlDocument *ToDocument()  { return this; }

	virtual bool Accept( XmlVisitor* content ) const;

protected :
	virtual XmlNode* Clone() const;

private:
	void CopyTo(XmlDocument* target) const;

	bool error;
	int  errorId;
	XML_STRING errorDesc;
	int tabsize;
	XmlCursor errorLocation;
	bool useMicrosoftBOM;
};

#endif
