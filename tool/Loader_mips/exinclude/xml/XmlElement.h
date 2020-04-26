/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlElement.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file  XmlElement.h
 * @brief
 *    <li>XmlElement类定义</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _XMLELEMENT_H
#define _XMLELEMENT_H
#include "XmlUtils.h"
#include "XmlAttributeSet.h"
#include "XmlNode.h"
#include "XmlVisitor.h"

class XmlElement : public XmlNode
{
public:
	XmlElement (const char * in_value);

	XmlElement( const XmlElement& );

	void operator=( const XmlElement& base );

	virtual ~XmlElement();

	/*根据属性名字获得属性值*/
	const char *Attribute(const char *name) const;

	const char *Attribute(const char *name, int *i) const;

	const char *Attribute(const char *name, double *d) const;

	int QueryIntAttribute(const char *name, int *_value) const;
	
	int QueryDoubleAttribute(const char *name, double *_value) const;
	
	int QueryFloatAttribute(const char *name, float *_value) const 
	{
		double d;
		int result = QueryDoubleAttribute(name, &d);
		if ( result == XML_SUCCESS ) 
		{
			*_value = (float)d;
		}
		return result;
	}

	/*设置属性*/
	void SetAttribute(const char *name, const char *_value);

	void SetAttribute(const char *name, int value);

	void SetDoubleAttribute(const char *name, double value);

	/*删除指定属性*/
	void RemoveAttribute(const char *name);

	/*获取节点的第一个属性指针*/
	const XmlAttribute *FirstAttribute() const	{ return attributeSet.First(); }		
	
	XmlAttribute *FirstAttribute() 				{ return attributeSet.First(); }
	
	/*获取节点的最后一个属性指针*/
	const XmlAttribute *LastAttribute() const 	{ return attributeSet.Last(); }
	
	XmlAttribute *LastAttribute()					{ return attributeSet.Last(); }

	/*获得元素的文本内容*/
	const char *GetText() const;

	virtual XmlNode *Clone() const;

	virtual void Print( FILE* cfile, int depth ) const;

	/*解析元素*/
	virtual const char *Parse( const char *p, XmlParsingData* data, XmlEncoding encoding );

	virtual const XmlElement*  ToElement()     const { return this; } 
	
	virtual XmlElement *ToElement()	 { return this; } 

	virtual bool Accept(XmlVisitor* visitor) const;

protected:

	void CopyTo( XmlElement *target ) const;
	
	void ClearThis();

	const char *ReadValue( const char *in, XmlParsingData *prevData, XmlEncoding encoding );

private:

	XmlAttributeSet attributeSet;
};


#endif
