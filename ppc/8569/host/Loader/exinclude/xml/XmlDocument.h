/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlDocument.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file  XmlDocument.h
 * @brief
 *    <li>Document�ඨ��</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */
#ifndef _XMLDOCUMENT_H
#define _XMLDOCUMENT_H

#include "XmlNode.h"
#include "XmlEncoding.h"

class XmlDocument : public XmlNode
{
public:
	XmlDocument();

	XmlDocument( const char * documentName );

	XmlDocument( const XmlDocument& copy );
	
	void operator=( const XmlDocument& copy );

	virtual ~XmlDocument() {}

	/*����XMLһ���ļ�*/
	bool LoadFile(XmlEncoding encoding = XML_DEFAULT_ENCODING);
	
	/*����һ��XML�ļ�*/
	bool SaveFile() const;
	
	bool LoadFile( const char * filename, XmlEncoding encoding = XML_DEFAULT_ENCODING );
	
	bool SaveFile( const char * filename ) const;

	bool LoadFile( FILE*, XmlEncoding encoding = XML_DEFAULT_ENCODING );
	
	bool SaveFile( FILE* ) const;

	/*����XML�ļ�*/
	virtual const char* Parse( const char* p, XmlParsingData* data = 0, XmlEncoding encoding = XML_DEFAULT_ENCODING );

	/*��ø�Ԫ��*/
	const XmlElement* RootElement() const		{ return FirstChildElement(); }
	
	XmlElement* RootElement()	{ return FirstChildElement();}

	/*������ִ���error��������Ϊtrue*/
	bool Error() const	{ return error; }

	/*��ô����ַ���*/
	const char * ErrorDesc() const	{ return errorDesc.c_str (); }

	/*��ô���ID*/
	int ErrorId() const { return errorId; }

	/*��÷��������λ��*/
	int ErrorRow() const { return errorLocation.row+1; }
	
	int ErrorCol() const { return errorLocation.col+1; }

	void SetTabSize(int _tabsize) { tabsize = _tabsize; }

	int TabSize() const { return tabsize; }

	/*�������״̬*/
	void ClearError()
	{	
		error = false;
		errorId = 0;
		errorDesc = "";
		errorLocation.row = errorLocation.col = 0;
	}

	/*��ӡXML�ļ�������̨*/
	void Print() const	{ Print(stdout, 0 ); }

	virtual void Print(FILE* cfile, int depth = 0) const;

	/*���ó�����Ϣ*/
	void SetError(int err, const char* errorLocation, XmlParsingData* prevData, XmlEncoding encoding );

	virtual const XmlDocument *ToDocument() const { return this; }
	
	virtual XmlDocument *ToDocument()  { return this; }

	virtual bool Accept( XmlVisitor* content ) const;

protected :
	virtual XmlNode* Clone() const;

private:
	void CopyTo(XmlDocument* target) const;

	bool error;
	int  errorId;
	XML_STRING errorDesc;
	int tabsize;
	XmlCursor errorLocation;
	bool useMicrosoftBOM;
};

#endif
