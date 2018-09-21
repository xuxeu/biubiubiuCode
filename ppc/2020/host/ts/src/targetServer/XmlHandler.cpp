/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  XmlHandler.cpp
* @brief
*       ���ܣ�
*       <li> QT XML�ļ���ȡ��</li>
*/

/************************ͷ�ļ�********************************/

#include "xmlhandler.h"
#include "Manager.h"
#include "QDataStream.h"
#include "QByteArray.h"
#include "config.h"
#include "serverAgent.h"
#include "serverAgentWithDll.h"

#include "common.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/


/**
* ����xml�ļ���ǩ�ĵ�һ��,��Щ���ݳ�Ա�ĳ�ʼ��
* @param namespaceURI URI����,��Ŀǰû���ô�
* @param localName��Ŀǰû���ô�
* @param qName ��ǩ����
*/
bool XmlHandler::startElement(const QString &  namespaceURI ,
                              const QString & localName ,
                              const QString &qName,
                              const QXmlAttributes &  atts )
{
    namespaceURI;
    localName;
    atts;
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());//ʶ������
    bool ok = false;

    if (qName.indexOf("channelInfo") == 0)
    {
        strWay = atts.value("way");        //ͨ������

        protocolType = atts.value("protocolType");            //Э������
        strIP = atts.value("IP");                //IP��ַ

        strCom = atts.value("com");            //���ں�

        strBaud = atts.value("baud");            //���ò�����

        strPort = atts.value("port");            //�˿�
        int port = strPort.toInt();

        strTimeout = atts.value("timeout");        //��ʱ

        strRetry = atts.value("retry");            //����

        strGuid = atts.value("GUID");                 //GUID

        QString str =QString("%1").arg(protocolType.toInt(&ok, 10) , 0, 16) + CONTROL_FLAG \
                     + QString("%1").arg(strWay.toInt(&ok, 10) , 0, 16) +CONTROL_FLAG;

        //����ͨ�ŵķ�ʽ�õ��ַ���
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
        case DeviceConfigBase::UART_DEVICE:    //����
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
        //sa������Ϣ
        strWay.truncate(0);
        name.truncate(0);
        logFlag.truncate(0);
        state.truncate(0);
        protocolType.truncate(0);
        reportType = "";

        QString str;
        QString strName1("name");
        name = atts.value(strName1);        //����

        reportType = atts.value("reportType");        //�ϱ�����

        state = atts.value("state");    //����

        logFlag = atts.value("logFlag");        //��־���
        channelType = atts.value("channelType");        //ͨ������
        extInfo = atts.value("extInfo");
    }

    else if (qName == QString("serverAgentWithDll"))
    {
        //sa������Ϣ
        strWay.truncate(0);
        name.truncate(0);
        logFlag.truncate(0);
        state.truncate(0);
        protocolType.truncate(0);

        name = atts.value("name");                //��̬������
        dllName = atts.value("dllName");                //��̬������

        QString str ;
        reportType =  atts.value("reportType");        //�ϱ�����


        dllBuf = atts.value("dllBuf");                //��̬������
        state = atts.value("state");            //����
        logFlag = atts.value("logFlag");        //��־���
        extInfo = atts.value("extInfo");

    }

    else if (qName == QString("ListenConfig"))
    {
        //sa������Ϣ
        QString str = atts.value("port");                //�õ������˿ں�
        Manager::getInstance()->setListenPort(str.toInt());
    }

    else if(qName == QString("SystemLogConfig") )
    {
        //ϵͳ��־���������
        QString str = atts.value("currentLevel");
        //�����־��Ч��Ĭ�����WARNING���ϵ�
        LogLevel lglevel = ((LDEBUG <= str.toInt()) &&(str.toInt() <= LFATAL))?(LogLevel)(str.toInt()):LWARNING;
        SystemLog::getInstance()->setLogLevel(lglevel);
    }

    /*Modified by tangxp on 2007.11.12 [begin]*/
    /*  brief: ���Ӽ��س�פģ��*/
    else if(qName == QString("psa"))
    {
        //��פģ��������Ϣ
        name.truncate(0);
        dllName.truncate(0);
        logFlag.truncate(0);
        extInfo.truncate(0);
        state.truncate(0);
        dllBuf.truncate(0);
        reportType.truncate(0);

        name    = atts.value("name");           //��̬������
        dllName = atts.value("dllName");        //��̬���ļ�����
        logFlag = atts.value("logFlag");        //��־���
        dllBuf  = atts.value("extInfo");        //��̬���ʼ������

        //���ó�פģ��Ϊ����״̬
        state.operator = ("1");

        //����Ϊ��פģ������
        reportType.setNum( PSA_REG_TYPE );
    }

    /*Modified by tangxp on 2007.11.12 [end]*/
    else
    {
        //��Ч��
    }

    accum.truncate(0);
    return true;
}

/**
* ����xml�ļ���ǩ�����һ��,�Զ�ȡ��Ԫ��ֵ���и���
* @param namespaceURI URI����,��Ŀǰû���ô�
* @param localName��Ŀǰû���ô�
* @param qName ��ǩ����
*/
bool XmlHandler::endElement(const QString &  namespaceURI ,
                            const QString &  localName ,
                            const QString &qName)
{
    namespaceURI;
    localName;

    if (qName == QString("serverAgent"))
    {
        //��ȡ��SA��ǩ
        createSA();
    }

    else if (qName == QString("serverAgentWithDll"))
    {
        //��ȡ��SA��ǩ
        createSAWithDll();
    }

    else if (qName.indexOf("channelInfo") == 0)
    {
        //������Ϣ,�����κδ���
    }

    /*Modified by tangxp on 2007.11.12 [begin]*/
    /*  brief: ���Ӽ��س�פģ��*/
    else if( qName == QString("psa") )
    {
        //���س�פģ��
        createSAWithDll();
    }

    /*Modified by tangxp on 2007.11.12 [end]*/
    return true;
}
/**
* �������ж�̬��sa
*/
void XmlHandler::createSAWithDll()
{
    ServerAgentWithDll* pSA = new ServerAgentWithDll(Manager::getInstance()->createID());    //����sa����

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
        if (pSA->run())    //����sa����,���гɹ��ͼ�¼���ֺ�AID����
        {
            pSA->setState(SASTATE_ACTIVE);
        }

        else
        {
            pSA->setState(SASTATE_INACTIVE);
        }
    }

    Manager::getInstance()->addServerAgent(pSA);
    //��sa���������ͺ�
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
* ����sa
*/
void XmlHandler::createSA()
{
    bool ok = false;
    ServerAgent* pSA = NULL;

    try
    {
        pSA = new ServerAgent(Manager::getInstance()->createID());    //����sa����
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
    /*  Modified brief: ����ͨ������*/
    pSA->setChannelType(channelType.toInt());
    /*Modified by tangxp for BUG NO.3452 on 2008.6.23 [end]*/
    //pSA->setConfig(new ServerConfig());    //���������豸����ָ��

    if (logFlag.toInt() > 0)
    {
        pSA->setLogFlag(true);
    }

    else
    {
        pSA->setLogFlag(false);
    }

    //�õ�ͨ��������Ϣ
    for (int i=0; i<vectorChannel.size(); ++i)
    {
        if (i == channelType.toInt())
        {
            QVector<QString> vec;
            //����ת��,ת����UTF8
            QString str;
            int next = 0;    //��ѯ�Ŀ�ʼλ��

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
            protocol = vec[iCount++].toInt();//Э������
            strWay = vec[iCount];
            int deviceWay = vec[iCount++].toInt();

            pConfigBase = DeviceConfigBase::newDeviceConfig(deviceWay);    //�õ�������Ϣ

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

                pTcpCfg->setIP(vec[iCount++]);                     //IP��ַ
                pTcpCfg->setPort(vec[iCount++].toInt(&ok, 16));                        //�˿�
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

                pUdpCfg->setIP(vec[iCount++]);                     //IP��ַ
                pUdpCfg->setPort(vec[iCount++].toInt(&ok, 16));                    //�˿�
                pUdpCfg->setTimeout(vec[iCount++].toInt(&ok, 16));                    //��ʱ
                pUdpCfg->setRetry(vec[iCount++].toInt(&ok, 16));                    //���Դ���
                sysLoger(LDEBUG, "XmlHandler::createSA: Created UDP device config for current channel"\
                         " %d success. IP:%s, Port:%d, TimeOut:%d, Retry:%d",
                         i, pUdpCfg->getIP().toAscii().data(), pUdpCfg->getPort(),
                         pUdpCfg->getTimeout(), pUdpCfg->getRetry());
            }
            break;
            case DeviceConfigBase::UART_DEVICE:    //����
            {
                UartConfig *pUartCfg = dynamic_cast<UartConfig*>(pConfigBase);

                if(NULL == pUartCfg)
                {
                    sysLoger(LWARNING, "XmlHandler::createSA: Created UART device config failure!");
                    break;
                }

                pUartCfg->setCom(vec[iCount++]);                     //�˿ں�
                pUartCfg->setBaud(vec[iCount++].toInt(&ok, 16));                        //������
                pUartCfg->setTimeout(vec[iCount++].toInt(&ok, 16));                    //��ʱ
                pUartCfg->setRetry(vec[iCount++].toInt(&ok, 16));                    //���Դ���
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
                //����������ö���
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
        if (pSA->run())    //����sa����,���гɹ��ͼ�¼���ֺ�AID����
        {
            pSA->setState(SASTATE_ACTIVE);
        }

        else
        {
            pSA->setState(SASTATE_INACTIVE);
        }
    }

    Manager::getInstance()->addServerAgent(pSA);
    //��sa���������ͺ�
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
* ���ó�ʱʱ��
*/
void XmlHandler::setTimeout(int timeout)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //�õ�ͨ������

    switch(deviceWay)
    {
        //�õ�ͨ�ŷ�ʽ
    case DeviceConfigBase::UART_DEVICE:    //����
    {
        UartConfig* pUartConfig = dynamic_cast<UartConfig*>(pConfigBase);    //ǿ��ת������

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
        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>(pConfigBase);    //ǿ��ת������

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
* ���ò�����
*/
void XmlHandler::setBaud(int baud)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //�õ�ͨ������

    switch(deviceWay)
    {
        //�õ�ͨ�ŷ�ʽ
    case DeviceConfigBase::UART_DEVICE:    //����
    {
        UartConfig* pUartConfig = dynamic_cast<UartConfig*>(pConfigBase);    //ǿ��ת������
        pUartConfig->setBaud(baud);
    }
    break;
    default:
        break;
    }

}


/**
* ���ô���
*/
void XmlHandler::setCom(QString com)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //�õ�ͨ������

    switch(deviceWay)
    {
        //�õ�ͨ�ŷ�ʽ
    case DeviceConfigBase::UART_DEVICE:    //����
    {
        UartConfig* pUartConfig = dynamic_cast<UartConfig*>(pConfigBase);    //ǿ��ת������
        pUartConfig->setCom(com);
    }
    break;
    default:
        break;
    }

}

/**
* �������Դ���
*/
void XmlHandler::setRetry(int retry)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //�õ�ͨ������

    switch(deviceWay)
    {
        //�õ�ͨ�ŷ�ʽ
    case DeviceConfigBase::UART_DEVICE:    //����
    {
        UartConfig* pUartConfig = dynamic_cast<UartConfig*>(pConfigBase);    //ǿ��ת������
        pUartConfig->setRetry(retry);
    }
    break;
    case DeviceConfigBase::UDP_DEVICE:    //UDPͨ�ŷ�ʽ
    {
        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>(pConfigBase);    //ǿ��ת������
        pUdpConfig->setRetry(retry);
    }
    break;
    default:
        break;
    }

}


/**
* ����IP��ַ
*/
void XmlHandler::setPort(int port)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //�õ�ͨ������

    switch(deviceWay)
    {
        //�õ�ͨ�ŷ�ʽ
    case DeviceConfigBase::TCP_DEVICE:
    {
        TcpConfig* pTcpConfig = dynamic_cast<TcpConfig*>(pConfigBase);    //ǿ��ת������
        pTcpConfig->setPort(port);
    }
    break;
    case DeviceConfigBase::UDP_DEVICE:
    {
        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>(pConfigBase);    //ǿ��ת������
        pUdpConfig->setPort(port);
    }
    break;
    default:
        break;
    }

}
/**
* ����IP��ַ
*/
void XmlHandler::setIP(const QString &IP)
{
    if(pConfigBase == NULL)
    {
        return;
    }

    int deviceWay = strWay.toInt();    //�õ�ͨ������

    switch(deviceWay)
    {
        //�õ�ͨ�ŷ�ʽ
    case DeviceConfigBase::TCP_DEVICE:
    {
        TcpConfig* pTcpConfig = dynamic_cast<TcpConfig*>(pConfigBase);    //ǿ��ת������
        pTcpConfig->setIP(IP);
    }
    break;
    case DeviceConfigBase::UDP_DEVICE:
    {
        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>(pConfigBase);    //ǿ��ת������
        pUdpConfig->setIP(IP);
    }
    break;
    default:
        break;
    }
}

/**
* ����xml�ļ���ǩ,�Զ�ȡ��Ԫ��ֵ���и�ֵ����Ա����accum��
* @param ch ��ȡ��ǩԪ�ص�ֵ
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
