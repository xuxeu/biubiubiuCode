/************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: NodeType.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * �����ļ���
 */


/*
 * @file   NodeType.h
 * @brief
 *    <li>������֧�ֵ�XML�ڵ�����,���в�֧�ֵĶ���UNKNOWN����</li>
 *                      
 * @author   ��Ԫ־
 * @date     2009-10-20
 * <p>���ţ� ϵͳ��
 */

#ifndef _XML_NODETYPE_H
#define _XML_NODETYPE_H

/*
W3C��׼֧�ֵĽڵ�������:
�ڵ�����                            ����                                                      ��Ԫ�� 
Document               ��ʾ�����ĵ���DOM ���ĸ��ڵ㣩		                Element(max.one)/ProcessingInstruction/Comment/DocumentType  
DocumentFragment       ��ʾ�������� Document ��������������һ�����ĵ���   ProcessingInstruction/Comment/Text/CDATASection/EntityReference 
DocumentType           ��Ϊ�ĵ������ʵ���ṩ�ӿڡ�                         None 
ProcessingInstruction  ��ʾ����ָ�                                       None 
EntityReference        ��ʾʵ������Ԫ�ء�                                   ProcessingInstruction/Comment/Text/CDATASection/EntityReference 
Element                ��ʾ element��Ԫ�أ�Ԫ��                             Text /Comment /ProcessingInstruction/CDATASection/EntityReference 
Attr                   ��ʾ���ԡ�                                           Text /EntityReference 
Text                   ��ʾԪ�ػ������е��ı����ݡ�                         None 
CDATASection           ��ʾ�ĵ��е� CDATA ���Σ��ı����ᱻ������������      None 
Comment                ��ʾע�͡�                                           None 
Entity                 ��ʾʵ�塣                                           ProcessingInstruction/Comment/Text/CDATASection/EntityReference 
Notation               ��ʾ�� DTD �������ķ��š�                            None 
*/
enum NodeType
{
	DOCUMENT,
	ELEMENT,
	COMMENT,
	UNKNOWN,
	TEXT,
	DECLARATION,
	TYPECOUNT
};

#endif
