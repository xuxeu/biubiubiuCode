/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  XmlHandler.h
 * @Version        :  1.0.0
 * @Brief           :  ���豸��ص������ļ��Ľ���
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007��3��6�� 10:04:23
 * @History        : 
 *   
 */

#ifndef _XML_HANDLER_
#define _XML_HANDLER_

/************************���ò���*****************************/
#include <qfile.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qxml.h>
#include <qstring.h>
#include <QMap.h>
#include <QList.h>
#include "sysTypes.h"
#include <map>
#include "iceServerComm.h"

using namespace std;
/************************ǰ����������***********************/

/************************���岿��*****************************/
class XmlHandler : public QXmlDefaultHandler
{
public:
 //   XmlHandler(PhraseBook *phraseBook)
  //      : pb(phraseBook), ferrorCount(0) { }
    XmlHandler()
    {
        m_bpMem = 0;
        m_cpsr = 0;
        m_targetEndian = FALSE;
        dccEnable = FALSE;
        dccAddr = 0;
        jtagClock = 0;
        curCore = 0;
        coreNum = 0;
        memWidth = 0;
        m_targetType = LAMBDA_JTAG_ICE;
    }
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

    T_WORD  getBpMem();
    T_WORD  getCPSR();
    T_BOOL   getTargetEndian();
    QMap<T_WORD, T_WORD>   getCppRegisterInfo() const;
    QList<pair<T_WORD, T_WORD>*>   getMemeryConfigInfo() const;
    QList<T_WORD>  getMemConfigInfoWithDelay() const;
    
    QList<T_WORD>  getMemWidthCfg() const;
    
    QString  getICEConfigBuffer() const;

    T_BOOL getDccEnable();

    T_WORD getDccAddr() const;
    /*Modified by qingxiaohai on 2007��4��6�� 14:53:52 for TS3.2  [Begin]*/
    /*  Modified brief:record the target type, wcs simulator or JTAG ICE*/
    T_WORD getTargetType() const;

    T_WORD getTargetCpuType() const;
    /*Modified for TS3.2 [End]*/
private:

    //���������ļ������ICE��ͨ�Ÿ�ʽҪ����װ��buffer
    void  constructICEConfigBuffer();
    /*Modified by qingxiaohai on 2007��4��6�� 14:53:52 for TS3.2  [Begin]*/
    /*  Modified brief:record the target type, wcs simulator or JTAG ICE*/
    T_WORD  m_targetType;
    T_WORD  m_CpuType;                                         //ģ����ģ���CPU����
    /*Modified for TS3.2 [End]*/
    T_WORD m_bpMem;                                             //����ϵ��ַ����
    T_WORD m_cpsr;                                                  //CPSR�Ĵ�������   
    QMap<T_WORD, T_WORD> m_cpp;                     //CP15�Ĵ�����
    QList<pair<T_WORD, T_WORD>*> m_memCfg;   //�ڴ��ʼ������ pair->first() addr pair->second() value;
    QList<T_WORD> m_delayCfg;                               //�ӳ�����
    QList<T_WORD> m_memWidth;                           //ÿ����ʼ���ڴ�Ŀ��
    T_BOOL m_targetEndian;                                       //Ŀ����Ĵ�С��

    T_BOOL   dccEnable;                                               //DCCͨ���Ƿ񿪻�
    T_WORD  dccAddr;                                                 //DCC��ַ
    T_WORD  jtagClock;                                               //jtag ʱ��Ƶ��
    
    int curCore;                                                            //��ǰ�ں�ID
    int coreNum;                                                             //�ں�����
    QString coreInfo;                                                    //�ں�������Ϣ
    QString restrInfo;                                                    //������Ϣ
    QString IO;                                                                //IO��Ϣ
    QString arch;                                                             //��ϵ�ṹ��Ϣ
    QString app;                                                                //
    QString enableCashInfo;                                             //
    QString disableCashInfo;                                            //
    QString empCashInfo;                                                //
    QString strInfo;
    QString accum;                    //��ǰԪ�ص�ֵ
    int ferrorCount;
    int memWidth;                                                           //Ŀ����ڴ���
    
    QString iceCfgbuf;                                                      //ICE�����ļ����������װ����
};
/************************ʵ�ֲ���****************************/

#endif /*_XML_HANDLER_*/

