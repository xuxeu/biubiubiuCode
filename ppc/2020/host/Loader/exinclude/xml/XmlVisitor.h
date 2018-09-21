/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlVisitor.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file   XmlVisitor.h
 * @brief
 *    <li>������Ӧ�ڵ�ķ���</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */
#ifndef _XMLVISITOR_H
#define _XMLVISITOR_H
#include "XmlElement.h"

class XmlDocument;
class XmlDeclaration;
class XmlText;
class XmlComment;
class XmlUnknown;

class XmlVisitor
{
public:
	virtual ~XmlVisitor() {}

	virtual bool VisitEnter( const XmlDocument& /*doc*/ )	{ return true; }
	
	virtual bool VisitExit( const XmlDocument& /*doc*/ )	{ return true; }

	virtual bool VisitEnter( const XmlElement& /*element*/, const XmlAttribute* /*firstAttribute*/ ){ return true; }
	
	virtual bool VisitExit(const XmlElement& /*element*/){ return true; }

	virtual bool Visit( const XmlDeclaration& /*declaration*/ ) { return true; }
	
	virtual bool Visit( const XmlText& /*text*/ )	{ return true; }
	
	virtual bool Visit( const XmlComment& /*comment*/ ) { return true; }
	
	virtual bool Visit( const XmlUnknown& /*unknown*/ ) { return true; }
};


#endif
