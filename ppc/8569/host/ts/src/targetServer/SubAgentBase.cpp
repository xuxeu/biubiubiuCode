/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  SubAgentBase.cpp
* @brief
*       ���ܣ�
*       <li> manager�Ķ������������Ϳ���������</li>
*/


/************************ͷ�ļ�********************************/
#include "subagentbase.h"
#include "tsInclude.h"
#include "manager.h"
#include "serverAgent.h"
#include "config.h"
#include "router.h"
#include "device.h"
#include "protocol.h"
#include "ServerAgentWithDll.h"
#include "qfile.h"
#include <string>
#include <windows.h>

using namespace std;

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/
/************************ʵ��*********************************/

SubAgentBase::~SubAgentBase(void)
{
}


/////////////////////////////////////////////
ControlSubAgent::ControlSubAgent(short said) : SubAgentBase(said)
{
    init();
}
/**
* ���ܣ���ʼ����ǰ�Ķ�������
* @param SAID ���������
*/
void ControlSubAgent::init()
{
    Manager::getInstance()->addSubAgent(this);    //���Լ�ע�ᵽӳ�����
    packLoger.open("manager.log");
}

/**
* ���ܣ�������ǰ�����ݰ�,��ִ�в���
* @param pack ���ݱ�
*/
void ControlSubAgent::process(Packet* pack)
{
    try
    {
        if(NULL == pack)
        {
            sysLoger(LWARNING, "ControlSubAgent::process:pack is NULL pionter!FILE:%s, LINE%d",
                     __MYFILE__, __LINE__);
            return;
        }

        //���յ������ݽ�����־����
        if(Manager::getInstance()->getLogFlag())
        {
            packLoger.write(RECV_PACK, *pack);
        }

        //�洢DID
        US dialogID = 0;
        char cmd = 0;
        __fetch(&dialogID, pack->getData(), DIDLEN);
        Manager::getInstance()->setCurDID(pack->getSrc_aid(), dialogID);

        //���а��ļ��
        if(pack->getSize() < (int)(DIDLEN + (2*CONTROL_FLAG_LEN) + CONTROL_COMMAND_LEN))
        {
            sysLoger(LWARNING, "ControlSubAgent::proccess: Received unknow format packet!");
            
            //�ͷŷ���İ�
            PacketManager::getInstance()->free(pack);
            
            return;
        }

        sysLoger(LDEBUG, "ControlSubAgent::proccess: Starting get command char!");

        //ȡ��������
        cmd = pack->getData()[DIDLEN+CONTROL_FLAG_LEN];

        sysLoger(LDEBUG, "ControlSubAgent::proccess: Received command %c", cmd);

        //��������ַ�����
        switch(cmd)
        {
        case GETID:
            handleGetID(pack);
            break;

        case CREATE_SERVER:
            handleCreateServer(pack);
            break;

        case CREATE_SERVERWITHDLL:
            handleCreateServerWithDll(pack);
            break;

        case CLOSE_TS:
            setSuccessPacket(pack);

            if(Manager::getInstance()->getLogFlag())
            {
                packLoger.write(SEND_PACK, *pack);
            }

            Router::getInstance()->putpkt(pack);
            //�ȴ�2����ͣTS,��֤���ݰ������ͻ���
            {
                /*Modified by tangxp for BUG NO.0002816 on 2008.3.3 [begin]*/
                /*  Modified brief: �޸���ʱʹ�ú���*/
                Sleep(2000);
                /*Modified by tangxp for BUG NO.0002816 on 2008.3.3 [end]*/
            }
            Manager::getInstance()->stop();
            break;

        case DELETE_SERVER:
            handleDeleteServer(pack);
            break;

        case CONNECT_FLAG:
            handleTClientOpen(pack);
            break;

        case QUERY_SERVER_STATE:
            handleQueryServer(pack);
            break;

        case SEARCH_SERVER_COUNT:
            handleSearchServer(pack);
            break;

        case ACTIVE_SERVER:
            handleActiveServer(pack);
            break;

        case REST_SERVER:
            handleInactiveServer(pack);
            break;

        case MODIFY_SERVER:
            handleModifyServer(pack);
            break;

        case REGISTER_SERVER:
            handleRegisterServer(pack);
            break;

        case UNREGISTER_SERVER:
            handleUnregisterServer(pack);
            break;

        case SET_SERVER_TYPE:
            handleSetRegisterType(pack);
            break;

        case CLEAR_SERVER_TYPE:
            handleClearRegisterType(pack);
            break;

        case QUERY_HOST_COMM_CONFIG:
            handleQueryHostSerialPort(pack);
            break;


        case SET_TS_SYSTEM_LOGOUT_LEVEL:
            handleSetSystemLogLevel(pack);
            break;

        case SET_SERVICE_PACK_LOG_FLAG:
            handleSetAgentLogFlag(pack);
            break;

        case CLOSE_LINK:
            handleCloseClient(pack);
            break;

        case UPLOAD_FILE:
            handleUpLoadFile(pack);
            break;

        case DOWNLOAD_FILE:
            handleDownLoadFile(pack);
            break;

        case DELETE_FILE:
            handleDeleteFile(pack);
            break;

        default:
            sysLoger(LWARNING, "ControlSubAgent::process: Recieved unknow control command pack!"\
                     " the control commad is %c, FILE:%s, LINE:%d",
                     cmd, __MYFILE__, __LINE__);
            changePacketID(pack);
            pack->clearData();
            int size = pack->setErrorPacket(ERR_UNKNOW_DATA, FAILURE_ACK_LEN, dialogID);
            pack->setSize(size);

            //������־
            if(Manager::getInstance()->getLogFlag())
            {
                packLoger.write(SEND_PACK, *pack);
            }

            Router::getInstance()->putpkt(pack);

            return;
            break;
        }

        return;
    }

    catch(int err)
    {
        sysLoger(LWARNING, "ControlSubAgent::process:Catch an error :%d.", err);
    }
}
/**
* ���ܣ�����TClient���͹�����createServer��������, ���ѵõ���AID���ظ�TClient
* @param block ȥ��createserver����
* @param pack ���յĿ��ư�
��ʽ��:����, ��־, ͨ����, ��̬������, ��ʽ, ��̬���ַ���
*/
void ControlSubAgent::handleCreateServerWithDll(Packet* packet)
{
    try
    {
        if(NULL == packet)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServerWithDll: Received NULL point "\
                     "parameter.  FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            return;
        }

        //�����������Ѿ��ﵽ�������
        if(Manager::getInstance()->isAgentCountReachedToMAX())
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServerWithDll: Agent objects in TS have"\
                     "reatched max count!");
            sendErrorAck(packet, ERR_SRV_NUM_EXPIRE, FAILURE_ACK_LEN);
            return;
        }

        sysLoger(LDEBUG, "ControlSubAgent::handleCreateServerWithDll: Enter this function");
        /*
            ���п��ư��Ľ�������ʽΪ
            --------------------------------
            "DID, TserviceName, extinfo, dllName, isLog, DllBuf,"
           --------------------------------
        */
        char SAName[NAME_SIZE] = "\0";
        char DllFileName[NAME_SIZE] = "\0";
        char DllBuf[DLL_BUF] = "\0";
        char extInfo[EXT_INFO_LEN] = "\0";
        char curPath[MAX_PATH] = "\0";    //TS��ǰ·��


        UI    uiDllBufLen = 0;
        int  logFlag = 0;

        char *pDataStartAddr = packet->getData()+DIDLEN+CONTROL_FLAG_LEN+CONTROL_COMMAND_LEN;
        char *pDest = NULL;
        /******************���ݰ��Ľ���************************/
        //�����������
        pDest = strstr(pDataStartAddr, CONTROL_FLAG);

        if(NULL == pDest)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServerWithDll: Received unknow format"\
                     "packet. FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            sendErrorAck(packet, ERR_UNKNOW_DATA, FAILURE_ACK_LEN);
            return;
        }

        memcpy(SAName, pDataStartAddr, min(pDest - pDataStartAddr, NAME_SIZE-1));
        //extInfo
        pDataStartAddr = pDest + CONTROL_FLAG_LEN;

        if(packet->getSize() < (int)(pDataStartAddr - packet->getData() + EXT_INFO_LEN + CONTROL_FLAG_LEN))
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServerWithDll: Received unknow format"\
                     "packet. FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            sendErrorAck(packet, ERR_UNKNOW_DATA, FAILURE_ACK_LEN);
            return;
        }

        memcpy(extInfo, pDataStartAddr, EXT_INFO_LEN);
        extInfo[EXT_INFO_LEN-1] = '\0';
        pDataStartAddr += EXT_INFO_LEN + CONTROL_FLAG_LEN;
        //DLL�ļ���
        pDest = strstr(pDataStartAddr, CONTROL_FLAG);

        if(NULL == pDest)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServerWithDll: Received unknow format"\
                     "packet. FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            sendErrorAck(packet, ERR_UNKNOW_DATA, FAILURE_ACK_LEN);
            return;
        }

        memcpy(DllFileName, pDataStartAddr, min(pDest - pDataStartAddr, NAME_SIZE -1));
        //isLog
        pDataStartAddr = pDest + CONTROL_FLAG_LEN;
        pDest = strstr(pDataStartAddr, CONTROL_FLAG);

        if(NULL == pDest)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServerWithDll: Received unknow format"\
                     "packet. FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            sendErrorAck(packet, ERR_UNKNOW_DATA, FAILURE_ACK_LEN);
            return;
        }

        hex2int(&pDataStartAddr, &logFlag);
        //DllBuf
        pDataStartAddr = pDest + CONTROL_FLAG_LEN;
        pDest = packet->getData() + packet->getSize() - CONTROL_FLAG_LEN;        //ָ�����ĩβ
        uiDllBufLen= min(pDest - pDataStartAddr, sizeof(DllBuf));
        memcpy(DllBuf, pDataStartAddr, uiDllBufLen);

        /**���DLL�ļ��Ƿ����**/
        //���в���
        HINSTANCE hinstLib = LoadLibrary(DllFileName);

        if(NULL == hinstLib)
        {
            sysLoger(LINFO, "ControlSubAgent::handleCreateServerWithDll:Didn't find the dll file %s"\
                     ". FILE:%s, LINE:%d", DllFileName, __MYFILE__, __LINE__);
            sendErrorAck(packet, ERR_FILE_NOT_EXIST);
            return;
        }

        FreeLibrary(hinstLib);

        /******************����Ĵ���************************/
        if(Manager::getInstance()->getIDFromName(QString::fromAscii(SAName, strlen(SAName))) != -1)
        {
            //�Ѿ�������ͬ���ֵķ���
            sysLoger(LINFO, "ControlSubAgent::handleCreateServerWithDll: Eixt the service Agent with"\
                     " the same name! FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            sendErrorAck(packet, ERR_SERVICE_NAME_REPEATED, FAILURE_ACK_LEN);
            return;
        }

        int aid = Manager::getInstance()->createID();

        if(aid < 1)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServerWithDll: No usable agent id!");
            sendErrorAck(packet, ERR_SYSTEM_ERR);
            return;
        }

        ServerAgentWithDll* pSA = new ServerAgentWithDll(aid);    //����sa����

        if(NULL == pSA)
        {
            sysLoger(LERROR, "ControlSubAgent::handleCreateServerWithDll: Created SA failure!FILE:%s, LINE:%d",
                     __MYFILE__, __LINE__);
            sendErrorAck(packet, ERR_SYSTEM_ERR);
            return;
        }

        sysLoger(LINFO, "ControlSubAgent::handleCreateServerWithDll: Alloc aid %d for ServerName %s",
                 aid, SAName);

        pSA->setType(AgentBase::SERVER_AGENT_WITH_DLL);
        pSA->createConfig(DeviceConfigBase::DLL_DEVICE);        //����sa��Ӧ���豸��ʽ, ��̬�ⷽʽ
        pSA->setName(QString::fromAscii(SAName, strlen(SAName)));    //�õ���̬������
        pSA->setDllName(QString::fromAscii(DllFileName, strlen(DllFileName)));    //�õ���̬������
        pSA->setLogFlag(logFlag);    //��־
        pSA->setInBuf(QString::fromAscii(DllBuf, uiDllBufLen));    //���洫�ݵĲ���, �������ݸ���̬����ַ���
        pSA->clearChannel();
        pSA->addChannel(QString("dll"));                                  //����һ��ͨ��
        pSA->setExtInfo(QString::fromAscii(extInfo, sizeof(extInfo)));
        Manager::getInstance()->addServerAgent(pSA);
        Router::getInstance()->registerAgent(pSA->getAID(), pSA);

        if(pSA->getRegisterType() != INVALID_REG_TYPE)
        {
            Manager::getInstance()->storeConfigure();
        }

        //�ظ���Ϣ
        changePacketID(packet);    //�������е�Ŀ���ԴAID
        //ֱ����ԭ��Ϣ�е�DID����ȥmanager��ӳ�����ȡ
        pDest = packet->getData() + DIDLEN + CONTROL_FLAG_LEN;
        memcpy(pDest, SUCCESS_ACK, SUCCESS_ACK_LEN);
        pDest += SUCCESS_ACK_LEN;
        memcpy(pDest, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pDest += CONTROL_FLAG_LEN;
        pDest += int2hex(pDest, 10, pSA->getAID());
        memcpy(pDest, CONTROL_FLAG, CONTROL_FLAG_LEN);        //����һ���ָ��
        pDest += CONTROL_FLAG_LEN;
        //���ð��Ĵ�С
        packet->setSize(pDest - packet->getData());

        if(Manager::getInstance()->getLogFlag())
        {
            packLoger.write(SEND_PACK, *packet);
        }

        Router::getInstance()->putpkt(packet);
        return;
    }

    catch(...)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleCreateServerWithDll:Catch an error");
        sendErrorAck(packet, ERR_SYSTEM_ERR);
    }

    return;
}

/**
* ���ܣ�����TClient���͹�����createServer��������, ���ѵõ���AID���ظ�TClient
* @param pack ���յĿ��ư�
���͸�ʽ: Csa����, extInfo, ��־,ͨ����, +����wayѡ�����������Ϣ(�Էָ��)
*/
void ControlSubAgent::handleCreateServer(Packet* pack)
{
    try
    {
        if(NULL == pack)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServer: Received NULL point parameter!"\
                     " FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            return;
        }

        QVector<QString> vec;
        QByteArray block(pack->getData()+DIDLEN+CONTROL_FLAG_LEN,
                         pack->getSize()-(DIDLEN+CONTROL_FLAG_LEN));    //ת����QT�ĸ�ʽ
        //����ת��,ת����UTF8
        QString buf = QString::fromLocal8Bit(block.data(),block.size());
        QString str;
        bool ok;
        int next = 0;
        int ret = 0;    //��ѯ�Ŀ�ʼλ��
        char *pBuf;
        char *pStr;

        //name
        pBuf = pack->getData()+DIDLEN+CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
        pStr = strstr(pBuf, CONTROL_FLAG);

        if(pStr == NULL )
        {
            sysLoger(LINFO, "ControlSubAgent::handleCreateServer:lost of name part!");
            sendErrorAck(pack, ERR_SRV_CFG_ERR);
            return;
        }

        buf = QString::fromLocal8Bit(pBuf, pStr-pBuf);

        vec.append(buf);

        //extInfo
        if(pack->getSize() < (int)(EXT_INFO_LEN + (pStr-pBuf) +DIDLEN+2*CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN))
        {
            sysLoger(LINFO, "ControlSubAgent::handleCreateServer:lost of extInfo part!");
            sendErrorAck(pack, ERR_SRV_CFG_ERR);
            return;
        }

        pBuf = pStr + CONTROL_FLAG_LEN;
        buf = QString::fromAscii(pBuf, EXT_INFO_LEN);
        vec.append(buf);
        //others
        pBuf += EXT_INFO_LEN + CONTROL_FLAG_LEN;
        buf = QString::fromLocal8Bit(pBuf, pack->getSize() - (pBuf - pack->getData()));

        while(true)
        {
            str = buf.section(CONTROL_FLAG, next, next);

            if (str.size() == 0)
            {
                break;
            }

            vec.append(str);
            next++;
        }

        int iCount = 0;
        int dataIndex = 0;
        QString name(vec[iCount]);            //�õ�Ҫ����sa������

        sysLoger(LDEBUG, "ControlSubAgent::handleCreateServer: Enter this function.!");

        //�����������Ѿ��ﵽ�������
        if(Manager::getInstance()->isAgentCountReachedToMAX())
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServer: Agent objects in TS have"\
                     "reatched max count!");
            sendErrorAck(pack, ERR_SRV_NUM_EXPIRE);
            return;
        }

        /**
        TC�����������ݰ���ʽ:
        "DID, CsaName, extInfo, isLog, channelCount, config of each channel, "
        */


        //������ȥ�����Ƿ��Ѿ�����ͬ������
        int srvID = Manager::getInstance()->getIDFromName(name);

        if(srvID != -1)
        {
            //�Ѿ�������ͬ���ֵķ���
            sendErrorAck(pack, ERR_SERVICE_NAME_REPEATED);
            sysLoger(LINFO, "ControlSubAgent::handleCreateServer: Exit the service Agent with "\
                     " the same name.. FILE:%s, LINE:%d.", __MYFILE__, __LINE__);
            return;
        }

        //����extInfo�ֶ�
        ++iCount;
        //log
        int isLog = vec[++iCount].toInt(&ok, 16);        //��־��¼

        if(isLog > 0)
        {
            isLog = 1;
        }

        else
        {
            isLog = 0;
        }

        int channelCount = vec[++iCount].toInt(&ok, 16);        //ͨ����
        ServerAgent* pSA = NULL;

        if(channelCount > MAX_CHANNEL_COUNT)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServer: Invalid Channel Count: %d.",
                     channelCount);
            sendErrorAck(pack, ERR_SRV_CFG_ERR);
            return;
        }

        for (int i=0; i<channelCount; ++i)
        {
            dataIndex = iCount;
            str.clear();
            str += vec[++iCount];                        //Э��
            str += CONTROL_FLAG;
            int way = vec[++iCount].toInt(&ok, 16);            //ͨ�ŷ�ʽ
            str += vec[iCount];                        //�豸ͨ�ŷ�ʽ
            str += CONTROL_FLAG;

            switch(way)
            {
            case TCP:    //tcp
            {
                QString strIP;
                strIP = vec[++iCount];
                str += vec[iCount];
                str += CONTROL_FLAG;

                int port = vec[++iCount].toInt(&ok, 16);        //�˿�
                str += QString("%1").arg(port, 0, 16 );
                str += CONTROL_FLAG;

                int timeout = vec[++iCount].toInt(&ok, 16);                //��ʱ
                str += QString("%1").arg(timeout, 0, 16 );
                str += CONTROL_FLAG;
                int retry = vec[++iCount].toInt(&ok, 16);                //����
                str += QString("%1").arg(retry, 0, 16 );
                str += CONTROL_FLAG;

                //Ϊ��һͨ�������豸
                if(i == 0)
                {
                    TcpConfig *ptcpConfig = new TcpConfig();//TCP, port, strIP);                        //tcp��������Ϣ

                    if(ptcpConfig != NULL)
                    {
                        ptcpConfig->setDeviceType(TCP);
                        ptcpConfig->setIP(strIP);
                        ptcpConfig->setPort(port);
                        pSA = createSA(ptcpConfig, name, isLog, way, SASTATE_INACTIVE);
                    }
                }
            }
            break;

            case UDP:    //udp
            {
                QString strIP;
                strIP = vec[++iCount];
                str += vec[iCount];
                str += CONTROL_FLAG;

                int port = vec[++iCount].toInt(&ok, 16);        //�˿�
                str += QString("%1").arg(port, 0, 16 );
                str += CONTROL_FLAG;
                int timeout = vec[++iCount].toInt(&ok, 16);    //��ʱ
                str += QString("%1").arg(timeout, 0, 16 );
                str += CONTROL_FLAG;
                int retry = vec[++iCount].toInt(&ok, 16);    //����
                str += QString("%1").arg(retry, 0, 16 );
                str += CONTROL_FLAG;

                //Ϊ��һͨ�������豸
                if(i == 0)
                {
                    UdpConfig *pudpConfig = new UdpConfig();//UDP, port, strIP, timeout, retry);

                    if(pudpConfig != NULL)
                    {
                        pudpConfig->setDeviceType(UDP);
                        pudpConfig->setIP(strIP);
                        pudpConfig->setPort(port);
                        pudpConfig->setTimeout(timeout);
                        pudpConfig->setRetry(retry);

                        pSA = createSA(pudpConfig, name, isLog, way, SASTATE_INACTIVE);
                    }
                }
            }
            break;

            case UART:                        //����
            {
                QString comChannel = vec[++iCount];
                str += comChannel;
                str += CONTROL_FLAG;
                //��鴮��ͨ���Ƿ���Ч
                char bufCom[200] = "\0";
                int idx = 0;
                string localCOMcfg;     //�������ڵ�����

                if(queryHostSerialPort(bufCom, sizeof(bufCom)) > 0)
                {
                    localCOMcfg = string(bufCom, strlen(bufCom));
                    idx = localCOMcfg.find(comChannel.toAscii().data());

                    if(idx == string::npos)
                    {
                        //TS����û�иô�������
                        sysLoger(LINFO, "ControlSubAgent::handleCreateServer:There is no COM port %s"\
                                 " in local machine!", comChannel.toAscii().data());
                        //�ظ�
                        sendErrorAck(pack, ERR_INVALID_UART_PORT);

                        //����ͨ���������Ϣ
                        if(pSA != NULL)
                        {
                            pSA->clearConfig();
                            //�ͷ�ID��
                            Manager::getInstance()->freeAid(pSA->getAID());
                            delete pSA;
                            pSA = NULL;
                        }

                        return;
                    }
                }

                int baud = vec[++iCount].toInt(&ok, 16);        //������
                str += QString("%1").arg(baud, 0, 16 );
                str += CONTROL_FLAG;
                int timeout = vec[++iCount].toInt(&ok, 16);    //��ʱ
                str += QString("%1").arg(timeout, 0, 16 );
                str += CONTROL_FLAG;
                int retry = vec[++iCount].toInt(&ok, 16);    //����
                str += QString("%1").arg(retry, 0, 16 );
                str += CONTROL_FLAG;

                //Ϊ��һͨ�������豸
                if (i == 0)
                {
                    UartConfig *pUartConfig = new UartConfig();//UART, comChannel, baud, timeout, retry);

                    if(pUartConfig != NULL)
                    {
                        pUartConfig->setDeviceType(UART);
                        pUartConfig->setCom(comChannel);
                        pUartConfig->setBaud(baud);
                        pUartConfig->setTimeout(timeout);
                        pUartConfig->setRetry(retry);
                        pSA = createSA(pUartConfig, name, isLog, way, SASTATE_INACTIVE);
                    }
                }
            }
            break;

            default:
                sysLoger(LWARNING, "ControlSubAgent::handleCreateServer: Created SA with unsupported "\
                         "communicaton way %d , failed.  FILE:%s, LINE:%d",
                         way, __MYFILE__, __LINE__);
                //�ظ�TClient
                sendErrorAck(pack, ERR_UNKNOW_COMM_WAY);

                //����ͨ���������Ϣ
                if(pSA != NULL)
                {
                    pSA->clearConfig();
                    //�ͷ�ID��
                    Manager::getInstance()->freeAid(pSA->getAID());
                    delete pSA;
                    pSA = NULL;
                }

                return;
                break;
            }

            //������û����������

            //������������
            if(pSA != NULL)
            {
                pSA->addChannel(str);
            }
        }

        //�жϴ����Ƿ�ɹ�
        if (pSA == NULL)
        {
            sendErrorAck(pack, ERR_SYSTEM_ERR);
            sysLoger(LINFO, "ControlSubAgent::handleCreateServer: Created SA failure, name: %s",
                     name);

            //�������aid�ɹ������ͷ�
            if(srvID > 1)
            {
                Manager::getInstance()->freeAid(srvID);
            }
        }

        else
        {

            sysLoger(LINFO, "ControlSubAgent::handleCreateServer: Created SA, aid: %d, name: %s",
                     pSA->getAID(), pSA->getName().toAscii().data());

            pSA->getConfig()->setLogFlag(isLog);
            pSA->setLogFlag(isLog);
            pSA->setExtInfo(vec[1]);
            Manager::getInstance()->addServerAgent(pSA);

            if(pSA->getRegisterType() != INVALID_REG_TYPE)
            {
                Manager::getInstance()->storeConfigure();
            }

            //�����ɹ����ظ�TClient
            changePacketID(pack);    //�������е�Ŀ���ԴAID
            char* pBuf = pack->getData();
            __store(CURDID(pack->getDes_aid()), pBuf, DIDLEN);
            pBuf += DIDLEN;
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //OK
            memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
            pBuf += SUCCESS_ACK_LEN;
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //AID
            pBuf += int2hex(pBuf, 10, pSA->getAID());
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //���ð���С
            pack->setSize(pBuf - pack->getData());

            if(Manager::getInstance()->getLogFlag())
            {
                packLoger.write(SEND_PACK, *pack);
            }

            Router::getInstance()->putpkt(pack);
        }

        return;
    }

    catch(...)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleCreateServer:Catch an error!");
        sendErrorAck(pack, ERR_SYSTEM_ERR);
    }
}


/**
* ���ܣ�����TClient���͹�����deleteServer��������, ֹͣsa
* @param block ȥ��createserver����
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::handleDeleteServer(Packet* pack)
{
    int iRet = 0;
    char *pDest = NULL;
    char *pStr = NULL;
    int aid = 0;

    // ���ݰ���ʽ"DID, Raid, "
    if(NULL == pack)
    {
        sysLoger(LWARNING, "handleDeleteServer:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    //��proccess���Ѿ�������֤�����ݰ��ĳ���
    //����ֱ������DID����
    pDest = pack->getData() + DIDLEN + CONTROL_FLAG_LEN;
    //AID
    pStr = strstr(pDest, CONTROL_FLAG);

    if(NULL == pStr)
    {
        sysLoger(LWARNING, "handleDeleteServer:Received unknow format packet!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        sendErrorAck(pack, ERR_UNKNOW_DATA);
        return;
    }

    pDest += CONTROL_COMMAND_LEN;
    hex2int(&pDest, &aid);

    sysLoger(LINFO, "ControlSubAgent::handleDeleteServer: Starting to delete agent:%d.", aid);
    //TC�˷�0��ɾ��TS�����е�SERVER
    iRet = Manager::getInstance()->deleteServer(aid);

    if(iRet == TS_SUCCESS)
    {
        sysLoger(LINFO, "ControlSubAgent::handleDeleteServer: delete ServiceAgent successfully, "\
                 "serviceAgentID = %d.  FILE:%s, LINE:%d", aid, __MYFILE__, __LINE__);
        setSuccessPacket(pack);
    }

    else
    {
        sysLoger(LINFO, "ControlSubAgent::handleDeleteServer: delete ServiceAgent failure, "\
                 "serviceAgentID = %d.  FILE:%s, LINE:%d", aid, __MYFILE__, __LINE__);
        sendErrorAck(pack, getErrStrByErrValue(iRet));
        return;
    }

    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    Router::getInstance()->putpkt(pack);
    return;
}


/**
* ���ܣ�����TClient���͹�����createServer��������, ���ѵõ���AID���ظ�TClient
* @param deviceConfig �豸������Ϣ
* @param name sa������
* @param isLog sa�Ƿ���Ҫ��־��¼
* @param protocolType sa��Ӧ��Э��
* @param state sa��״̬
* @return ServerAgent* ����sa����ָ��
*/
ServerAgent* ControlSubAgent::createSA(DeviceConfigBase* deviceConfig, QString name, bool isLog,
                                       int protocolType, int state)
{
    //����һ��sa����
    try
    {
        ServerAgent* pSA = new ServerAgent(Manager::getInstance()->createID());    //����sa����

        if(NULL == pSA)
        {
            sysLoger(LERROR, "ControlSubAgent::createSA: Created service Agent object failure! "\
                     "FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            Packet pack;
            sendErrorAck(&pack, ERR_SYSTEM_ERR);
            return NULL;
        }

        pSA->setName(name);
        pSA->setLogFlag(isLog);
        ServerConfig *pSACfg = new  ServerConfig(deviceConfig, protocolType, isLog);

        if(NULL == pSACfg)
        {
            sysLoger(LWARNING, "ControlSubAgent::createSA: Created config for SA :%s failure!",
                     name.toAscii().data());
            return NULL;
        }

        else
        {
            pSA->setConfig(pSACfg);
        }

        if (pSA->getState())
        {
            if (pSA->run())
            {
                //����sa�߳�
                //    mapName2ID.insert(pServerAgent->getName(),  pServerAgent->getAID());    //�Ѵ�����sa�����ֺ�AID�����������
                pSA->setState(SASTATE_ACTIVE);    //����saΪ�״̬
            }

            else
            {
                pSA->setState(SASTATE_INACTIVE);
            }
        }

        return pSA;
    }

    catch(...)
    {
        sysLoger(LERROR, "ControlSubAgent::createSA:Catch an exception when createSA!");
        return NULL;
    }
}


/**
* ���ܣ�����TCLIENT_OPEN����, ����ԴAID��������
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::handleTClientOpen(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::HandleTClientOpen:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    //���ͻظ�����
    //����ԴAID��Ŀ��AID��
    sysLoger(LDEBUG, "ControlSubAgent::HandleTClientOpen: Received clientAgent %d 's open command",
             pack->getSrc_aid());
    changePacketID(pack);
    char* pBuf =pack->getData();
    memset(pBuf, 0, PACKET_DATASIZE);
    //�洢DID
    __store(CURDID(pack->getDes_aid()),  pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //���ͻظ���Ϣ��
    pack->setSize(pBuf-pack->getData());                //���°��Ĵ�С

    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    Router::getInstance()->putpkt(pack);
    return;
}
/**
* ���ܣ�����TClient_queryServerState����, ��ѯTS������ڵ����еĿ���Ŀ�����ID
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::handleSearchServer(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSearchServer:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    //�õ���ǰ�����server����
    QMap<QString , short> mapName2ID = Manager::getInstance()->getName2AIDMap();    //�õ�managerӳ��
    QMap<QString, short>::const_iterator cItrName2ID = mapName2ID.constBegin();
    changePacketID(pack);    //�������е�Ŀ���ԴAID
    char* pBuf =pack->getData();
    pack->clearData();
    //DID
    __store(Manager::getInstance()->getCurDID(pack->getDes_aid()), pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //OK
    memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
    pBuf += SUCCESS_ACK_LEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    // ServerAgent Count
    pBuf += int2hex(pBuf, 10, mapName2ID.size());
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    sysLoger(LINFO, "ControlSubAgent::handleSearchServer: Had been Searched %d serverAgents!",
             mapName2ID.size());

    //each ServerAgent ID

    /*Modified by tangxp for BUG NO.0002555 on 2008.2.3 [gebin]*/
    /*  Modified brief: �������û�ȡ���������ĺ���,���ӻ�������ж�*/
    while((cItrName2ID != mapName2ID.constEnd()) && ((pBuf - pack->getData()) < PACKET_DATASIZE - 10))
        /*Modified by tangxp for BUG NO.0002555 on 2008.2.3 [end]*/
    {
        pBuf += int2hex(pBuf, 10, cItrName2ID.value());
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        ++cItrName2ID;
    }

    pack->setSize(pBuf-pack->getData());        //���ð��Ĵ�С

    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    Router::getInstance()->putpkt(pack);                        //���а��ķ���
    return;
}

/**
* ���ܣ�����TClient_activeServer����, ����ָ���Ĵ�������״̬��Ŀ�����
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::handleActiveServer(Packet* pack)
{
    char *pDest = NULL;
    char *pStr = NULL;
    int aid = 0;

    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleActiveServer:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    try
    {
        /*process���Ѿ���������ݰ��ĳ��ȣ�����ֱ������DID����*/
        pDest = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
        pStr = strstr(pDest, CONTROL_FLAG);

        if(NULL == pStr)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleActiveServer:Received unknow packet!");
            sendErrorAck(pack, ERR_UNKNOW_DATA);
            return;
        }

        hex2int(&pDest, &aid);
        sysLoger(LINFO, "ControlSubAgent::handleActiveServer:Active server [aid = %d] starting...", aid);
        //Ҫ������ǿ��ת��
        AgentWithQueue* pAgent = dynamic_cast<AgentWithQueue*>
                                 (Manager::getInstance()->getServerAgentFromID(aid));

        if(NULL == pAgent)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleActiveServer: Active absent SA[aid = %d]"\
                     " FILE:%s, LINE:%d", aid, __MYFILE__, __LINE__);
            //�ظ���Ϣ
            sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
            return;
        }

        switch(pAgent->getType())
        {
        case AgentBase::SERVER_AGENT:
        {
            ServerAgent* pSA = dynamic_cast<ServerAgent*>(pAgent);

            if(pSA != NULL)
            {
                if(pSA->getState() == SASTATE_INACTIVE)
                {
                    //û�м����򼤻�(����SA)
                    if(pSA->run())
                    {
                        sysLoger(LINFO, "HandleActiveServer: Active server [aid = %d] Successully!", aid);
                        pSA->setState(SASTATE_ACTIVE);
                        setSuccessPacket(pack);
                    }

                    else
                    {
                        sysLoger(LINFO, "HandleActiveServer: Active server [aid = %d] failure!", aid);
                        pSA->setState(SASTATE_INACTIVE);
                        changePacketID(pack);
                        pack->setErrorPacket(ERR_DEVICE_CANNOT_USE, FAILURE_ACK_LEN,
                                             CURDID(pack->getDes_aid()));
                    }
                }

                else if(pSA->getState() == SASTATE_ACTIVE)
                {
                    setSuccessPacket(pack);
                }

                if(Manager::getInstance()->getLogFlag())
                {
                    packLoger.write(SEND_PACK, *pack);
                }

                Router::getInstance()->putpkt(pack);
                //�����ϱ�
                Manager::getInstance()->sendSAReportToCA(pAgent);

                if(pAgent->getRegisterType() != INVALID_REG_TYPE)
                {
                    Manager::getInstance()->storeConfigure();
                }

                return;
            }
        }
        break;

        case AgentBase::SERVER_AGENT_WITH_DLL:
        {
            ServerAgentWithDll* pSA = dynamic_cast<ServerAgentWithDll*>(pAgent);

            if(pSA != NULL)
            {
                if(pSA->getState() == SASTATE_INACTIVE)
                {
                    if(pSA->run())
                    {
                        pSA->setState(SASTATE_ACTIVE);
                        //���ظ���Ϣ��
                        changePacketID(pack);    //�������е�Ŀ���ԴAID
                        pack->clearData();
                        char* pBuf = pack->getData();
                        //DID
                        __store(CURDID(pack->getDes_aid()), pBuf, DIDLEN);
                        pBuf += DIDLEN;
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //����һ���ָ��
                        pBuf += CONTROL_FLAG_LEN;
                        //OK
                        memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
                        pBuf += SUCCESS_ACK_LEN;
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //����һ���ָ��
                        pBuf += CONTROL_FLAG_LEN;
                        //aid
                        pBuf += int2hex(pBuf, 10, pSA->getAID());
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //����һ���ָ��
                        pBuf += CONTROL_FLAG_LEN;

                        int outbufsize = strlen(pSA->getOutBuf());

                        if(pBuf - pack->getData() < (int)(outbufsize + CONTROL_FLAG_LEN))
                        {
                            sysLoger(LWARNING, "ControlSubAgent::handleActiveServer: Dll server's outBuf"\
                                     " is too large to an ACK packet size!");
                        }

                        outbufsize = min((int)(PACKET_DATASIZE - (pBuf - pack->getData()) - \
                                               CONTROL_FLAG_LEN), outbufsize);
                        //���ͳ�ȥ��DLLBUF��С
                        pBuf += int2hex(pBuf, 10, outbufsize);            //�ַ����Ĵ�С
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
                        pBuf += CONTROL_FLAG_LEN;

                        memcpy(pBuf, pSA->getOutBuf(), outbufsize);                    //����������
                        pBuf += outbufsize;
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //����һ���ָ��
                        pBuf += CONTROL_FLAG_LEN;
                        pack->setSize(pBuf - pack->getData());                //���°��Ĵ�С
                        sysLoger(LINFO, "ControlSubAgent::handleActiveServer: Active server agent [aid=%d] "\
                                 " successfully!", aid);
                    }

                    else
                    {
                        pSA->setState(SASTATE_INACTIVE);
                        changePacketID(pack);
                        pack->setErrorPacket(ERR_SYSTEM_ERR, FAILURE_ACK_LEN,
                                             CURDID(pack->getDes_aid()));
                        sysLoger(LINFO, "ControlSubAgent::handleActiveServer: Active server agent [aid=%d] "\
                                 " failure!", aid);
                    }
                }

                else if(pSA->getState() == SASTATE_ACTIVE)                //�Ѿ��Ǽ���״̬
                {
                    //���ظ���Ϣ��
                    changePacketID(pack);    //�������е�Ŀ���ԴAID
                    pack->clearData();
                    char* pBuf = pack->getData();
                    //DID
                    __store(CURDID(pack->getDes_aid()), pBuf, DIDLEN);
                    pBuf += DIDLEN;
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //����һ���ָ��
                    pBuf += CONTROL_FLAG_LEN;
                    //OK
                    memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
                    pBuf += SUCCESS_ACK_LEN;
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //����һ���ָ��
                    pBuf += CONTROL_FLAG_LEN;
                    //aid
                    pBuf += int2hex(pBuf, 10, pSA->getAID());
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //����һ���ָ��
                    pBuf += CONTROL_FLAG_LEN;
                    int outbufsize = strlen(pSA->getOutBuf());

                    if(pBuf - pack->getData() < (int)(outbufsize + CONTROL_FLAG_LEN))
                    {
                        sysLoger(LWARNING, "ControlSubAgent::handleActiveServer: Dll server's outBuf"\
                                 " is too large to an ACK packet size!");
                    }

                    outbufsize = min((int)(PACKET_DATASIZE - (pBuf - pack->getData()) - \
                                           CONTROL_FLAG_LEN), outbufsize);
                    //���ͳ�ȥ��DLLBUF��С
                    pBuf += int2hex(pBuf, 10, outbufsize);            //�ַ����Ĵ�С
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
                    pBuf += CONTROL_FLAG_LEN;

                    memcpy(pBuf, pSA->getOutBuf(), outbufsize);                    //����������
                    pBuf += outbufsize;
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //����һ���ָ��
                    pBuf += CONTROL_FLAG_LEN;
                    pack->setSize(pBuf - pack->getData());                //���°��Ĵ�С
                    sysLoger(LINFO, "ControlSubAgent::handleActiveServer: Active server agent [aid=%d] "\
                             ", but the agent is already in active state!", aid);
                }

                else
                {
                    //��������״̬
                    sysLoger(LWARNING, "handleActiveServer: the serverAgent [aid = %d] in the state[%d]"\
                             ", activing operator failure!", aid, pSA->getState());
                    sendErrorAck(pack, ERR_SYSTEM_ERR);
                    return;
                }

                if(Manager::getInstance()->getLogFlag())
                {
                    packLoger.write(SEND_PACK, *pack);
                }

                Router::getInstance()->putpkt(pack);
                //�����ϱ�
                Manager::getInstance()->sendSAReportToCA(pAgent);

                if(pAgent->getRegisterType() != INVALID_REG_TYPE)
                {
                    Manager::getInstance()->storeConfigure();
                }

                return;
            }
        }
        break;

        default:
            sysLoger(LWARNING, "handleActiveServer: The Object that actived is not SA!");
            break;
        }

        //�쳣����(pSA == NULL) /pAgent->getType() = other
        sendErrorAck(pack, ERR_SYSTEM_ERR);
        sysLoger(LWARNING, "handleActiveServer:Actived server agent failure!");
        return;
    }

    catch(...)
    {
        sysLoger(LERROR, "handleActiveServer: catch an exception!Active server Agent failure!");
        sendErrorAck(pack, ERR_SYSTEM_ERR);
    }

    return;
}

/**
* ���ܣ�����TClient_inActiveServer����, ����ָ���Ĵ��ڼ���״̬��Ŀ�����
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::handleInactiveServer(Packet* pack)
{
    char *pDest = NULL;
    char *pStr = NULL;
    int aid  = 0;

    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleInactiveServer:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    try
    {
        /*process���Ѿ���������ݰ��ĳ��ȣ�����ֱ������DID����*/
        pDest = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
        pStr = strstr(pDest, CONTROL_FLAG);

        if(NULL == pStr)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleInactiveServer:Received unknow packet!");
            sendErrorAck(pack, ERR_UNKNOW_DATA);
            return;
        }

        hex2int(&pDest, &aid);
        sysLoger(LINFO, "ControlSubAgent::handleInactiveServer:Inactive server [aid = %d] starting...", aid);
        AgentWithQueue* pAgent = dynamic_cast<AgentWithQueue*>
                                 (Manager::getInstance()->getServerAgentFromID(aid));

        if(NULL == pAgent)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleInactiveServer: Inactive absent SA[aid = %d]"\
                     " FILE:%s, LINE:%d", aid, __MYFILE__, __LINE__);
            //�ظ���Ϣ
            sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
            return;
        }

        switch(pAgent->getType())
        {
        case AgentBase::SERVER_AGENT:
        {
            ServerAgent* pServerAgent = dynamic_cast<ServerAgent*>(pAgent);

            if(pServerAgent != NULL)
            {
                /*Modified by tangxp for BUG NO.3453 on 2008.6.23 [begin]*/
                /*  Modified brief: ����SAʱ,ֱ�ӹر�SA,�����豸���ӶϿ�ʱ,û��ɾ��TX,RX�߳�*/
                if(pServerAgent->getState() == SASTATE_ACTIVE)
                {
                    Router::getInstance()->unRegisterAgent(pServerAgent->getAID());    //ɾ����router��ע��
                }

                /*Modified by tangxp for BUG NO.3453 on 2008.6.23 [end]*/
                setSuccessPacket(pack);
                sysLoger(LINFO, "ControlSubAgent::handleInactiveServer: Inactive SA [aid = %d] successfully",
                         aid);

                //��־
                if(Manager::getInstance()->getLogFlag())
                {
                    packLoger.write(SEND_PACK, *pack);
                }

                /* Ŀ���������ʱ����������ᳬʱ�ط�,��IDEֻ�ȴ�6��,�ͻ���ɵ�һ�ζϿ�����ʧ�ܡ��������Ŀ������ͶϿ����C��ǰ,����IDE�ظ��Ͽ������ɹ�����. */
                Router::getInstance()->putpkt(pack);

                pServerAgent->stop();

                //�ϱ�
                Manager::getInstance()->sendSAReportToCA(pAgent);

                if(pAgent->getRegisterType() != INVALID_REG_TYPE)
                {
                    Manager::getInstance()->storeConfigure();
                }

                return;
            }
        }
        break;

        case AgentBase::SERVER_AGENT_WITH_DLL:
        {
            ServerAgentWithDll* pServerAgent = dynamic_cast<ServerAgentWithDll*>(pAgent);

            if(pServerAgent != NULL)
            {
                if(pServerAgent->getState() == SASTATE_ACTIVE)
                {
                    pServerAgent->stop();
                    Router::getInstance()->unRegisterAgent(pServerAgent->getAID());    //ɾ����router��ע��
                }

                setSuccessPacket(pack);
                sysLoger(LINFO, "ControlSubAgent::handleInactiveServer: Inactive SA [aid = %d] successfully",
                         aid);

                //��־
                if(Manager::getInstance()->getLogFlag())
                {
                    packLoger.write(SEND_PACK, *pack);
                }

                Router::getInstance()->putpkt(pack);
                //�ϱ�
                Manager::getInstance()->sendSAReportToCA(pAgent);

                if(pAgent->getRegisterType() != INVALID_REG_TYPE)
                {
                    Manager::getInstance()->storeConfigure();
                }

                return;
            }
        }
        break;

        default:
            sysLoger(LWARNING, "handleInactiveServer: The Object that Inactived is not SA!");
            break;
        }

        //�쳣����(pServerAgent == NULL) /pAgent->getType() = other
        sendErrorAck(pack, ERR_SYSTEM_ERR);
        sysLoger(LWARNING, "handleInactiveServer:Inactived server agent failure!");
        return;
    }

    catch(...)
    {
        sysLoger(LERROR, "handleInactiveServer: catch an exception!Inactive server Agent failure!");
        sendErrorAck(pack, ERR_SYSTEM_ERR);
    }

    return;
}

/**
* ���ܣ�����TClient_queryServerState����, ָ��Ŀ�����ID�Ų��Ҹ�Ŀ��������ò�����״̬��Ϣ
* @param pack ���յĿ��ư�
* �ظ�����ʽ
TCP/UDP/UART
OK, name, extInfo,state, log, chcount, curch, config of each ch..
DLL
OK, name, extInfo,state, log, chcount, curch, protocol, way, dllname, outbuf, inbuf,

*/
void ControlSubAgent::handleQueryServer(Packet* pack)
{
    char *pDest = NULL, *pStr = NULL;
    int aid = 0;

    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleQueryServer:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    try
    {
        char extInfo[EXT_INFO_LEN] = "\0";

        /*process���Ѿ���������ݰ��ĳ��ȣ�����ֱ������DID����*/
        pDest = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;

        pStr = strstr(pDest, CONTROL_FLAG);

        if(NULL == pStr)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleQueryServer:Received unknow packet!");
            sendErrorAck(pack, ERR_UNKNOW_DATA);
            return;
        }

        hex2int(&pDest, &aid);

        sysLoger(LINFO, "ControlSubAgent::handleQueryServer:Query server [aid = %d] starting...", aid);

        //���Ҹ�AID��Ӧ�Ķ���
        AgentWithQueue* pAgentWithQueue = dynamic_cast<AgentWithQueue*>
                                          (Manager::getInstance()->getServerAgentFromID(aid));

        if(NULL == pAgentWithQueue)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleQueryServer: Queried absent SA[aid = %d]"\
                     " FILE:%s, LINE:%d", aid, __MYFILE__, __LINE__);
            //�ظ���Ϣ
            sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
            return;
        }

        //���ݷ�����������ϱ���Ϣ
        switch(pAgentWithQueue->getType())
        {
        case AgentBase::SERVER_AGENT:
        case AgentBase::SERVER_AGENT_WITH_DLL:
        {
            //ServerAgent* pServerAgent = dynamic_cast<ServerAgent*>(pAgentWithQueue);
            //int way = pServerAgent->getConfig()->getDeviceConfig()->getDeviceType();    //�õ�ͨ�ŷ�ʽ
            //���ظ���Ϣ��
            changePacketID(pack);
            pack->clearData();
            char *pBuf = pack->getData();
            //DID
            __store(CURDID(pack->getDes_aid()), pBuf, DIDLEN);
            pBuf += DIDLEN;
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //OK
            memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
            pBuf += SUCCESS_ACK_LEN;
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //Name
            memcpy(pBuf, pAgentWithQueue->getName().toAscii().data(),
                   pAgentWithQueue->getName().toAscii().length());
            pBuf += pAgentWithQueue->getName().toAscii().length();
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //extInfo
            memcpy(extInfo, pAgentWithQueue->getExtInfo().toAscii().data(),
                   min(EXT_INFO_LEN, pAgentWithQueue->getExtInfo().toAscii().length()));
            memcpy(pBuf, extInfo, sizeof(extInfo));
            pBuf += sizeof(extInfo);
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //state
            pBuf += int2hex(pBuf, 10, pAgentWithQueue->getState());
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //LogFlag
            pBuf += int2hex(pBuf, 10, pAgentWithQueue->getLogFlag());
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //ChannelCount
            pBuf += int2hex(pBuf, 10, pAgentWithQueue->getChannelCount());
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            //CurrentChannel
            pBuf += int2hex(pBuf, 10, pAgentWithQueue->getChannelType());
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;

            if(pAgentWithQueue->getType() == AgentBase::SERVER_AGENT)
            {
                //config of each channel
                QVector<QString> channelInfo = pAgentWithQueue->getChannelInfo();

                //�ϱ�ͨ��״̬
                for (int i=0; i<pAgentWithQueue->getChannelCount(); ++i)
                {
                    QByteArray bufBlock = channelInfo[i].toAscii();
                    memcpy(pBuf,bufBlock.data(),bufBlock.size());    //ͨ��������Ϣ
                    pBuf += bufBlock.size();
                }
            }

            else
            {
                ServerAgentWithDll* pServerAgentWithDll = dynamic_cast<ServerAgentWithDll*>(pAgentWithQueue);
                /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [begin]*/
                /*  Modified brief: ��ȡͨ��Э���ͨ�ŷ�ʽʱ,ֱ����ΪDLL_DEVICE */

                int protocol = DeviceConfigBase::DLL_DEVICE;                        //Э������

                pBuf += int2hex(pBuf, 10, protocol);                                    //����ͨ�ŷ�ʽ
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //����һ���ָ��
                pBuf += CONTROL_FLAG_LEN;

                int way = DeviceConfigBase::DLL_DEVICE;                             //�õ�ͨ�ŷ�ʽ
                pBuf += int2hex(pBuf, 10, way);                                    //����ͨ�ŷ�ʽ
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //����һ���ָ��
                pBuf += CONTROL_FLAG_LEN;
                /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [end]*/

                //DllName
                memcpy(pBuf, pServerAgentWithDll->getDllName().toAscii().data(),
                       pServerAgentWithDll->getDllName().toAscii().length());                    //��̬������
                pBuf += pServerAgentWithDll->getDllName().toAscii().length();
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //����һ���ָ��
                pBuf += CONTROL_FLAG_LEN;

                //OutBuf
                memcpy(pBuf, pServerAgentWithDll->getOutBuf(), strlen(pServerAgentWithDll->getOutBuf()));
                pBuf += strlen(pServerAgentWithDll->getOutBuf());
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //����һ���ָ��
                pBuf += CONTROL_FLAG_LEN;

                //InBuf
                memcpy(pBuf, pServerAgentWithDll->getInBuf().toAscii().data(),
                       pServerAgentWithDll->getInBuf().toAscii().length());
                pBuf += pServerAgentWithDll->getInBuf().toAscii().length();
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //����һ���ָ��
                pBuf += CONTROL_FLAG_LEN;
            }

            pack->setSize(pBuf - pack->getData());
        }
        break;

        default:
            sysLoger(LWARNING, "ControlSubAgent::handleQueryServer: Tried to query information of "\
                     " nonsupport agent type[aid=%d,type=%d]!  FILE:%s, LINE:%d",
                     aid, pAgentWithQueue->getType(), __MYFILE__, __LINE__);
            //�ظ�ʧ����Ϣ��
            sendErrorAck(pack, ERR_INVALID_SERVICE_ID);
            return;
            break;
        }

        //�������ݲ���¼��־
        if(Manager::getInstance()->getLogFlag())
        {
            packLoger.write(SEND_PACK, *pack);
        }

        Router::getInstance()->putpkt(pack);

        return;
    }

    catch(...)
    {
        sysLoger(LERROR, "ControlSubAgent::handleQueryServer: catch an exception!Query server failure!");
        sendErrorAck(pack, ERR_SYSTEM_ERR);
    }

    return;
}
/**
* ���ܣ�����TClient_modifyServer����, �޸�һ���Ѿ�������TS�е�Ŀ���������
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::handleModifyServer(Packet* pack)
{
    QVector<QString> vec;
    QByteArray block(pack->getData()+DIDLEN+CONTROL_FLAG_LEN+ CONTROL_COMMAND_LEN,
                     pack->getSize()-(DIDLEN+CONTROL_FLAG_LEN+ CONTROL_COMMAND_LEN));    //ת����QT�ĸ�ʽ
    //����ת��,ת����UUTF8
    QString buf = QString::fromLocal8Bit(block.data(), block.size());
    QString str;
    bool ok;
    int next = 0;    //��ѯ�Ŀ�ʼλ��
    bool isSrvRunbfMod = false;            //�޸�֮ǰ��������Ƿ�������̬

    /*
    TClient ���͹�������Ϣ��ʽ
     NET/UART��ʽ
     -----------------------------------------------------------------------------
     |DID|, |Maid|, |name|, |extInfo|,|isLog|, |channelCount|, |curChannel|, |channelConfig..............|
     -----------------------------------------------------------------------------
     DLL��ʽ
      -----------------------------------------------------------------------------
     |DID|, |Maid|, |name|, |extInfo|,|isLog|, |way|, |DllBuf|,
     -----------------------------------------------------------------------------

    */
    try
    {
        char *pBuf;
        char *pStr;
        //aid
        pBuf = pack->getData()+DIDLEN+CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
        pStr = strstr(pBuf, CONTROL_FLAG);

        if(pStr == NULL )
        {
            sysLoger(LINFO, "ControlSubAgent::handleCreateServer:lost of name part!");
            sendErrorAck(pack, ERR_SRV_CFG_ERR);
            return;
        }

        buf = QString::fromLocal8Bit(pBuf, pStr-pBuf);
        vec.append(buf);

        //name
        pBuf = pStr + CONTROL_FLAG_LEN;
        pStr = strstr(pBuf, CONTROL_FLAG);

        if(pStr == NULL )
        {
            sysLoger(LINFO, "ControlSubAgent::handleCreateServer:lost of name part!");
            sendErrorAck(pack, ERR_SRV_CFG_ERR);
            return;
        }

        buf = QString::fromLocal8Bit(pBuf, pStr-pBuf);
        vec.append(buf);

        //extInfo
        if(pack->getSize() < (int)(EXT_INFO_LEN + (pStr-pBuf) +DIDLEN+2*CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN))
        {
            sysLoger(LINFO, "ControlSubAgent::handleCreateServer:lost of extInfo part!");
            sendErrorAck(pack, ERR_SRV_CFG_ERR);
            return;
        }

        pBuf = pStr + CONTROL_FLAG_LEN;
        buf = QString::fromLocal8Bit(pBuf, EXT_INFO_LEN);
        vec.append(buf);
        //others
        pBuf += EXT_INFO_LEN + CONTROL_FLAG_LEN;
        buf = QString::fromLocal8Bit(pBuf, pack->getSize() - (pBuf - pack->getData()));

        while(true)
        {
            str = buf.section(CONTROL_FLAG,next,next);

            if (str.size() == 0)
            {
                break;
            }

            vec.append(str);
            next++;
        }

        int iRet = 0;
        int iCount = 0;
        int aid = vec[iCount++].toInt(&ok, 16);    //aid��
        sysLoger(LINFO, "ControlSubAgent::handleModifyServer:Modified server [aid = %d] starting...", aid);
        AgentBase* pAgent = Manager::getInstance()->getServerAgentFromID(aid);

        if(NULL == pAgent)
        {
            sysLoger(LINFO, "ControlSubAgent::handleModifyServer:Didn't find the agent object by aid %d",
                     aid);
            sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
            return;
        }

        AgentWithQueue* pAgentWithQueue = dynamic_cast<AgentWithQueue*>(pAgent);

        if(NULL  == pAgentWithQueue)
        {
            sysLoger(LINFO, "ControlSubAgent::handleModifyServer:Dynamic cast pointer from class AgentBase"\
                     "to AgentWithQueue failure!");
            sendErrorAck(pack, ERR_SYSTEM_ERR);
            return;
        }

        switch (pAgentWithQueue->getType())
        {
        case AgentBase::SERVER_AGENT:
        {
            ServerAgent* pServerAgent = dynamic_cast<ServerAgent*>(pAgentWithQueue);

            if(NULL == pServerAgent)
            {
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer:Dynamic cast pointer from "\
                         "class AgentWithQueue to Class ServerAgent failure!");
                sendErrorAck(pack, ERR_SYSTEM_ERR);
                return;
            }

            //��ֹͣ�������������Ϣ���޸�
            if(pServerAgent->getState() == SASTATE_ACTIVE)
            {
                isSrvRunbfMod = true;
                pServerAgent->stop();
                pServerAgent->setState(SASTATE_INACTIVE);
            }

            //�Ƚ������Ƿ�ı�
            if((!vec[iCount].isEmpty()) && (pServerAgent->getName() != vec[iCount]))
            {
                QString oldSAName = pServerAgent->getName();
                pServerAgent->setName(vec[iCount]);

                //�޸�Manager��ID=��NAME��ӳ���
                Manager::getInstance()->modifyName2AIDMap(oldSAName, vec[iCount]);
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer: Modified ServerAgent name"\
                         " from %s to newName %s!",
                         oldSAName.toAscii().data(), vec[iCount].toAscii().data());

				//bug6420Ŀ�������Ϊ��ts��־�в���ͬ���������֣�����ǰ��log���ļ����б��ݣ�ͬʱ���õ�ǰlog�ļ�ָ��Ϊ��
				if(pServerAgent->getLogFlag()){
				
					PackLog* oldPackLog = pServerAgent->getPacketLogger();
					if( oldPackLog != NULL ){
						oldPackLog->close();
						oldPackLog->deleteLogFile();
						delete oldPackLog;
						pServerAgent->setPacketLogger(NULL);
						sysLoger(LINFO, "ControlSubAgent::handleModifyServer: Modified ServerAgent name,delete old Log %s",
							oldSAName.toAscii().data());
					}else{
					
					}


				}//end of if(pServerAgent->getLogFlag())
            }

            //extInfo
            iCount++;
            pServerAgent->setExtInfo(vec[iCount]);
            //��־��־�Ƿ��޸�
            iCount++;
            bool isLog = (bool)vec[iCount++].toInt(&ok, 16);        //��־��¼

            if(isLog != pServerAgent->getLogFlag())
            {
                bool oldLogFlag = pServerAgent->getLogFlag();
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer: Modified ServerAgent logflag"\
                         " from %d to  %d!", oldLogFlag, isLog);
            }
			//bug6420Ŀ�������Ϊ��ts��־�в���ͬ����������
			 pServerAgent->setLogFlag(isLog);
             pServerAgent->getConfig()->setLogFlag(isLog);
            //�õ�ͨ��������Ϣ
            int channelCount = vec[iCount++].toInt(&ok, 16);        //ͨ����
            int curChannel = vec[iCount++].toInt(&ok, 16);        //��ǰͨ��

            if (curChannel >= channelCount || channelCount < 0)
            {
                sysLoger(LWARNING, "ControlSubAgent::handleModifyServer: Modified info from TC is "\
                         " error, current channel is larger than channel count!");
                sendErrorAck(pack, ERR_SRV_CFG_ERR);
                return;
            }

            pServerAgent->clearChannel();

            for(int i=0; i<channelCount; ++i)
            {
                str.clear();
                PROTOCOL protocolType = (PROTOCOL)vec[iCount++].toInt(&ok, 16);    //Э������
                str += QString("%1").arg(protocolType, 0, 16 );
                str += CONTROL_FLAG;

                PROTOCOL way = (PROTOCOL)vec[iCount++].toInt(&ok, 16);    //ͨ�ŷ�ʽ
                str += QString("%1").arg(way, 0, 16 );
                str += CONTROL_FLAG;

                switch(way)
                {
                case TCP:    //tcp
                {
                    QString ip = vec[iCount++];
                    str += ip;
                    str += CONTROL_FLAG;
                    int port = vec[iCount++].toInt(&ok, 16);        //�˿ں�
                    str += QString("%1").arg(port, 0, 16 );
                    str += CONTROL_FLAG;
                    int timeout = vec[iCount++].toInt(&ok, 16);    //��ʱ
                    str += QString("%1").arg(timeout, 0, 16 );
                    str += CONTROL_FLAG;
                    int retry = vec[iCount++].toInt(&ok, 16);        //���Դ���
                    str += QString("%1").arg(retry, 0, 16 );
                    str += CONTROL_FLAG;

                    //���浱ǰ��ͨ��
                    if (i == curChannel )
                    {
                        //������ͨ�ŷ�ʽ
                        if (pServerAgent->getConfig()->getDeviceConfig()->getDeviceType() !=
                                DeviceConfigBase::TCP_DEVICE)
                        {
                            ServerConfig* pServerConfig = pServerAgent->getConfig();
                            pServerConfig->setDeviceConfig(
                                DeviceConfigBase::newDeviceConfig(DeviceConfigBase::TCP_DEVICE));
                            pServerConfig->setProtocolType(protocolType);
                        }

                        //tcp��������Ϣ
                        TcpConfig* pTcpConfig = dynamic_cast<TcpConfig*>(pServerAgent->getConfig()->getDeviceConfig());

                        pTcpConfig->setPort(port);        //���ö˿�
                        pTcpConfig->setIP(ip);    //����IP��ַ

                    }
                }
                break;

                case UDP:    //UDP
                {
                    QString ip = vec[iCount++];
                    str += ip;
                    str += CONTROL_FLAG;
                    int port = vec[iCount++].toInt(&ok, 16);        //�˿ں�
                    str += QString("%1").arg(port, 0, 16 );
                    str += CONTROL_FLAG;
                    int timeout = vec[iCount++].toInt(&ok, 16);    //��ʱ
                    str += QString("%1").arg(timeout, 0, 16 );
                    str += CONTROL_FLAG;
                    int retry = vec[iCount++].toInt(&ok, 16);        //���Դ���
                    str += QString("%1").arg(retry, 0, 16 );
                    str += CONTROL_FLAG;

                    if (i == curChannel)
                    {
                        //������ͨ�ŷ�ʽ
                        if (pServerAgent->getConfig()->getDeviceConfig()->getDeviceType() !=
                                DeviceConfigBase::UDP_DEVICE)
                        {
                            pServerAgent->getConfig()->setDeviceConfig(
                                DeviceConfigBase::newDeviceConfig(DeviceConfigBase::UDP_DEVICE));
                            pServerAgent->getConfig()->setProtocolType(protocolType);
                        }

                        //UDP��������Ϣ
                        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>\
                                                (pServerAgent->getConfig()->getDeviceConfig());
                        pUdpConfig->setRetry(retry);        //�������Դ���
                        pUdpConfig->setIP(ip);    //����IP��ַ
                        pUdpConfig->setPort(port);        //���ö˿�
                        pUdpConfig->setTimeout(timeout);        //���ó�ʱʱ��
                    }
                }
                break;

                case UART:    //����
                {
                    QString com = vec[iCount++];
                    str += com;
                    str += CONTROL_FLAG;
                    //��鴮��ͨ���Ƿ���Ч
                    char bufCom[200] = "\0";
                    int idx = 0;
                    string localCOMcfg;     //�������ڵ�����

                    if(queryHostSerialPort(bufCom, sizeof(bufCom)) > 0)
                    {
                        localCOMcfg = string(bufCom, strlen(bufCom));
                        idx = localCOMcfg.find(com.toAscii().data());

                        if(idx == string::npos)
                        {
                            //TS����û�иô�������
                            sysLoger(LINFO, "ControlSubAgent::handleCreateServer:There is no COM port %s"\
                                     " in local machine!", com.toAscii().data());
                            //�ظ�
                            sendErrorAck(pack, ERR_INVALID_UART_PORT);
                            return;
                        }
                    }

                    int port = vec[iCount++].toInt(&ok, 16);        //�˿ں�
                    str += QString("%1").arg(port, 0, 16 );
                    str += CONTROL_FLAG;
                    int timeout = vec[iCount++].toInt(&ok, 16);    //��ʱ
                    str += QString("%1").arg(timeout, 0, 16 );
                    str += CONTROL_FLAG;
                    int retry = vec[iCount++].toInt(&ok, 16);        //���Դ���
                    str += QString("%1").arg(retry, 0, 16 );
                    str += CONTROL_FLAG;

                    if (i==curChannel)
                    {
                        //������ͨ�ŷ�ʽ
                        if (pServerAgent->getConfig()->getDeviceConfig()->getDeviceType() !=
                                DeviceConfigBase::UART_DEVICE)
                        {
                            pServerAgent->getConfig()->setDeviceConfig(
                                DeviceConfigBase::newDeviceConfig(DeviceConfigBase::UART_DEVICE));
                            pServerAgent->getConfig()->setProtocolType(protocolType);
                        }

                        //UART��������Ϣ
                        UartConfig* pUartConfig = dynamic_cast<UartConfig*>\
                                                  (pServerAgent->getConfig()->getDeviceConfig());
                        pUartConfig->setRetry(retry);        //�����شδ���
                        pUartConfig->setTimeout(timeout);        //���ó�ʱʱ��

                        pUartConfig->setBaud(port);        //���ò�����
                        pUartConfig->setCom(com);    //���ô��ڶ˿�
                    }
                }
                break;

                default:
                    sysLoger(LWARNING, "ControlSubAgent::handleModifyServer: Invalid channel "\
                             " configuration, the communication way is %d", way);
                    sendErrorAck(pack, ERR_SRV_CFG_ERR);
                    return;
                    break;
                }

                pServerAgent->addChannel(str);
            }

            pServerAgent->setChannelType(curChannel);

            if (isSrvRunbfMod)
            {
                if (pServerAgent->run())
                {
                    //����sa����,
                    pServerAgent->setState(SASTATE_ACTIVE);    //����sa��״̬Ϊ����״̬
                }

                else
                {
                    sysLoger(LWARNING, "ControlSubAgent::handleModifyServer: "\
                             "Resume the serviceAgent failure after modified!");
                }
            }
        }
        break;

        case AgentBase::SERVER_AGENT_WITH_DLL:
        {
            ServerAgentWithDll* pServerAgentWithDll = dynamic_cast\
                    <ServerAgentWithDll*>(pAgentWithQueue);

            if(NULL == pServerAgentWithDll)
            {
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer:Dynamic cast pointer from "\
                         "class AgentWithQueue to Class ServerAgentWithDll failure!");
                sendErrorAck(pack, ERR_SYSTEM_ERR);
                return;
            }

            QString name = vec[iCount];
            iCount++;
            //extInfo
            pServerAgentWithDll->setExtInfo(vec[iCount]);
            iCount++;
            //logFlag
            bool logFlag = (bool)vec[iCount++].toInt(&ok, 16);        //��־
            PROTOCOL way = (PROTOCOL)vec[iCount].toInt(&ok, 16);

            if(way != DLL)
            {
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer:Communication way %d for Dll "\
                         "Agent is invalid", way);
                sendErrorAck(pack, ERR_SRV_CFG_ERR);
                return;
            }

            //��ֹͣ����
            if(pServerAgentWithDll->getState() == SASTATE_ACTIVE)
            {
                isSrvRunbfMod = true;
                pServerAgentWithDll->stop();
                pServerAgentWithDll->setState(SASTATE_INACTIVE);
                Router::getInstance()->unRegisterAgent(pServerAgentWithDll->getAID());    //ɾ����router��ע��
            }

            if ((!name.isEmpty()) && (name != pServerAgentWithDll->getName()))
            {
                QString oldName = pServerAgentWithDll->getName();
                Manager::getInstance()->modifyName2AIDMap(oldName, name);
                pServerAgentWithDll->setName(name);
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer:Had been modified AgentwithDll"\
                         " object's name from %s to %s",
                         oldName.toAscii().data(), name.toAscii().data());
            }

            if(logFlag != pServerAgentWithDll->getLogFlag())
            {
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer:Had been modified AgentwithDll"\
                         " object's name from %d to %d",
                         pServerAgentWithDll->getLogFlag(), logFlag);
                pServerAgentWithDll->setLogFlag(logFlag);                //��־
            }

            //buf ��|isLog|, |way|, |DllBuf|,
            int lIdx = 0;
            int rLen = 0;
            lIdx += vec[iCount -1].length() + CONTROL_FLAG_LEN;    //isLog
            lIdx += vec[iCount].length() + CONTROL_FLAG_LEN;          //way
            rLen = max(0, buf.length() - (lIdx+CONTROL_FLAG_LEN));
            QString dllInbuf = buf.mid(lIdx, rLen);
            sysLoger(LDEBUG, "handleModifyServer:Dll [%d] set inBuf:%s", aid, dllInbuf.toAscii().data());
            pServerAgentWithDll->setInBuf(dllInbuf);    //���洫�ݵĲ���,�������ݸ���̬����ַ���

            if (isSrvRunbfMod)
            {
                if (pServerAgentWithDll->run())
                {
                    //����sa����,
                    pServerAgentWithDll->setState(SASTATE_ACTIVE);    //����sa��״̬Ϊ����״̬
                }

                else
                {
                    sysLoger(LWARNING, "ControlSubAgent::handleModifyServer: "\
                             "Resume the serviceAgent failure after modified!");
                }
            }
        }
        break;
        default:
            sysLoger(LWARNING, "ControlSubAgent::handleModifyServer: "\
                     "Tried to modify other type %d of agent object, failure!",
                     pAgentWithQueue->getType());
            sendErrorAck(pack, ERR_INVALID_SERVICE_ID);
            return;
            break;
        }

        //�ظ��ɹ�����
        sendSuccessPacket(pack);

        //�洢������Ϣ
        if(pAgentWithQueue->getRegisterType() != INVALID_REG_TYPE)
        {
            Manager::getInstance()->storeConfigure();
        }

        //�����ϱ�
        Manager::getInstance()->sendSAReportToCA(pAgentWithQueue);
        return;
    }

    catch(...)
    {
        sysLoger(LERROR, "ControlSubAgent::handleQueryServer: catch an exception!Query server failure!");
        sendErrorAck(pack, ERR_SYSTEM_ERR);
    }

    return;
}

/**
* ���ܣ�����TClient_register����, TS����ע��, ע���, TS����TC�Զ�������Ϣ��״̬
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::handleRegisterServer(Packet* pack)
{
    int regType = 0;
    US caAid = 0;
    int ret = TS_SUCCESS;

    /**
    TC�Ŀ�������" DID, EregType,"
    */
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleRegisterServer: Parameter pack is null pointer");
        return;
    }

    //�õ�ע��aid��
    char* pBuf = pack->getData()+ DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
    hex2int(&pBuf, &regType);

    //CA��aid
    caAid = pack->getSrc_aid();

    //�������Ҫ�ظ���Ϣ
    ret = Manager::getInstance()->registerServer(pack, regType);

    if (ret == TS_SUCCESS)
    {
        sysLoger(LINFO, "ControlSubAgent::handleRegisterServer: Regisgered CA [aid = %d] to type"\
                 " %d successfully!", pack->getSrc_aid(), regType);
        //sendSuccessPacket(pack);
        //���и����͵��ϱ�
        Manager::getInstance()->sendRegTypeSAToCA(regType, caAid);
    }

    else
    {
        sysLoger(LINFO, "ControlSubAgent::handleRegisterServer: Regisgered CA [aid = %d] to type"\
                 " %d failure!", pack->getSrc_aid(), regType);
    }

    PacketManager::getInstance()->free(pack);

    return;

}
/**
* ���ܣ�����TClient_unregister����, ע���ỰID, �ر�SOCKET
* @param pack ���յĿ��ư�
* @param offset ����ƫ��λ��
*/
void ControlSubAgent::handleUnregisterServer(Packet* pack)
{
    int regType = 0;
    int ret = TS_SUCCESS;

    /**
    TC�Ŀ�������" DID, UregType,"
    */
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleUnregisterServer: Parameter pack is null pointer");
        return;
    }

    //�õ�ע������
    char* pBuf = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
    hex2int(&pBuf, &regType);

    ret = Manager::getInstance()->unRegisterServer(pack, regType);

    if (ret == TS_SUCCESS)
    {
        sysLoger(LINFO, "ControlSubAgent::handleUnregisterServer: Unregisgered CA [aid = %d] from type"\
                 " %d successfully!", pack->getSrc_aid(), regType);
    }

    else
    {
        sysLoger(LINFO, "ControlSubAgent::handleUnregisterServer: Unregisgered CA [aid = %d] from type"\
                 " %d failure!", pack->getSrc_aid(), regType);
    }

    PacketManager::getInstance()->free(pack);

    return;
}

/**
* ���ܣ�����TClient_setRegisterType����, ����sa������, �������ϱ�
* @param pack ���յĿ��ư�
��ʽ��aid��, ����
*/
void ControlSubAgent::handleSetRegisterType(Packet* pack)
{
    /**
      TC���͵Ŀ��������: " DID, s Aid, regType,"
    */
    bool blRet = false;
    int aid = 0;
    int regType = 0;

    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetRegisterType: Parameter pack is null pointer");
        return;
    }

    //ָ��AID��ַ
    char* pBuf = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
    hex2int(&pBuf, &aid);
    pBuf += CONTROL_FLAG_LEN;
    hex2int(&pBuf, &regType);

    //���Ҷ�Ӧ�ķ���������ע��
    AgentBase *pAgent = Manager::getInstance()->getServerAgentFromID((short)(aid));

    if(NULL == pAgent)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetRegisterType: Server Agent [aid = %d] is absent",
                 aid);
        sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
        return;
    }

    //�������Ƿ��Ƿ�SA����(CA)
    if((pAgent->getType() != AgentBase::SERVER_AGENT)
            && (pAgent->getType() != AgentBase::SERVER_AGENT_WITH_DLL))
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetRegisterType: Server Agent [aid = %d] is "\
                 "not a valid target service agent!", aid);
        sendErrorAck(pack, ERR_INVALID_SERVICE_ID);
        return;
    }

    AgentWithQueue *pAgentQ = dynamic_cast<AgentWithQueue*>
                              (pAgent);

    if(NULL == pAgentQ)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetRegisterType: Server Agent [aid = %d] is absent",
                 aid);
        sendErrorAck(pack, ERR_SYSTEM_ERR);
        return;
    }

    pAgentQ->setRegisterType((UC)(regType));

    sysLoger(LINFO, "ControlSubAgent::handleSetRegisterType:Set server [aid = %d] to RegType %d..",
             aid, regType);
    //���лظ�
    sendSuccessPacket(pack);
    //�����ϱ�
    Manager::getInstance()->sendSAReportToCA(pAgentQ);
    //�洢������Ϣ
    Manager::getInstance()->storeConfigure();
    return;
}

/**
* ���ܣ�����TClient_clearRegisterType����, ���sa��ע������, �������ϱ�
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::handleClearRegisterType(Packet* pack)
{
    char *pBuf = NULL;
    char *pStr = NULL;
    AgentBase *pAgent = NULL;
    AgentWithQueue *pAgentQ= NULL;
    US aid = 0;
    US regType = 0;

    /**
     TC���͹����Ŀ��ư���"DID, caid, regType"
    */
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleClearRegisterType: Parameter pack is null pointer");
        return;
    }

    //ָ��AID��ַ
    pBuf = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;

    //aid
    pStr = strstr(pBuf, CONTROL_FLAG);

    if(NULL == pStr)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleClearRegisterType: Unknow format packet!");
        sendErrorAck(pack, ERR_UNKNOW_DATA);
        return;
    }

    hex2int(&pBuf, (int*)&aid);

    //regType
    pStr = strstr(pBuf, CONTROL_FLAG);

    if(NULL == pStr)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleClearRegisterType: Unknow format packet!");
        sendErrorAck(pack, ERR_UNKNOW_DATA);
        return;
    }

    hex2int(&pBuf, (int*)&regType);

    //����aid��Ӧ��SA
    pAgent = Manager::getInstance()->getServerAgentFromID(aid);

    if(NULL == pAgent)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleClearRegisterType: ServerAgent [aid = %d]"\
                 " is absent!", aid);
        sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
        return;
    }

    pAgentQ = dynamic_cast<AgentWithQueue*>(pAgent);

    if(NULL == pAgentQ)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleClearRegisterType: Dynamic cast pointer "\
                 " of Agent class to AgentWithQueue Class failure!");
        sendErrorAck(pack, ERR_SYSTEM_ERR);
        return;
    }

    if((pAgentQ->getType() != AgentBase::SERVER_AGENT)
            && (pAgentQ->getType() != AgentBase::SERVER_AGENT_WITH_DLL))
    {
        //�ظ�
        sendErrorAck(pack, ERR_INVALID_SERVICE_ID);
        sysLoger(LINFO, "ControlSubAgent::handleClearRegisterType: Invalid serverAgent type %d,"\
                 " the aid %d", pAgentQ->getType(), aid);
        return;
    }

    //�ظ�ȡ��ע��ɹ�
    pAgentQ->setRegisterType(INVALID_REG_TYPE);
    sysLoger(LINFO, "ControlSubAgent::handleClearRegisterType:Set the SA [%s] to NULL register "\
             "type successfully!", pAgentQ->getName().toAscii().data());
    sendSuccessPacket(pack);
    //�洢������Ϣ
    Manager::getInstance()->storeConfigure();
    return;
}

/**
* ���ܣ�����TClient���͹�����GetID��������, ���ѵõ���AID���ظ�TClient
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::handleGetID(Packet* pack)
{
    char *pBuf = NULL;
    char *pStr = NULL;
    string name;
    int aid = 0;

    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleGetID:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    /**
    TC�������Ŀ�����Ϣ: "DID, Iname,"
    */
    pBuf = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;

    pStr = strstr(pBuf, CONTROL_FLAG);

    if(NULL == pStr)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleGetID: Unknow format of pack.");
        sendErrorAck(pack, ERR_UNKNOW_DATA);
        return;
    }

    name = string(pBuf, pStr-pBuf);

    aid = Manager::getInstance()->getIDFromName(QString::fromStdString(name));        //ͨ�����ֵõ�ID

    if (aid != -1)
    {
        setSuccessPacket(pack);
        pBuf = pack->getData() + pack->getSize();
        pBuf += int2hex(pBuf, 10, aid);
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        pack->setSize(pBuf-pack->getData());//���°��Ĵ�С
    }

    else
    {
        sysLoger(LINFO, "ControlSubAgent::handleGetID: It's failure to find the ID by name :%s",
                 name.data());
        sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
        return;
    }

    sysLoger(LINFO, "ControlSubAgent::handleGetID:Query ID by SA name [%s] successfully!",
             name.data());

    //���ͻظ�������¼��־
    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    Router::getInstance()->putpkt(pack);
    return;
}

/******************************TS 3.2 ������*****************************/

/**
 * @Funcname:  handleSetSystemLogLevel
 * @brief        :  ����ϵͳ��־���������
 * @para1      : pack ���յĿ��ư�
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��1��10�� 14:46:06
 *
**/
void ControlSubAgent::handleSetSystemLogLevel(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetSystemLogLevel:pack is NULL"\
                 " pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    /**
    TC�������Ŀ�����Ϣ: "DID, LlogLevel,"
    */
    char *pDest = NULL;
    char *pStr = NULL;
    int iLogLevel = 0;
    pDest = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
    pStr = strstr(pDest, CONTROL_FLAG);

    if(NULL == pStr)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetSystemLogLevel:Unknow packet!"\
                 " Lost CONTROL_FLAG after logLevel!");
        sendErrorAck(pack, ERR_UNKNOW_DATA);
        return;
    }

    //����AID
    hex2int(&pDest, &iLogLevel);
    LogLevel level= ((LDEBUG <= iLogLevel) && (iLogLevel <= LFATAL))?(LogLevel)(iLogLevel):LBUTT;
    SystemLog::getInstance()->setLogLevel(level);
    //�洢������Ϣ
    Manager::getInstance()->storeConfigure();
    //����Ҫ�ظ�
    PacketManager::getInstance()->free(pack);
    sysLoger(LWARNING, "ControlSubAgent::handleSetSystemLogLevel:Success to set system log "\
             "output levle as %d.", level);
    return;
}

/**
 * @Funcname:  handleSetAgentLogFlag
 * @brief        :  ���÷���������־���
 * @para1      : pack ���յĿ��ư�
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��1��10�� 14:51:34
 *
**/
void ControlSubAgent::handleSetAgentLogFlag(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetAgentLogFlag:pack is NULL"\
                 " pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    /**
    TC�������Ŀ�����Ϣ: "DID, laid, logflag,"
    */
    char *pDest = NULL;
    char *pStr = NULL;
    AgentBase *pAgent = NULL;
    AgentWithQueue *pAgentQ = NULL;
    int aid = 0;
    int iflag = 0;
    bool flag = false;

    //���а��Ľ���
    pDest = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
    pStr = strstr(pDest, CONTROL_FLAG);

    if(NULL == pStr)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetAgentLogFlag:Unknow packet!"\
                 " Lost CONTROL_FLAG after AID!");
        sendErrorAck(pack, ERR_UNKNOW_DATA);
        return;
    }

    //����AID
    hex2int(&pDest, &aid);

    //����FLAG
    pDest = pStr + CONTROL_FLAG_LEN;
    pStr = strstr(pDest, CONTROL_FLAG);

    if(NULL == pStr)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetAgentLogFlag:Unknow packet!"\
                 " Lost CONTROL_FLAG after log flag!");
        sendErrorAck(pack, ERR_UNKNOW_DATA);
        return;
    }

    hex2int(&pDest, &iflag);
    flag = (iflag == 0) ? false:true;

    //�ҵ��������
    pAgent = Manager::getInstance()->getServerAgentFromID((US)aid);

    if(NULL == pAgent)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetAgentLogFlag: didn't find the"\
                 " server agent by aid %d", aid);
        sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
        return;
    }

    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [begin]*/
    /*  Modified brief: ���Ӽ���������*/
    if(AgentBase::CLIENT_AGENT == pAgent->getType())
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetAgentLogFlag: this is client agent"\
                 " by aid %d", aid);
        sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
        return;
    }

    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [end]*/
    pAgentQ = dynamic_cast<AgentWithQueue*>(pAgent);

    if(NULL == pAgentQ)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetAgentLogFlag: Dynamic cast "\
                 " point of class Agent to AgentWithQueue failure!");
        sendErrorAck(pack, ERR_SYSTEM_ERR);
        return;
    }

    pAgentQ->setLogFlag(flag);
    sysLoger(LDEBUG, "ControlSubAgent::handleSetAgentLogFlag:Success to set SA [aid = %d] "\
             " log flag as %d!", aid, flag);
    sendSuccessPacket(pack);

    //�洢������Ϣ
    if(pAgentQ->getRegisterType() != INVALID_REG_TYPE)
    {
        Manager::getInstance()->storeConfigure();
    }

    return;
}

/**
 * @Funcname:  handleQueryHostSerialPort
 * @brief        :  �����ѯTS�����������õ�����
 * @para1      : pack ���յĿ��ư�
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��1��10�� 15:01:46
 *
**/
void ControlSubAgent::handleQueryHostSerialPort(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleQueryHostSerialPort:pack is NULL"\
                 " pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    /**
    TC�������Ŀ�����Ϣ: "DID, p,"
    */
    /*ͨ����ѯע������HKEY_LOCAL_MACHINE�����
    HARDWARE\\DEVICEMAP\\SERIALCOMM��������ѯ��������
    */
    sysLoger(LDEBUG, "ControlSubAgent::handleQueryHostSerialPort:Enter this function!");

    char buf[1024] = "\0";
    int ret = 0;
    ret = queryHostSerialPort(buf, sizeof(buf));

    if(ret < 0)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleQueryHostSerialPort:Query COM config failure!");
        sendErrorAck(pack, ERR_SYSTEM_ERR);
        return;
    }

    if(ret == 0)
    {
        sysLoger(LINFO, "ControlSubAgent::handleQueryHostSerialPort:No COM port in the host!");
        sendSuccessPacket(pack);
        return;
    }

    if(ret + pack->getSize() < PACKET_DATASIZE)
    {
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [begin]*/
        /*  Modified brief: �����ù��ܰ�ͷ��������ѭ����,�������ʱ�������ν���AID*/
        setSuccessPacket(pack);
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [end]*/
        memcpy(pack->getData() + pack->getSize(), buf, ret);
        pack->setSize(pack->getSize() + ret);

        if(Manager::getInstance()->getLogFlag())
        {
            packLoger.write(SEND_PACK, *pack);
        }

        Router::getInstance()->putpkt(pack);
        sysLoger(LINFO, "ControlSubAgent::handleQueryHostSerialPort:Query COM config result:%s!",
                 buf);
        return;
    }

    sysLoger(LWARNING, "ControlSubAgent::handleQueryHostSerialPort:Packet data size is too little for "\
             " COM!config data : %s!", buf);
    sendErrorAck(pack, ERR_SYSTEM_ERR);
    return;
}

/**
 * @Funcname:  handleCloseClient
 * @brief        :  ����ͻ��˹ر���������,TS��Ҫ���ϵͳ����֮��ص�����
 * @para1      : pack ���յĿ��ư�
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��1��11�� 11:29:26
 *
**/
void ControlSubAgent::handleCloseClient(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleCloseClient:pack is NULL"\
                 " pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    //ɾ���ÿͻ��˴�������
    Manager::getInstance()->deleteServer(pack->getSrc_aid());
    Router::getInstance()->unRegisterAgent(pack->getSrc_aid());
    sysLoger(LINFO, "ControlSubAgent::handleCloseClient:Delete client agent %d !"\
             "FILE:%s, LINE:%d",  pack->getSrc_aid(),
             __MYFILE__, __LINE__);
    PacketManager::getInstance()->free(pack);
    return;
}

/**
 * @Funcname:  handleUpLoadFile
 * @brief        :  ����TSAPI�ϴ��ļ�
 * @para1      : pack�����
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��3��2�� 18:00:06
 * @ command pack format:
 *    "DID, Ffilename, aid, isReplace, isComplete, curlen, content"
**/
void ControlSubAgent::handleUpLoadFile(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleUpLoadFile: Paramenter pack is invalid pointer!");
        return;
    }

    try
    {
        char cFileName[MAX_PATH] = "\0";              //�ļ���
        int isComplete = TS_FILE_BEGIN;                              //�Ƿ������
        int isReplace = 0;                                //�Ƿ���и���(������ͬ���ļ�)
        int   aid = 0;
        int curLen = 0;
        char *pDest = NULL;
        char *pStr = NULL;
        string sFn;

        sysLoger(LDEBUG, "ControlSubAgent::handleUpLoadFile: Recieved the request for uploading file!");

        //����DID�������ֲ���
        pStr = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;

        //filename
        pDest = strstr(pStr, CONTROL_FLAG);

        if(NULL == pDest)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleUpLoadFile: Lost the filename part, invalid packet!");
            sendErrorAck(pack, ERR_UNKNOW_DATA);
            return;
        }

        memcpy(cFileName, pStr, (UI)(pDest - pStr));

        //isReplace
        pStr = pDest + CONTROL_FLAG_LEN;
        pDest = strstr(pStr, CONTROL_FLAG);

        if(NULL == pDest)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleUpLoadFile: Lost the isReplace part, invalid packet!");
            sendErrorAck(pack, ERR_UNKNOW_DATA);
            return;
        }

        hex2int(&pStr, &isReplace);
        //isComplete
        pStr = pDest + CONTROL_FLAG_LEN;
        pDest = strstr(pStr, CONTROL_FLAG);

        if(NULL == pDest)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleUpLoadFile: Lost the isComplete part, invalid packet!");
            sendErrorAck(pack, ERR_UNKNOW_DATA);
            return;
        }

        hex2int(&pStr, &isComplete);
        //curlen
        pStr = pDest + CONTROL_FLAG_LEN;
        pDest = strstr(pStr, CONTROL_FLAG);

        if(NULL == pDest)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleUpLoadFile: Lost the isComplete part, invalid packet!");
            sendErrorAck(pack, ERR_UNKNOW_DATA);
            return;
        }

        hex2int(&pStr, &curLen);

        //��װ�ļ���(���·��)
        sFn.clear();
        sFn = Manager::tmpfilepath + truncatPathFromFilename(cFileName);
        //ȫ·��
        sFn = sFn.insert(0, Manager::getInstance()->getAppDir().toAscii().data());
        //data
        pStr = pDest+CONTROL_FLAG_LEN;
        char *fData = (char *)malloc(curLen);

        if(NULL == fData)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleUpLoadFile: Alloc spool for file content failure!");
            sendErrorAck(pack, ERR_SYSTEM_ERR);
            return;
        }

        memcpy(fData, pStr, (UI)(pack->getData() + pack->getSize() - pStr));

        FILE *pfile = NULL;

        if((isComplete == TS_FILE_BEGIN) ||(isComplete == TS_FILE_BEGIN_END))
        {
            if(isReplace == 0)       //don't replace the exits file
            {
                pfile = fopen(sFn.data(), "rb");

                if(pfile != NULL)
                {
                    sysLoger(LINFO, "ControlSubAgent::handleUpLoadFile:File [%s] already exits!",
                             sFn.data());
                    sendErrorAck(pack, ERR_FILE_ALREADY_EXIT);
                    fclose(pfile);
                    pfile = NULL;
                    free(fData);
                    return;
                }
            }

            //open the file by binary mode
            pfile = fopen(sFn.data(), "wb");
            sysLoger(LINFO, "ControlSubAgent::handleUpLoadFile: Open new file [%s] for write!",
                     sFn.data());
            //insert the file and name to mapName2File
            Manager::getInstance()->insertFileToMap(sFn, pfile);
        }

        else
        {
            pfile = Manager::getInstance()->getFileFromName(sFn);
        }

        /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [begin]*/
        /*  Modified brief: ���ӳ����쳣�Ͳ�������ͷ����ݰ�*/
        if(NULL == pfile)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleUpLoadFile: open file failure!");

            if(isComplete == TS_FILE_END)
            {
                sendErrorAck(pack, ERR_SYSTEM_ERR);
            }

            else
            {
                PacketManager::getInstance()->free(pack);
            }

            free(fData);
            return;
        }

        fwrite(fData, sizeof(char), curLen, pfile);
        fflush(pfile);

        //��ʼд�ĵ�һ���������һ�������ͻظ�
        if((isComplete == TS_FILE_END) || (isComplete == TS_FILE_BEGIN_END))
        {
            sendSuccessPacket(pack);
            Manager::getInstance()->removeAndCloseFileFromMap(sFn);
        }

        else
        {
            PacketManager::getInstance()->free(pack);
        }

        free(fData);
        return;
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [end]*/
    }

    catch(...)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleUpLoadFile: catch an exception!");
    }

    return;
}

/**
 * @Funcname:  handleDownLoadFile
 * @brief        :  ����TSAPI�����ļ�
 * @para1      : pack�����
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��3��2�� 18:00:06
 *
**/
void ControlSubAgent::handleDownLoadFile(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleDownLoadFile: Paramenter pack is invalid pointer!");
        return;
    }

    try
    {
        char cFileName[MAX_PATH] = "\0";              //�ļ���
        int isComplete = TS_FILE_BEGIN;                              //�Ƿ������
        int isReplace = 0;                                //�Ƿ���и���(������ͬ���ļ�)
        int curLen = 0, fnIdx = 0;
        char *pDest = NULL;
        char *pStr = NULL;
        char *pBuf = NULL;
        string sFn;

        sysLoger(LDEBUG, "ControlSubAgent::handleDownLoadFile: Recieved the request for uploading file!");

        //����DID�������ֲ���
        pStr = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;

        //filename
        pDest = strstr(pStr, CONTROL_FLAG);

        if(NULL == pDest)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleDownLoadFile: Lost the filename part, invalid packet!");
            sendErrorAck(pack, ERR_UNKNOW_DATA);
            return;
        }

        memcpy(cFileName, pStr, (UI)(pDest - pStr));

        //��װ�ļ���
        sFn.clear();
        sFn = Manager::tmpfilepath + truncatPathFromFilename(cFileName);

        //ȫ·��
        sFn = sFn.insert(0, Manager::getInstance()->getAppDir().toAscii().data());

        //open the file
        FILE *pfile = fopen(sFn.data(), "rb");

        if(NULL == pfile)
        {
            sysLoger(LDEBUG, "ControlSubAgent::handleDownLoadFile:Open file %s failure!", sFn.data());
            sendErrorAck(pack, ERR_FILE_NOT_EXIST);
            return;
        }

        /*Modified by tangxp for BUG NO.0002548 on 2008.1.16 [begin]*/
        /*  Modified brief: �������óɹ�����Ϣ*/
        //OK
        pBuf = pack->getData() + DIDLEN +CONTROL_FLAG_LEN;
        memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
        pBuf += SUCCESS_ACK_LEN;
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        /*Modified by tangxp for BUG NO.0002548 on 2008.1.16 [end]*/

        //store the filename
        memcpy(pBuf, cFileName, strlen(cFileName));
        pBuf += strlen(cFileName);
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        //save the aid part idx
        fnIdx = pBuf - pack->getData();

        char *fBuf = (char*)malloc(FILEBUFSIZE);

        if(NULL == fBuf)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleDownLoadFile:Alloc buffer for read file failure!");
            sendErrorAck(pack, ERR_SYSTEM_ERR);
            fclose(pfile);
            return;
        }

        int icount = 0;      //���ڷ��ͼ���

        //�ı�ظ���ͷ
        changePacketID(pack);

        while(!feof(pfile))
        {
            pBuf = pack->getData() + fnIdx;

            if(pBuf - pack->getData() < PACKET_DATASIZE)
            {
                curLen = fread(fBuf, sizeof(char), FILEBUFSIZE, pfile);
            }

            else
            {
                curLen = fread(fBuf, sizeof(char), (PACKET_DATASIZE - (pBuf - pack->getData())), pfile);
            }

            if(0 == icount)
            {
                if(feof(pfile))
                {
                    isComplete = TS_FILE_BEGIN_END;
                }

                else
                {
                    isComplete = TS_FILE_BEGIN;
                }
            }

            else
            {
                if(feof(pfile))
                {
                    isComplete = TS_FILE_END;
                }

                else
                {
                    isComplete = TS_FILE_CONTINUE;
                }
            }

            pBuf += int2hex(pBuf, 10, isComplete);
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            pBuf += int2hex(pBuf, 10, curLen);
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
            memcpy(pBuf, fBuf, curLen);
            pBuf += curLen;
            //���ð���С
            pack->setSize(pBuf - pack->getData());                //���°��Ĵ�С

            if(Manager::getInstance()->getLogFlag())
            {
                packLoger.write(SEND_PACK, *pack);
            }

            /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [begin]*/
            /*  Modified brief: ѭ������ʱ,���ͷ����ݰ�,���������,���ͷ�*/
            //���Ͱ�
            Router::getInstance()->putpkt(pack, false);
            icount++;
        }

        PacketManager::getInstance()->free(pack);
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [end]*/
        fclose(pfile);
        /*Modified by tangxp for BUG NO.0002204 on 2008.1.15 [begin]*/
        /*  Modified brief: �ͷŶ�̬�����ڴ�*/
        free(fBuf);
        fBuf = NULL;
        /*Modified by tangxp for BUG NO.0002204 on 2008.1.15 [end]*/
    }

    catch(...)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleDownLoadFile: catch an exception!");
    }

    return;
}

/**
 * @Funcname:  handleDeleteFile
 * @brief        :  ����ɾ���ļ�����
 * @para1      : pack�����
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��3��12�� 11:31:31
 *
**/
void ControlSubAgent::handleDeleteFile(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleDeleteFile: Paramenter pack is invalid pointer!");
        return;
    }

    try
    {
        char cFileName[MAX_PATH] = "\0";              //�ļ���
        char *pDest = NULL;
        char *pStr = NULL;
        char *pBuf = NULL;
        string sFn;
        bool bret = false;

        sysLoger(LDEBUG, "ControlSubAgent::handleDeleteFile: Recieved the request for uploading file!");

        //����DID�������ֲ���
        pStr = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;

        //filename
        pDest = strstr(pStr, CONTROL_FLAG);

        if(NULL == pDest)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleDeleteFile: Lost the filename part, invalid packet!");
            sendErrorAck(pack, ERR_UNKNOW_DATA);
            return;
        }

        memcpy(cFileName, pStr, (UI)(pDest - pStr));

        //��װ�ļ���
        sFn.clear();
        sFn = Manager::tmpfilepath + truncatPathFromFilename(cFileName);
        //ȫ·��
        sFn = sFn.insert(0, Manager::getInstance()->getAppDir().toAscii().data());
        //ɾ���ļ�
        bret = QFile::remove(QString::fromStdString(sFn));

        if(bret)
        {
            sendSuccessPacket(pack);
        }

        else
        {
            sendErrorAck(pack, ERR_SYSTEM_ERR);
        }

        return;
    }

    catch(...)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleDeleteFile: catch an exception!");
    }

    return;
}

/**
* ���ܣ���������Ŀ��SAID��AID
* @param pack ���յĿ��ư�
*/
void ControlSubAgent::changePacketID(Packet* pack)
{
    int src_aid = pack->getSrc_aid();    //�õ����ư�ԴID��
    int des_aid = pack->getDes_aid();
    int src_said = pack->getSrc_said();    //�õ����ư�ԴID��
    int des_said = pack->getDes_said();
    pack->setDes_aid(src_aid);
    pack->setSrc_aid(des_aid);
    pack->setDes_said(src_said);
    pack->setSrc_said(des_said);
}
/**
* ���ܣ����ô������Ϣ
* @param pack Ҫ���͵����ݰ�
* @param srtErrMsg ��Ҫ�ظ���ʧ��ֵ
* @param size ʧ����Ϣ���ֽڴ�С
*/
void ControlSubAgent::sendErrorAck(Packet* pack, char *strErrMsg, UI size)
{
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [begin]*/
    /*  Modified brief: ���ӳ����쳣ʱ,�ͷ����ݰ�*/
    if(NULL == pack || NULL == strErrMsg)
    {
        sysLoger(LWARNING, "ControlSubAgent::sendErrorAck: NULL pointer encountered!");

        if(NULL != pack)
        {
            PacketManager::getInstance()->free(pack);
        }

        return;
    }

    //����ԴAID��Ŀ��AID��
    changePacketID(pack);
    pack->clearData();
    char *pBuf = pack->getData();
    __store(Manager::getInstance()->getCurDID(pack->getDes_aid()), pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    if(PACKET_DATASIZE - (pBuf - pack->getData()) < (int)size)
    {
        sysLoger(LWARNING, "ControlSubAgent::sendErrorAck: strErrMsg is too large for the pack!");
        PacketManager::getInstance()->free(pack);
        return;
    }

    /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [end]*/
    memcpy(pBuf, strErrMsg, size);
    pBuf += size;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    //���ð���С
    pack->setSize(pBuf - pack->getData());                //���°��Ĵ�С

    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    //���Ͱ�
    Router::getInstance()->putpkt(pack);
    return;
}

/**
* ���ܣ����óɹ�����Ϣ
* @param pack Ҫ���͵����ݰ�
*/
void ControlSubAgent::setSuccessPacket(Packet* pack)
{
    //����ԴAID��Ŀ��AID��
    changePacketID(pack);
    pack->clearData();
    //����DID
    char *pBuf = pack->getData();
    __store(CURDID(pack->getDes_aid()), pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
    pBuf += SUCCESS_ACK_LEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    pack->setSize((int)(pBuf - pack->getData()));
    return;
}

/**
 * @Funcname:  sendSuccessPacket
 * @brief        :  ��ͻ��˻ظ������ɹ�
 * @param     : pack ԭ��Ϣ��,����Ŀ�ĺ�ԭaid
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��1��10�� 11:31:11
 *
**/
void ControlSubAgent::sendSuccessPacket(Packet *pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::sendSuccessPacket: NULL pointer encountered!");
        return;
    }

    //����ԴAID��Ŀ��AID��
    changePacketID(pack);
    pack->clearData();
    char *pBuf = pack->getData();
    __store(CURDID(pack->getDes_aid()), pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
    pBuf += SUCCESS_ACK_LEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;

    //���ð���С
    pack->setSize(pBuf - pack->getData());                //���°��Ĵ�С

    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    //���Ͱ�
    Router::getInstance()->putpkt(pack);
    return;
}

/**
 * @Funcname:  queryHostSerialPort
 * @brief        : ��ѯTS����������������
 * @para1      :  pBuf ���ղ�ѯ���ݵĻ���
 * @para2      :  bufsize: ������ֽڴ�С
 * @return      :  ��ѯ�ɹ��򷵻��ֽڳ��ȣ����򷵻�-1
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007��1��10�� 11:57:38
 *
**/
int ControlSubAgent::queryHostSerialPort(char* pBuf, US bufsize)
{
    if(NULL == pBuf)
    {
        return TS_NULLPTR_ENCONTERED;
    }

    DWORD tmpSize = 0;
    char    achKey[MAX_KEY_LENGTH] = "\0";   // buffer for subkey name
    //DWORD    cbName;                   // size of name string
    char    achClass[MAX_PATH] = "\0";  // buffer for class name
    DWORD    cchClassName = MAX_PATH;  // size of class string
    DWORD    cSubKeys=0;               // number of subkeys
    DWORD    cbMaxSubKey;              // longest subkey size
    DWORD    cchMaxClass;              // longest class string
    DWORD    cValues;              // number of values for key
    DWORD    cchMaxValue;          // longest value name
    DWORD    cbMaxValueData;       // longest value data
    DWORD    cbSecurityDescriptor; // size of security descriptor
    FILETIME ftLastWriteTime;      // last write time

    DWORD i;
    DWORD retCode;
    DWORD ret = 0;

    char  achValue[MAX_VALUE_NAME] = "\0";
    DWORD cchValue = MAX_VALUE_NAME;
    HKEY hKey;

    //open the key
    if(RegOpenKeyExA( HKEY_LOCAL_MACHINE,
                      "HARDWARE\\DEVICEMAP\\SERIALCOMM",
                      0,
                      KEY_READ,
                      &hKey) != ERROR_SUCCESS)
    {
        sysLoger(LINFO, "ControlSubAgent::queryHostSerialPort:Open the register key failure!");
        return TS_SYSTEM_ERROR;
    }

    // Get the class name and the value count.
    retCode = RegQueryInfoKeyA(
                  hKey,                   // key handle
                  achClass,               // buffer for class name
                  &cchClassName,          // size of class string
                  NULL,                   // reserved
                  &cSubKeys,              // number of subkeys
                  &cbMaxSubKey,           // longest subkey size
                  &cchMaxClass,           // longest class string
                  &cValues,               // number of values for this key
                  &cchMaxValue,           // longest value name
                  &cbMaxValueData,        // longest value data
                  &cbSecurityDescriptor,  // security descriptor
                  &ftLastWriteTime);       // last write time
    // �����˵��ע������Ĺ�
    if(retCode != ERROR_SUCCESS || cSubKeys > 0)
    {
        sysLoger(LINFO, "ControlSubAgent::queryHostSerialPort:The COM config register key had been"\
                 " modified, unknow!");
        RegCloseKey(hKey);
        return TS_SYSTEM_ERROR;
    }

    // Enumerate the key values.
    if (cValues)
    {
        for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++)
        {
            cchValue = sizeof(achValue);
            memset(achValue, 0, cchValue);
            retCode = RegEnumValueA(hKey, i,
                                    achValue,
                                    &cchValue,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL);

            if (retCode == ERROR_SUCCESS )
            {
                tmpSize = sizeof(achKey);
                retCode = RegQueryValueExA(hKey,achValue,NULL,NULL,(LPBYTE)achKey,&tmpSize);

                if(i != 0)
                {
                    if(strlen(pBuf) +1 > bufsize )
                    {
                        RegCloseKey(hKey);
                        return TS_REC_BUF_LIMITED;
                    }

                    strcat_s(pBuf, bufsize, CONTROL_FLAG);
                }

                if(strlen(pBuf) + strlen(achKey) > bufsize )
                {
                    RegCloseKey(hKey);
                    return TS_REC_BUF_LIMITED;
                }

                strcat_s(pBuf, bufsize, achKey);
            }
        }

        if(retCode == ERROR_SUCCESS )
        {
            strcat_s(pBuf, bufsize, CONTROL_FLAG);
            RegCloseKey(hKey);
            return strlen(pBuf);
        }
    }

    sysLoger(LINFO, "ControlSubAgent::queryHostSerialPort:No usable COM config!");
    RegCloseKey(hKey);
    return TS_SYSTEM_ERROR;

}

