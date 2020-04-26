/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log:XmlHandle.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file  XmlHandle.h
 * @brief
 *    <li>XmlHandle���װ�˶Խڵ�ָ��Ŀռ��</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _XMLHANDLE_H
#define _XMLHANDLE_H

#include "XmlNode.h"
class XmlElement;
class XmlUnknown;
class XmlText;

class XmlHandle
{
public:
	XmlHandle( XmlNode* _node )	{ this->node = _node;}
	
	XmlHandle( const XmlHandle& ref )			{ this->node = ref.node; }
	
	XmlHandle operator=( const XmlHandle& ref ) { this->node = ref.node; return *this; }

	/*��õ�һ���ӽڵ���*/
	XmlHandle FirstChild() const;
	
	XmlHandle FirstChild( const char * value ) const;
	
	XmlHandle FirstChildElement() const;
		
	/*��õ�һ��Ԫ�ؾ��*/
	XmlHandle FirstChildElement( const char * value ) const;

	XmlHandle Child( const char* value, int index ) const;
	
	XmlHandle Child(int index ) const;
	
	XmlHandle ChildElement( const char* value, int index ) const;
	
	XmlHandle ChildElement( int index ) const;

	/*��ýڵ�ָ��*/
	XmlNode *ToNode() const { return node; } 
	
	/*���Ԫ��ָ��*/
	XmlElement *ToElement() const	{ return ((node && node->ToElement()) ? node->ToElement() : 0); }
	
	/*����ı�ָ��*/
	XmlText *ToText() const	{ return (( node && node->ToText()) ? node->ToText() : 0); }
	
	XmlUnknown *ToUnknown() const	{ return ((node && node->ToUnknown()) ? node->ToUnknown() : 0); }

	XmlNode *Node() const	{ return ToNode(); } 
	
	XmlElement *Element() const{ return ToElement(); }
	
	XmlText *Text() const	{ return ToText(); }

	XmlUnknown *Unknown() const{ return ToUnknown(); }

private:
	XmlNode *node;
};


#endif
