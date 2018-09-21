/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log:XmlHandle.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file  XmlHandle.h
 * @brief
 *    <li>XmlHandle类封装了对节点指针的空检查</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
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

	/*获得第一个子节点句柄*/
	XmlHandle FirstChild() const;
	
	XmlHandle FirstChild( const char * value ) const;
	
	XmlHandle FirstChildElement() const;
		
	/*获得第一个元素句柄*/
	XmlHandle FirstChildElement( const char * value ) const;

	XmlHandle Child( const char* value, int index ) const;
	
	XmlHandle Child(int index ) const;
	
	XmlHandle ChildElement( const char* value, int index ) const;
	
	XmlHandle ChildElement( int index ) const;

	/*获得节点指针*/
	XmlNode *ToNode() const { return node; } 
	
	/*获得元素指针*/
	XmlElement *ToElement() const	{ return ((node && node->ToElement()) ? node->ToElement() : 0); }
	
	/*获得文本指针*/
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
