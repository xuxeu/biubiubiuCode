/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlAttributeSet.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file XmlAttributeSet.h
 * @brief
 *    <li>XmlAttributeSet是一个内部类，用于管理一组属性，提供对属性的相关操作</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _XMLATTRIBUTESET_H
#define _XMLATTRIBUTESET_H

/************************引用部分*****************************/

#include "XmlAttribute.h"

/************************定义部分*****************************/

/*属性类定义*/
class XmlAttributeSet
{
public:

	/*构造函数*/
	XmlAttributeSet();

	/*析构函数*/
	~XmlAttributeSet();
	
	/*添加一个属性*/
	void Add(XmlAttribute *attribute);
	
	/*删除一个属性*/
	void Remove(XmlAttribute *attribute);
	
	/*节点的第一个属性*/
	const XmlAttribute *First() const	{ return (sentinel.next == &sentinel) ? 0 : sentinel.next; }
	
	XmlAttribute *First()	{ return(sentinel.next == &sentinel) ? 0 : sentinel.next; }
	
	/*节点的最后一个属性*/
	const XmlAttribute *Last() const {return (sentinel.prev == &sentinel) ? 0 : sentinel.prev;}
	
	XmlAttribute *Last()	{ return(sentinel.prev == &sentinel) ? 0 : sentinel.prev; }
	
	/*根据名字查找相应属性*/
	const XmlAttribute*	Find(const char *_name) const;
	
	XmlAttribute*	Find(const char *_name) 
	{
		return const_cast< XmlAttribute* >((const_cast< const XmlAttributeSet* >(this))->Find( _name ));
	}

private:
	XmlAttributeSet(const XmlAttributeSet&);
	void operator=(const XmlAttributeSet&);
	XmlAttribute sentinel;
};

#endif
