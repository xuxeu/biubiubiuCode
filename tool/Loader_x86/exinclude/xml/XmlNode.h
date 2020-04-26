/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlNode.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   XmlNode.h
 * @brief
 *    <li>节点类定义</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _TINYXML_TIXMLNODE_H
#define _TINYXML_TIXMLNODE_H

/************************引用部分*****************************/
#include "XmlBase.h"
#include "XmlNodeType.h"

/************************声明部分*****************************/
class XmlDocument;
class XmlElement;
class XmlVisitor;
class XmlDeclaration;
class XmlText;
class XmlComment;
class XmlUnknown;

/************************定义部分*****************************/

class XmlNode:public XmlBase
{
	friend class XmlDocument;
	friend class XmlElement;

public:

	virtual ~XmlNode();

	const char *Value() const { return value.c_str (); }

	const XML_STRING& ValueTStr() const { return value; }

	void SetValue(const char * _value) { value = _value;}

	/*删除本节点的所有子结点*/
	void Clear();

	/*获得父节点*/
	XmlNode* Parent()	{ return parent; }
	
	const XmlNode* Parent() const	{ return parent; }

	/*获得该节点的第一个子结点*/
	const XmlNode* FirstChild() const { return firstChild;}
	
	XmlNode* FirstChild()	{ return firstChild;}
	
	const XmlNode* FirstChild(const char *value) const;
	
	XmlNode* FirstChild(const char *_value) 
	{
		return const_cast< XmlNode* > ((const_cast< const XmlNode* >(this))->FirstChild(_value));
	}
	
	/*获得该节点的最后一个子结点*/
	const XmlNode* LastChild() const	{ return lastChild; }		
	
	XmlNode* LastChild()	{ return lastChild; }

	const XmlNode* LastChild( const char * value ) const;
	
	XmlNode *LastChild( const char * _value ) 
	{
		return const_cast< XmlNode* > ((const_cast< const XmlNode* >(this))->LastChild( _value ));
	}

	/*遍历改节点所有子节点*/
	const XmlNode *IterateChildren( const XmlNode* previous ) const;

	XmlNode* IterateChildren( const XmlNode* previous ) 
	{
		return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->IterateChildren( previous ) );
	}

	const XmlNode* IterateChildren( const char * value, const XmlNode* previous ) const;
	
	XmlNode* IterateChildren( const char * _value, const XmlNode* previous ) 
	{
		return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->IterateChildren( _value, previous ) );
	}

	/*在最后节点添加一个节点*/
	XmlNode* InsertEndChild( const XmlNode& addThis );

	XmlNode* LinkEndChild( XmlNode* addThis );

	/*在该节点前添加一个节点*/
	XmlNode* InsertBeforeChild( XmlNode* beforeThis, const XmlNode& addThis );

	/*在该节点后添加一个节点*/
	XmlNode *InsertAfterChild(XmlNode *afterThis, const XmlNode &addThis);

	/*替代指定节点*/
	XmlNode *ReplaceChild( XmlNode *replaceThis, const XmlNode &withThis);

	/*删除指定节点*/
	bool RemoveChild(XmlNode* removeThis);

	/*获得该节点的前一个兄弟节点*/
	const XmlNode *PreviousSibling() const { return prev; }
	
	XmlNode *PreviousSibling() { return prev; }

	const XmlNode *PreviousSibling(const char *) const;
	
	XmlNode* PreviousSibling(const char *_prev) 
	{
		return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->PreviousSibling( _prev ) );
	}

	/*获得该节点的下一个兄弟节点*/
	const XmlNode* NextSibling() const { return next; }
	
	XmlNode* NextSibling() { return next; }

	const XmlNode* NextSibling( const char * ) const;
	
	XmlNode* NextSibling( const char* _next ) 
	{
		return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->NextSibling( _next ) );
	}

	/*获得下一个兄弟元素节点如果不是元素节点将跳过*/
	const XmlElement *NextSiblingElement() const;
	
	XmlElement* NextSiblingElement() 
	{
		return const_cast< XmlElement* >((const_cast< const XmlNode* >(this))->NextSiblingElement() );
	}

	const XmlElement* NextSiblingElement(const char *) const;
	
	XmlElement* NextSiblingElement( const char *_next ) 
	{
		return const_cast< XmlElement* >( (const_cast< const XmlNode* >(this))->NextSiblingElement( _next ) );
	}

	/*获得第一个元素节点如果不是元素节点将跳过*/
	const XmlElement* FirstChildElement()	const;
	
	XmlElement* FirstChildElement() 
	{
		return const_cast< XmlElement* >( (const_cast< const XmlNode* >(this))->FirstChildElement() );
	}

	const XmlElement* FirstChildElement( const char * _value ) const;
	
	XmlElement* FirstChildElement( const char * _value ) 
	{
		return const_cast< XmlElement* >( (const_cast< const XmlNode* >(this))->FirstChildElement( _value ) );
	}

	/*获得节点类型*/
	int Type() const { return type; }

	/*获得 Document节点指针*/
	const XmlDocument *GetDocument() const;
	
	XmlDocument *GetDocument() 
	{
		return const_cast< XmlDocument* >( (const_cast< const XmlNode* >(this))->GetDocument() );
	}

	/*判断是否有子结点*/
	bool NoChildren() const	{ return !firstChild;}

	virtual const XmlDocument*    ToDocument()    const { return 0; } 
	virtual const XmlElement*     ToElement()     const { return 0; } 
	virtual const XmlComment*     ToComment()     const { return 0; } 
	virtual const XmlUnknown*     ToUnknown()     const { return 0; } 
	virtual const XmlText*        ToText()        const { return 0; } 
	virtual const XmlDeclaration* ToDeclaration() const { return 0; } 

	virtual XmlDocument*          ToDocument()    { return 0; }
	virtual XmlElement*           ToElement()	    { return 0; } 
	virtual XmlComment*           ToComment()     { return 0; }
	virtual XmlUnknown*           ToUnknown()	    { return 0; } 
	virtual XmlText*	            ToText()        { return 0; }
	virtual XmlDeclaration*       ToDeclaration() { return 0; } 

	virtual XmlNode* Clone() const = 0;

	virtual bool Accept( XmlVisitor* visitor ) const = 0;

protected:
	XmlNode(NodeType _type);

	void CopyTo(XmlNode *target) const;

	XmlNode* Identify(const char *start, XmlEncoding encoding);

	XmlNode *parent;
	NodeType  type;

	XmlNode *firstChild;
	XmlNode *lastChild;

	XML_STRING value;

	XmlNode *prev;
	XmlNode *next;
  
private:
	XmlNode( const XmlNode& );				
	void operator=( const XmlNode& base );
};

#endif
