/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  XmlHandler.h
* @brief
*       功能：
*       <li>QT XML文件读取类</li>
*/

#ifndef XMLHANDLER_H
#define XMLHANDLER_H
/************************头文件********************************/


#pragma once

#include <qfile.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qxml.h>
#include "tsinclude.h"

/************************宏定义********************************/
/************************类型定义******************************/

class DeviceConfigBase;
class XmlHandler : public QXmlDefaultHandler
{
public:
    XmlHandler()
    {
        ferrorCount = 0;
    }
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
/************************接口声明******************************/

#endif