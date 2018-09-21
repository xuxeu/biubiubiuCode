/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlUnknown.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*˫
 * @file   XmlUnknown.h
 * @brief
 *    <li>������֧�ֵ�XML�ڵ�����,���в�֧�ֵĶ���UNKNOWN����</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */
#ifndef _XMLUNKNOWN_H
#define _XMLUNKNOWN_H

/************************���ò���*****************************/
#include "XmlNodeType.h"
#include "XmlNode.h"
#include "XmlVisitor.h"

/************************���岿��*****************************/

class XmlUnknown : public XmlNode
{
public:

	/*���캯��*/
	XmlUnknown() : XmlNode(UNKNOWN) {}

	/*����������*/
	virtual ~XmlUnknown() {}

	/*�������캯��*/
	XmlUnknown(const XmlUnknown& copy):XmlNode(UNKNOWN)		
	{ 
		copy.CopyTo(this); 
	}

	void operator=(const XmlUnknown& copy) { copy.CopyTo( this ); }

	/*��¡λ�ýڵ�*/
	virtual XmlNode *Clone() const;

	/*��ӡδ֪�ڵ����ݵ��ļ�*/
	virtual void Print(FILE *cfile, int depth) const;

	/*����δ֪�ڵ�*/
	virtual const char *Parse(const char* p, XmlParsingData* data, XmlEncoding encoding);

	virtual const XmlUnknown*  ToUnknown()  const { return this; } 
	
	virtual XmlUnknown*  ToUnknown()	 { return this; }

	virtual bool Accept( XmlVisitor* content ) const;

protected:

	void CopyTo(XmlUnknown* target) const;

};

#endif
