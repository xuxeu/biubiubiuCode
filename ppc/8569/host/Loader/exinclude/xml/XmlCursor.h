/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: XmlCursor.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file   XmlCursor.h
 * @brief
 *    <li>һ���ڲ��ṹ�壬��¼����ʱ�����ԭ�ļ��е�λ��</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */
#ifndef _XMLCURSOR_H
#define _XMLCURSOR_H

struct XmlCursor
{
	XmlCursor()		{ Clear(); }
	void Clear()		{ row = col = -1; }

	int row;//��
	int col;//��
};

#endif
