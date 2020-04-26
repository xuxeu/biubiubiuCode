/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlAttribute.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file  XmlAttribute.h
 * @brief
 *    <li>XML节点的属性类,属性是一个键值对，元素可以有无数多的属性，
 *		元素的每个属性都有唯一的名字</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _XMLATTRIBUTE_H
#define _XMLATTRIBUTE_H

/************************引用部分*****************************/

#include "XmlBase.h"

/************************定义部分*****************************/

/*属性类定义*/
class XmlAttribute : public XmlBase
{

	friend class XmlAttributeSet;

public:

	/*构造函数*/
	XmlAttribute() : XmlBase()
	{
		document = 0;
		prev = next = 0;
	}

	XmlAttribute(const char * _name, const char * _value)
	{
		name = _name;
		value = _value;
		document = 0;
		prev = next = 0;
	}
	
	/*获得属性的名字*/
	const char *Name()  const { return name.c_str();}		
	
	/*获得属性的值*/
	const char *Value() const { return value.c_str();}
 
	/*将属性的值转换为整形*/
	int	IntValue() const;	

	/*将属性的值转换为双精度浮点型*/
	double	DoubleValue() const;

	/*获得属性的名字 返回类型为XML_STRING*/
	const XML_STRING& NameTStr() const { return name; }

	/*检查价值是否为整形，如果是就把值存储在_value中返回XML_SUCCESS
	如果不是返回XML_WRONG_TYPE*/
	int QueryIntValue(int *_value) const;

	int QueryDoubleValue(double *_value) const;

	/*设置属性的名字*/
	void SetName(const char *_name)	{ name = _name; }

	/*设置属性的值*/
	void SetValue(const char *_value) {value = _value;}				

	/*设置属性的值为整形*/
	void SetIntValue(int _value);	

	/*设置属性的值为双精度浮点型*/
	void SetDoubleValue(double _value);						

	/*获得当前属性的下一个属性*/
	const XmlAttribute *Next() const;

	XmlAttribute *Next() 
	{
		return const_cast< XmlAttribute* >( (const_cast< const XmlAttribute* >(this))->Next() );
	}

	/*获得当前属性的前一个属性*/
	const XmlAttribute* Previous() const;

	XmlAttribute* Previous() 
	{
		return const_cast< XmlAttribute* >( (const_cast< const XmlAttribute* >(this))->Previous() );
	}

	bool operator==( const XmlAttribute& rhs ) const { return rhs.name == name; }
	bool operator<( const XmlAttribute& rhs )	 const { return name < rhs.name; }
	bool operator>( const XmlAttribute& rhs )  const { return name > rhs.name; }

	virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

	/* 打印属性到文件流*/
	virtual void Print( FILE *cfile, int depth) const 
	{
		Print(cfile, depth, 0);
	}
	
	/* 打印属性到文件流和到字符串*/
	void Print(FILE* cfile, int depth, XML_STRING* str) const;

	void SetDocument( XmlDocument* doc )	{ document = doc; }

private:
	XmlAttribute( const XmlAttribute& );
	void operator=( const XmlAttribute& base );

	XmlDocument *document;
	XML_STRING name;
	XML_STRING value;
	XmlAttribute *prev;
	XmlAttribute *next;
};


#endif
