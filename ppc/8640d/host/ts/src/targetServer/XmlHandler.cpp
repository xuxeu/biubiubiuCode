/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  XmlHandler.cpp
* @brief
*       功能：
*       <li> QT XML文件读取类</li>
*/

/************************头文件********************************/

#include "xmlhandler.h"
#include "Manager.h"
#include "QDataStream.h"
#include "QByteArray.h"
#include "config.h"
#include "serverAgent.h"
#include "serverAgentWithDll.h"

#include "common.h"

/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/


/**
* 解释xml文件标签的第一步,作些数据成员的初始化
* @param namespaceURI URI名字,在目前没有用处
* @param localName在目前没有用处
* @param qName 标签内容
*/
bool XmlHandler::startElement(const QString &  namespaceURI ,
                              const QString & localName ,
                              const QString &qName,
                              const QXmlAttributes &  atts )
{
    namespaceURI;
    localName;
    atts;
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());//识别中文
    bool ok = false;

    if (qName.indexOf("channelInfo") == 0)
    {
        strWay = atts.value("way");        //通信类型

        protocolType = atts.value("protocolType");            //协议类型
        strIP = atts.value("IP");                //IP地址

        strCom = atts.value("com");            //串口号

        strBaud = atts.value("baud");            //设置波特率

        strPort = atts.value("port");            //端口
        int port = strPort.toInt();

        strTimeout = atts.value("timeout");        //超时

        strRetry = atts.value("retry");            //重试

        strGuid = atts.value("GUID");                 //GUID

        QString str =QString("%1").arg(protocolType.toInt(&ok, 10) , 0, 16) + CONTROL_FLAG \
                     + QString("%1").arg(strWay.toInt(&ok, 10) , 0, 16) +CONTROL_FLAG;

        //根据通信的方式得到字符串
        switch(strWay.toInt())
        {
        case DeviceConfigBase::TCP_DEVICE:    //TCP
        {
            str += strIP + CONTROL_FLAG + \
                   QString("%1").arg(strPort.toInt(&ok, 10) , 0, 16)+ CONTROL_FLAG;
            vectorChannel.append(str);
        }
        break;
        case DeviceConfigBase::UDP_DEVICE:    //UDP
            str += strIP + CONTROL_FLAG + QString("%1").arg(strPort.toInt(&ok, 10) , 0, 16) + \
                   CONTROL_FLAG + QString("%1").arg(strTimeout.toInt(&ok, 10) , 0, 16) + \
                   CONTROL_FLAG + QString("%1").arg(strRetry.toInt(&ok, 10) , 0, 16) + CONTROL_FLAG;
            vectorChannel.append(str);

            break;
        case DeviceConfigBase::UART_DEVICE:    //串口
            str += strCom + CONTROL_FLAG + QString("%1").arg(strBaud.toInt(&ok, 10) , 0, 16)\
                   + CONTROL_FLAG + QString("%1").arg(strTimeout.toInt(&ok, 10) , 0, 16) + \
                   CONTROL_FLAG + QString("%1").arg(strRetry.toInt(&ok, 10) , 0, 16) + CONTROL_FLAG;
            vectorChannel.append(str);
            break;

        default:
            break;
        }



    }

    else if (qName == QString("serverAgent"))
    {
        //sa配置信息
        strWay.truncate(0);
        name.truncate(0);
        logFlag.truncate(0);
        state.truncate(0);
        protocolType.truncate(0);
        reportType = "";

        QString str;
        QString strName1("name");
        name = atts.value(strName1);        //名字

        reportType = atts.value("reportType");        //上报类型

        state = atts.value("state");    //类型

        logFlag = atts.value("logFlag");        //日志标记
        channelType = atts.value("channelType");        //通道类型
        extInfo = atts.value("extInfo");
    }

    else if (qName == QString("serverAgentWithDll"))
    {
        //sa配置信息
        strWay.truncate(0);
        name.truncate(0);
        logFlag.truncate(0);
        state.truncate(0);
        protocolType.truncate(0);

        name = atts.value("name");                //动态库名称
        dllName = atts.value("dllName");                //动态库名称

        QString str ;
        reportType =  atts.value("reportType");        //上报类型


        dllBuf = atts.value("dllBuf");                //动态库名称
        state = atts.value("state");            //类型
        logFlag = atts.value("logFlag");        //日志标记
        extInfo = atts.value("extInfo");

    }

    else if (qName == QString("ListenConfig"))
    {
        //sa配置信息
        QString str = atts.value("port");                //得到监听端口号
        Manager::getInstance()->setListenPort(str.toInt());
    }

    else if(qName == QString("SystemLogConfig") )
    {
        //系统日志的输出级别
        QString str = atts.value("currentLevel");
        //如果日志无效则默认输出WARNING以上的
        LogLevel lglevel = ((LDEBUG <= str.toInt()) &&(str.toInt() <= LFATAL))?(LogLevel)(str.toInt()):LWARNING;
        SystemLog::getInstance()->setLogLevel(lglevel);
    }

    /*Modified by tangxp on 2007.11.12 [begin]*/
    /*  brief: 增加加载常驻模块*/
    else if(qName == QString("psa"))
    {
        //常驻模块配置信息
        name.truncate(0);
        dllName.truncate(0);
        logFlag.truncate(0);
        extInfo.truncate(0);
        state.truncate(0);
        dllBuf.truncate(0);
        reportType.truncate(0);

        name    = atts.value("name");           //动态库名称
        dllName = atts.value("dllName");        //动态库文件名称
        logFlag = atts.value("logFlag");        //日志标记
        dllBuf  = atts.value("extInfo");        //动态库初始化参数

        //设置常驻模块为运行状态
        state.operator = ("1");

        //设置为常驻模块类型
        reportType.setNum( PSA_REG_TYPE );
    }

    /*Modified by tangxp on 2007.11.12 [end]*/
    else
    {
        //无效的
    }

    accum.truncate(0);
    return true;
}

/**
* 解释xml文件标签的最后一步,对读取的元素值进行复制
* @param namespaceURI URI名字,在目前没有用处
* @param localName在目前没有用处
* @param qName 标签内容
*/
bool XmlHandler::endElement(const QString &  namespaceURI ,
                            const QString &  localName ,
                            const QString &qName)
{
    namespaceURI;
    localName;

    if (qName == QString("serverAgent"))
    {
        //读取到SA标签
        createSA();
    }

    else if (qName == QString("serverAgentWithDll"))
    {
        //读取到SA标签
        createSAWithDll();
    }

    else if (qName.indexOf("channelInfo") == 0)
    {
        //配置信息,不做任何处理
    }

    /*Modified by tangxp on 2007.11.12 [begin]*/
    /*  brief: 增加加载常驻模块*/
    else if( qName == QString("psa") )
    {
        //加载常驻模块
        createSAWithDll();
    }

    /*Modified by tangxp on 2007.11.12 [end]*/
    return true;
}
/**
* 创建带有动态库sa
*/
void XmlHandler::createSAWithDll()
{
    ServerAgentWithDll* pSA = new ServerAgentWithDll(Manager::getInstance()->createID());    //创建sa对象

    if(NULL == pSA)
    {
        sysLoger(LWARNING, "XmlHandler::createSAWithDll:Created SA object of Dll failure!");
        return;
    }

    pSA->setName(name);
    pSA->setDllName(dllName);
    pSA->clearChannel();
    pSA->addChannel(QString("dll"));
    pSA->setExtInfo(extInfo);

    if (logFlag.toInt() > 0)
    {
        pSA->setLogFlag(true);
    }

    else
    {
        pSA->setLogFlag(false);
    }

    pSA->setState(state.toInt());
    pSA->setInBuf(dllBuf);

    if (pSA->getState() != SASTATE_INACTIVE)
    {
        if (pSA->run())    //运行sa程序,运行成功就记录名字和AID关联
        {
            pSA->setState(SASTATE_ACTIVE);
        }

        else
        {
            pSA->setState(SASTATE_INACTIVE);
        }
    }

    Manager::getInstance()->addServerAgent(pSA);
    //把sa关联的类型号
    int next = 0;
    QString str;

    while(true)
    {
        str = reportType.section(CONTROL_FLAG,next,next);

        if (str.size() <= 0)
        {
            break;
        }

        Manager::getInstance()->setRegisterType(pSA->getAID(), str.toInt(), false);
        next++;
    }

    sysLoger(LDEBUG, "XmlHandler::createSAWithDll: Creatad SA of dll, name: %s, Dll:%s",
             pSA->getName().toAscii().data(), pSA->getDllName().toAscii().data());
    return;
}
/**
* 创建sa
*/
void XmlHandler::createSA()
{
    bool ok = false;
    ServerAgent* pSA = NULL;

    try
    {
        pSA = new ServerAgent(Manager::getInstance()->createID());    //创建sa对象
    }

    catch(...)
    {
        sysLoger(LERROR, "XmlHandler::createSA:Alloc SA object  failure!");
        return;
    }

    if(NULL == pSA)
    {
        sysLoger(LWARNING, "XmlHandler::createSA:Created SA object  failure!");
        return;
    }

    pSA->setName(name);
    pSA->setState(state.toInt(&ok, 10));
    pSA->setRegisterType(reportType.toInt(&ok, 10));
    pSA->setExtInfo(extInfo);
    /*Modified by tangxp for BUG NO.3452 on 2008.6.23[ [begin]*/
    /*  Modified brief: 设置通道类型*/
    pSA->setChannelType(channelType.toInt());
    /*Modified by tangxp for BUG NO.3452 on 2008.6.23 [end]*/
    //pSA->setConfig(new ServerConfig());    //设置配置设备配置指针

    if (logFlag.toInt() > 0)
    {
        pSA->setLogFlag(true);
    }

    else
    {
        pSA->setLogFlag(false);
    }

    //得到通信配置信息
    for (int i=0; i<vectorChannel.size(); ++i)
    {
        if (i == channelType.toInt())
        {
            QVector<QString> vec;
            //中文转换,转换成UTF8
            QString str;
            int next = 0;    //查询的开始位置

            while(true)
            {
                str = vectorChannel[i].section(CONTROL_FLAG,next,next);

                if (str.size() == 0)
                {
                    break;
                }

                vec.append(str);
                next++;
            }

            int iCount = 0, protocol = 0;
            protocol = vec[iCount++].toInt();//协议类型
            strWay = vec[iCount];
            int deviceWay = vec[iCount++].toInt();

            pConfigBase = DeviceConfigBase::newDeviceConfig(deviceWay);    //得到配置信息

            sysLoger(LDEBUG, "XmlHandler::createSA: Channel:%d  config string: %s", i, vectorChannel[i]);

            switch(deviceWay)
            {
            case DeviceConfigBase::TCP_DEVICE:    //TCP
            {
                TcpConfig *pTcpCfg = dynamic_cast<TcpConfig*>(pConfigBase);

                if(NULL == pTcpCfg)
                {
                    sysLoger(LWARNING, "XmlHandler::createSA: Created TCP device config failure!");
                    break;
                }

                pTcpCfg->setIP(vec[iCount++]);                     //IP地址
                pTcpCfg->setPort(vec[iCount++].toInt(&ok, 16));                        //端口
                sysLoger(LDEBUG, "XmlHandler::createSA: Created UDP device config for current channel"\
                         " %d success. IP:%s, Port:%d",
                         i, pTcpCfg->getIP().toAscii().data(), pTcpCfg->getPort());
            }
            break;
            case DeviceConfigBase::UDP_DEVICE:    //UDP
            {
                UdpConfig *pUdpCfg = dynamic_cast<UdpConfig*>(pConfigBase);

                if(NULL == pUdpCfg)
                {
                    sysLoger(LWARNING, "XmlHandler::createSA: Created UDP device config failure!");
                    break;
                }

                pUdpCfg->setIP(vec[iCount++]);                     //IP地址
                pUdpCfg->setPort(vec[iCount++].toInt(&ok, 16));                    //端口
                pUdpCfg->setTimeout(vec[iCount++].toInt(&ok, 16));                    //超时
                pUdpCfg->setRetry(vec[iCount++].toInt(&ok, 16));                    //重试次数
                sysLoger(LDEBUG, "XmlHandler::createSA: Created UDP device config for current channel"\
                         " %d success. IP:%s, Port:%d, TimeOut:%d, Retry:%d",
                         i, pUdpCfg->getIP().toAscii().data(), pUdpCfg->getPort(),
                         pUdpCfg->getTimeout(), pUdpCfg->getRetry());
            }
            break;
            case DeviceConfigBase::UART_DEVICE:    //串口
            {
                UartConfig *pUartCfg = dynamic_cast<UartConfig*>(pConfigBase);

                if(NULL == pUartCfg)
                {
                    sysLoger(LWARNING, "XmlHandler::createSA: Created UART device config failure!");
                    break;
                }

                pUartCfg->setCom(vec[iCount++]);                     //端口号
                pUartCfg->setBaud(vec[iCount++].toInt(&ok, 16));                        //波特率
                pUartCfg->setTimeout(vec[iCount++].toInt(&ok, 16));                    //超时
                pUartCfg->setRetry(vec[iCount++].toInt(&ok, 16));                    //重试次数
                sysLoger(LDEBUG, "XmlHandler::createSA: Created UART device config for current channel"\
                         " %d success. COM:%s, Baud:%d, TimeOut:%d, Retry:%d",
                         i, pUartCfg->getCom().toAscii().data(), pUartCfg->getBaud(),
                         pUartCfg->getTimeout(), pUartCfg->getRetry());
            }
            break;

            default:
                sysLoger(LWARNING, "XmlHandler::createSA:Unknow device type %d of current channel %d"\
                         , deviceWay, i);
                break;
            }

            if (pConfigBase !=    NULL)
            {
                //分配服务配置对象
                ServerConfig *pSrvCfg = new ServerConfig(pConfigBase, protocol, pSA->getLogFlag());

                if(NULL == pSrvCfg)
                {
                    sysLoger(LWARNING, "XmlHandler::createSA: Created server config for SA :%s failure!",
                             pSA->getName().toAscii().data());
                }

                else
                {
                    //pSrvCfg->setDeviceConfig(pConfigBase);
                    pSA->setConfig(pSrvCfg);
                }
            }
        }

        pSA->addChannel(vectorChannel[i]);
    }

    vectorChannel.clear();

    if (pSA->getState() != SASTATE_INACTIVE)
    {
        if (pSA->run())    //运行sa程序,运行成功就记录名字和AID关联
        {
            pSA->setState(SASTATE_ACTIVE);
        }

        else
        {
            pSA->setState(SASTATE_INACTIVE);
        }
    }

    Manager::getInstance()->addServerAgent(pSA);
    //把sa关联的类型号
    int next = 0;
    QString str;

    while(true)
    {
        str = reportType.section(CONTROL_FLAG,next,next);

        if (str.size() <= 0)
        {
            break;
        }

        Manager::getInstance()->setRegisterType(pSA->getAID(), str.toInt(), false);
        next++;
    }
}

/**
* 设置超时时间
*/
void XmlHandler::setTimeout(int timeout)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //得到通信类型

    switch(deviceWay)
    {
        //得到通信方式
    case DeviceConfigBase::UART_DEVICE:    //串口
    {
        UartConfig* pUartConfig = dynamic_cast<UartConfig*>(pConfigBase);    //强制转换类型

        if(NULL == pUartConfig)
        {
            sysLoger(LWARNING, "XmlHandler::setTimeout: pUartConfig is bad pointer!");
            return;
        }

        pUartConfig->setTimeout(timeout);
    }
    break;
    case DeviceConfigBase::UDP_DEVICE:
    {
        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>(pConfigBase);    //强制转换类型

        if(NULL == pUdpConfig)
        {
            sysLoger(LWARNING, "XmlHandler::setTimeout: pUdpConfig is bad pointer!");
            return;
        }

        pUdpConfig->setTimeout(timeout);
    }
    break;
    default:
        break;
    }

    return;
}

/**
* 设置波特率
*/
void XmlHandler::setBaud(int baud)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //得到通信类型

    switch(deviceWay)
    {
        //得到通信方式
    case DeviceConfigBase::UART_DEVICE:    //串口
    {
        UartConfig* pUartConfig = dynamic_cast<UartConfig*>(pConfigBase);    //强制转换类型
        pUartConfig->setBaud(baud);
    }
    break;
    default:
        break;
    }

}


/**
* 设置串口
*/
void XmlHandler::setCom(QString com)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //得到通信类型

    switch(deviceWay)
    {
        //得到通信方式
    case DeviceConfigBase::UART_DEVICE:    //串口
    {
        UartConfig* pUartConfig = dynamic_cast<UartConfig*>(pConfigBase);    //强制转换类型
        pUartConfig->setCom(com);
    }
    break;
    default:
        break;
    }

}

/**
* 设置重试次数
*/
void XmlHandler::setRetry(int retry)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //得到通信类型

    switch(deviceWay)
    {
        //得到通信方式
    case DeviceConfigBase::UART_DEVICE:    //串口
    {
        UartConfig* pUartConfig = dynamic_cast<UartConfig*>(pConfigBase);    //强制转换类型
        pUartConfig->setRetry(retry);
    }
    break;
    case DeviceConfigBase::UDP_DEVICE:    //UDP通信方式
    {
        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>(pConfigBase);    //强制转换类型
        pUdpConfig->setRetry(retry);
    }
    break;
    default:
        break;
    }

}


/**
* 设置IP地址
*/
void XmlHandler::setPort(int port)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //得到通信类型

    switch(deviceWay)
    {
        //得到通信方式
    case DeviceConfigBase::TCP_DEVICE:
    {
        TcpConfig* pTcpConfig = dynamic_cast<TcpConfig*>(pConfigBase);    //强制转换类型
        pTcpConfig->setPort(port);
    }
    break;
    case DeviceConfigBase::UDP_DEVICE:
    {
        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>(pConfigBase);    //强制转换类型
        pUdpConfig->setPort(port);
    }
    break;
    default:
        break;
    }

}
/**
* 设置IP地址
*/
void XmlHandler::setIP(const QString &IP)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //得到通信类型

    switch(deviceWay)
    {
        //得到通信方式
    case DeviceConfigBase::TCP_DEVICE:
    {
        TcpConfig* pTcpConfig = dynamic_cast<TcpConfig*>(pConfigBase);    //强制转换类型
        pTcpConfig->setIP(IP);
    }
    break;
    case DeviceConfigBase::UDP_DEVICE:
    {
        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>(pConfigBase);    //强制转换类型
        pUdpConfig->setIP(IP);
    }
    break;
    default:
        break;
    }
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
    if (ferrorCount++ == 0)
    {
        QString msg;
        msg.sprintf("Parse error at line %d, column %d (%s).",
                    exception.lineNumber(), exception.columnNumber(),
                    exception.message().toLatin1().constData());
    }

    return false;
}
