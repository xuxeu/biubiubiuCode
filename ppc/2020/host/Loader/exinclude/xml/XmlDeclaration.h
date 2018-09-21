/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlDeclaration.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   XmlDeclaration.h
 * @brief
 *    <li>XmlDeclaration类定义，声明是XML文件的开始部分，
 *	   声明包含三种属性version, encoding, and standalone</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _XMLDECLARATION_H
#define _XMLDECLARATION_H

#include "XmlNodeType.h"
#include "XmlNode.h"
#include "XmlVisitor.h"

class XmlDeclaration : public XmlNode
{
public:
	/*构造函数*/
	XmlDeclaration() : XmlNode(DECLARATION) {}

	XmlDeclaration(const char* _version,
					const char* _encoding,
					const char* _standalone);

	XmlDeclaration( const XmlDeclaration& copy );
	
	void operator=( const XmlDeclaration& copy );

	/*虚析构函数*/
	virtual ~XmlDeclaration()	{}
	
	/*获得版本字符串*/
	const char *Version() const	{ return version.c_str (); }
	
	/*获得编码字符串*/
	const char *Encoding() const { return encoding.c_str (); }
	
	/*获得standalone字符串*/
	const char *Standalone() const		{ return standalone.c_str (); }

	virtual XmlNode* Clone() const;
	
	/*写注释到文件流*/
	virtual void Print( FILE* cfile, int depth, XML_STRING* str ) const;
	
	virtual void Print( FILE* cfile, int depth ) const 
	{
		Print( cfile, depth, 0 );
	}
	
	/*解析声明节点*/
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
