/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: NodeType.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   NodeType.h
 * @brief
 *    <li>解析器支持的XML节点类型,所有不支持的都是UNKNOWN类型</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _XML_NODETYPE_H
#define _XML_NODETYPE_H

/*
W3C标准支持的节点类型有:
节点类型                            描述                                                      子元素 
Document               表示整个文档（DOM 树的根节点）		                Element(max.one)/ProcessingInstruction/Comment/DocumentType  
DocumentFragment       表示轻量级的 Document 对象，其中容纳了一部分文档。   ProcessingInstruction/Comment/Text/CDATASection/EntityReference 
DocumentType           向为文档定义的实体提供接口。                         None 
ProcessingInstruction  表示处理指令。                                       None 
EntityReference        表示实体引用元素。                                   ProcessingInstruction/Comment/Text/CDATASection/EntityReference 
Element                表示 element（元素）元素                             Text /Comment /ProcessingInstruction/CDATASection/EntityReference 
Attr                   表示属性。                                           Text /EntityReference 
Text                   表示元素或属性中的文本内容。                         None 
CDATASection           表示文档中的 CDATA 区段（文本不会被解析器解析）      None 
Comment                表示注释。                                           None 
Entity                 表示实体。                                           ProcessingInstruction/Comment/Text/CDATASection/EntityReference 
Notation               表示在 DTD 中声明的符号。                            None 
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
