/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  XmlHandler.h
 * @Version        :  1.0.0
 * @Brief           :  与设备相关的配置文件的解析
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年3月6日 10:04:23
 * @History        : 
 *   
 */

#ifndef _XML_HANDLER_
#define _XML_HANDLER_

/************************引用部分*****************************/
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
/************************前向声明部分***********************/

/************************定义部分*****************************/
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
    /*Modified by qingxiaohai on 2007年4月6日 14:53:52 for TS3.2  [Begin]*/
    /*  Modified brief:record the target type, wcs simulator or JTAG ICE*/
    T_WORD getTargetType() const;

    T_WORD getTargetCpuType() const;
    /*Modified for TS3.2 [End]*/
private:

    //解析配置文件后根据ICE的通信格式要求组装成buffer
    void  constructICEConfigBuffer();
    /*Modified by qingxiaohai on 2007年4月6日 14:53:52 for TS3.2  [Begin]*/
    /*  Modified brief:record the target type, wcs simulator or JTAG ICE*/
    T_WORD  m_targetType;
    T_WORD  m_CpuType;                                         //模拟器模拟的CPU类型
    /*Modified for TS3.2 [End]*/
    T_WORD m_bpMem;                                             //软件断点地址内容
    T_WORD m_cpsr;                                                  //CPSR寄存器配置   
    QMap<T_WORD, T_WORD> m_cpp;                     //CP15寄存器组
    QList<pair<T_WORD, T_WORD>*> m_memCfg;   //内存初始化配置 pair->first() addr pair->second() value;
    QList<T_WORD> m_delayCfg;                               //延迟配置
    QList<T_WORD> m_memWidth;                           //每条初始化内存的宽度
    T_BOOL m_targetEndian;                                       //目标机的大小端

    T_BOOL   dccEnable;                                               //DCC通道是否开户
    T_WORD  dccAddr;                                                 //DCC地址
    T_WORD  jtagClock;                                               //jtag 时钟频率
    
    int curCore;                                                            //当前内核ID
    int coreNum;                                                             //内核总数
    QString coreInfo;                                                    //内核配置信息
    QString restrInfo;                                                    //限制信息
    QString IO;                                                                //IO信息
    QString arch;                                                             //体系结构信息
    QString app;                                                                //
    QString enableCashInfo;                                             //
    QString disableCashInfo;                                            //
    QString empCashInfo;                                                //
    QString strInfo;
    QString accum;                    //当前元素的值
    int ferrorCount;
    int memWidth;                                                           //目标机内存宽度
    
    QString iceCfgbuf;                                                      //ICE配置文件解析后的组装缓冲
};
/************************实现部分****************************/

#endif /*_XML_HANDLER_*/

