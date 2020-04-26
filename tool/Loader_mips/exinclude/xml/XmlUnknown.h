/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlUnknown.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*双
 * @file   XmlUnknown.h
 * @brief
 *    <li>解析器支持的XML节点类型,所有不支持的都是UNKNOWN类型</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */
#ifndef _XMLUNKNOWN_H
#define _XMLUNKNOWN_H

/************************引用部分*****************************/
#include "XmlNodeType.h"
#include "XmlNode.h"
#include "XmlVisitor.h"

/************************定义部分*****************************/

class XmlUnknown : public XmlNode
{
public:

	/*构造函数*/
	XmlUnknown() : XmlNode(UNKNOWN) {}

	/*虚析构函数*/
	virtual ~XmlUnknown() {}

	/*拷贝构造函数*/
	XmlUnknown(const XmlUnknown& copy):XmlNode(UNKNOWN)		
	{ 
		copy.CopyTo(this); 
	}

	void operator=(const XmlUnknown& copy) { copy.CopyTo( this ); }

	/*克隆位置节点*/
	virtual XmlNode *Clone() const;

	/*打印未知节点内容到文件*/
	virtual void Print(FILE *cfile, int depth) const;

	/*解析未知节点*/
	virtual const char *Parse(const char* p, XmlParsingData* data, XmlEncoding encoding);

	virtual const XmlUnknown*  ToUnknown()  const { return this; } 
	
	virtual XmlUnknown*  ToUnknown()	 { return this; }

	virtual bool Accept( XmlVisitor* content ) const;

protected:

	void CopyTo(XmlUnknown* target) const;

};

#endif
