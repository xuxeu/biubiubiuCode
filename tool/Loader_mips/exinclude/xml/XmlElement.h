/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlElement.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file  XmlElement.h
 * @brief
 *    <li>XmlElement�ඨ��</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
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

	/*�����������ֻ������ֵ*/
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

	/*��������*/
	void SetAttribute(const char *name, const char *_value);

	void SetAttribute(const char *name, int value);

	void SetDoubleAttribute(const char *name, double value);

	/*ɾ��ָ������*/
	void RemoveAttribute(const char *name);

	/*��ȡ�ڵ�ĵ�һ������ָ��*/
	const XmlAttribute *FirstAttribute() const	{ return attributeSet.First(); }		
	
	XmlAttribute *FirstAttribute() 				{ return attributeSet.First(); }
	
	/*��ȡ�ڵ�����һ������ָ��*/
	const XmlAttribute *LastAttribute() const 	{ return attributeSet.Last(); }
	
	XmlAttribute *LastAttribute()					{ return attributeSet.Last(); }

	/*���Ԫ�ص��ı�����*/
	const char *GetText() const;

	virtual XmlNode *Clone() const;

	virtual void Print( FILE* cfile, int depth ) const;

	/*����Ԫ��*/
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
