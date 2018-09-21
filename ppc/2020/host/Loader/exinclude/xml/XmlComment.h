/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlComment.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file  XmlComment.h
 * @brief
 *    <li>XML注释节点类定义</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _XMLCOMMENT_H
#define _XMLCOMMENT_H

#include "XmlNode.h"
#include "XmlParsingData.h"
#include "XmlNodeType.h"

class XmlComment : public XmlNode
{
public:
	
	/*构造函数*/
	XmlComment() : XmlNode(COMMENT) {}

	XmlComment( const char* _value ) : XmlNode(COMMENT) 
	{
		SetValue(_value);
	}

	XmlComment( const XmlComment& );

	void operator=( const XmlComment& base );

	/*虚析构函数*/
	virtual ~XmlComment()	{}
	
	/*克隆注释*/
	virtual XmlNode* Clone() const;

	/*写注释到一个文件流*/
	virtual void Print( FILE* cfile, int depth ) const;

	/*解析注释*/
	virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

	/*获得当前注释节点*/
	virtual const XmlComment*  ToComment() const { return this; }
	
	virtual XmlComment*  ToComment() { return this; }

	virtual bool Accept( XmlVisitor* visitor ) const;

protected:
	void CopyTo(XmlComment* target) const;

};


#endif
