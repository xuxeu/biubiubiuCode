/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlParsingData.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   XmlParsingData.h
 * @brief
 *    <li>XmlParsingData类主要管理解析数据在源文件的位置</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _TINYXML_TIXMLPARSINGDATA_H
#define _TINYXML_TIXMLPARSINGDATA_H

/************************引用部分*****************************/

#include "XmlEncoding.h"
#include "XmlCursor.h"

/************************定义部分*****************************/

class XmlParsingData
{
	friend class XmlDocument;
  public:
	/*更新光标位置*/
	void Stamp( const char* now, XmlEncoding encoding );
	
	/*获得当前光标位置*/
	const XmlCursor& Cursor()	{ return cursor; }
  private:
	/*构造函数*/
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
