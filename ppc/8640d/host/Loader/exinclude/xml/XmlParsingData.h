/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlParsingData.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file   XmlParsingData.h
 * @brief
 *    <li>XmlParsingData����Ҫ�������������Դ�ļ���λ��</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _TINYXML_TIXMLPARSINGDATA_H
#define _TINYXML_TIXMLPARSINGDATA_H

/************************���ò���*****************************/

#include "XmlEncoding.h"
#include "XmlCursor.h"

/************************���岿��*****************************/

class XmlParsingData
{
	friend class XmlDocument;
  public:
	/*���¹��λ��*/
	void Stamp( const char* now, XmlEncoding encoding );
	
	/*��õ�ǰ���λ��*/
	const XmlCursor& Cursor()	{ return cursor; }
  private:
	/*���캯��*/
	XmlParsingData(const char* start, int _tabsize, int row, int col)
	{
		assert( start );
		stamp = start;
		tabsize = _tabsize;
		cursor.row = row;
		cursor.col = col;
	}

	XmlCursor	cursor;
	const char*	stamp;
	int	tabsize;
};

#endif
