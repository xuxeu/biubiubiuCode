/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlCursor.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   XmlCursor.h
 * @brief
 *    <li>一个内部结构体，记录解析时光标在原文件中的位置</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */
#ifndef _XMLCURSOR_H
#define _XMLCURSOR_H

struct XmlCursor
{
	XmlCursor()		{ Clear(); }
	void Clear()		{ row = col = -1; }

	int row;//行
	int col;//列
};

#endif
