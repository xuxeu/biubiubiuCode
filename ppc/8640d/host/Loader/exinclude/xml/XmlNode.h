/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlNode.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file   XmlNode.h
 * @brief
 *    <li>�ڵ��ඨ��</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _TINYXML_TIXMLNODE_H
#define _TINYXML_TIXMLNODE_H

/************************���ò���*****************************/
#include "XmlBase.h"
#include "XmlNodeType.h"

/************************��������*****************************/
class XmlDocument;
class XmlElement;
class XmlVisitor;
class XmlDeclaration;
class XmlText;
class XmlComment;
class XmlUnknown;

/************************���岿��*****************************/

class XmlNode:public XmlBase
{
	friend class XmlDocument;
	friend class XmlElement;

public:

	virtual ~XmlNode();

	const char *Value() const { return value.c_str (); }

	const XML_STRING& ValueTStr() const { return value; }

	void SetValue(const char * _value) { value = _value;}

	/*ɾ�����ڵ�������ӽ��*/
	void Clear();

	/*��ø��ڵ�*/
	XmlNode* Parent()	{ return parent; }
	
	const XmlNode* Parent() const	{ return parent; }

	/*��øýڵ�ĵ�һ���ӽ��*/
	const XmlNode* FirstChild() const { return firstChild;}
	
	XmlNode* FirstChild()	{ return firstChild;}
	
	const XmlNode* FirstChild(const char *value) const;
	
	XmlNode* FirstChild(const char *_value) 
	{
		return const_cast< XmlNode* > ((const_cast< const XmlNode* >(this))->FirstChild(_value));
	}
	
	/*��øýڵ�����һ���ӽ��*/
	const XmlNode* LastChild() const	{ return lastChild; }		
	
	XmlNode* LastChild()	{ return lastChild; }

	const XmlNode* LastChild( const char * value ) const;
	
	XmlNode *LastChild( const char * _value ) 
	{
		return const_cast< XmlNode* > ((const_cast< const XmlNode* >(this))->LastChild( _value ));
	}

	/*�����Ľڵ������ӽڵ�*/
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

	/*�����ڵ����һ���ڵ�*/
	XmlNode* InsertEndChild( const XmlNode& addThis );

	XmlNode* LinkEndChild( XmlNode* addThis );

	/*�ڸýڵ�ǰ���һ���ڵ�*/
	XmlNode* InsertBeforeChild( XmlNode* beforeThis, const XmlNode& addThis );

	/*�ڸýڵ�����һ���ڵ�*/
	XmlNode *InsertAfterChild(XmlNode *afterThis, const XmlNode &addThis);

	/*���ָ���ڵ�*/
	XmlNode *ReplaceChild( XmlNode *replaceThis, const XmlNode &withThis);

	/*ɾ��ָ���ڵ�*/
	bool RemoveChild(XmlNode* removeThis);

	/*��øýڵ��ǰһ���ֵܽڵ�*/
	const XmlNode *PreviousSibling() const { return prev; }
	
	XmlNode *PreviousSibling() { return prev; }

	const XmlNode *PreviousSibling(const char *) const;
	
	XmlNode* PreviousSibling(const char *_prev) 
	{
		return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->PreviousSibling( _prev ) );
	}

	/*��øýڵ����һ���ֵܽڵ�*/
	const XmlNode* NextSibling() const { return next; }
	
	XmlNode* NextSibling() { return next; }

	const XmlNode* NextSibling( const char * ) const;
	
	XmlNode* NextSibling( const char* _next ) 
	{
		return const_cast< XmlNode* >( (const_cast< const XmlNode* >(this))->NextSibling( _next ) );
	}

	/*�����һ���ֵ�Ԫ�ؽڵ��������Ԫ�ؽڵ㽫����*/
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

	/*��õ�һ��Ԫ�ؽڵ��������Ԫ�ؽڵ㽫����*/
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

	/*��ýڵ�����*/
	int Type() const { return type; }

	/*��� Document�ڵ�ָ��*/
	const XmlDocument *GetDocument() const;
	
	XmlDocument *GetDocument() 
	{
		return const_cast< XmlDocument* >( (const_cast< const XmlNode* >(this))->GetDocument() );
	}

	/*�ж��Ƿ����ӽ��*/
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
