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
* QT XML�ļ���ȡ��
*
* @file     XmlHandler.h
* @brief     QT XML�ļ���ȡ��
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
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
    * ����xml�ļ���ǩ�ĵ�һ��,��Щ���ݳ�Ա�ĳ�ʼ��
    * @param namespaceURI URI����,��Ŀǰû���ô�
    * @param localName��Ŀǰû���ô�
    * @param qName ��ǩ����
    */
    virtual bool startElement(const QString &namespaceURI,
        const QString &localName, const QString &qName,
        const QXmlAttributes &atts);
    /**
    * ����xml�ļ���ǩ�����һ��,�Զ�ȡ��Ԫ��ֵ���и���
    * @param namespaceURI URI����,��Ŀǰû���ô�
    * @param localName��Ŀǰû���ô�
    * @param qName ��ǩ����
    */
    virtual bool endElement(const QString &namespaceURI,
        const QString &localName, const QString &qName);
    /**
    * ����xml�ļ���ǩ,�Զ�ȡ��Ԫ��ֵ���и�ֵ����Ա����accum��
    * @param ch ��ȡ��ǩԪ�ص�ֵ
    */
    virtual bool characters(const QString &ch);
    virtual bool fatalError(const QXmlParseException &exception);
    /**
    * �������Դ���
    */
    void setRetry(int retry) ;
    /**
    * ���ó�ʱʱ��
    */
    void setTimeout(int timeout);
    /**
    * ���ô���
    */
    void setCom(QString com) ;
    /**
    * ���ò�����
    */
    void setBaud(int baud) ;

private:
    /**
    * �������ж�̬��sa
    */
    void createSAWithDll() ;
    /**
    * ����IP��ַ
    */
    void setPort(int port) ;

    /**
    * ����IP��ַ
    */
    void setIP(const QString &IP) ;


    /**
    * ����sa
    */
    void createSA() ;


    QString name;                    //����
    QString dllName;                    //��̬������
    QString reportType;                    //sa����,�ϱ�����
    QString dllBuf;                    //��̬����Ҫ���ַ���
    QString state;                    //״̬
    QString logFlag;                //��־���
    QString protocolType;            //Э������

    QString accum;                    //��ǰԪ�ص�ֵ
    int ferrorCount;
    QVector<QString> vectorChannel;            //ͨ������ 

    QString extInfo;                         
    QString channelType;            //ͨ������
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
