/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlText.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file   XmlText.h
 * @brief
 *    <li>�ı��ڵ��ඨ��</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _XMLTEXT_H
#define _XMLTEXT_H

/************************���ò���*****************************/

#include "XmlNode.h"
#include "XmlVisitor.h"
#include "XmlEncoding.h"

/************************��������*****************************/

class XmlParsingData;

/************************���岿��*****************************/

class XmlText:public XmlNode
{
	friend class XmlElement;
public:
	
	/*���캯��*/
	XmlText(const char * initValue):XmlNode(TEXT)
	{
		SetValue(initValue);
		cdata = false;
	}

	/*����������*/
	virtual ~XmlText() {}

	/*�������캯��*/
	XmlText( const XmlText& copy ):XmlNode(TEXT) 
	{ 
		copy.CopyTo( this ); 
	}
	
	void operator=(const XmlText& base)
	{ 
		base.CopyTo( this ); 
	}

	/*��ӡ�ı����ݵ��ļ�*/
	virtual void Print( FILE *cfile, int depth ) const;

	/*�鿴��ǰ�ı��Ƿ��õ���CDATA*/
	bool CDATA() const	{ return cdata; }
	
	void SetCDATA(bool _cdata) { cdata = _cdata; }

	/*�����ı��ڵ�*/
	virtual const char *Parse( const char* p, XmlParsingData* data, XmlEncoding encoding );

	virtual const XmlText *ToText() const { return this; } 
	
	virtual XmlText *ToText() { return this; } 

	virtual bool Accept( XmlVisitor* content ) const;

protected :
	virtual XmlNode *Clone() const;

	void CopyTo(XmlText *target) const;
	
	/*�ж��ı������Ƿ�Ϊ��*/
	bool Blank() const;	
private:
	bool cdata;	

};


#endif
