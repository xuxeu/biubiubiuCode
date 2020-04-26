/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  XmlHandler.cpp
 * @Version        :  1.0.0
 * @Brief           :  与设备相关的配置文件的解析处理实现
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年3月6日 10:07:32
 * @History        : 
 *   
 */


/************************引用部分*****************************/
#include "xmlhandler.h"
#include "QDataStream.h"
#include "QByteArray.h"
#include "Qtextcodec.h"
#include "util.h"
#include "QByteArray.h"
#include "IceServerComm.h"
#include "stdlib.h"
#include "common.h"
/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/

//获取ICE的ID号


T_WORD XmlHandler::getCPSR()
{
    return m_cpsr;
}


T_BOOL XmlHandler::getTargetEndian()
{
    return m_targetEndian;
}

T_WORD  XmlHandler::getBpMem()
{
    return m_bpMem;
}

QMap<T_WORD, T_WORD>   XmlHandler::getCppRegisterInfo() const
{
    return m_cpp;
}

QList<pair<T_WORD, T_WORD>*>   XmlHandler::getMemeryConfigInfo() const
{
    return m_memCfg;
}

QList<T_WORD>  XmlHandler::getMemConfigInfoWithDelay() const
{
    return m_delayCfg;
}

QList<T_WORD> XmlHandler::getMemWidthCfg() const
{
    return m_memWidth;
}

QString XmlHandler::getICEConfigBuffer() const
{
    return iceCfgbuf;
}

T_BOOL  XmlHandler::getDccEnable()
{
    return dccEnable;
}

T_WORD XmlHandler::getDccAddr() const
{
    return dccAddr;
}

T_WORD XmlHandler::getTargetType() const
{
    return m_targetType;
}

T_WORD XmlHandler::getTargetCpuType() const
{
    return m_CpuType;
}
bool XmlHandler::startElement(const QString &  namespaceURI ,
                              const QString & localName ,
                              const QString &qName,
                              const QXmlAttributes &  atts )
{
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());//识别中文
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));//识别中文
    //QTextCodec::setCodecForTr(QTextCodec::codecForLocale());//识别中文
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8")) ;    
    bool ok = false;

#ifdef ICE_DEBUG
    sysLoger(LDEBUG, "XmlHandler:Start to parse the '%s 'part!", qName.toAscii().data());
#endif
    if(qName.compare("config") == 0)
    {
    /*Modified by qingxiaohai on 2007年4月6日 14:53:52 for TS3.2  [Begin]*/
    /*  Modified brief:record the target type, wcs simulator or JTAG ICE*/
        if(atts.value("name").compare("lambdaICE Simulator")==0)
        {
            m_targetType = LAMBDAICE_SIMULATOR_TARGET;
        }
        else
        {
            m_targetType = LAMBDA_JTAG_ICE;
        }
    
    }       
    else if(qName.compare("cpuInfo") == 0)
    {
        m_CpuType = atts.value("cpuType").toInt();
    }
    /*Modified for TS3.2 [End]*/
    else if(qName.compare("coreInfo") == 0)
    {
        char tmpbuf[100] = "\0";
        coreNum = atts.value("coreNum").toInt();
        int2hex(tmpbuf, 10, coreNum);
        coreInfo.append(tmpbuf);
        curCore = atts.value("curcore").toInt();
    }
    else if(qName.compare("coreItem") == 0)
    {
        char tmpbuf[50] = "\0";
        coreInfo.append(ICE_SEPARATE_CHAR).append("(");
        memset(tmpbuf, 0, sizeof(tmpbuf));
        _itoa_s(atts.value("instructionList").toInt(), tmpbuf, sizeof(tmpbuf), HEX_NUM);
        coreInfo.append(tmpbuf);
        coreInfo.append(ICE_SEPARATE_CHAR);
        memset(tmpbuf, 0, sizeof(tmpbuf));
        _itoa_s(atts.value("dataList").toInt(), tmpbuf, sizeof(tmpbuf), HEX_NUM);
        coreInfo.append(tmpbuf);
        coreInfo.append(ICE_SEPARATE_CHAR);
        memset(tmpbuf, 0, sizeof(tmpbuf));
        _itoa_s(atts.value("idcode").toInt(), tmpbuf, sizeof(tmpbuf), HEX_NUM);
        coreInfo.append(tmpbuf);
        coreInfo.append(")");    
    }
    else if(qName.compare("restrictInfo") == 0)
    {
        IO.append(atts.value("io"));
        arch.append(atts.value("dev"));
        app.append(atts.value("app"));
    }
    else if(qName.compare("isBigEndian") == 0)
    {
        QString endianflag = atts.value("value").toLower();
        if(endianflag.compare("true") == 0)
        {
            m_targetEndian = TRUE;
        }
        else
        {
            m_targetEndian = FALSE;
        }
    }
    else if(qName.compare("cpsr") == 0)
    {
        sysLoger(LDEBUG, "XmlHandler::startElement:CPSR value:0x%x", atts.value("value").toUInt(&ok, 16));
        m_cpsr = atts.value("value").toUInt(&ok, HEX_NUM);
    }
    else if(qName.compare("memory") == 0)
    {
        memWidth = atts.value("width").toUInt();        
    }
    else if(qName.compare("subMem") == 0)
    {
        pair<T_WORD, T_WORD> *memInfo = new pair<T_WORD, T_WORD>;
        if(memInfo != NULL)
        {
            memInfo->first = atts.value("addr").toUInt(&ok, HEX_NUM);
            memInfo->second = atts.value("value").toUInt(&ok, HEX_NUM);
            m_memCfg.push_back(memInfo);
            QString delay = atts.value("delay");
            if(!delay.isEmpty())
            {
                m_delayCfg.push_back(delay.toInt());
            }
            else
            {
                m_delayCfg.push_back(0);
            }
            QString swidth = atts.value("width");
            if(!swidth.isEmpty())
            {
                m_memWidth.push_back(swidth.toUInt());
            }
            else
            {
                m_memWidth.push_back(memWidth);
            }
        }
    }
    else if(qName.compare("cp15") == 0)
    {
    
    }
    else if(qName.compare("reg") == 0)
    {
        int cppRegNo = atts.value("num").toInt();
        int cppRegValue = atts.value("value").toUInt(&ok, HEX_NUM);
        m_cpp.insert(cppRegNo, cppRegValue);
    }
    else if(qName.compare("softBreakpoint") == 0)
    {
        m_bpMem = atts.value("sbp").toUInt(&ok, HEX_NUM);
    }
    else if(qName.compare("encache") == 0)
    {
        enableCashInfo.append("encache=").append(atts.value("instrCnt")).append(ICE_SEPARATE_CHAR);
    }
    else if(qName.compare("discache") == 0)
    {
        disableCashInfo.append("discache=").append(atts.value("instrCnt")).append(ICE_SEPARATE_CHAR);
    }
    else if(qName.compare("empcache") == 0)
    {
        empCashInfo.append("empcache=").append(atts.value("instrCnt")).append(ICE_SEPARATE_CHAR);
    }
    else if(qName.compare("encache-Instruction") == 0)
    {
        UI itmp = 0;
        char sTmp[100] = "\0";
        
        memset(sTmp, 0, sizeof(sTmp));
        itmp = atts.value("wmWidth").toUInt(&ok, HEX_NUM);
        sprintf(sTmp, "%x", itmp);
        enableCashInfo.append(sTmp).append(ICE_SEPARATE_CHAR);
        memset(sTmp, 0, sizeof(sTmp));
        itmp = atts.value("addr").toUInt(&ok, HEX_NUM);
        sprintf(sTmp, "%x", itmp);
        enableCashInfo.append(sTmp).append(ICE_SEPARATE_CHAR);
        memset(sTmp, 0, sizeof(sTmp));
        itmp = atts.value("data").toUInt(&ok, HEX_NUM);
        sprintf(sTmp, "%x", itmp);
        enableCashInfo.append(sTmp).append(ICE_SEPARATE_CHAR);
    }
    else if(qName.compare("discache-Instruction") == 0)
    {
        UI itmp = 0;
        char sTmp[100] = "\0";
        
        memset(sTmp, 0, sizeof(sTmp));
        itmp = atts.value("wmWidth").toUInt(&ok, HEX_NUM);
        sprintf(sTmp, "%x", itmp);
        disableCashInfo.append(sTmp).append(ICE_SEPARATE_CHAR);
        memset(sTmp, 0, sizeof(sTmp));
        itmp = atts.value("addr").toUInt(&ok, HEX_NUM);
        sprintf(sTmp, "%x", itmp);
        disableCashInfo.append(sTmp).append(ICE_SEPARATE_CHAR);
        memset(sTmp, 0, sizeof(sTmp));
        itmp = atts.value("data").toUInt(&ok, HEX_NUM);
        sprintf(sTmp, "%x", itmp);
        disableCashInfo.append(sTmp).append(ICE_SEPARATE_CHAR);

    }
    else if(qName.compare("empcache-Instruction") == 0)
    {
        UI itmp = 0;
        char sTmp[100] = "\0";
        
        memset(sTmp, 0, sizeof(sTmp));
        itmp = atts.value("wmWidth").toUInt(&ok, HEX_NUM);
        sprintf(sTmp, "%x", itmp);
        empCashInfo.append(sTmp).append(ICE_SEPARATE_CHAR);
        memset(sTmp, 0, sizeof(sTmp));
        itmp = atts.value("addr").toUInt(&ok, HEX_NUM);
        sprintf(sTmp, "%x", itmp);
        empCashInfo.append(sTmp).append(ICE_SEPARATE_CHAR);
        memset(sTmp, 0, sizeof(sTmp));
        itmp = atts.value("data").toUInt(&ok, HEX_NUM);
        sprintf(sTmp, "%x", itmp);
        empCashInfo.append(sTmp).append(ICE_SEPARATE_CHAR);

    }
    else if(qName.compare("string") == 0) 
    {

    }
   else if(qName.compare("subStr") == 0) 
    {
        strInfo.append(atts.value("value")).append("\n");
    }    
    else if(qName.compare("dccenable") == 0)
    {
        if(atts.value("value").toLower().compare("true") == 0)
        {
            dccEnable = TRUE;
        }
        else
        {
            dccEnable = false;
        }
    }
    else if(qName.compare("dcchandleraddr") == 0)
    {
        dccAddr = atts.value("value").toUInt(&ok, HEX_NUM);
    }
    else if(qName.compare("jtagclock") == 0)
    {
        jtagClock = atts.value("value").toUInt(&ok, HEX_NUM);
    }
    else
    {
        //无效的
    }
    accum.truncate(0);
    return true;
}

bool XmlHandler::endElement(const QString &  namespaceURI ,
                            const QString &  localName ,
                            const QString &qName)
{
#ifdef ICE_DEBUG
    sysLoger(LDEBUG, "XmlHandler:Finish parsing the '%s 'part!", qName.toAscii().data());
#endif
    if(qName.compare("config") == 0)
    {
        constructICEConfigBuffer();
    }
    return true;
}

/**
* 解释xml文件标签,对读取的元素值进行赋值到成员变量accum中
* @param ch 读取标签元素的值
*/
bool XmlHandler::characters(const QString &ch)
{
    accum += ch;
    return true;
}

bool XmlHandler::fatalError(const QXmlParseException &exception)
{
    if(true)// (ferrorCount++ == 0) 
    {
        ferrorCount++;
        QString msg;
        msg.sprintf("Parse error at line %d, column %d (%s).",
            exception.lineNumber(), exception.columnNumber(),
            exception.message().toLatin1().constData());
#ifdef ICE_DEBUG
    sysLoger(LDEBUG, "XmlHandler:Parse config file failure!exception '%s '!", msg.toAscii().data());
    sysLoger(LDEBUG, "XmlHandler:Currrent content: %s", accum.toAscii().data());
#endif
    }
    return false;
}

//根据与ICE通信格式的要求进行配置参数的组装
void XmlHandler::constructICEConfigBuffer()
{
    char tmpbuf[100] = "\0";
    char *pBuf = tmpbuf;
    
    if(m_targetType == LAMBDA_JTAG_ICE)
    {
        /*
            通信格式：
         *    f -io 1,2 -dev 1,2 -com 1 -app 1 -sargs "xxx" *
         *        -io    端口描述表id，可多个*
         *        -dev 硬件抽象设备描述表id,可多个*
         *        -com 通讯id               *     
         *        -app 应用描述表id         *
         *        -sarg 应用需要的参数      *
        */
        iceCfgbuf.clear();
        iceCfgbuf.append("f -io ").append(IO);
        iceCfgbuf.append(" -dev ").append(arch);
        memset(tmpbuf, 0, sizeof(tmpbuf));
        _itoa_s(11, tmpbuf, sizeof(tmpbuf), DEC_NUM);
        iceCfgbuf.append(" -com ").append(tmpbuf);
        iceCfgbuf.append(" -app ").append(app);
        iceCfgbuf.append(" -sargs ").append("\"");
        iceCfgbuf.append("endian=");
        if(m_targetEndian)
        {
            iceCfgbuf.append("big");
        }
        else
        {
            iceCfgbuf.append("little");
        }
        iceCfgbuf.append("\n");
        iceCfgbuf.append("coreinfo=").append(coreInfo).append("\n");
        memset(tmpbuf, 0, sizeof(tmpbuf));
        int2hex(tmpbuf, 10, curCore);
        iceCfgbuf.append("curcore=").append(tmpbuf).append("\n");
        //Jtag clock
        if(jtagClock != 0)
        {
            memset(tmpbuf, 0, sizeof(tmpbuf));
            int2hex(tmpbuf, 20, jtagClock);
            iceCfgbuf.append("jtagclock=").append(tmpbuf).append("\n");        
        }
        if(!enableCashInfo.isEmpty())
        {
            iceCfgbuf.append(enableCashInfo).append("\n");;
        }
        if(!disableCashInfo.isEmpty())
        {
            iceCfgbuf.append(disableCashInfo).append("\n");;
        }
        if(!empCashInfo.isEmpty())
        {
            iceCfgbuf.append(empCashInfo).append("\n");;
        }
        if(!strInfo.isEmpty())
        {
            iceCfgbuf.append(strInfo);
        }
        iceCfgbuf.append("\"");
        //加0
        iceCfgbuf.append("\0");    
    }
    else
    {
        //模拟器初始化的数据
        /*
              f -cputype=? - endian=?
        */
        memset(tmpbuf, 0, sizeof(tmpbuf));
        pBuf = tmpbuf;
        int2hex(pBuf, 10, m_CpuType);
        iceCfgbuf.clear();
        iceCfgbuf.append("f -cputype=").append(pBuf).append(" -endian=");
        if(m_targetEndian)
        {
            iceCfgbuf.append("big");
        }
        else
        {
            iceCfgbuf.append("little");
        }
        iceCfgbuf.append(ICE_SEPARATE_CHAR).append("\0");
    }

    return;
}
