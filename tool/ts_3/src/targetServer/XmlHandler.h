/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* QT XML文件读取类
*
* @file     XmlHandler.h
* @brief     QT XML文件读取类
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/


#pragma once

#include <qfile.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qxml.h>
#include "tsinclude.h"
class DeviceConfigBase;
class XmlHandler : public QXmlDefaultHandler
{
public:
 //   XmlHandler(PhraseBook *phraseBook)
  //      : pb(phraseBook), ferrorCount(0) { }
    /**
    * 解释xml文件标签的第一步,作些数据成员的初始化
    * @param namespaceURI URI名字,在目前没有用处
    * @param localName在目前没有用处
    * @param qName 标签内容
    */
    virtual bool startElement(const QString &namespaceURI,
        const QString &localName, const QString &qName,
        const QXmlAttributes &atts);
    /**
    * 解释xml文件标签的最后一步,对读取的元素值进行复制
    * @param namespaceURI URI名字,在目前没有用处
    * @param localName在目前没有用处
    * @param qName 标签内容
    */
    virtual bool endElement(const QString &namespaceURI,
        const QString &localName, const QString &qName);
    /**
    * 解释xml文件标签,对读取的元素值进行赋值到成员变量accum中
    * @param ch 读取标签元素的值
    */
    virtual bool characters(const QString &ch);
    virtual bool fatalError(const QXmlParseException &exception);
    /**
    * 设置重试次数
    */
    void setRetry(int retry) ;
    /**
    * 设置超时时间
    */
    void setTimeout(int timeout);
    /**
    * 设置串口
    */
    void setCom(QString com) ;
    /**
    * 设置波特率
    */
    void setBaud(int baud) ;

private:
    /**
    * 创建带有动态库sa
    */
    void createSAWithDll() ;
    /**
    * 设置IP地址
    */
    void setPort(int port) ;

    /**
    * 设置IP地址
    */
    void setIP(const QString &IP) ;


    /**
    * 创建sa
    */
    void createSA() ;


    QString name;                    //名字
    QString dllName;                    //动态库名字
    QString reportType;                    //sa类型,上报类型
    QString dllBuf;                    //动态库需要的字符串
    QString state;                    //状态
    QString logFlag;                //日志标记
    QString protocolType;            //协议类型

    QString accum;                    //当前元素的值
    int ferrorCount;
    QVector<QString> vectorChannel;            //通道属性 

    QString extInfo;                         
    QString channelType;            //通道类型
    QString strIP;
    QString strCom;
    QString strBaud;
    QString strPort;
    QString strTimeout;
    QString strRetry;
    QString strWay;
    QString strGuid;
    DeviceConfigBase* pConfigBase;
};
