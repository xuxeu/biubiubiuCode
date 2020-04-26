/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlAttributeSet.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file XmlAttributeSet.h
 * @brief
 *    <li>XmlAttributeSet��һ���ڲ��࣬���ڹ���һ�����ԣ��ṩ�����Ե���ز���</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _XMLATTRIBUTESET_H
#define _XMLATTRIBUTESET_H

/************************���ò���*****************************/

#include "XmlAttribute.h"

/************************���岿��*****************************/

/*�����ඨ��*/
class XmlAttributeSet
{
public:

	/*���캯��*/
	XmlAttributeSet();

	/*��������*/
	~XmlAttributeSet();
	
	/*���һ������*/
	void Add(XmlAttribute *attribute);
	
	/*ɾ��һ������*/
	void Remove(XmlAttribute *attribute);
	
	/*�ڵ�ĵ�һ������*/
	const XmlAttribute *First() const	{ return (sentinel.next == &sentinel) ? 0 : sentinel.next; }
	
	XmlAttribute *First()	{ return(sentinel.next == &sentinel) ? 0 : sentinel.next; }
	
	/*�ڵ�����һ������*/
	const XmlAttribute *Last() const {return (sentinel.prev == &sentinel) ? 0 : sentinel.prev;}
	
	XmlAttribute *Last()	{ return(sentinel.prev == &sentinel) ? 0 : sentinel.prev; }
	
	/*�������ֲ�����Ӧ����*/
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
