/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  SubAgentBase.cpp
* @brief
*       功能：
*       <li> manager的二级代理抽象类和控制命令类</li>
*/


/************************头文件********************************/
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

/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/
/************************实现*********************************/

SubAgentBase::~SubAgentBase(void)
{
}


/////////////////////////////////////////////
ControlSubAgent::ControlSubAgent(short said) : SubAgentBase(said)
{
    init();
}
/**
* 功能：初始化当前的二级代理
* @param SAID 二级代理号
*/
void ControlSubAgent::init()
{
    Manager::getInstance()->addSubAgent(this);    //把自己注册到映射表中
    packLoger.open("manager.log");
}

/**
* 功能：解析当前的数据包,并执行操作
* @param pack 数据保
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

        //对收到的数据进行日志处理
        if(Manager::getInstance()->getLogFlag())
        {
            packLoger.write(RECV_PACK, *pack);
        }

        //存储DID
        US dialogID = 0;
        char cmd = 0;
        __fetch(&dialogID, pack->getData(), DIDLEN);
        Manager::getInstance()->setCurDID(pack->getSrc_aid(), dialogID);

        //进行包的检查
        if(pack->getSize() < (int)(DIDLEN + (2*CONTROL_FLAG_LEN) + CONTROL_COMMAND_LEN))
        {
            sysLoger(LWARNING, "ControlSubAgent::proccess: Received unknow format packet!");
            
            //释放分配的包
            PacketManager::getInstance()->free(pack);
            
            return;
        }

        sysLoger(LDEBUG, "ControlSubAgent::proccess: Starting get command char!");

        //取出命令字
        cmd = pack->getData()[DIDLEN+CONTROL_FLAG_LEN];

        sysLoger(LDEBUG, "ControlSubAgent::proccess: Received command %c", cmd);

        //进行命令分发处理
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
            //等待2秒再停TS,保证数据包发到客户端
            {
                /*Modified by tangxp for BUG NO.0002816 on 2008.3.3 [begin]*/
                /*  Modified brief: 修改延时使用函数*/
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

            //进行日志
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
* 功能：处理TClient发送过来的createServer控制命令, 并把得到的AID返回给TClient
* @param block 去掉createserver数据
* @param pack 接收的控制包
格式是:名字, 日志, 通道数, 动态库名字, 方式, 动态库字符串
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

        //如果服务代理已经达到最大容量
        if(Manager::getInstance()->isAgentCountReachedToMAX())
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServerWithDll: Agent objects in TS have"\
                     "reatched max count!");
            sendErrorAck(packet, ERR_SRV_NUM_EXPIRE, FAILURE_ACK_LEN);
            return;
        }

        sysLoger(LDEBUG, "ControlSubAgent::handleCreateServerWithDll: Enter this function");
        /*
            进行控制包的解析，格式为
            --------------------------------
            "DID, TserviceName, extinfo, dllName, isLog, DllBuf,"
           --------------------------------
        */
        char SAName[NAME_SIZE] = "\0";
        char DllFileName[NAME_SIZE] = "\0";
        char DllBuf[DLL_BUF] = "\0";
        char extInfo[EXT_INFO_LEN] = "\0";
        char curPath[MAX_PATH] = "\0";    //TS当前路径


        UI    uiDllBufLen = 0;
        int  logFlag = 0;

        char *pDataStartAddr = packet->getData()+DIDLEN+CONTROL_FLAG_LEN+CONTROL_COMMAND_LEN;
        char *pDest = NULL;
        /******************数据包的解析************************/
        //服务解析名字
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
        //DLL文件名
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
        pDest = packet->getData() + packet->getSize() - CONTROL_FLAG_LEN;        //指向包的末尾
        uiDllBufLen= min(pDest - pDataStartAddr, sizeof(DllBuf));
        memcpy(DllBuf, pDataStartAddr, uiDllBufLen);

        /**检查DLL文件是否存在**/
        //进行查找
        HINSTANCE hinstLib = LoadLibrary(DllFileName);

        if(NULL == hinstLib)
        {
            sysLoger(LINFO, "ControlSubAgent::handleCreateServerWithDll:Didn't find the dll file %s"\
                     ". FILE:%s, LINE:%d", DllFileName, __MYFILE__, __LINE__);
            sendErrorAck(packet, ERR_FILE_NOT_EXIST);
            return;
        }

        FreeLibrary(hinstLib);

        /******************服务的创建************************/
        if(Manager::getInstance()->getIDFromName(QString::fromAscii(SAName, strlen(SAName))) != -1)
        {
            //已经存在相同名字的服务
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

        ServerAgentWithDll* pSA = new ServerAgentWithDll(aid);    //创建sa对象

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
        pSA->createConfig(DeviceConfigBase::DLL_DEVICE);        //创建sa对应的设备方式, 动态库方式
        pSA->setName(QString::fromAscii(SAName, strlen(SAName)));    //得到动态库名称
        pSA->setDllName(QString::fromAscii(DllFileName, strlen(DllFileName)));    //得到动态库名称
        pSA->setLogFlag(logFlag);    //日志
        pSA->setInBuf(QString::fromAscii(DllBuf, uiDllBufLen));    //保存传递的参数, 用来传递给动态库的字符串
        pSA->clearChannel();
        pSA->addChannel(QString("dll"));                                  //增加一个通道
        pSA->setExtInfo(QString::fromAscii(extInfo, sizeof(extInfo)));
        Manager::getInstance()->addServerAgent(pSA);
        Router::getInstance()->registerAgent(pSA->getAID(), pSA);

        if(pSA->getRegisterType() != INVALID_REG_TYPE)
        {
            Manager::getInstance()->storeConfigure();
        }

        //回复消息
        changePacketID(packet);    //交换包中的目标和源AID
        //直接用原消息中的DID，不去manager的映射表中取
        pDest = packet->getData() + DIDLEN + CONTROL_FLAG_LEN;
        memcpy(pDest, SUCCESS_ACK, SUCCESS_ACK_LEN);
        pDest += SUCCESS_ACK_LEN;
        memcpy(pDest, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pDest += CONTROL_FLAG_LEN;
        pDest += int2hex(pDest, 10, pSA->getAID());
        memcpy(pDest, CONTROL_FLAG, CONTROL_FLAG_LEN);        //增加一个分割符
        pDest += CONTROL_FLAG_LEN;
        //设置包的大小
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
* 功能：处理TClient发送过来的createServer控制命令, 并把得到的AID返回给TClient
* @param pack 接收的控制包
发送格式: Csa名字, extInfo, 日志,通道数, +根据way选择各类配置信息(以分割符)
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
                         pack->getSize()-(DIDLEN+CONTROL_FLAG_LEN));    //转换成QT的格式
        //中文转换,转换成UTF8
        QString buf = QString::fromLocal8Bit(block.data(),block.size());
        QString str;
        bool ok;
        int next = 0;
        int ret = 0;    //查询的开始位置
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
        QString name(vec[iCount]);            //得到要创建sa的名字

        sysLoger(LDEBUG, "ControlSubAgent::handleCreateServer: Enter this function.!");

        //如果服务代理已经达到最大容量
        if(Manager::getInstance()->isAgentCountReachedToMAX())
        {
            sysLoger(LWARNING, "ControlSubAgent::handleCreateServer: Agent objects in TS have"\
                     "reatched max count!");
            sendErrorAck(pack, ERR_SRV_NUM_EXPIRE);
            return;
        }

        /**
        TC发过来的数据包格式:
        "DID, CsaName, extInfo, isLog, channelCount, config of each channel, "
        */


        //用名字去查找是否已经存在同名服务
        int srvID = Manager::getInstance()->getIDFromName(name);

        if(srvID != -1)
        {
            //已经存在相同名字的服务
            sendErrorAck(pack, ERR_SERVICE_NAME_REPEATED);
            sysLoger(LINFO, "ControlSubAgent::handleCreateServer: Exit the service Agent with "\
                     " the same name.. FILE:%s, LINE:%d.", __MYFILE__, __LINE__);
            return;
        }

        //跳过extInfo字段
        ++iCount;
        //log
        int isLog = vec[++iCount].toInt(&ok, 16);        //日志记录

        if(isLog > 0)
        {
            isLog = 1;
        }

        else
        {
            isLog = 0;
        }

        int channelCount = vec[++iCount].toInt(&ok, 16);        //通道数
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
            str += vec[++iCount];                        //协议
            str += CONTROL_FLAG;
            int way = vec[++iCount].toInt(&ok, 16);            //通信方式
            str += vec[iCount];                        //设备通信方式
            str += CONTROL_FLAG;

            switch(way)
            {
            case TCP:    //tcp
            {
                QString strIP;
                strIP = vec[++iCount];
                str += vec[iCount];
                str += CONTROL_FLAG;

                int port = vec[++iCount].toInt(&ok, 16);        //端口
                str += QString("%1").arg(port, 0, 16 );
                str += CONTROL_FLAG;

                int timeout = vec[++iCount].toInt(&ok, 16);                //超时
                str += QString("%1").arg(timeout, 0, 16 );
                str += CONTROL_FLAG;
                int retry = vec[++iCount].toInt(&ok, 16);                //重试
                str += QString("%1").arg(retry, 0, 16 );
                str += CONTROL_FLAG;

                //为第一通道创建设备
                if(i == 0)
                {
                    TcpConfig *ptcpConfig = new TcpConfig();//TCP, port, strIP);                        //tcp的配置信息

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

                int port = vec[++iCount].toInt(&ok, 16);        //端口
                str += QString("%1").arg(port, 0, 16 );
                str += CONTROL_FLAG;
                int timeout = vec[++iCount].toInt(&ok, 16);    //超时
                str += QString("%1").arg(timeout, 0, 16 );
                str += CONTROL_FLAG;
                int retry = vec[++iCount].toInt(&ok, 16);    //重试
                str += QString("%1").arg(retry, 0, 16 );
                str += CONTROL_FLAG;

                //为第一通道创建设备
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

            case UART:                        //串口
            {
                QString comChannel = vec[++iCount];
                str += comChannel;
                str += CONTROL_FLAG;
                //检查串口通道是否有效
                char bufCom[200] = "\0";
                int idx = 0;
                string localCOMcfg;     //主机串口的配置

                if(queryHostSerialPort(bufCom, sizeof(bufCom)) > 0)
                {
                    localCOMcfg = string(bufCom, strlen(bufCom));
                    idx = localCOMcfg.find(comChannel.toAscii().data());

                    if(idx == string::npos)
                    {
                        //TS主机没有该串口配置
                        sysLoger(LINFO, "ControlSubAgent::handleCreateServer:There is no COM port %s"\
                                 " in local machine!", comChannel.toAscii().data());
                        //回复
                        sendErrorAck(pack, ERR_INVALID_UART_PORT);

                        //其他通道分配的信息
                        if(pSA != NULL)
                        {
                            pSA->clearConfig();
                            //释放ID号
                            Manager::getInstance()->freeAid(pSA->getAID());
                            delete pSA;
                            pSA = NULL;
                        }

                        return;
                    }
                }

                int baud = vec[++iCount].toInt(&ok, 16);        //波特率
                str += QString("%1").arg(baud, 0, 16 );
                str += CONTROL_FLAG;
                int timeout = vec[++iCount].toInt(&ok, 16);    //超时
                str += QString("%1").arg(timeout, 0, 16 );
                str += CONTROL_FLAG;
                int retry = vec[++iCount].toInt(&ok, 16);    //重试
                str += QString("%1").arg(retry, 0, 16 );
                str += CONTROL_FLAG;

                //为第一通道创建设备
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
                //回复TClient
                sendErrorAck(pack, ERR_UNKNOW_COMM_WAY);

                //其他通道分配的信息
                if(pSA != NULL)
                {
                    pSA->clearConfig();
                    //释放ID号
                    Manager::getInstance()->freeAid(pSA->getAID());
                    delete pSA;
                    pSA = NULL;
                }

                return;
                break;
            }

            //保存好用户定义的数据

            //保存在数组中
            if(pSA != NULL)
            {
                pSA->addChannel(str);
            }
        }

        //判断创建是否成功
        if (pSA == NULL)
        {
            sendErrorAck(pack, ERR_SYSTEM_ERR);
            sysLoger(LINFO, "ControlSubAgent::handleCreateServer: Created SA failure, name: %s",
                     name);

            //如果分配aid成功，则释放
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

            //创建成功，回复TClient
            changePacketID(pack);    //交换包中的目标和源AID
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
            //设置包大小
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
* 功能：处理TClient发送过来的deleteServer控制命令, 停止sa
* @param block 去掉createserver数据
* @param pack 接收的控制包
*/
void ControlSubAgent::handleDeleteServer(Packet* pack)
{
    int iRet = 0;
    char *pDest = NULL;
    char *pStr = NULL;
    int aid = 0;

    // 数据包格式"DID, Raid, "
    if(NULL == pack)
    {
        sysLoger(LWARNING, "handleDeleteServer:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    //在proccess里已经初步验证了数据包的长度
    //这里直接跳过DID部分
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
    //TC端发0来删除TS中所有的SERVER
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
* 功能：处理TClient发送过来的createServer控制命令, 并把得到的AID返回给TClient
* @param deviceConfig 设备配置信息
* @param name sa的名字
* @param isLog sa是否需要日志记录
* @param protocolType sa对应的协议
* @param state sa的状态
* @return ServerAgent* 返回sa对象指针
*/
ServerAgent* ControlSubAgent::createSA(DeviceConfigBase* deviceConfig, QString name, bool isLog,
                                       int protocolType, int state)
{
    //创建一个sa对象
    try
    {
        ServerAgent* pSA = new ServerAgent(Manager::getInstance()->createID());    //创建sa对象

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
                //启动sa线程
                //    mapName2ID.insert(pServerAgent->getName(),  pServerAgent->getAID());    //把创建的sa的名字和AID插入关联链表
                pSA->setState(SASTATE_ACTIVE);    //设置sa为活动状态
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
* 功能：处理TCLIENT_OPEN函数, 返回源AID给调用着
* @param pack 接收的控制包
*/
void ControlSubAgent::handleTClientOpen(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::HandleTClientOpen:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    //发送回复数据
    //交换源AID和目标AID号
    sysLoger(LDEBUG, "ControlSubAgent::HandleTClientOpen: Received clientAgent %d 's open command",
             pack->getSrc_aid());
    changePacketID(pack);
    char* pBuf =pack->getData();
    memset(pBuf, 0, PACKET_DATASIZE);
    //存储DID
    __store(CURDID(pack->getDes_aid()),  pBuf, DIDLEN);
    pBuf += DIDLEN;
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    //发送回复消息包
    pack->setSize(pBuf-pack->getData());                //更新包的大小

    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    Router::getInstance()->putpkt(pack);
    return;
}
/**
* 功能：处理TClient_queryServerState函数, 查询TS上面存在的所有的可用目标服务ID
* @param pack 接收的控制包
*/
void ControlSubAgent::handleSearchServer(Packet* pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSearchServer:pack is NULL pionter!FILE:%s, LINE%d",
                 __MYFILE__, __LINE__);
        return;
    }

    //得到当前保存的server对象
    QMap<QString , short> mapName2ID = Manager::getInstance()->getName2AIDMap();    //得到manager映射
    QMap<QString, short>::const_iterator cItrName2ID = mapName2ID.constBegin();
    changePacketID(pack);    //交换包中的目标和源AID
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
    /*  Modified brief: 修正调用获取结束迭代的函数,增加缓冲溢出判断*/
    while((cItrName2ID != mapName2ID.constEnd()) && ((pBuf - pack->getData()) < PACKET_DATASIZE - 10))
        /*Modified by tangxp for BUG NO.0002555 on 2008.2.3 [end]*/
    {
        pBuf += int2hex(pBuf, 10, cItrName2ID.value());
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        ++cItrName2ID;
    }

    pack->setSize(pBuf-pack->getData());        //设置包的大小

    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    Router::getInstance()->putpkt(pack);                        //进行包的发送
    return;
}

/**
* 功能：处理TClient_activeServer函数, 激活指定的处于休眠状态的目标服务
* @param pack 接收的控制包
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
        /*process里已经检查了数据包的长度，这里直接跳过DID部分*/
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
        //要个类型强制转换
        AgentWithQueue* pAgent = dynamic_cast<AgentWithQueue*>
                                 (Manager::getInstance()->getServerAgentFromID(aid));

        if(NULL == pAgent)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleActiveServer: Active absent SA[aid = %d]"\
                     " FILE:%s, LINE:%d", aid, __MYFILE__, __LINE__);
            //回复消息
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
                    //没有激活则激活(启动SA)
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
                //进行上报
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
                        //填充回复消息包
                        changePacketID(pack);    //交换包中的目标和源AID
                        pack->clearData();
                        char* pBuf = pack->getData();
                        //DID
                        __store(CURDID(pack->getDes_aid()), pBuf, DIDLEN);
                        pBuf += DIDLEN;
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //增加一个分割符
                        pBuf += CONTROL_FLAG_LEN;
                        //OK
                        memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
                        pBuf += SUCCESS_ACK_LEN;
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //增加一个分割符
                        pBuf += CONTROL_FLAG_LEN;
                        //aid
                        pBuf += int2hex(pBuf, 10, pSA->getAID());
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //增加一个分割符
                        pBuf += CONTROL_FLAG_LEN;

                        int outbufsize = strlen(pSA->getOutBuf());

                        if(pBuf - pack->getData() < (int)(outbufsize + CONTROL_FLAG_LEN))
                        {
                            sysLoger(LWARNING, "ControlSubAgent::handleActiveServer: Dll server's outBuf"\
                                     " is too large to an ACK packet size!");
                        }

                        outbufsize = min((int)(PACKET_DATASIZE - (pBuf - pack->getData()) - \
                                               CONTROL_FLAG_LEN), outbufsize);
                        //发送出去的DLLBUF大小
                        pBuf += int2hex(pBuf, 10, outbufsize);            //字符串的大小
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
                        pBuf += CONTROL_FLAG_LEN;

                        memcpy(pBuf, pSA->getOutBuf(), outbufsize);                    //传出缓冲区
                        pBuf += outbufsize;
                        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //增加一个分割符
                        pBuf += CONTROL_FLAG_LEN;
                        pack->setSize(pBuf - pack->getData());                //更新包的大小
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

                else if(pSA->getState() == SASTATE_ACTIVE)                //已经是激活状态
                {
                    //填充回复消息包
                    changePacketID(pack);    //交换包中的目标和源AID
                    pack->clearData();
                    char* pBuf = pack->getData();
                    //DID
                    __store(CURDID(pack->getDes_aid()), pBuf, DIDLEN);
                    pBuf += DIDLEN;
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //增加一个分割符
                    pBuf += CONTROL_FLAG_LEN;
                    //OK
                    memcpy(pBuf, SUCCESS_ACK, SUCCESS_ACK_LEN);
                    pBuf += SUCCESS_ACK_LEN;
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //增加一个分割符
                    pBuf += CONTROL_FLAG_LEN;
                    //aid
                    pBuf += int2hex(pBuf, 10, pSA->getAID());
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //增加一个分割符
                    pBuf += CONTROL_FLAG_LEN;
                    int outbufsize = strlen(pSA->getOutBuf());

                    if(pBuf - pack->getData() < (int)(outbufsize + CONTROL_FLAG_LEN))
                    {
                        sysLoger(LWARNING, "ControlSubAgent::handleActiveServer: Dll server's outBuf"\
                                 " is too large to an ACK packet size!");
                    }

                    outbufsize = min((int)(PACKET_DATASIZE - (pBuf - pack->getData()) - \
                                           CONTROL_FLAG_LEN), outbufsize);
                    //发送出去的DLLBUF大小
                    pBuf += int2hex(pBuf, 10, outbufsize);            //字符串的大小
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
                    pBuf += CONTROL_FLAG_LEN;

                    memcpy(pBuf, pSA->getOutBuf(), outbufsize);                    //传出缓冲区
                    pBuf += outbufsize;
                    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);        //增加一个分割符
                    pBuf += CONTROL_FLAG_LEN;
                    pack->setSize(pBuf - pack->getData());                //更新包的大小
                    sysLoger(LINFO, "ControlSubAgent::handleActiveServer: Active server agent [aid=%d] "\
                             ", but the agent is already in active state!", aid);
                }

                else
                {
                    //处理其他状态
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
                //进行上报
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

        //异常处理(pSA == NULL) /pAgent->getType() = other
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
* 功能：处理TClient_inActiveServer函数, 休眠指定的处于激活状态的目标服务
* @param pack 接收的控制包
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
        /*process里已经检查了数据包的长度，这里直接跳过DID部分*/
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
            //回复消息
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
                /*  Modified brief: 休眠SA时,直接关闭SA,避免设备连接断开时,没有删除TX,RX线程*/
                if(pServerAgent->getState() == SASTATE_ACTIVE)
                {
                    Router::getInstance()->unRegisterAgent(pServerAgent->getAID());    //删除到router的注册
                }

                /*Modified by tangxp for BUG NO.3453 on 2008.6.23 [end]*/
                setSuccessPacket(pack);
                sysLoger(LINFO, "ControlSubAgent::handleInactiveServer: Inactive SA [aid = %d] successfully",
                         aid);

                //日志
                if(Manager::getInstance()->getLogFlag())
                {
                    packLoger.write(SEND_PACK, *pack);
                }

                /* 目标机不存在时发送命令包会超时重发,但IDE只等待6秒,就会造成第一次断开连接失败。因此在向目标机发送断开命令“C”前,先向IDE回复断开操作成功命令. */
                Router::getInstance()->putpkt(pack);

                pServerAgent->stop();

                //上报
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
                    Router::getInstance()->unRegisterAgent(pServerAgent->getAID());    //删除到router的注册
                }

                setSuccessPacket(pack);
                sysLoger(LINFO, "ControlSubAgent::handleInactiveServer: Inactive SA [aid = %d] successfully",
                         aid);

                //日志
                if(Manager::getInstance()->getLogFlag())
                {
                    packLoger.write(SEND_PACK, *pack);
                }

                Router::getInstance()->putpkt(pack);
                //上报
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

        //异常处理(pServerAgent == NULL) /pAgent->getType() = other
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
* 功能：处理TClient_queryServerState函数, 指定目标服务ID号查找该目标机的配置参数和状态信息
* @param pack 接收的控制包
* 回复包格式
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

        /*process里已经检查了数据包的长度，这里直接跳过DID部分*/
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

        //查找该AID对应的对象
        AgentWithQueue* pAgentWithQueue = dynamic_cast<AgentWithQueue*>
                                          (Manager::getInstance()->getServerAgentFromID(aid));

        if(NULL == pAgentWithQueue)
        {
            sysLoger(LWARNING, "ControlSubAgent::handleQueryServer: Queried absent SA[aid = %d]"\
                     " FILE:%s, LINE:%d", aid, __MYFILE__, __LINE__);
            //回复消息
            sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
            return;
        }

        //根据服务类型填充上报消息
        switch(pAgentWithQueue->getType())
        {
        case AgentBase::SERVER_AGENT:
        case AgentBase::SERVER_AGENT_WITH_DLL:
        {
            //ServerAgent* pServerAgent = dynamic_cast<ServerAgent*>(pAgentWithQueue);
            //int way = pServerAgent->getConfig()->getDeviceConfig()->getDeviceType();    //得到通信方式
            //填充回复消息包
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

                //上报通信状态
                for (int i=0; i<pAgentWithQueue->getChannelCount(); ++i)
                {
                    QByteArray bufBlock = channelInfo[i].toAscii();
                    memcpy(pBuf,bufBlock.data(),bufBlock.size());    //通道配置信息
                    pBuf += bufBlock.size();
                }
            }

            else
            {
                ServerAgentWithDll* pServerAgentWithDll = dynamic_cast<ServerAgentWithDll*>(pAgentWithQueue);
                /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [begin]*/
                /*  Modified brief: 获取通信协议和通信方式时,直接置为DLL_DEVICE */

                int protocol = DeviceConfigBase::DLL_DEVICE;                        //协议类型

                pBuf += int2hex(pBuf, 10, protocol);                                    //保存通信方式
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //增加一个分割符
                pBuf += CONTROL_FLAG_LEN;

                int way = DeviceConfigBase::DLL_DEVICE;                             //得到通信方式
                pBuf += int2hex(pBuf, 10, way);                                    //保存通信方式
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //增加一个分割符
                pBuf += CONTROL_FLAG_LEN;
                /*Modified by tangxp for BUG NO.0002555 on 2008.2.1 [end]*/

                //DllName
                memcpy(pBuf, pServerAgentWithDll->getDllName().toAscii().data(),
                       pServerAgentWithDll->getDllName().toAscii().length());                    //动态库名字
                pBuf += pServerAgentWithDll->getDllName().toAscii().length();
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //增加一个分割符
                pBuf += CONTROL_FLAG_LEN;

                //OutBuf
                memcpy(pBuf, pServerAgentWithDll->getOutBuf(), strlen(pServerAgentWithDll->getOutBuf()));
                pBuf += strlen(pServerAgentWithDll->getOutBuf());
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //增加一个分割符
                pBuf += CONTROL_FLAG_LEN;

                //InBuf
                memcpy(pBuf, pServerAgentWithDll->getInBuf().toAscii().data(),
                       pServerAgentWithDll->getInBuf().toAscii().length());
                pBuf += pServerAgentWithDll->getInBuf().toAscii().length();
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);                    //增加一个分割符
                pBuf += CONTROL_FLAG_LEN;
            }

            pack->setSize(pBuf - pack->getData());
        }
        break;

        default:
            sysLoger(LWARNING, "ControlSubAgent::handleQueryServer: Tried to query information of "\
                     " nonsupport agent type[aid=%d,type=%d]!  FILE:%s, LINE:%d",
                     aid, pAgentWithQueue->getType(), __MYFILE__, __LINE__);
            //回复失败消息包
            sendErrorAck(pack, ERR_INVALID_SERVICE_ID);
            return;
            break;
        }

        //发送数据并记录日志
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
* 功能：处理TClient_modifyServer函数, 修改一个已经存在在TS中的目标服务配置
* @param pack 接收的控制包
*/
void ControlSubAgent::handleModifyServer(Packet* pack)
{
    QVector<QString> vec;
    QByteArray block(pack->getData()+DIDLEN+CONTROL_FLAG_LEN+ CONTROL_COMMAND_LEN,
                     pack->getSize()-(DIDLEN+CONTROL_FLAG_LEN+ CONTROL_COMMAND_LEN));    //转换成QT的格式
    //中文转换,转换成UUTF8
    QString buf = QString::fromLocal8Bit(block.data(), block.size());
    QString str;
    bool ok;
    int next = 0;    //查询的开始位置
    bool isSrvRunbfMod = false;            //修改之前服务代理是否处理运行态

    /*
    TClient 发送过来的消息格式
     NET/UART方式
     -----------------------------------------------------------------------------
     |DID|, |Maid|, |name|, |extInfo|,|isLog|, |channelCount|, |curChannel|, |channelConfig..............|
     -----------------------------------------------------------------------------
     DLL方式
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
        int aid = vec[iCount++].toInt(&ok, 16);    //aid号
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

            //先停止服务进行配置信息的修改
            if(pServerAgent->getState() == SASTATE_ACTIVE)
            {
                isSrvRunbfMod = true;
                pServerAgent->stop();
                pServerAgent->setState(SASTATE_INACTIVE);
            }

            //比较名字是否改变
            if((!vec[iCount].isEmpty()) && (pServerAgent->getName() != vec[iCount]))
            {
                QString oldSAName = pServerAgent->getName();
                pServerAgent->setName(vec[iCount]);

                //修改Manager中ID=》NAME的映射表
                Manager::getInstance()->modifyName2AIDMap(oldSAName, vec[iCount]);
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer: Modified ServerAgent name"\
                         " from %s to newName %s!",
                         oldSAName.toAscii().data(), vec[iCount].toAscii().data());

				//bug6420目标机改名为，ts日志中不会同步更新名字，对以前的log的文件进行备份，同时设置当前log文件指针为空
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
            //日志标志是否修改
            iCount++;
            bool isLog = (bool)vec[iCount++].toInt(&ok, 16);        //日志记录

            if(isLog != pServerAgent->getLogFlag())
            {
                bool oldLogFlag = pServerAgent->getLogFlag();
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer: Modified ServerAgent logflag"\
                         " from %d to  %d!", oldLogFlag, isLog);
            }
			//bug6420目标机改名为，ts日志中不会同步更新名字
			 pServerAgent->setLogFlag(isLog);
             pServerAgent->getConfig()->setLogFlag(isLog);
            //得到通信配置信息
            int channelCount = vec[iCount++].toInt(&ok, 16);        //通道数
            int curChannel = vec[iCount++].toInt(&ok, 16);        //当前通道

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
                PROTOCOL protocolType = (PROTOCOL)vec[iCount++].toInt(&ok, 16);    //协议类型
                str += QString("%1").arg(protocolType, 0, 16 );
                str += CONTROL_FLAG;

                PROTOCOL way = (PROTOCOL)vec[iCount++].toInt(&ok, 16);    //通信方式
                str += QString("%1").arg(way, 0, 16 );
                str += CONTROL_FLAG;

                switch(way)
                {
                case TCP:    //tcp
                {
                    QString ip = vec[iCount++];
                    str += ip;
                    str += CONTROL_FLAG;
                    int port = vec[iCount++].toInt(&ok, 16);        //端口号
                    str += QString("%1").arg(port, 0, 16 );
                    str += CONTROL_FLAG;
                    int timeout = vec[iCount++].toInt(&ok, 16);    //超时
                    str += QString("%1").arg(timeout, 0, 16 );
                    str += CONTROL_FLAG;
                    int retry = vec[iCount++].toInt(&ok, 16);        //重试次数
                    str += QString("%1").arg(retry, 0, 16 );
                    str += CONTROL_FLAG;

                    //保存当前的通道
                    if (i == curChannel )
                    {
                        //更换了通信方式
                        if (pServerAgent->getConfig()->getDeviceConfig()->getDeviceType() !=
                                DeviceConfigBase::TCP_DEVICE)
                        {
                            ServerConfig* pServerConfig = pServerAgent->getConfig();
                            pServerConfig->setDeviceConfig(
                                DeviceConfigBase::newDeviceConfig(DeviceConfigBase::TCP_DEVICE));
                            pServerConfig->setProtocolType(protocolType);
                        }

                        //tcp的配置信息
                        TcpConfig* pTcpConfig = dynamic_cast<TcpConfig*>(pServerAgent->getConfig()->getDeviceConfig());

                        pTcpConfig->setPort(port);        //设置端口
                        pTcpConfig->setIP(ip);    //设置IP地址

                    }
                }
                break;

                case UDP:    //UDP
                {
                    QString ip = vec[iCount++];
                    str += ip;
                    str += CONTROL_FLAG;
                    int port = vec[iCount++].toInt(&ok, 16);        //端口号
                    str += QString("%1").arg(port, 0, 16 );
                    str += CONTROL_FLAG;
                    int timeout = vec[iCount++].toInt(&ok, 16);    //超时
                    str += QString("%1").arg(timeout, 0, 16 );
                    str += CONTROL_FLAG;
                    int retry = vec[iCount++].toInt(&ok, 16);        //重试次数
                    str += QString("%1").arg(retry, 0, 16 );
                    str += CONTROL_FLAG;

                    if (i == curChannel)
                    {
                        //更换了通信方式
                        if (pServerAgent->getConfig()->getDeviceConfig()->getDeviceType() !=
                                DeviceConfigBase::UDP_DEVICE)
                        {
                            pServerAgent->getConfig()->setDeviceConfig(
                                DeviceConfigBase::newDeviceConfig(DeviceConfigBase::UDP_DEVICE));
                            pServerAgent->getConfig()->setProtocolType(protocolType);
                        }

                        //UDP的配置信息
                        UdpConfig* pUdpConfig = dynamic_cast<UdpConfig*>\
                                                (pServerAgent->getConfig()->getDeviceConfig());
                        pUdpConfig->setRetry(retry);        //设置重试次数
                        pUdpConfig->setIP(ip);    //设置IP地址
                        pUdpConfig->setPort(port);        //设置端口
                        pUdpConfig->setTimeout(timeout);        //设置超时时间
                    }
                }
                break;

                case UART:    //串口
                {
                    QString com = vec[iCount++];
                    str += com;
                    str += CONTROL_FLAG;
                    //检查串口通道是否有效
                    char bufCom[200] = "\0";
                    int idx = 0;
                    string localCOMcfg;     //主机串口的配置

                    if(queryHostSerialPort(bufCom, sizeof(bufCom)) > 0)
                    {
                        localCOMcfg = string(bufCom, strlen(bufCom));
                        idx = localCOMcfg.find(com.toAscii().data());

                        if(idx == string::npos)
                        {
                            //TS主机没有该串口配置
                            sysLoger(LINFO, "ControlSubAgent::handleCreateServer:There is no COM port %s"\
                                     " in local machine!", com.toAscii().data());
                            //回复
                            sendErrorAck(pack, ERR_INVALID_UART_PORT);
                            return;
                        }
                    }

                    int port = vec[iCount++].toInt(&ok, 16);        //端口号
                    str += QString("%1").arg(port, 0, 16 );
                    str += CONTROL_FLAG;
                    int timeout = vec[iCount++].toInt(&ok, 16);    //超时
                    str += QString("%1").arg(timeout, 0, 16 );
                    str += CONTROL_FLAG;
                    int retry = vec[iCount++].toInt(&ok, 16);        //重试次数
                    str += QString("%1").arg(retry, 0, 16 );
                    str += CONTROL_FLAG;

                    if (i==curChannel)
                    {
                        //更换了通信方式
                        if (pServerAgent->getConfig()->getDeviceConfig()->getDeviceType() !=
                                DeviceConfigBase::UART_DEVICE)
                        {
                            pServerAgent->getConfig()->setDeviceConfig(
                                DeviceConfigBase::newDeviceConfig(DeviceConfigBase::UART_DEVICE));
                            pServerAgent->getConfig()->setProtocolType(protocolType);
                        }

                        //UART的配置信息
                        UartConfig* pUartConfig = dynamic_cast<UartConfig*>\
                                                  (pServerAgent->getConfig()->getDeviceConfig());
                        pUartConfig->setRetry(retry);        //设置重次次数
                        pUartConfig->setTimeout(timeout);        //设置超时时间

                        pUartConfig->setBaud(port);        //设置波特率
                        pUartConfig->setCom(com);    //设置串口端口
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
                    //运行sa程序,
                    pServerAgent->setState(SASTATE_ACTIVE);    //设置sa的状态为激活状态
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
            bool logFlag = (bool)vec[iCount++].toInt(&ok, 16);        //日志
            PROTOCOL way = (PROTOCOL)vec[iCount].toInt(&ok, 16);

            if(way != DLL)
            {
                sysLoger(LINFO, "ControlSubAgent::handleModifyServer:Communication way %d for Dll "\
                         "Agent is invalid", way);
                sendErrorAck(pack, ERR_SRV_CFG_ERR);
                return;
            }

            //先停止服务
            if(pServerAgentWithDll->getState() == SASTATE_ACTIVE)
            {
                isSrvRunbfMod = true;
                pServerAgentWithDll->stop();
                pServerAgentWithDll->setState(SASTATE_INACTIVE);
                Router::getInstance()->unRegisterAgent(pServerAgentWithDll->getAID());    //删除到router的注册
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
                pServerAgentWithDll->setLogFlag(logFlag);                //日志
            }

            //buf 从|isLog|, |way|, |DllBuf|,
            int lIdx = 0;
            int rLen = 0;
            lIdx += vec[iCount -1].length() + CONTROL_FLAG_LEN;    //isLog
            lIdx += vec[iCount].length() + CONTROL_FLAG_LEN;          //way
            rLen = max(0, buf.length() - (lIdx+CONTROL_FLAG_LEN));
            QString dllInbuf = buf.mid(lIdx, rLen);
            sysLoger(LDEBUG, "handleModifyServer:Dll [%d] set inBuf:%s", aid, dllInbuf.toAscii().data());
            pServerAgentWithDll->setInBuf(dllInbuf);    //保存传递的参数,用来传递给动态库的字符串

            if (isSrvRunbfMod)
            {
                if (pServerAgentWithDll->run())
                {
                    //运行sa程序,
                    pServerAgentWithDll->setState(SASTATE_ACTIVE);    //设置sa的状态为激活状态
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

        //回复成功激活
        sendSuccessPacket(pack);

        //存储配置信息
        if(pAgentWithQueue->getRegisterType() != INVALID_REG_TYPE)
        {
            Manager::getInstance()->storeConfigure();
        }

        //进行上报
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
* 功能：处理TClient_register函数, TS进行注册, 注册后, TS会向TC自动发送消息和状态
* @param pack 接收的控制包
*/
void ControlSubAgent::handleRegisterServer(Packet* pack)
{
    int regType = 0;
    US caAid = 0;
    int ret = TS_SUCCESS;

    /**
    TC的控制命令" DID, EregType,"
    */
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleRegisterServer: Parameter pack is null pointer");
        return;
    }

    //得到注册aid号
    char* pBuf = pack->getData()+ DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
    hex2int(&pBuf, &regType);

    //CA的aid
    caAid = pack->getSrc_aid();

    //该命令不需要回复消息
    ret = Manager::getInstance()->registerServer(pack, regType);

    if (ret == TS_SUCCESS)
    {
        sysLoger(LINFO, "ControlSubAgent::handleRegisterServer: Regisgered CA [aid = %d] to type"\
                 " %d successfully!", pack->getSrc_aid(), regType);
        //sendSuccessPacket(pack);
        //进行该类型的上报
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
* 功能：处理TClient_unregister函数, 注销会话ID, 关闭SOCKET
* @param pack 接收的控制包
* @param offset 数据偏移位置
*/
void ControlSubAgent::handleUnregisterServer(Packet* pack)
{
    int regType = 0;
    int ret = TS_SUCCESS;

    /**
    TC的控制命令" DID, UregType,"
    */
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleUnregisterServer: Parameter pack is null pointer");
        return;
    }

    //得到注册类型
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
* 功能：处理TClient_setRegisterType函数, 设置sa的类型, 并进行上报
* @param pack 接收的控制包
格式是aid号, 类型
*/
void ControlSubAgent::handleSetRegisterType(Packet* pack)
{
    /**
      TC发送的控制命令包: " DID, s Aid, regType,"
    */
    bool blRet = false;
    int aid = 0;
    int regType = 0;

    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetRegisterType: Parameter pack is null pointer");
        return;
    }

    //指向AID地址
    char* pBuf = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
    hex2int(&pBuf, &aid);
    pBuf += CONTROL_FLAG_LEN;
    hex2int(&pBuf, &regType);

    //查找对应的服务代理进行注册
    AgentBase *pAgent = Manager::getInstance()->getServerAgentFromID((short)(aid));

    if(NULL == pAgent)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetRegisterType: Server Agent [aid = %d] is absent",
                 aid);
        sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
        return;
    }

    //检查对象是否是非SA对象(CA)
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
    //进行回复
    sendSuccessPacket(pack);
    //进行上报
    Manager::getInstance()->sendSAReportToCA(pAgentQ);
    //存储配置信息
    Manager::getInstance()->storeConfigure();
    return;
}

/**
* 功能：处理TClient_clearRegisterType函数, 清除sa的注册类型, 并进行上报
* @param pack 接收的控制包
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
     TC发送过来的控制包："DID, caid, regType"
    */
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleClearRegisterType: Parameter pack is null pointer");
        return;
    }

    //指向AID地址
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

    //查找aid对应的SA
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
        //回复
        sendErrorAck(pack, ERR_INVALID_SERVICE_ID);
        sysLoger(LINFO, "ControlSubAgent::handleClearRegisterType: Invalid serverAgent type %d,"\
                 " the aid %d", pAgentQ->getType(), aid);
        return;
    }

    //回复取消注册成功
    pAgentQ->setRegisterType(INVALID_REG_TYPE);
    sysLoger(LINFO, "ControlSubAgent::handleClearRegisterType:Set the SA [%s] to NULL register "\
             "type successfully!", pAgentQ->getName().toAscii().data());
    sendSuccessPacket(pack);
    //存储配置信息
    Manager::getInstance()->storeConfigure();
    return;
}

/**
* 功能：处理TClient发送过来的GetID控制命令, 并把得到的AID返回给TClient
* @param pack 接收的控制包
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
    TC发过来的控制消息: "DID, Iname,"
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

    aid = Manager::getInstance()->getIDFromName(QString::fromStdString(name));        //通过名字得到ID

    if (aid != -1)
    {
        setSuccessPacket(pack);
        pBuf = pack->getData() + pack->getSize();
        pBuf += int2hex(pBuf, 10, aid);
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        pack->setSize(pBuf-pack->getData());//更新包的大小
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

    //发送回复包，记录日志
    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    Router::getInstance()->putpkt(pack);
    return;
}

/******************************TS 3.2 新增的*****************************/

/**
 * @Funcname:  handleSetSystemLogLevel
 * @brief        :  设置系统日志的输出级别
 * @para1      : pack 接收的控制包
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年1月10日 14:46:06
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
    TC发过来的控制消息: "DID, LlogLevel,"
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

    //解析AID
    hex2int(&pDest, &iLogLevel);
    LogLevel level= ((LDEBUG <= iLogLevel) && (iLogLevel <= LFATAL))?(LogLevel)(iLogLevel):LBUTT;
    SystemLog::getInstance()->setLogLevel(level);
    //存储配置信息
    Manager::getInstance()->storeConfigure();
    //不需要回复
    PacketManager::getInstance()->free(pack);
    sysLoger(LWARNING, "ControlSubAgent::handleSetSystemLogLevel:Success to set system log "\
             "output levle as %d.", level);
    return;
}

/**
 * @Funcname:  handleSetAgentLogFlag
 * @brief        :  设置服务代理的日志标记
 * @para1      : pack 接收的控制包
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年1月10日 14:51:34
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
    TC发过来的控制消息: "DID, laid, logflag,"
    */
    char *pDest = NULL;
    char *pStr = NULL;
    AgentBase *pAgent = NULL;
    AgentWithQueue *pAgentQ = NULL;
    int aid = 0;
    int iflag = 0;
    bool flag = false;

    //进行包的解析
    pDest = pack->getData() + DIDLEN + CONTROL_FLAG_LEN + CONTROL_COMMAND_LEN;
    pStr = strstr(pDest, CONTROL_FLAG);

    if(NULL == pStr)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetAgentLogFlag:Unknow packet!"\
                 " Lost CONTROL_FLAG after AID!");
        sendErrorAck(pack, ERR_UNKNOW_DATA);
        return;
    }

    //解析AID
    hex2int(&pDest, &aid);

    //解析FLAG
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

    //找到服务代理
    pAgent = Manager::getInstance()->getServerAgentFromID((US)aid);

    if(NULL == pAgent)
    {
        sysLoger(LWARNING, "ControlSubAgent::handleSetAgentLogFlag: didn't find the"\
                 " server agent by aid %d", aid);
        sendErrorAck(pack, ERR_SERVICE_NOT_EXIST);
        return;
    }

    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [begin]*/
    /*  Modified brief: 增加检查代理类型*/
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

    //存储配置信息
    if(pAgentQ->getRegisterType() != INVALID_REG_TYPE)
    {
        Manager::getInstance()->storeConfigure();
    }

    return;
}

/**
 * @Funcname:  handleQueryHostSerialPort
 * @brief        :  处理查询TS主机串口配置的请求
 * @para1      : pack 接收的控制包
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年1月10日 15:01:46
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
    TC发过来的控制消息: "DID, p,"
    */
    /*通过查询注册表项的HKEY_LOCAL_MACHINE下面的
    HARDWARE\\DEVICEMAP\\SERIALCOMM子项来查询串口配置
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
        /*  Modified brief: 将设置功能包头操作放入循环内,避免错误时后面两次交换AID*/
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
 * @brief        :  处理客户端关闭连接命令,TS需要清除系统中与之相关的数据
 * @para1      : pack 接收的控制包
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年1月11日 11:29:26
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

    //删除该客户端代理数据
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
 * @brief        :  处理TSAPI上传文件
 * @para1      : pack命令包
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年3月2日 18:00:06
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
        char cFileName[MAX_PATH] = "\0";              //文件名
        int isComplete = TS_FILE_BEGIN;                              //是否传送完毕
        int isReplace = 0;                                //是否进行覆盖(若存在同名文件)
        int   aid = 0;
        int curLen = 0;
        char *pDest = NULL;
        char *pStr = NULL;
        string sFn;

        sysLoger(LDEBUG, "ControlSubAgent::handleUpLoadFile: Recieved the request for uploading file!");

        //跳过DID和命令字部分
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

        //组装文件名(相对路径)
        sFn.clear();
        sFn = Manager::tmpfilepath + truncatPathFromFilename(cFileName);
        //全路径
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
        /*  Modified brief: 增加出现异常和操作完成释放数据包*/
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

        //开始写的第一个包和最后一个包发送回复
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
 * @brief        :  处理TSAPI下载文件
 * @para1      : pack命令包
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年3月2日 18:00:06
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
        char cFileName[MAX_PATH] = "\0";              //文件名
        int isComplete = TS_FILE_BEGIN;                              //是否传送完毕
        int isReplace = 0;                                //是否进行覆盖(若存在同名文件)
        int curLen = 0, fnIdx = 0;
        char *pDest = NULL;
        char *pStr = NULL;
        char *pBuf = NULL;
        string sFn;

        sysLoger(LDEBUG, "ControlSubAgent::handleDownLoadFile: Recieved the request for uploading file!");

        //跳过DID和命令字部分
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

        //组装文件名
        sFn.clear();
        sFn = Manager::tmpfilepath + truncatPathFromFilename(cFileName);

        //全路径
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
        /*  Modified brief: 增加设置成功包信息*/
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

        int icount = 0;      //用于发送计数

        //改变回复包头
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
            //设置包大小
            pack->setSize(pBuf - pack->getData());                //更新包的大小

            if(Manager::getInstance()->getLogFlag())
            {
                packLoger.write(SEND_PACK, *pack);
            }

            /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [begin]*/
            /*  Modified brief: 循环发包时,不释放数据包,待发送完后,再释放*/
            //发送包
            Router::getInstance()->putpkt(pack, false);
            icount++;
        }

        PacketManager::getInstance()->free(pack);
        /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [end]*/
        fclose(pfile);
        /*Modified by tangxp for BUG NO.0002204 on 2008.1.15 [begin]*/
        /*  Modified brief: 释放动态分配内存*/
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
 * @brief        :  处理删除文件命令
 * @para1      : pack命令包
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年3月12日 11:31:31
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
        char cFileName[MAX_PATH] = "\0";              //文件名
        char *pDest = NULL;
        char *pStr = NULL;
        char *pBuf = NULL;
        string sFn;
        bool bret = false;

        sysLoger(LDEBUG, "ControlSubAgent::handleDeleteFile: Recieved the request for uploading file!");

        //跳过DID和命令字部分
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

        //组装文件名
        sFn.clear();
        sFn = Manager::tmpfilepath + truncatPathFromFilename(cFileName);
        //全路径
        sFn = sFn.insert(0, Manager::getInstance()->getAppDir().toAscii().data());
        //删除文件
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
* 功能：交换包的目标SAID和AID
* @param pack 接收的控制包
*/
void ControlSubAgent::changePacketID(Packet* pack)
{
    int src_aid = pack->getSrc_aid();    //得到控制包源ID号
    int des_aid = pack->getDes_aid();
    int src_said = pack->getSrc_said();    //得到控制包源ID号
    int des_said = pack->getDes_said();
    pack->setDes_aid(src_aid);
    pack->setSrc_aid(des_aid);
    pack->setDes_said(src_said);
    pack->setSrc_said(des_said);
}
/**
* 功能：设置错误包信息
* @param pack 要发送的数据包
* @param srtErrMsg 需要回复的失败值
* @param size 失败信息的字节大小
*/
void ControlSubAgent::sendErrorAck(Packet* pack, char *strErrMsg, UI size)
{
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.28 [begin]*/
    /*  Modified brief: 增加出现异常时,释放数据包*/
    if(NULL == pack || NULL == strErrMsg)
    {
        sysLoger(LWARNING, "ControlSubAgent::sendErrorAck: NULL pointer encountered!");

        if(NULL != pack)
        {
            PacketManager::getInstance()->free(pack);
        }

        return;
    }

    //交换源AID和目标AID号
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

    //设置包大小
    pack->setSize(pBuf - pack->getData());                //更新包的大小

    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    //发送包
    Router::getInstance()->putpkt(pack);
    return;
}

/**
* 功能：设置成功包信息
* @param pack 要发送的数据包
*/
void ControlSubAgent::setSuccessPacket(Packet* pack)
{
    //交换源AID和目标AID号
    changePacketID(pack);
    pack->clearData();
    //设置DID
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
 * @brief        :  向客户端回复操作成功
 * @param     : pack 原消息包,带有目的和原aid
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年1月10日 11:31:11
 *
**/
void ControlSubAgent::sendSuccessPacket(Packet *pack)
{
    if(NULL == pack)
    {
        sysLoger(LWARNING, "ControlSubAgent::sendSuccessPacket: NULL pointer encountered!");
        return;
    }

    //交换源AID和目标AID号
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

    //设置包大小
    pack->setSize(pBuf - pack->getData());                //更新包的大小

    if(Manager::getInstance()->getLogFlag())
    {
        packLoger.write(SEND_PACK, *pack);
    }

    //发送包
    Router::getInstance()->putpkt(pack);
    return;
}

/**
 * @Funcname:  queryHostSerialPort
 * @brief        : 查询TS所在主机串口配置
 * @para1      :  pBuf 接收查询数据的缓冲
 * @para2      :  bufsize: 缓冲的字节大小
 * @return      :  查询成功则返回字节长度，否则返回-1
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2007年1月10日 11:57:38
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
    // 有子项，说明注册表被更改过
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

