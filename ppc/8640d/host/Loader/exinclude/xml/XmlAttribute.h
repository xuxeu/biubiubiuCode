/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlAttribute.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file  XmlAttribute.h
 * @brief
 *    <li>XML�ڵ��������,������һ����ֵ�ԣ�Ԫ�ؿ���������������ԣ�
 *		Ԫ�ص�ÿ�����Զ���Ψһ������</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _XMLATTRIBUTE_H
#define _XMLATTRIBUTE_H

/************************���ò���*****************************/

#include "XmlBase.h"

/************************���岿��*****************************/

/*�����ඨ��*/
class XmlAttribute : public XmlBase
{

	friend class XmlAttributeSet;

public:

	/*���캯��*/
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
	
	/*������Ե�����*/
	const char *Name()  const { return name.c_str();}		
	
	/*������Ե�ֵ*/
	const char *Value() const { return value.c_str();}
 
	/*�����Ե�ֵת��Ϊ����*/
	int	IntValue() const;	

	/*�����Ե�ֵת��Ϊ˫���ȸ�����*/
	double	DoubleValue() const;

	/*������Ե����� ��������ΪXML_STRING*/
	const XML_STRING& NameTStr() const { return name; }

	/*����ֵ�Ƿ�Ϊ���Σ�����ǾͰ�ֵ�洢��_value�з���XML_SUCCESS
	������Ƿ���XML_WRONG_TYPE*/
	int QueryIntValue(int *_value) const;

	int QueryDoubleValue(double *_value) const;

	/*�������Ե�����*/
	void SetName(const char *_name)	{ name = _name; }

	/*�������Ե�ֵ*/
	void SetValue(const char *_value) {value = _value;}				

	/*�������Ե�ֵΪ����*/
	void SetIntValue(int _value);	

	/*�������Ե�ֵΪ˫���ȸ�����*/
	void SetDoubleValue(double _value);						

	/*��õ�ǰ���Ե���һ������*/
	const XmlAttribute *Next() const;

	XmlAttribute *Next() 
	{
		return const_cast< XmlAttribute* >( (const_cast< const XmlAttribute* >(this))->Next() );
	}

	/*��õ�ǰ���Ե�ǰһ������*/
	const XmlAttribute* Previous() const;

	XmlAttribute* Previous() 
	{
		return const_cast< XmlAttribute* >( (const_cast< const XmlAttribute* >(this))->Previous() );
	}

	bool operator==( const XmlAttribute& rhs ) const { return rhs.name == name; }
	bool operator<( const XmlAttribute& rhs )	 const { return name < rhs.name; }
	bool operator>( const XmlAttribute& rhs )  const { return name > rhs.name; }

	virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

	/* ��ӡ���Ե��ļ���*/
	virtual void Print( FILE *cfile, int depth) const 
	{
		Print(cfile, depth, 0);
	}
	
	/* ��ӡ���Ե��ļ����͵��ַ���*/
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
