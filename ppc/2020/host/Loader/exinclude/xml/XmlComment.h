/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlComment.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file  XmlComment.h
 * @brief
 *    <li>XMLע�ͽڵ��ඨ��</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _XMLCOMMENT_H
#define _XMLCOMMENT_H

#include "XmlNode.h"
#include "XmlParsingData.h"
#include "XmlNodeType.h"

class XmlComment : public XmlNode
{
public:
	
	/*���캯��*/
	XmlComment() : XmlNode(COMMENT) {}

	XmlComment( const char* _value ) : XmlNode(COMMENT) 
	{
		SetValue(_value);
	}

	XmlComment( const XmlComment& );

	void operator=( const XmlComment& base );

	/*����������*/
	virtual ~XmlComment()	{}
	
	/*��¡ע��*/
	virtual XmlNode* Clone() const;

	/*дע�͵�һ���ļ���*/
	virtual void Print( FILE* cfile, int depth ) const;

	/*����ע��*/
	virtual const char* Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

	/*��õ�ǰע�ͽڵ�*/
	virtual const XmlComment*  ToComment() const { return this; }
	
	virtual XmlComment*  ToComment() { return this; }

	virtual bool Accept( XmlVisitor* visitor ) const;

protected:
	void CopyTo(XmlComment* target) const;

};


#endif
