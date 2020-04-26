/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlText.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   XmlText.h
 * @brief
 *    <li>文本节点类定义</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _XMLTEXT_H
#define _XMLTEXT_H

/************************引用部分*****************************/

#include "XmlNode.h"
#include "XmlVisitor.h"
#include "XmlEncoding.h"

/************************声明部分*****************************/

class XmlParsingData;

/************************定义部分*****************************/

class XmlText:public XmlNode
{
	friend class XmlElement;
public:
	
	/*构造函数*/
	XmlText(const char * initValue):XmlNode(TEXT)
	{
		SetValue(initValue);
		cdata = false;
	}

	/*虚析构函数*/
	virtual ~XmlText() {}

	/*拷贝构造函数*/
	XmlText( const XmlText& copy ):XmlNode(TEXT) 
	{ 
		copy.CopyTo( this ); 
	}
	
	void operator=(const XmlText& base)
	{ 
		base.CopyTo( this ); 
	}

	/*打印文本内容到文件*/
	virtual void Print( FILE *cfile, int depth ) const;

	/*查看当前文本是否用的是CDATA*/
	bool CDATA() const	{ return cdata; }
	
	void SetCDATA(bool _cdata) { cdata = _cdata; }

	/*解析文本节点*/
	virtual const char *Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

	virtual const XmlText *ToText() const { return this; } 
	
	virtual XmlText *ToText() { return this; } 

	virtual bool Accept( XmlVisitor* content ) const;

protected :
	virtual XmlNode *Clone() const;

	void CopyTo(XmlText *target) const;
	
	/*判断文本内容是否为空*/
	bool Blank() const;	
private:
	bool cdata;	

};


#endif
