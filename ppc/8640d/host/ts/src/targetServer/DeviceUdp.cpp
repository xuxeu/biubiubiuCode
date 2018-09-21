/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  deviceUdp.cpp
* @brief
*       ���ܣ�
*       <li> udp��ʽ���豸����</li>
*/

/************************ͷ�ļ�********************************/
#include ".\deviceudp.h"
#include "common.h"
/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/
DeviceUdp::DeviceUdp(void)
{
#ifdef WIN32
    WSADATA wsaData;
    WORD version = MAKEWORD(2, 0);
    int ret = WSAStartup(version, &wsaData);

    if (ret)
    {
        sysLoger(LWARNING, "DeviceUdp::Initialized the socket failure! FILE:%s, LINE:%d",
                 __MYFILE__, __LINE__);
    }

#endif
    pUdpSocket = INVALID_SOCKET;
}

DeviceUdp::~DeviceUdp(void)
{
    if(pUdpSocket != INVALID_SOCKET)
    {
        closesocket( pUdpSocket);
        pUdpSocket = INVALID_SOCKET;
    }
}
/**
* ͨ��������Ϣ���豸.
* @param ServerConfig* ������Ϣ
* @return true�򿪳ɹ�,false��ʧ��
*/
bool DeviceUdp::open(ServerConfig* config)
{
    if(NULL == config)
    {
        sysLoger(LWARNING, "DeviceUdp::open: Server Config is null, can't get device config from it!");
        return false;
    }

    if (pUdpSocket != INVALID_SOCKET)
    {
        closesocket( pUdpSocket);
    }

    UdpConfig* pUdpconfig = (UdpConfig*)config->getDeviceConfig();

    if(NULL == pUdpconfig)
    {
        sysLoger(LWARNING, "DeviceUdp::open: No usable UDP config!");
        return false;
    }

    IP = pUdpconfig->getIP().toAscii().data();
    port = pUdpconfig->getPort();
    retry = pUdpconfig->getRetry();
    timeout = pUdpconfig->getTimeout();

    pUdpSocket = socket(AF_INET, SOCK_DGRAM, 0); //�������ݰ���ʽ

    if(pUdpSocket < 0)
    {
        sysLoger(LWARNING, "DeviceUdp::open: Created the DGRAM socket failure!");
        return false;
    }

    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [begin]*/
    /*  Modified brief: ������ն���ΪlocalAddr*/
    memset(&localAddr, 0, sizeof(sockaddr_in));
    /*Modified by tangxp for BUG NO.0002555 on 2008.1.17 [end]*/
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    //sockaddrin.sin_addr.s_addr = inet_addr(IP.toAscii());
    localAddr.sin_port = htons(0);

    //�󶨱�����ַ,UDP��Ϊ�ͻ��˲���Ҫ�󶨶˿�
    if (bind(pUdpSocket, (sockaddr *)&localAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        sysLoger(LWARNING, "DeviceUdp::open: Bind the socket to local machine failure!");
        close();
        return false;
    }

    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP.toAscii());
    //�󶨱�����ַ,UDP��Ϊ�ͻ��˲���Ҫ�󶨶˿�
    addr.sin_port = htons(port);

    sysLoger(LDEBUG, "DeviceUdp::open: Open UDP device, IP:%s, Port:%d, Timeout:%d, Retry:%d",
             IP.toAscii().data(), port, timeout, retry);
    return true;
}
/**
* �ر��豸.
* @return true�رճɹ�,false�ر�ʧ��
*/
bool DeviceUdp::close()
{
	sysLoger(LWARNING, "DeviceUdp::close");
    if (pUdpSocket != INVALID_SOCKET)
    {
        closesocket(pUdpSocket);
        pUdpSocket = INVALID_SOCKET;
    }

    return true;
}

/**
* ���豸�ж�ȡ����.,��û�ж�ȡsize����һֱ�ȴ�
* @param pBuff ��ȡ��������Ļ�����
* @param size Ҫ��ȡ���ݵĴ�С
* @param timeout �ȴ�ʱ��
* @return int��ȡ������
*/
int  DeviceUdp::read(char* pBuff, int size)
{
    int recSize = 0;
    sockaddr_in sockAddr;
    int fromlen = sizeof(sockaddr_in);

    if(NULL == pBuff)
    {
        sysLoger(LWARNING, "DeviceUdp::read:Bad received data pointer, exit read!");
        return -1;
    }

    if(size <= 0)
    {
        sysLoger(LWARNING, "DeviceUdp::read:Invalid received data size [%d], exit read!", size);
        return -1;
    }

    recSize = recvfrom(pUdpSocket, pBuff, size, 0, (struct sockaddr*)&sockAddr, &fromlen);

    if (recSize == -1)
    {
        int error = WSAGetLastError();
        sysLoger(LWARNING, "DeviceUdp::read: Read the data error, errorNo: %d.",
                 error);

        if ((error == 10040) || (error == 10054))
        {
            return -error;    //���յ����ݰ���СС�������Ϣ�Ĵ�С
        }

        return -1;
    }

    return recSize;

}

/**
* ���豸д����,��û��д��size����һֱ�ȴ�
* @param pBuff д���ݵĻ�����
* @param size д���ݵĴ�С
*/
void DeviceUdp::write(char* pBuff, int size)
{
    int sendSize = 0;

    if(NULL == pBuff)
    {
        sysLoger(LWARNING, "DeviceUdp::write: Bad pointer of pBuff, exit write!");
        return;
    }

    if(size <= 0)
    {
        sysLoger(LWARNING, "DeviceUdp::write: Invalid data size [%d] for write, exit write!", size);
        return;
    }

    sendSize = sendto(pUdpSocket, pBuff, size, 0, (struct sockaddr*)&addr, sizeof( sockaddr_in));

    if (sendSize == -1)
    {
        int error = WSAGetLastError();
        sysLoger(LWARNING, "DeviceUdp::write: Write the data error, errorNo: %d. detial config.."\
                 "IP:%s, Port:%d, TimeOut:%d, Retry:%d",
                 error, IP.toAscii().data(), port, timeout, retry);

        if (error == 10040)
        {
            return ;    //���͵����ݰ��Ĵ�С���ڻ������Ĭ�ϴ�С
        }

        return ;
    }

    return ;
}

/**
* ��������socket
*/
bool DeviceUdp::connect()
{
    return true;
}
