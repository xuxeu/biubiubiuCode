/************************************************************************
*                �����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * $Log: rspTcpServer.cpp,v $
 * Revision 1.12  2008/03/19 10:25:11  guojc
 * Bug ���  	0002430
 * Bug ���� 	ts���������Windows���������̣��޷���Ӧ��ݼ�
 *
 * ���룺������
 * ���ӣ���Т��,������
 *
 * Revision 1.2  2008/03/19 09:03:00  guojc
 * Bug ���  	0002430
 * Bug ���� 	ts���������Windows���������̣��޷���Ӧ��ݼ�
 *
 * ���룺������
 * ���ӣ���Т��,������
 *
 * Revision 1.1  2008/02/04 05:33:49  zhangxc
 * ��Ʊ������ƣ���ACoreOS CVS����ֿ����
 * ��д��Ա��������
 * ������Ա������F
 * ����������������ACoreOS�ֿ���ȡ�����µ�Դ���룬������ύ���µĲֿ�Ŀ¼�ṹ�й���
 * ��һ���ύsrcģ�顣
 *
 * Revision 1.11  2007/06/21 11:37:28  guojc
 * 1. �޸���IS��simihost��ɾ��Ӳ���ϵ�Ĵ���
 * 2. ͳһ����IS������־����
 *
 * Revision 1.10  2007/06/07 09:59:47  guojc
 * IS���Ӷ�cache��ԭ�Ӳ�����֧��
 *
 * Revision 1.9  2007/05/29 03:27:41  guojc
 * ������������Թ������жϵ㣬���ܵ��¶��Ĵ���ʧ�ܵ�����
 *
 * Revision 1.8  2007/04/30 06:47:35  guojc
 * ������release�汾assert�������õ����⣬ͨ�����¶���assert���
 *
 * Revision 1.7  2007/04/30 01:38:30  guojc
 * ����־�ļ�����������
 * ��Ŀ�����λ�жϽ���������
 *
 * Revision 1.6  2007/04/20 07:10:50  guojc
 * �����˶�ICE DCC���ܵ�֧��
 * �����˶�Ŀ��帴λ���ܵ�֧��
 * �����˶��ڴ��ȿ����õ�֧��
 * �����˶Ի���ģ�������Լ��ݵ�֧��
 *
 * Revision 1.5  2007/04/16 02:20:53  guojc
 * ����ICE Server��Э��semihosting������ֵ�һЩ����
 *
 * Revision 1.4  2007/04/06 03:48:15  guojc
 * �����������ϱ��Ķ�̬����IDE�������޷���Ŀ���UDP�������������
 *
 * Revision 1.3  2007/04/02 02:56:53  guojc
 * �ύ���µ�IS���룬֧��������IDEʹ�ã�֧�������ļ�����
 *
 * Revision 1.2  2007/02/25 06:05:14  guojc
 * ������ICE Server��TRA Server��������־�ļ�������
 * ������TSAPI����Դ��벻ͬ��������
 *
 * Revision 1.1  2006/11/13 03:40:55  guojc
 * �ύts3.1����Դ��,��VC8�±���
 *
 * Revision 1.17  2006/10/31 03:54:24  zhangym
 * �ύICE Server�ڴ��Ż��㷨��Ĭ�Ϲر��Ż��㷨
 *
 * Revision 1.16  2006/10/23 01:59:48  zhangym
 * �ر�ICE Server���ڴ��Ż��㷨
 *
 * Revision 1.15  2006/09/30 01:52:53  tanjw
 * P�����޸�cpsr�Ĵ���ʱ,���Ӵ�����ģʽ���
 *
 * Revision 1.14  2006/09/28 09:01:46  tanjw
 * no message
 *
 * Revision 1.13  2006/07/17 09:30:01  tanjw
 * 1.����arm,x86 rsp��־�򿪺͹رչ���
 * 2.����arm,x86 rsp�ڴ��ȡ�Ż��㷨�򿪺͹رչ���
 *
 * Revision 1.11  2006/07/06 02:12:24  tanjw
 * �޸�gdb����is��,isִ���ϵ��ʼ�������ȴ�ʱ��
 *
 * Revision 1.9  2006/06/30 01:39:08  tanjw
 * ��gdb�������Ӻ��Զ���ʼ��Ŀ���
 *
 * Revision 1.8  2006/06/26 09:09:30  tanjw
 * 1.�޸�arm��x86�ڴ��ȡ����bug
 * 2.iceserverȥ��wsocket
 *
 * Revision 1.3  2006/06/06 12:08:11  pengh
 * no message
 *
 * Revision 1.2  2006/05/08 03:57:21  guojc
 * �����˳�ʼ��Э������������
 *
 * Revision 1.1.1.1  2006/04/19 06:29:13  guojc
 * no message
 *
 * Revision 1.7  2006/04/18 07:05:43  guojc
 * �����˶����������ڴ��ʧ�ܵ�����
 *
 * Revision 1.6  2006/04/18 06:24:27  guojc
 * �����˶�Ŀ���״̬�ĸ���
 *
 * Revision 1.5  2006/04/17 05:57:19  guojc
 * �����˱���ʱ���ֵľ�����Ϣ
 *
 * Revision 1.4  2006/04/14 02:02:28  guojc
 * �����˶�CP15Э�������Ĵ����޸ĵ�֧��
 *
 * Revision 1.3  2006/04/12 09:57:53  guojc
 * ȡ���ϵ������
 *
 * Revision 1.2  2006/04/06 12:36:02  guojc
 * �����˶�����ڴ�ʧ�ܵ�����
 *
 * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
 * TS��������
 *
 * Revision 1.5  2006/04/06 01:14:07  guojc
 * �����˴򿪹رյ����bug,ȷ����̬���ܹ���ȷ�˳�
 *
 * Revision 1.4  2006/03/30 01:29:56  guojc
 * �ڲ���ϵ�ָ��֮ǰ����˱��ػ��棬���Ȿ�ػ����жϵ�ָ������
 *
 * Revision 1.3  2006/03/29 08:38:13  guojc
 * �����˹�������ϵ�ָ�������
 *
 * Revision 1.1  2006/03/04 09:25:10  guojc
 * �״��ύICE Server��̬��Ĵ���
 *
 * Revision 1.0  2006/01/12 01:10:14  guojc
 * �����ļ���ʵ��RSP_TcpServer��Ļ����ӿڡ�
 */

/**
 * @file     rspTcpServer.cpp
 * @brief
 *    <li>���ܣ�ʵ��TCPͨ�ŷ�ʽ�Ļ���RSPЭ��ķ����� </li>
 * @author     ������
 * @date     2006-02-20
 * <p>���ţ�ϵͳ��
 */

/**************************** ���ò��� *********************************/
#include <cassert>
#include <iostream>            /// ʹ��IO���������
#include <utility>
#include "rspTcpServer.h"
#include "wsocket.h"
//#include "util.h"
#include "armConst.h"
#include "iceServerComm.h"
#include "common.h"
#include "XmlHandler.h"
#include "util.h"
/*************************** ǰ���������� ******************************/

#define PUTPKT_RETURN(flag)     {\
                                                            m_quitMutex.lock();\
                                                            m_txExitReady = TRUE;\
                                                            m_quitMutex.unlock();\
                                                            return flag;\
                                                         }

T_MODULE T_WORD Int2Hex(T_CHAR *cpBuf, T_WORD wNum, T_WORD wWidth);



int count = 0;
/**************************** ���岿�� *********************************/
T_MODULE T_CONST T_WORD PACKET_DATA_SIZE = 8192;
T_MODULE T_CONST T_WORD waCurUsrRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //�û�ģʽ�µļĴ���ӳ���
T_MODULE T_CONST T_WORD waCurFiqRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //FIQģʽ�µļĴ���ӳ���
T_MODULE T_CONST T_WORD waCurIrqRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 23, 24, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //IRQģʽ�µļĴ���ӳ���
T_MODULE T_CONST T_WORD waCurSvcRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 27, 28, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //SVCģʽ�µļĴ���ӳ���
T_MODULE T_CONST T_WORD waCurAbtRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 25, 26, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //ABTģʽ�µļĴ���ӳ���
T_MODULE T_CONST T_WORD waCurUndRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 29, 30, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //UNDģʽ�µļĴ���ӳ���
T_MODULE T_CONST T_WORD waCurSysRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //SYSģʽ�µļĴ���ӳ���

T_CONST T_CHAR vcaHexchars[] = "0123456789abcdef";
/**************************** ʵ�ֲ��� *********************************/

/**
 * @brief
 *    ���캯��
 */
RSP_TcpServer::RSP_TcpServer()
{
    //��ʼ����������ϵ����Դ���
    m_retryTimes = 5;

    //����������־��¼
    //m_errLogger = SystemLogger::getInstance();

    //Ĭ�ϴ��ڴ��Ż��㷨
    SetMop(FALSE);
    //Ĭ�Ϲر����ݰ���־����
#ifdef IS_PACK_LOG
    SetLog(TRUE);
#endif
    m_sClient = NULL;

    m_waitForIce = FALSE;
    m_hardBp = FALSE;
    m_iceEndian = FALSE;
    //��ʱ��ʵ��
    m_lambdaIce.SetServer(this);

    m_rxExitReady = FALSE;
    m_localExitReady = FALSE;
    m_txExitReady = TRUE;

    m_validDeleWp = FALSE;
    m_validForWp = FALSE;
    memset(m_ICEName, 0, sizeof(m_ICEName));
    SetTargetStatus(RSP_ArmConst::CORE_POWER_OFF);
    
    m_SimiRegisterFlag = FALSE;
    m_SimiPCAddr = 0;
    m_NeedReplyS05 = TRUE;
    m_targetType = LAMBDA_JTAG_ICE;
}

/**
 * @brief
 *    ��������
 */
RSP_TcpServer::~RSP_TcpServer()
{
    //�ͷ���Դ
    //1.�ڴ���Դ
    RefreshMemory();

    //2.��ʼ���ڴ���Դ
    while(!m_memCfg.empty())
    {
        pair<T_WORD, T_WORD> *mem = m_memCfg.takeFirst();
        if(NULL != mem)
        {
            delete mem;
        }
    }

    //3. ���ݰ��������
    while(!m_packetQueue.empty())
    {
        RSP_Packet *pack = m_packetQueue.takeFirst();
        if(NULL != pack)
        {
            delete pack;
        }
    }

    //4.
    if(m_sClient != NULL){
        delete m_sClient;
    }
    m_sClient = NULL;

#ifdef IS_PACK_LOG
    m_logger.deleteLogFile();
#endif
}

/**
* @brief
*    �򿪲���������Э��������õ���Ҫ�Ĳ�����Ϣ
* @param[in] id ��Ӧ���������󶨵�AID��
* @param[in] cpBuf ��������ָ��,���������Դ��н���������Ĳ���
* @return �����ɹ����ؼ����Ķ˿ںţ�ʧ�ܷ���FAIL
*/
int RSP_TcpServer::Open(T_WORD wId, T_CHAR *cpBuf)
{
    /*buf�еĸ�ʽ
    *     ICEID, �Ƿ�֧��˽����չ, �����ļ��� 
    */
    T_CHAR *ptr = cpBuf;
    //iceaid
    T_CHAR *pdest = strstr(ptr,CONTROL_FLAG);        //�Ƿ��зָ��
    if(NULL == pdest)
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Lost the ICEID part!");
        return FAIL;
    }
    Hex2int(&ptr, &m_iceId);
    //iceName
    ptr = pdest + CONTROL_FLAG_LEN;
    pdest = strstr(ptr,CONTROL_FLAG);        //�Ƿ��зָ��
    if(NULL == pdest)
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Lost the ICE Name part!");
        return FAIL;
    }
    memcpy(m_ICEName, ptr, pdest - ptr);
    //isSupportPrivateProtocol
    ptr = pdest + CONTROL_FLAG_LEN;
    pdest = strstr(ptr,CONTROL_FLAG);        //�Ƿ��зָ��
    if(NULL == pdest)
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Lost the isSupportPrivateProtocol part!");
        return FAIL;
    }
    Hex2int(&ptr, (T_WORD*)(&m_isSupportPrivateProtocol));
    m_isSupportPrivateProtocol = m_isSupportPrivateProtocol?TRUE:FALSE;
    //configfile
    ptr = pdest + CONTROL_FLAG_LEN;
    pdest = strstr(ptr,CONTROL_FLAG);        //�Ƿ��зָ��
    if(NULL == pdest)
    {
        /*���ȱ�ٷָ���������Ҳ��Ϊ������*/
        if(strlen(ptr) > 0)
        {
            pdest = ptr + strlen(ptr);
        }
        else
        {
            sysLoger(LWARNING, "RSP_TcpServer::Open: Lost the config file part!");
            return FAIL;
        }
    }
    char configfile[MAX_PATH] = "\0";
    if(pdest - ptr > MAX_PATH)
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Invalid config file name!");
        return FAIL;
    }
    memcpy(configfile, ptr, (T_UWORD)(pdest - ptr));
    m_CfgFile = QString::fromLocal8Bit(configfile);

    //���������ļ��Ľ���
    m_CfgFile = QString::fromLocal8Bit(m_CfgFile.toAscii().data());
    //�����Ƿ������·��������ȷ���Ƿ��Ǳ��������ļ�����Զ��
    if(!m_CfgFile.contains("\\"))
    {
        //�����ļ�������TS��LoadFileĿ¼��
        QString cfgFileName;
        char path[MAX_PATH] = "\0";
        GetCurrentDirectory(MAX_PATH, path);
        cfgFileName = QString::fromLocal8Bit(path);
        cfgFileName += QString(IS_CFGFILE) + m_CfgFile;
        m_CfgFile = cfgFileName;
    }
    sysLoger(LDEBUG, "RSP_TcpServer::Open: Start to prase config file [%s].", m_CfgFile.toAscii().data());
    if(!parseConfig())
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Parse ice config file failure!");
        return FAIL;
    }

    m_lambdaIce.SetServer(this);

    //���ݶ˿ں����������������˿ں��Ƿ�ռ��
    if (INVALID_SOCKET == m_serverSocket.m_sock)
    {
            if (!m_serverSocket.createServerSocket(0))
            {
                return FAIL;
            }
    }
    else
    {
        return FAIL;
    }

    m_port = m_serverSocket.GetLocalPort();
    //QString str = QString("�����˿ں�:%1").arg(m_port);
    //m_errLogger->write(1, QString("ִ������"), str);

    
    //�����̣߳�����������
    this->start();  //������
    return m_port;
}

T_BOOL RSP_TcpServer::GetIceEndian()
{
    return m_iceEndian;
}

T_BOOL RSP_TcpServer::GetTargetEndian()
{
    return m_targetEndian;
}

/**
* @brief
*    �رղ������رշ�����
*/
T_VOID RSP_TcpServer::Close()
{
    m_sktMutex.lock();
    //�ر��߳�
    CloseThread();

    //����RX�̣߳����������˳�
    WakeUp();

    //�����SOCKETû�йر�,�ر�֮
    //---------------------------------
    //2006.06.19 ��������SOCKET���б���������ر�˲��SOCKETΪ��ָ������쳣
    m_clientMutex.lock();
    if(m_sClient != NULL)
    {
        m_sClient->Close();
    }
    m_clientMutex.unlock();
    m_sktMutex.unlock();

    //�رշ�����
    m_serverSocket.Close();

    //�ȴ��߳��˳�
    m_quitMutex.lock();
    while (m_rxExitReady == FALSE || m_localExitReady == FALSE || m_txExitReady == FALSE)
    {
        m_quitReady.wait(&m_quitMutex, 1000);    //�ȴ�5��ĳ�ʱ
    }
    m_quitMutex.unlock();


    #ifdef IS_PACK_LOG
    m_logger.close();
    #endif
}

T_VOID RSP_TcpServer::HandleIDECmd(T_HWORD hwSrcAid,T_HWORD hwSrcSaid, T_CHAR* cpBuf,T_WORD wSize)
{
#if 0
    //����������Ϸ���
    if(wSize < 4 || cpBuf[0] != 'r' || cpBuf[1] != 's' || cpBuf[2] != 'p' || cpBuf[3] != '-')
    {
        SendIDEResult("error:input unkown cmd!", strlen("error:input unkown cmd!"), hwSrcAid, hwSrcSaid);
        return;
    }
#endif
    //��¼��Ϣ��Դ
    m_MtxMsgSrc.lock();
    m_CurCmdSource = TSAPI_DATA;
    m_CurSrcAid = hwSrcAid;
    m_CurSrcSaid = hwSrcSaid;
    m_MtxMsgSrc.unlock();
    
    switch(cpBuf[0])
    {
#if 0
    case 'l':
        {
            //��־ѡ�����ã�Ĭ�Ϲر�
            if(wSize < 6 || cpBuf[0] != 'l' || cpBuf[1] != 'o' || cpBuf[2] != 'g' || cpBuf[3] != '-')
            {
                SendIDEResult("����δ֪����־�������", strlen("����δ֪����־�������"), hwSrcAid, hwSrcSaid);
                break;
            }
            
            if(cpBuf[5] == 'n')
            {
                sysLoger(LINFO, "HandleIDECmd:Open the log success!");
                SendIDEResult("��־�򿪳ɹ���", strlen("��־�򿪳ɹ���"), hwSrcAid, hwSrcSaid);
            }
            else if(cpBuf[5] == 'f')
            {
                sysLoger(LINFO, "HandleIDECmd:Close the log success!");
                SendIDEResult("��־�رճɹ���", strlen("��־�رճɹ���"), hwSrcAid, hwSrcSaid);
            }
            else
            {
                sysLoger(LINFO, "HandleIDECmd:Unknow command packet!");
                SendIDEResult("����δ֪����־�������", strlen("����δ֪����־�������"), hwSrcAid, hwSrcSaid);
            }
            break;
        }
    case 'm':
        {
            //�ڴ��Ż�ѡ���㷨���ã�Ĭ�ϴ�
            if(wSize < 6 || cpBuf[0] != 'm' || cpBuf[1] != 'o' || cpBuf[2] != 'a' || cpBuf[3] != '-')
            {
                SendIDEResult("����δ֪���ڴ��Ż��������", strlen("����δ֪���ڴ��Ż��������"), hwSrcAid, hwSrcSaid);
                break;
            }
            
            if(cpBuf[5] == 'n')
            {
                //���ڴ��Ż��㷨
                int ret = SetMop(TRUE);
                if(SUCC == ret)
                {
                    SendIDEResult("�ڴ��Ż��㷨�򿪳ɹ���", strlen("�ڴ��Ż��㷨�򿪳ɹ���"), hwSrcAid, hwSrcSaid);
                }
                else
                {
                    SendIDEResult("�ڴ��Ż��㷨��ʧ�ܣ�", strlen("�ڴ��Ż��㷨��ʧ�ܣ�"), hwSrcAid, hwSrcSaid);
                }

            }
            else if(cpBuf[5] == 'f')
            {
                //�ر��ڴ��Ż��㷨
                int ret = SetMop(FALSE);
                if(SUCC == ret)
                {
                    SendIDEResult("�ڴ��Ż��㷨�رճɹ���", strlen("�ڴ��Ż��㷨�رճɹ���"), hwSrcAid, hwSrcSaid);
                }
                else
                {
                    SendIDEResult("�ڴ��Ż��㷨�ر�ʧ�ܣ�", strlen("�ڴ��Ż��㷨�ر�ʧ�ܣ�"), hwSrcAid, hwSrcSaid);
                }
            }
            else
            {
                SendIDEResult("����δ֪���ڴ��Ż��������", strlen("����δ֪���ڴ��Ż��������"), hwSrcAid, hwSrcSaid);
            }

            break;
        }
#endif

    case 'i':
        {
            /*��ѯĿ�����CPU IDCODE*/
            sysLoger(LINFO, "HandleIDECmd:Recieved Query target CPU ID Code command!");
            PushBackCmd(RSP_ArmConst::CMD_QUERY_TARGET_CPUID);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_QUER_TARGET_CPUID);
            //װ���������
            QList<T_VOID*> *queryPara = new QList<T_VOID*>();
            QList<T_VOID*> *ReplyDest = new QList<T_VOID*>();
            T_HWORD *pSrcAid = new T_HWORD(hwSrcAid);
            T_HWORD *pSrcSAid = new T_HWORD(hwSrcSaid);
            ReplyDest->push_back(pSrcAid);
            ReplyDest->push_back(pSrcSAid);                
            m_atomParas.push_back(queryPara);
            m_atomParas.push_back(ReplyDest);
        }
        break;

    case 'j':
        {
            /*��ѯ���мĴ�����ֵ*/
            char buf[PACKET_DATA_SIZE] = "\0";
            char *pBuf = buf;
            sysLoger(LINFO, "HandleIDECmd:Recieved Query all Registers command!");
            for(int i = 0; i < RSP_ArmConst::ARM_REG_NUM; i++)
            {
                pBuf += int2hex(pBuf, 10, m_currentRegs[i]);
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
                pBuf += CONTROL_FLAG_LEN;
            }
            //����
            SendIDEResult(buf, (T_WORD)(pBuf - buf), hwSrcAid, hwSrcSaid);
        }
        break;

    case 'J':
        {
            /*�������мĴ�����ֵ*/
            QString qsRegValues = QString::fromLocal8Bit(&cpBuf[1], wSize-1);
            QString qsSingleValue;
            T_WORD value = 0, idx = 0, controlCharCount = 0; 
            bool ok = false;
            sysLoger(LINFO, "HandleIDECmd:Recieved Set all Registers value command!");
            //�ж����ݰ��Ƿ�Ϸ�(������36���Ĵ�����ֵ)
            for(int i = 0 ; i < qsRegValues.length(); i++)
            {
                if((idx = qsRegValues.indexOf(CONTROL_FLAG, idx+1))>0)
                    controlCharCount++;
                else
                    break;
            }
            if(controlCharCount < 35)
            {
                sysLoger(LINFO, "HandleIDECmd:set register value, data is invaid!");
                SendIDEResult(ERR_UNKONW_DATA, FAIL_ACK_LEN, hwSrcAid, hwSrcSaid);
                break;
            }
            char *pBuf = NULL;
            idx = 0;
            for(int i = 0; (i < RSP_ArmConst::ARM_REG_NUM)&&(idx < qsRegValues.length()); i++)
            {
                qsSingleValue = qsRegValues.section(CONTROL_FLAG, idx, idx);
                pBuf = qsSingleValue.toAscii().data();
                hex2int(&pBuf, &value);                
                sysLoger(LINFO, "HandleIDECmd: Set Register's [regno= %d] value; old value:%x, new value:%x",
                                                i, m_currentRegs[i], value);
                m_currentRegs[i] = value;
                idx++;
            }
            SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
        }
        break;
        
    case 'Y':
            //�յ�simihost��ע������
            handleSimiHostCommand(cpBuf, wSize, hwSrcAid, hwSrcSaid);            
        break;
        
    default:      //����semihost��һЩ��д�ڴ�����
        {
            T_CHAR *pData = cpBuf;
            T_WORD dataSize = wSize;
            if(MessageHandler(pData, dataSize) == IS_NEED_EXC_CMD)
            {
                ExcuteCommand(NULL, 0, TRUE);
            }
            
            break;
        }
    }
    return;
}
T_VOID RSP_TcpServer::SendIDEResult(T_CHAR *cpBuf, T_WORD wSize, T_HWORD hwSrcAid,T_HWORD hwSrcSaid)
{
    //�������һ���µĻ����
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //��װЭ�黺�壬��ʽ Maddr,size,data
    memcpy(pBuf, cpBuf, wSize);
    pBuf += wSize;

    pack->SetSize(pBuf - pack->GetData());

    pack->SetDesAid(hwSrcAid);
    pack->SetDesSaid(hwSrcSaid);
    pack->SetSrcSaid(1);
    //�����ɣ����뵽������ȥ
    Enqueue(pack);
}
/**
* @brief
*    �ص��������÷������յ���ICE�����ݽ��н������ƶ��������Ĺ���
* @param[in] hwDesSaid ����������Ӧ�Ķ����ỰID��
* @param[in] hwSrcAid ICE��ID��
* @param[in] hwSrcSaid ICE�ϻỰ��ID��
* @param[in] cpBuf ���ݻ���ָ��,���������Դ��н���������Ĳ���
* @param[in] wSize ���ݻ���Ĵ�С
* @return SUCC����FAIL
*/
int RSP_TcpServer::Putpkt(T_HWORD hwDesSaid,T_HWORD hwSrcAid,T_HWORD hwSrcSaid,T_CHAR* cpBuf,T_WORD wSize)
{
    m_quitMutex.lock();
    m_txExitReady = FALSE;
    m_quitMutex.unlock();

#ifdef IS_PACK_LOG
    if(IsLog())
    {
        m_logger.write(RECV_PACK, cpBuf, wSize);
    }
#endif
    sysLoger(LDEBUG, "RSP_TcpServer::Putpkt: TX thread runing!");
    //--------------------------------------------------------------------------
    //2006.07.14 by guojc
    //���ӽ���IDE���������Ӧ���ù��ܣ�ͨ���ж�desSaidΪ1����ʾ�����Ƿ��͸����ô����
    //Ŀǰ�����ṩ�����ù�����Ҫ������־���ܵĴ򿪹رգ��ڴ��Ż����ܵĴ򿪹ر�
    if(hwDesSaid == 1)
    {        
        HandleIDECmd(hwSrcAid, hwSrcSaid, cpBuf, wSize);
        
        PUTPKT_RETURN(SUCC)
    }
    
    //-------------------------------------------------------------------------

    if(hwSrcAid != m_iceId)
    {
        //�յ�IDE��S05����
        if(cpBuf[0] == 'S' && cpBuf[1] == '0' && cpBuf[2] == '5' && GetTargetStatus() == RSP_ArmConst::CORE_RUNNING)
        {
            //������������л���,��ʱ����ӦIDE������
            if(CmdSize() != 0)
            {
                PUTPKT_RETURN(SUCC)
            }
            sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S05 from IDE or other tool via target server!");
            //�����ڴ�������Ч,����ڴ�
            RefreshMemory();

            //��������״̬,ǿ��ֹͣĿ���
            //װ����������
            SetTargetStatus(RSP_ArmConst::CORE_DEBUG_BKWTPT);  //��״̬��Ϊֹͣ
            PushBackCmd(RSP_ArmConst::CMD_STOP_TRT);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
            PushBackCmd(RSP_ArmConst::CMD_READ_REG);
            PushBackCmd(RSP_ArmConst::CMD_UPDATE_READ_REG);
            PushBackCmd(RSP_ArmConst::CMD_REPLY_S05);
            //װ�ز���,��Ϊ������Ҫ����������ֻ��Ҫ�����������
            //1. ֹͣĿ���,�޲���
            QList<T_VOID*> *stp = new QList<T_VOID*>();
            m_atomParas.push_back(stp);

            //2. ����ִ�н��,�޲���
            QList<T_VOID*> *rwm = new QList<T_VOID*>();
            m_atomParas.push_back(rwm);

            QList<T_VOID*> *readReg = new QList<T_VOID*>();
            QList<T_VOID*> *replyReg = new QList<T_VOID*>();
            QList<T_VOID*> *replyS05 = new QList<T_VOID*>();
            m_atomParas.push_back(readReg);
            m_atomParas.push_back(replyReg);
            m_atomParas.push_back(replyS05);
            ExcuteCommand(cpBuf, wSize, TRUE);
            
            PUTPKT_RETURN(SUCC)
        }
    }

    //FIXME:ICE��Ŀ������磬�ϵ����ʱ�򻹻ᷢ��S05�����������IS�ڶ�ȡ�Ĵ�����ʱ����յ�PN0������IS����
    //������⻹û�н��
    //������������Ƿ�Ϊ�ź�
    if(cpBuf[0] == 'S' && cpBuf[1] == '0')
    {
        switch(cpBuf[2])
        {
        case '5'://Ŀ���ֹͣ,Ӧ��װ���������У�1. ���Ĵ��� 2.���¼Ĵ��� 3.����S05��GDB
            {
                if(CmdSize() != 0)
                {
                    sysLoger(LDEBUG, "RSP_TcpServer::Putpkt:Recieved S05 from ICE , but cmd queue have cmds recived!");
                    PUTPKT_RETURN(SUCC)
                }
                if(m_targetStatus != RSP_ArmConst::CORE_RUNNING)
                {
                    sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S05 from ICE in the invalid state!");
                    PUTPKT_RETURN(SUCC)
                }
                sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S05 from ICE !");
                //�����ڴ�������Ч,����ڴ�
                RefreshMemory();
                //װ����������
                SetTargetStatus(RSP_ArmConst::CORE_DEBUG_BKWTPT);  //��״̬��Ϊֹͣ
                PushBackCmd(RSP_ArmConst::CMD_READ_REG);
                PushBackCmd(RSP_ArmConst::CMD_UPDATE_READ_REG);
                QList<T_VOID*> *readReg = new QList<T_VOID*>();
                QList<T_VOID*> *replyReg = new QList<T_VOID*>();
                m_atomParas.push_back(readReg);
                m_atomParas.push_back(replyReg);
                
                //--------------------------------------------------------------------------------------
                //���ϣ�2006.06.15 ���ֵ��Թ����в��뵽�ڴ�Ķϵ�û����S05��д��ȥ��������GDBû�з�ɾ���ϵ�����
                //��ȻҲ���ų���IS�ڲ��������
                //�޸ķ�����Ŀ���ֹͣ��IS����������ϵ��ַ��ʵ�����������ָ�
                //�ָ��ϵ�
                if(!m_bpMap.empty())
                {
                    QMapIterator<T_WORD, T_WORD> i(m_bpMap);
                    while (i.hasNext())
                    {
                        i.next();
                        T_WORD armAddr = i.key();
                        T_WORD value = i.value();

                        //װ��ɾ������ϵ�ָ��
                        //���ڸöϵ㣬�Ӷϵ������ȡ���öϵ�
                        T_WORD *pArmAddr = new T_WORD(armAddr);
                        T_WORD *pValue = new T_WORD(value);
                        T_WORD *pWidth = new T_WORD(32);
                        //װ���������� 1.д�ڴ� 2. ������� 3. �ظ�OK
                        PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
                        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);

                        //װ�ز���
                        //1. д�ڴ����
                        QList<T_VOID*> *pWriteMem = new QList<T_VOID*>();
                        pWriteMem->push_back(pArmAddr);
                        pWriteMem->push_back(pValue);
                        pWriteMem->push_back(pWidth);
                        m_atomParas.push_back(pWriteMem);

                        //2. �������û�в���
                        QList<T_VOID*> *pCheckWriteMem = new QList<T_VOID*>();
                        m_atomParas.push_back(pCheckWriteMem);
                    }

                    
                }

                //��նϵ�map
                m_bpMap.clear();
                /*add for simihost, todo: analy the pc rsgister, if the value equal the registered address, 
                then  send the SWI to simihost*/
                if(m_SimiRegisterFlag == TRUE)
                {
                    sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S05, push the CMD_ANALY_SIMIHOST_BREAK cmd!");
                    PushBackCmd(RSP_ArmConst::CMD_ANALY_SIMIHOST_BREAK);
                    QList<T_VOID*> *replySimihost = new QList<T_VOID*>();
                    m_atomParas.push_back(replySimihost);
                }

                //-------------------------------------------------------------------------------------------
                //װ�ز���,��Ϊ������Ҫ����������ֻ��Ҫ�����������
                PushBackCmd(RSP_ArmConst::CMD_REPLY_S05);
                QList<T_VOID*> *replyS05 = new QList<T_VOID*>();
                m_atomParas.push_back(replyS05);
                ExcuteCommand(cpBuf, wSize, TRUE);
                break;
            }
        case '6'://�����¼�,Ӧ�ü�鵱ǰ�����������Ƿ���ֵ��������ȡ�����һ������ִ�С�����Ŀ���״̬��Ϊ������
            {
                if(!m_atomCmds.isEmpty())
                {
                    ExcuteCommand(cpBuf, wSize, FALSE);
                }
                sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S06 from ICE !");
                SetTargetStatus(RSP_ArmConst::CORE_POWER_OFF);
                //���֧��˽����չ�����Զ��ϱ������¼�
                if(m_isSupportPrivateProtocol)
                {
                    //����������,IS��TC�Զ��ϱ����µ��¼�,������Ҫ��SOCKET���ݷ���
                    m_CurCmdSource = SOCKET_DATA;
                    PutData("Poff", strlen("Poff"));                
                }                
                break;
            }
        case '7'://�ϵ��¼�,Ӧ�ò�ѯĿ���״̬�����ݻظ�����ƶ���ʼ���¼�            {
            {    //����ڴ�
                RefreshMemory();
                sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S07 from ICE !");
                //���֧��˽����չ�����Զ��ϱ��ϵ��¼�
                if(m_isSupportPrivateProtocol)
                {
                    //����������,IS��TC�Զ��ϱ����µ��¼�,������Ҫ��SOCKET���ݷ���
                    m_CurCmdSource = SOCKET_DATA;
                    PutData("Pon", strlen("Pon"));
                }
                //��ѯ״̬
                InitTarget();
                QueryTarget();
                break;
            }

        default:
            //FIXME:��¼������Ϣ
            PUTPKT_RETURN(SUCC)
        }
    }
    else
    if(cpBuf[0] == 'P')
    {
        //�ϵ�,��ʼ��Ŀ���,ִ����������
        sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved %s from ICE !", cpBuf);
        if(cpBuf[1] == 'N')
        {
            InitTarget();
            ExcuteCommand(cpBuf, wSize, TRUE);
        }
        else
        {
            SetTargetStatus(RSP_ArmConst::CORE_POWER_OFF);
        }

    }
    else
    {
        //����һ������Ļظ����ƶ���һ�������ִ��
        ExcuteCommand(cpBuf, wSize, TRUE);
    }

    PUTPKT_RETURN(SUCC)
    sysLoger(LDEBUG, "RSP_TcpServer::Putpkt: TX thread exiting!");
}

/**
* @brief
*    �ص��������÷��������ڷ�����ά���Ļ�����н��հ����յ����ݺ�\n
*    �����ݿ������������ṩ�Ļ������󷵻�\n
* @param[in] hwpDesAid  ICE��ͨ��ID��
* @param[in] hwpDesSaid ICE�Ͼ���Ự��ID��
* @param[in] hwpSrcSaid Rsp�������Ķ���ID��
* @param     cpBuf    �������ṩ�����ݻ���
* @param[in] wSize    �������ṩ�Ļ������Ĵ�С
* @return  ʵ��Ҫ�������ݵĴ�С
*/
int RSP_TcpServer::Getpkt(T_HWORD *hwpDesAid,T_HWORD *hwpDesSaid,T_HWORD *hwpSrcSaid,T_CHAR* cpBuf,T_WORD wSize)
{
    //�Ӷ�����ȡ��һ����
    RSP_Packet *pack = NULL;
    sysLoger(LDEBUG, "RSP_TcpServer::Getpkt: RX thread starting!");
    pack = Dequeue();
    while(NULL == pack)
    {
        //����Ϊ�գ������ȴ�
        WaitUp();
        if(m_runFlag == FALSE)
        {
            m_quitMutex.lock();
            m_rxExitReady = TRUE;
            m_quitReady.wakeAll();
            m_quitMutex.unlock();
            return FAIL;
        }
        pack = Dequeue();
    }
    
#ifdef IS_PACK_LOG
    if(IsLog())
    {
        m_logger.write(SEND_PACK, pack->GetData(), pack->GetSize());
    }
#endif

    //������ݴ�С�������ṩ�Ļ�������С
    if(pack->GetSize() > wSize)
    {
        m_quitMutex.lock();
        m_rxExitReady = TRUE;
        m_quitReady.wakeAll();
        m_quitMutex.unlock();
        return FAIL;
    }

    //�����ݿ������û�����
    if(pack->GetDesAid() == 0)
    {
        *hwpDesAid = m_iceId;
        *hwpDesSaid = m_sessId;
        *hwpSrcSaid = 0;
    }
    else
    {
        *hwpDesAid = pack->GetDesAid();
        *hwpDesSaid = pack->GetDesSaid();
        *hwpSrcSaid = pack->GetSrcSaid();
    }
    memcpy(cpBuf, pack->GetData(), pack->GetSize());

    int returnSize = pack->GetSize();

    //�ͷŻ����
    RSP_PacketManager::GetInstance()->Free(pack);
    sysLoger(LDEBUG, "RSP_TcpServer::Getpkt: RX thread exiting!");
    return returnSize;
}

/**
* @brief
*    ����Ŀ���״̬����������
* @param[in] wStatus Ŀ���״̬
*/
T_VOID RSP_TcpServer::SetTargetStatus(T_WORD wStatus)
{
    m_statusLock.lock();
    sysLoger(LINFO, "RSP_TcpServer::SetTargetStatus:set target status to %d", wStatus);
    m_targetStatus = wStatus;
    m_statusLock.unlock();
}

/**
* @brief
*    �õ�Ŀ���״̬����������
* @param[in] wStatus Ŀ���״̬
*/
T_WORD RSP_TcpServer::GetTargetStatus()
{
    T_WORD status = 0;
    m_statusLock.lock();
    status = m_targetStatus;
    m_statusLock.unlock();

    return status;
}

/**
* @brief
*    RX�߳���������
*/
T_VOID RSP_TcpServer::WaitUp()
{
    m_packMutex.lock();
    m_queueNotFull.wait(&m_packMutex);    //�ȴ�5��ĳ�ʱ
    m_packMutex.unlock();
}

/**
* @brief
*    �������̻߳���RX����
*/
T_VOID RSP_TcpServer::WakeUp()
{
    m_packMutex.lock();
    m_queueNotFull.wakeAll();
    m_packMutex.unlock();
}

/**
* @brief
*    ����������뵽������ά���Ļ��������
* @param[in] tpPack    �����
*/
T_VOID RSP_TcpServer::Enqueue(RSP_Packet* tpPack)
{
    m_packMutex.lock();
    m_packetQueue.append(tpPack);
    m_queueNotFull.wakeAll();
    m_packMutex.unlock();
}

/*
* @brief
*    �ӻ����������ȡ��һ�������\n
* @return  �������ݰ�
*/
RSP_Packet* RSP_TcpServer::Dequeue()
{
    RSP_Packet* pack = NULL;
    if (m_packetQueue.size() > 0)
    {
        m_packMutex.lock();
        pack = m_packetQueue.first ();
        m_packetQueue.pop_front();
        m_packMutex.unlock();
    }
    return pack;
}

/*
* @brief
*    �رշ������̷߳���
*/
T_VOID RSP_TcpServer::CloseThread()
{
    m_runFlag = FALSE;
}

/**
* @brief
*    �������߳�������ڷ���
*/
T_VOID RSP_TcpServer::run()
{
    //���建��
    T_CHAR recvBuf[PACKET_DATA_SIZE];
    bool bLocked = TRUE;

    m_sktMutex.lock();

#ifdef IS_PACK_LOG
    QString strPort = QString("%1").arg(m_port);
    QString name = strPort.append("IS.log");
    m_logger.open(name.toAscii().data());
#endif

    m_runFlag = TRUE;

    sysLoger(LDEBUG, "RSP_TcpServer::run: ICE Server sarting...!");
    
    SetTargetStatus(RSP_ArmConst::CORE_POWER_OFF);
    if(LAMBDA_JTAG_ICE == m_targetType)
    {
        //ɾ��ԭ���ĻỰ
        PushBackCmd(RSP_ArmConst::CMD_DELETE_SESSION);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_DELETE_SESSION);
        //װ�ز���
        QList<T_VOID*> *deleteSessionPara = new QList<T_VOID*>();
        QList<T_VOID*> *analydeleteSessionResult = new QList<T_VOID*>();
        m_atomParas.push_back(deleteSessionPara);
        m_atomParas.push_back(analydeleteSessionResult);
    }
    
    //�����Ự��    
    PushBackCmd(RSP_ArmConst::CMD_CREATE_SESSION);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_CREATE_SESSION);
    //װ�ز���
    QList<T_VOID*> *createSessionPara = new QList<T_VOID*>();
    QList<T_VOID*> *analySessionResult = new QList<T_VOID*>();
    int createSessionParaLen = m_IceBuf.length();
    createSessionPara->push_back(m_IceBuf.toAscii().data());
    createSessionPara->push_back((T_VOID*)(&createSessionParaLen));
    m_atomParas.push_back(createSessionPara);
    m_atomParas.push_back(analySessionResult);

    //��ѯICE��С��
    PushBackCmd(RSP_ArmConst::CMD_QUERY_ICEENDIAN);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_QUERY_ICEENDIAN);
    //PushBackCmd(RSP_ArmConst::CMD_UPDATE_ICEENDIAN);
    //װ�ز���
    QList<T_VOID*> *queryendian = new QList<T_VOID*>();
    QList<T_VOID*> *analyResult = new QList<T_VOID*>();
    m_atomParas.push_back(queryendian);
    m_atomParas.push_back(analyResult);

    //��ѯĿ���״̬
    PushBackCmd(RSP_ArmConst::CMD_QUERY_TRT);
    //װ�ز���
    QList<T_VOID*> *queryTargetPara = new QList<T_VOID*>();
    m_atomParas.push_back(queryTargetPara);
    
    sysLoger(LINFO, "RSP_TcpServer::run: Starting to delete session, create session, queryICE endian!");
    
    //ִ����������ƶ�����
    ExcuteCommand(NULL, 0, TRUE);
  
    while(m_runFlag)
    {
        //��������������
        if( bLocked != TRUE )
        {
            m_sktMutex.lock();
            bLocked = TRUE;
        }

        m_sClient = new WSocket;

        m_sktMutex.unlock();
        bLocked = FALSE;

        if (!m_serverSocket.acceptSocket(*m_sClient))
        {
            sysLoger(LINFO, "RSP_TcpServer::run: start to listen failure!");

            break;
        }
        else
        {
        
            sysLoger(LINFO, "RSP_TcpServer::run: Detected a client connected !");

            //��������ִ���ϵ��ʼ��         
            this->usleep(100 * 1000);
            if(!m_isSupportPrivateProtocol)
            {                
                T_WORD count = 40;
                T_WORD targetStatus = GetTargetStatus();
                while(CmdSize() != 0 || RSP_ArmConst::CORE_POWER_OFF == targetStatus || RSP_ArmConst::CORE_RUNNING == targetStatus)
                {
                    this->usleep(500 * 1000);
                    if(!count--)
                        break;
                    targetStatus = GetTargetStatus();    
                }
            }
        }

    

        while(m_runFlag)
        {
            //���ӵ�����������������
            memset(recvBuf, 0, sizeof(recvBuf));
            int ret = GetData(recvBuf, PACKET_DATA_SIZE);

            //��־
            #ifdef IS_PACK_LOG
            if(TRUE == IsLog())
            {
                m_logger.write(RECV_PACK, recvBuf, ret);
            }            
            #endif

            if(FAIL == ret)
            {
                //�������ӶϿ������¼�������
                if(NULL != m_sClient)
                {
                    sysLoger(LINFO, "RSP_TcpServer::run: a client [GDB] linker disconnected!");
                    //�������б���
                    DeleteSocket();
                }
                //ɾ��Ŀ����Ѵ��ڵĶϵ�
                if(ClearTarget())
                {
                    ExcuteCommand(NULL, 0, TRUE);
                }
                break;
            }
            //��¼��Ϣ��Դ
            m_MtxMsgSrc.lock();
            m_CurCmdSource = SOCKET_DATA;
            m_CurSrcAid = INVALID_AID;
            m_CurSrcSaid = INVALID_AID;
            //����Э�������װ����������
            if(MessageHandler(recvBuf, ret) != IS_NEED_EXC_CMD)
            {
                m_MtxMsgSrc.unlock();
                continue;
            }

            //ִ����������ƶ�����
            ExcuteCommand(NULL, 0, TRUE);
            m_MtxMsgSrc.unlock();
        }
    }

    m_quitMutex.lock();
    m_localExitReady = TRUE;
    m_quitReady.wakeAll();
    m_quitMutex.unlock();
    sysLoger(LDEBUG, "RSP_TcpServer::run: ICE Server exiting!");
}

//T_VOID RSP_TcpServer::SetLog(T_BOOL isLog)
//{
//    QMutexLocker locker(&m_logMutex);
//    m_isLog = isLog;
//}
//
//T_BOOL RSP_TcpServer::IsLog()
//{
//    QMutexLocker locker(&m_logMutex);
//    return m_isLog;
//}

int RSP_TcpServer::SetMop(T_BOOL isMop)
{
    QMutexLocker locker(&m_mopMutex);
    if(CmdSize() > 0)
    {
        return FAIL;
    }
    m_mopFlag = isMop;
    return SUCC;
}

T_BOOL RSP_TcpServer::GetMop()
{
    QMutexLocker locker(&m_mopMutex);
    return m_mopFlag;
}


T_VOID RSP_TcpServer::DeleteSocket()
{
    m_clientMutex.lock();
    delete m_sClient;
    m_sClient = NULL;
    m_clientMutex.unlock();
}
/*
* @brief
*    ��GDB�Ͽ���IS�����Ӻͼ��Ŀ������Ƿ��жϵ����ã�����У���װ��������նϵ�����\n
*/
T_BOOL RSP_TcpServer::ClearTarget()
{
    //����Ͽ�,ǿ�Ƚ�Ŀ���״̬����Ϊֹͣ״̬
    if(GetTargetStatus() == RSP_ArmConst::CORE_POWER_OFF)
    {
        sysLoger(LINFO, "RSP_TcpServer::ClearTarget:target is power off, start to clear command queue!");
        ExcuteCommand(NULL, 0, FALSE);
        m_hardBp = FALSE;
        m_bpMap.clear();
        return FALSE;
    }
    SetTargetStatus(RSP_ArmConst::CORE_DEBUG_BKWTPT);

    //��������������滹�������ʾICE��������������ӶϿ�����ʱ����Ҫ��ɾ��Ŀ��˵Ķϵ㣬ֻ��Ҫ�ڱ��ؽ�����ռ���
    if(CmdSize() > 0)
    {
        m_hardBp = FALSE;
        m_bpMap.clear();
        return FALSE;
    }
    sysLoger(LINFO, "RSP_TcpServer::ClearTarget: starting to excute the clear operating!");
    T_BOOL flag = FALSE;

    /*���û��ע��Simihost ������GDB�˳�ʱɾ��Ӳ���ϵ�--by qingxh*/
    if(!m_SimiRegisterFlag)
    {
        //װ���������� 1.ɾ��Ӳ���ϵ� 2. ɾ������ϵ� 3. ����Ŀ���״̬��ȷ��
        if(m_hardBp)
        {
            PushBackCmd(RSP_ArmConst::CMD_REMOVE_HBP);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);

            //1. ɾ��Ӳ���ϵ�
            T_WORD *pArmAddr = new T_WORD(0);
            T_WORD *pWidth = new T_WORD(4);
            QList<T_VOID*> *pRemoveHbp = new QList<T_VOID*>();
            pRemoveHbp->push_back(pArmAddr);
            pRemoveHbp->push_back(pWidth);
            m_atomParas.push_back(pRemoveHbp);

            //2.�������,û�в���
            QList<T_VOID*> *pCheckRemoveHbp = new QList<T_VOID*>();
            m_atomParas.push_back(pCheckRemoveHbp);

            flag = TRUE;
        }
    }

    if(!m_bpMap.empty())
    {
        QMapIterator<T_WORD, T_WORD> i(m_bpMap);
        while (i.hasNext())
        {
            i.next();
            T_WORD armAddr = i.key();
            T_WORD value = i.value();

            //װ��ɾ������ϵ�ָ��
            //���ڸöϵ㣬�Ӷϵ������ȡ���öϵ�
            T_WORD *pArmAddr = new T_WORD(armAddr);
            T_WORD *pValue = new T_WORD(value);
            T_WORD *pWidth = new T_WORD(32);
            //װ���������� 1.д�ڴ� 2. ������� 3. �ظ�OK
            PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);

            //װ�ز���
            //1. д�ڴ����
            QList<T_VOID*> *pWriteMem = new QList<T_VOID*>();
            pWriteMem->push_back(pArmAddr);
            pWriteMem->push_back(pValue);
            pWriteMem->push_back(pWidth);
            m_atomParas.push_back(pWriteMem);

            //2. �������û�в���
            QList<T_VOID*> *pCheckWriteMem = new QList<T_VOID*>();
            m_atomParas.push_back(pCheckWriteMem);
        }

        flag = TRUE;
    }
    

    //��նϵ�map
    m_bpMap.clear();

    return flag;
}

/*
* @brief
*    ��ѯĿ���״̬��ͨ��ICE�Ļظ��ƶ�״̬����ִ�У�װ����������\n
* @return
*/
T_VOID RSP_TcpServer::QueryTarget()
{

    m_lambdaIce.QueryTarget();

}

/*
* @brief
*    ��ʼ��Ŀ�����������Ŀ����ϵ��Ժ���У�װ����������\n
* @return
*/
T_VOID RSP_TcpServer::InitTarget()
{
    //�������а��� 1.ֹͣĿ��� 2. ����ִ�н�� 3. ���Ĵ��� 4.���¼Ĵ��� 5.��д�ڴ� ,����ִ�н���� ��ʱ��ѭ�� 6.����Ŀ���������״̬
    sysLoger(LINFO, "RSP_TcpServer::InitTarget: starting to excute the init operating!");
    PushBackCmd(RSP_ArmConst::CMD_STOP_TRT);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
    PushBackCmd(RSP_ArmConst::CMD_READ_REG);
    PushBackCmd(RSP_ArmConst::CMD_UPDATE_READ_REG);

    for(int i = 0; i < m_memCfg.size(); i++)
    {
        PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
        PushBackCmd(RSP_ArmConst::CMD_DELAY);
    }

    //����CP15
    T_WORD udateCp15Num = m_cpp.size();
    for(T_WORD j = 0; j < udateCp15Num; j++)
    {
        PushBackCmd(RSP_ArmConst::CMD_INIT_CP15);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);

    }
    //����DCC
    if(m_DccEnable == TRUE)
    {
        PushBackCmd(RSP_ArmConst::CMD_OPEN_DCC_FUNCTION);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_OPEN_DCC);
    }
    PushBackCmd(RSP_ArmConst::CMD_SET_TRT_STATUS);

    //װ�ز�������
    //1. ֹͣĿ������޲���
    QList<T_VOID*> *stopTargetPara = new QList<T_VOID*>();
    m_atomParas.push_back(stopTargetPara);
    //2. ����ִ�н�����޲���
    QList<T_VOID*> *checkReply_stp = new QList<T_VOID*>();
    m_atomParas.push_back(checkReply_stp);
    //3. ���Ĵ������޲���
    QList<T_VOID*> *readRegsPara = new QList<T_VOID*>();
    m_atomParas.push_back(readRegsPara);
    //4. ���¼Ĵ������޲���
    QList<T_VOID*> *updateRegsPara = new QList<T_VOID*>();
    m_atomParas.push_back(updateRegsPara);

    //5. д�ڴ棨ѭ������ ����
    for(int i = 0; i < m_memCfg.size(); i++)
    {
        //д�ڴ����
        QList<T_VOID*> *writePara = new QList<T_VOID*>();
        //��ַ��value
        pair<T_WORD, T_WORD> *memPair = m_memCfg.value(i);
        T_WORD *pAddr = new T_WORD(memPair->first);
        T_WORD *pValue = new T_WORD(memPair->second);
        T_WORD *pWidth = new T_WORD(m_widthCfg.value(i));
        T_WORD *delay = new T_WORD(m_delayCfg.value(i));
        writePara->push_back(pAddr);
        writePara->push_back(pValue);
        writePara->push_back(pWidth);
        writePara->push_back(delay);
        
        m_atomParas.push_back(writePara);

        //����д�ڴ���,�޲���
        QList<T_VOID*> *checkReply_wmem = new QList<T_VOID*>();
        m_atomParas.push_back(checkReply_wmem);

        //��ʱ,����timeout��λ����
        QList<T_VOID*> *delayPara = new QList<T_VOID*>();
        T_WORD *timeout = new T_WORD(m_delayCfg.value(i));
        delayPara->push_back(timeout);
        m_atomParas.push_back(delayPara);
    }

    //6. ����CP15�Ĵ���
    QList<T_WORD> keys;
    QMapIterator<T_WORD, T_WORD> i_keys(m_cpp);
    while (i_keys.hasNext()) {
        i_keys.next();
       keys.push_back(i_keys.key());
    }

    for(T_WORD j = 0; j < udateCp15Num; j++)
    {
        QList<T_VOID*> *initCp15Para = new QList<T_VOID*>();
        T_WORD *num = new T_WORD(keys.value(j));
        T_WORD *value = new T_WORD(m_cpp.value(keys.value(j)));
        
        initCp15Para->push_back(num);
        initCp15Para->push_back(value);
        m_atomParas.push_back(initCp15Para);

        //����дCP15���,�޲���
        QList<T_VOID*> *checkReply_wmem = new QList<T_VOID*>();
        m_atomParas.push_back(checkReply_wmem);
    }
    if(m_DccEnable)
    {
        QList<T_VOID*> *dccPara = new QList<T_VOID*>();
        T_WORD *dccAddr = new T_WORD(m_DccAddr);
        QList<T_VOID*> *dccResult = new QList<T_VOID*>();
        dccPara->push_back(dccAddr);
        m_atomParas.push_back(dccPara);
        m_atomParas.push_back(dccResult);
    }
    //6. ����Ŀ���״̬
    QList<T_VOID*> *updateStatusPara = new QList<T_VOID*>();
    m_atomParas.push_back(updateStatusPara);
    return;
}

/*
* @brief
*    ������GDB�����ݽ���Э�������װ����������\n
* @param[in] cpBuf    �������ṩ�����ݻ���
* @param[in] wSize    �������ṩ�Ļ������Ĵ�С
* @return
*/
int RSP_TcpServer::MessageHandler(T_CHAR *cpBuf, T_WORD wSize)
{
    QMutexLocker locker(&m_excuteLock);

    T_WORD iRet = IS_NEED_EXC_CMD;
    //�����������Ϸ���
    if(NULL == cpBuf)
    {
        return FAIL;
    }
    
    sysLoger(LINFO, "RSP_TcpServer::MessageHandler: Recieved the command:%c", cpBuf[0]);
    //˽����չ, ֧�ֲ�ѯĿ��������µ�״̬
    if(cpBuf[0] == 'A')
    {
        if((m_targetStatus != RSP_ArmConst::CORE_POWER_OFF)
            && (m_targetStatus != RSP_ArmConst::CORE_INVALID))
        {
            //�ϵ�״̬
            sysLoger(LDEBUG, "MessageHandler:Recieved Query target power state CMD, power on!");
            PutData("Pon", strlen("Pon"));
            return FAIL;
        }
        else
        {
            //�µ�״̬
            sysLoger(LDEBUG, "MessageHandler:Recieved Query target power state CMD, power off!");
            PutData("Poff", strlen("Poff"));
            return FAIL;
        }
    }
    
    T_WORD cmdSize = CmdSize();
    if(cmdSize > 0)
    {
        sysLoger(LINFO, "MessageHandler:command queue is not empty, refuse the command:%c"\
            " the first command in command queue:%d", cpBuf[0], m_atomCmds.first());
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    
    //���Ŀ���״̬�Ϸ���
    T_WORD targetStatus = GetTargetStatus();
    if(RSP_ArmConst::CORE_POWER_OFF == targetStatus || RSP_ArmConst::CORE_RUNNING == targetStatus)
    {
        //һЩ������Ŀ�������״̬������״̬��֧��
        sysLoger(LINFO, "MessageHandler:target is power off or running,refuse the command:%c", cpBuf[0]);
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    
    switch(cpBuf[0])
    {
    case 'P':
        {
            iRet = handleWriteSingleRegister(cpBuf, wSize);
            break;
        }
    case 'g':
         iRet = handleReadRegisters(cpBuf, wSize);
         break;
        
    case 'X':
        iRet = handleWriteMemory(cpBuf, wSize);
        break;
        
    case 'm':
        iRet = handleReadMemory(cpBuf, wSize);
        break;
        
    case 'c':
        iRet = handleContinueTarget(cpBuf, wSize);
        break;
        
    case 'Z':
        iRet = handleSetBreakPoint(cpBuf, wSize);
        break;
        
    case '?':
        PutData("S11", strlen("S11"));
        return FAIL;
        break;
        
    case 'z':
        iRet = handleRemoveBreakPoint(cpBuf, wSize);
        break;
        
    case 'R':
            /*Ŀ�����λ*/
            iRet = handleResetTarget(cpBuf, wSize);
        break;
        
    default:
        //��֧�ָ�����
        PutData("", strlen(""));
        return FAIL;
    }

    return iRet;
}

/*
* @brief
*    �ӱ����ڴ滺���л�ȡ��Ҫ���ڴ���Ϣ�����û�У��÷�����ѹ���������д�Ŀ����ж�ȡ��Ӧ���ڴ��\n
* @param[in] wAddr   �ڴ��ַ
* @param[in] wSize   �ڴ��С
* @param[in] cpBuf   �����߸����Ļ�����
* @param[in] type    ���������õ��ڴ�ĸ�ʽ 0ΪGDB��ʽ 1Ϊ�����Ƹ�ʽ
* @return  ���������������ڴ����Ĵ�С��0Ϊ�ޱ��ػ���
*/
int RSP_TcpServer::GetLocalMemory(T_WORD wAddr, T_WORD wSize, T_CHAR *cpBuf, T_WORD type)
{
    //�������ַ,��ӳ����в��ұ��ػ����Ƿ���ڸ��ڴ��
    T_WORD baseAddr = wAddr & RSP_Memory::MEMORY_BLOCK_MASK;
    T_WORD offset = wAddr - baseAddr;
    T_WORD leftSize = wSize;
    T_CHAR *pLocalBuf = cpBuf;


    //��������Ҫ�ڴ�����Ŀ
    T_WORD computeSize = (wSize + RSP_Memory::MEMORY_BLOCK_SIZE - 1 + offset) / RSP_Memory::MEMORY_BLOCK_SIZE;
    T_WORD count = 0;
/*    if(computeSize == 1)
    {
        count = 1;
    }
    else
    {
        count = computeSize + 1;
    }
*/
    count = computeSize;
    //��鱾���Ƿ����㹻���ڴ��
    T_WORD validAddr = baseAddr;  //�ڴ��ַ
    T_WORD i = count;
    while(i != 0)
    {
        if(!m_addr2mem.contains(validAddr))
        {
            break;
        }
        validAddr += RSP_Memory::MEMORY_BLOCK_SIZE;
        i--;
    }

    //����������㹻���ڴ滺��,���ڴ���Ϣ��������������
    if(i == 0)
    {
        T_WORD index = offset; //�����ڴ���������
        T_WORD localSize = 0;
        while(count)
        {
            localSize = wSize < RSP_Memory::MEMORY_BLOCK_SIZE ? wSize : RSP_Memory::MEMORY_BLOCK_SIZE;
            //
            if((wSize + index) < RSP_Memory::MEMORY_BLOCK_SIZE)
            {
                localSize = wSize;
            }
            else
            {
                localSize = RSP_Memory::MEMORY_BLOCK_SIZE - index;
            }

            //ȡ��һ���ڴ��
            RSP_Memory *pMem = m_addr2mem.value(baseAddr);
            for(T_WORD j = index; j < (localSize + index); j++)
            {
                if(type == 0)
                {//gdb��ʽ�ڴ�
                    pLocalBuf += Int2Hex(pLocalBuf, static_cast<int>(*(pMem->GetValue() + j)), 2);
                }
                else
                {//���ػ����ʽ�ڴ�
                    *pLocalBuf = *(pMem->GetValue() + j);
                    pLocalBuf++;
                }
            }

            baseAddr += RSP_Memory::MEMORY_BLOCK_SIZE;
            //clear offset while count > 2
            index = 0;
            wSize -= localSize;
            count--;
        }

        return pLocalBuf - cpBuf;//���ط������ݵĳ���
    }
    else
    {
        //-------------------------------------------------------------------------------------------
        //2006.10.30 �ڴ��Ż��㷨����һ�������⣬��Ҫ����Ϊ��û���㹻�Ļ�����ʱ�򣬻����refreshMemory����
        //�п��ܻ��ͷŵ��Ѿ����ڵģ������ڴ˴ε�����������Ҫ���ڴ�飬�������޷����ظ�GDB�㹻���ڴ����ݶ�ʧ��
        //����������Ϊ�ڷ�������֮ǰ�ȼ���Ƿ����㹻���ڴ�飬���û�У����ͷź�������֯�·�����
        if(i > m_memMan.LeftPackets())
        {
            //�����Ҫ����Ļ�����������ڿ��Է���ĸ����������㻺����
            this->RefreshMemory();
            //��Ϊˢ�£�����validAddr����ӻ���ַ��ʼ����
            validAddr = baseAddr;
            i = count;
        }

        //��Ŀ�����ȡ��Ӧ���ڴ棬��ַΪvalidAddr,��СΪ��
        T_WORD memSize = i * RSP_Memory::MEMORY_BLOCK_SIZE;

        //װ���������� 1.���ڴ� 2.�����ڴ���
        PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
        PushBackCmd(RSP_ArmConst::CMD_UPDATE_READ_MEM);

        //װ�ض��ڴ����
        QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
        T_WORD *pReadAddr = new T_WORD(validAddr);
        T_WORD *pReadSize = new T_WORD(memSize);
        readMemPara->push_back(pReadAddr);
        readMemPara->push_back(pReadSize);
        //װ�ظ����ڴ����
        QList<T_VOID*> *updateMemPara = new QList<T_VOID*>();
        T_WORD *pUpdateAddr = new T_WORD(validAddr);
        T_WORD *pUpdateSize = new T_WORD(memSize);
        updateMemPara->push_back(pUpdateAddr);
        updateMemPara->push_back(pUpdateSize);


        //װ�ز�������
        m_atomParas.push_back(readMemPara);
        m_atomParas.push_back(updateMemPara);

        return SUCC;
    }


}

/*
* @brief
*    ִ���������У��ƶ�����������\n
* @param[in] cpBuf   ����ֵ������Ϊ��
* @return  SUCC��FAIL
*/
int RSP_TcpServer::ExcuteCommand(T_CHAR *cpBuf, T_WORD wSize, T_BOOL bOpsFlag)
{
    QMutexLocker locker(&m_excuteLock);

    int result = 0;
    T_WORD nextCmd = 0;
    T_BOOL opsFlag = bOpsFlag;  //��ʶ��ǰ�����ɹ�

    while(TRUE)
    {
        result = FALSE;

        //ȡ����һ������,�������ִ����ϣ��˳�ѭ��
        nextCmd = GetNextCmd();

        if(nextCmd == FAIL)
        {
            sysLoger(LINFO, "RSP_TcpServer::ExcuteCommand:CMD queue is empty!");
            return SUCC;
        }

        //ȡ����һ�����
        QList<T_VOID*> *para = NULL;

        para = GetNextPara();

        if(NULL == para)
        {
            sysLoger(LINFO, "RSP_TcpServer::ExcuteCommand:CMD parameter is NULL!");
            return FAIL;
        }
        sysLoger(LINFO, "RSP_TcpServer::ExcuteCommand:Recieved the CMD:%d", nextCmd);
        
        switch(nextCmd)
        {
        case RSP_ArmConst::CMD_QUERY_ICEENDIAN:
            result = CmdQueryIceEndian(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_CREATE_SESSION:
            result = CmdCreateSession(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_DELETE_SESSION:
            result = CmdDeleteSession(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_DELETE_SESSION:
            result = CmdAnalyDeleteSession(cpBuf, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_QUERY_ICEENDIAN:
            result = CmdAnalyQueryICEEndian(cpBuf, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_CREATE_SESSION:
            result = CmdAnalyCreateSession(cpBuf, opsFlag);
            break;
        case RSP_ArmConst::CMD_WRITE_MEM:
            result = CmdWriteIceMemory(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_READ_MEM:
            result = CmdReadIceMemory(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_WRITE_REG:
            result = CmdWriteIceRegisters(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_READ_REG:
            result = CmdReadIceRegisters(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_RUN_TRT:
            result = CmdRunTarget(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_STOP_TRT:
            result = CmdStopTarget(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_RESULT:
            result = CmdCheckReply(cpBuf, opsFlag);
            break;
        case RSP_ArmConst::CMD_SIMPLE_REPLY_GDB:
            result = CmdSimpleReply(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_UPDATE_READ_MEM:
            result = CmdUpdateMems(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_INSERT_HBP:
            result = CmdInsertHardwareBp(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_UPDATE_READ_REG:
            result = CmdUpdateRegs(cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_WRITE_INT_MEM:
            result = CmdWriteIntMemory(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_SET_TRT_STATUS:
            result = CmdUpdateStatus(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_REPLY_READ_MEM:
            result = CmdReplyMems(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_REMOVE_HBP:
            result = CmdRemoveHardwareBp(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_REPLY_S05:
            result = CmdReplyS05(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_INSERT_SBP:
            result = CmdInsertSoftwareBp(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_INSERT_WBP:
            result = CmdInsertWatchPoint(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_REMOVE_WBP:
            result = CmdRemoveWatchPoint(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_DELAY:
            result = CmdDelay(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_INSERT_SBP:
            result = CmdCheckSbp(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_INIT_CP15:
            result = CmdWriteCp15Registers(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_GET_CP15:
            result = CmdReadCp15Registers(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_RESET_TARGET:
            result = CmdResetTarget(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_RESET_TARGET:
            result = CmdAnalyResetTarget(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_QUERY_TRT:
            result = CmdQueryTarget(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_BP_READ_MEM:
            result = CmdAnalyBreakPointReadMem(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_SIMIHOST_BREAK:
            result = CmdAnalySimihostBreakPoint(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_OPEN_DCC_FUNCTION:
            result = CmdOpenDccFunction(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_OPEN_DCC:
            result = CmdAnalyOpenDccResult(para, cpBuf, wSize, opsFlag);
            break;
            
        default:
            break;
        }

        //�ͷ���Դ
        if(NULL != para)
        {
            delete para;
            para = NULL;
        }

        //�������ִ��ʧ�ܣ���ô�����������кͲ������У�����ִ�����һ������
        if(result == FALSE)
        {
            sysLoger(LINFO, "RSP_TcpServer::ExcuteCommand:Excute the command %d failure!", nextCmd);
            opsFlag = FALSE;
        }

        if(opsFlag == FALSE)
        {
            continue;
        }

        //�������������Ҫ��ICE��������ô�˳�ѭ���ȴ�ICE�ظ�
        //���򣬼���ִ����һ������
        if(nextCmd > RSP_ArmConst::CMD_NEED_ICE)
        {
            continue;
        }
        else
        {
            m_waitForIce = TRUE;
            break;
        }
    }

    return SUCC;
}

/*
* @brief
*    ��������������ȡ����һ������\n
* @return  ������ţ����û�з���ʧ��
*/
int RSP_TcpServer::GetNextCmd()
{
    //��������
    QMutexLocker locker(&m_cmdMutex);

    if (m_atomCmds.isEmpty())
    {
        return FAIL;
    }

    return m_atomCmds.takeFirst();
}

/*
* @brief
*    װ��һ�����������������\n
* @para [in] wCmd �������
* @return  ������ţ����û�з���ʧ��
*/
T_VOID RSP_TcpServer::PushBackCmd(T_WORD wCmd)
{
    //��������
    QMutexLocker locker(&m_cmdMutex);

    m_atomCmds.push_back(wCmd);

}
/*
* @brief
*    �鿴�������еĴ�С\n
* @return  �������еĴ�С
*/
T_WORD RSP_TcpServer::CmdSize()
{
    //��������
    QMutexLocker locker(&m_cmdMutex);
    return m_atomCmds.size();
}

/*
* @brief
*    �Ӳ�����������ȡ����һ�����\n
* @return  ����ֵ�����û�з���ʧ��
*/
QList<T_VOID*> * RSP_TcpServer::GetNextPara()
{
    //��������
    QMutexLocker locker(&m_cmdMutex);

    if (m_atomParas.isEmpty())
    {
        return NULL;
    }

    return m_atomParas.takeFirst();

}

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<�������з���ʵ��>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


/*
* @brief
*    д�ڴ�,��Ӧ��������CMD_WRITE_MEM\n
* @param[in] tpArg  д�ڴ�����Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdWriteIceMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pSize = static_cast<T_WORD*>(tpArg->takeFirst());
    T_CHAR *pData = static_cast<T_CHAR*>(tpArg->takeFirst());

    //������ΪFALSE������ֻ��Ҫ������Դ������Ҫִ�в���
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdWriteIceMemory:Addr:%x, Length:%d, Value:%x", 
                                    *pAddr, *pSize, *pData);
        m_lambdaIce.WriteMemory(*pAddr, pData, 0, *pSize);
    }
    //�ͷ���Դ
    delete pAddr;
    delete pSize;
    delete pData;
    return TRUE;
}

/*
* @brief
*    д���Ͳ����ڴ�,��Ӧ��������CMD_WRITE_INT_MEM\n
* @param[in] tpArg  д�ڴ�����Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdWriteIntMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pValue = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pWidth = NULL;
    T_WORD *delay;
    if(tpArg->isEmpty())
    {
        pWidth = new T_WORD(32);
    }
    else
    {
        pWidth = static_cast<T_WORD*>(tpArg->takeFirst());    
    }
    if(tpArg->isEmpty())
    {
        delay = new T_WORD(0);
    }
    else
    {
        delay = static_cast<T_WORD*>(tpArg->takeFirst());
    }

    if(bFlag)
    {
        //����Ŀ�����С��ת��buf
        T_CHAR buf[4];
        Int2Char(*pValue, buf, m_targetEndian);
        sysLoger(LINFO, "RSP_TcpServer::CmdWriteIntMemory:Addr:%x,Value:%x, delay:%d, width:%dbit", 
                                    *pAddr, *pValue, *delay, *pWidth);
        //m_lambdaIce.WriteMemoryWithDelay(*pAddr, buf, 0, 4, *delay);
        m_lambdaIce.WriteMemory(*pAddr, buf, 0, *pWidth/8);
    }

    //�ͷ���Դ
    delete pAddr;
    delete pValue;
    delete delay;
    delete pWidth;
    return TRUE;

}

T_BOOL RSP_TcpServer::CmdCheckSbp(QList<T_VOID*> *tpArg,  T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        if((cpBuf[0] == 'E') && (wSize == FAIL_ACK_LEN))
        {
            return FALSE;
        }

        T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());


        int readValue = Char2Int(&cpBuf[4], m_targetEndian);
        sysLoger(LINFO, "RSP_TcpServer::CmdCheckSbp:Addr:%x,Value:%x.", 
                                    *pAddr, readValue);
        if(readValue != this->m_bpMem)
        {
            //2006.6.15������ISż������ִ��϶ϵ�����⣬��д��ȥ���ڴ����ݺͶ��������ڴ����ݲ�ƥ��
            //����������ϵ����Ի��ƣ���ʼ�����Դ���m_retryTimes �� 5��5������ʧ�ܺ��ٻظ�ʧ����Ϣ��GDB
            if(m_retryTimes > 0)
            {
                //���Դ�����Сһ��
                m_retryTimes--;

                //���ظ�GDB��������ȡ��
                GetNextCmd();

                QList<T_VOID*> *para = NULL;
                para = GetNextPara();
                if(NULL != para)
                {
                    delete para;
                    para = NULL;
                }

                //ѹ���ϵ���������
                PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_INSERT_SBP);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //ѹ���������
                //1д�ڴ棬��Ҫ����: ��ַ ֵ
                QList<T_VOID*> *writeMem = new QList<T_VOID*>();
                T_WORD *pWriteMemAddr = new T_WORD(*pAddr);
                T_WORD *pWriteMemValue = new T_WORD(m_bpMem);
                T_WORD *pWriteMemWidth = new T_WORD(32);
                writeMem->push_back(pWriteMemAddr);
                writeMem->push_back(pWriteMemValue);
                writeMem->push_back(pWriteMemWidth);
                m_atomParas.push_back(writeMem);

                //2.����ִ�н��������Ҫ����
                QList<T_VOID*> *analyWriteMem = new QList<T_VOID*>();
                m_atomParas.push_back(analyWriteMem);

                //3���ڴ�
                QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
                T_WORD *pReadAddr = new T_WORD(*pAddr);
                T_WORD *pReadSize = new T_WORD(4);
                readMemPara->push_back(pReadAddr);
                readMemPara->push_back(pReadSize);
                m_atomParas.push_back(readMemPara);

                //4�������ϵ��Ƿ����ɹ�
                QList<T_VOID*> *cmpMemPara = new QList<T_VOID*>();
                T_WORD *pCheckAddr = new T_WORD(*pAddr);
                cmpMemPara->push_back(pCheckAddr);
                m_atomParas.push_back(cmpMemPara);

                //5. �ظ�GDB���������Ҫ����
                QList<T_VOID*> *replyGdb = new QList<T_VOID*>();
                m_atomParas.push_back(replyGdb);

                return TRUE;
            }

            //��ԭm_retryTimes
            m_retryTimes = 5;
        
            //�Ӷϵ������ȡ���öϵ�
            assert((m_bpMap.contains(*pAddr)));
            m_bpMap.take(*pAddr);
            return FALSE;
        }
    }

    return TRUE;
}

/*
* @brief
*    ��ʱ����,��Ӧ��������CMD_DELAY\n
* @param[in] tpArg  ��ʱ����Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdDelay(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pTimeout = static_cast<T_WORD*>(tpArg->takeFirst());

    this->wait(*pTimeout);

    //�ͷ���Դ
    delete pTimeout;

    return TRUE;
}

/*
* @brief
*    ��ձ����ڴ滺��,
*/
T_VOID RSP_TcpServer::RefreshMemory()
{
    T_WORD size = m_addr2mem.size();

    if(size == 0)
    {
        return;
    }

    QList<T_WORD> keys;

    QMapIterator<T_WORD, RSP_Memory*> i(m_addr2mem);
    while (i.hasNext()) {
        i.next();
       keys.push_back(i.key());
    }

    for(T_WORD j = 0; j < size; j++)
    {
        RSP_Memory *mem = m_addr2mem.take(keys.value(j));
        if(NULL != mem)
        {
            m_memMan.Free(mem);
        }
    }

}

/*
* @brief
*    ���ڴ�,��Ӧ��������CMD_READ_MEM\n
* @param[in] tpArg  ���ڴ�����Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdReadIceMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pSize = static_cast<T_WORD*>(tpArg->takeFirst());

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdReadIceMemory:Addr:%x,length:%d.", 
                                    *pAddr, *pSize);
        m_lambdaIce.ReadMemory(*pAddr, *pSize);
    }

    //�ͷ���Դ
    delete pAddr;
    delete pSize;
    return TRUE;
}

/*
* @brief
*    ���Ĵ���,��Ӧ��������CMD_READ_REG\n
* @param[in] tpArg  ���Ĵ�����������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdReadIceRegisters(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdReadIceRegisters:Read all registers!");
        m_lambdaIce.ReadAllRegisters();
    }

    //û����Դ�����ͷ�
    return TRUE;

}

/*
* @brief
*    д�Ĵ���,��Ӧ��������CMD_WRITE_REG\n
* @param[in] tpArg  д�Ĵ�����������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdWriteIceRegisters(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdWriteIceRegisters:Write all registers!");
        m_lambdaIce.WriteAllRegisters(m_currentRegs, RSP_ArmConst::ARM_REG_NUM);
    }

    //û����Դ�����ͷ�
    return TRUE;

}

/*
* @brief 
*    дCP15�Ĵ���,��Ӧ��������CMD_INIT_CP15\n
* @param[in] tpArg  д�Ĵ�����������Ҫ�Ĳ��� 
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdWriteCp15Registers(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pNum = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pValue = static_cast<T_WORD*>(tpArg->takeFirst()); 

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdWriteCp15Registers:Write Cp15 register, No:%d, value:%x!",
                                        *pNum, *pValue);
        m_lambdaIce.WriteCp15Reg(*pNum, *pValue);
    }

    //�ͷ���Դ
    delete pNum;
    delete pValue;
    return TRUE;
}

/*
* @brief 
*    ��CP15�Ĵ���,��Ӧ��������CMD_GET_CP15\n
* @param[in] tpArg  ���Ĵ�����������Ҫ�Ĳ��� 
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdReadCp15Registers(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pNum = static_cast<T_WORD*>(tpArg->takeFirst());
    if(NULL == pNum)
    {
        sysLoger(LINFO, "CmdReadCp15Registers:The paramenter is null!");
        return FALSE;
    }
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdReadCp15Registers:Read Cp15 register, No:%d!", *pNum);
        m_lambdaIce.ReadCp15Reg(*pNum);
    }

    //�ͷ���Դ
    delete pNum;
    return TRUE;
}


/*
* @brief 
*    ����Ŀ���,��Ӧ��������CMD_RUN_TRT\n
* @param[in] tpArg  ����Ŀ�����������Ҫ�Ĳ��� 
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdRunTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdRunTarget:Continue target!");
        m_lambdaIce.ContinueTarget();
        SetTargetStatus(RSP_ArmConst::CORE_RUNNING);
    }

    //û����Դ�����ͷ�
    return TRUE;
}

/*
* @brief
*    ֹͣĿ���,��Ӧ��������CMD_STOP_TRT\n
* @param[in] tpArg  ֹͣĿ�����������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdStopTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdStopTarget:Stop target!");
        m_lambdaIce.StopTarget();
    }

    //û����Դ�����ͷ�
    return TRUE;
}


/*
* @brief
*    ���ICE�ظ��Ƿ�ɹ�,��Ӧ��������CMD_ANALY_RESULT\n
* @param[in] cpBuf  ICE�ظ�������
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdCheckReply(T_CHAR *cpBuf, T_BOOL bFlag)
{
    if(bFlag)
    {
        assert((NULL !=cpBuf));
        sysLoger(LDEBUG, "RSP_TcpServer::CmdCheckReply:ICE reply is %s!", cpBuf);
        if(cpBuf[0] == 'O' && cpBuf[1] == 'K')
        {
            return TRUE;
        }
        else
        if(cpBuf[0] == 'E')
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*
* @brief
*    ͨ��ICE�ظ����±��ؼĴ�������,��Ӧ��������CMD_UPDATE_READ_REG\n
* @param[in] cpBuf  ICE�ظ�������
* @param[in] wSize  ICE�ظ������ݵĴ�С
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdUpdateRegs(T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    if(bFlag)
    {
        assert((NULL != cpBuf));

        //��ʽ, �ɹ�data  ʧ��E
        if(('E' == cpBuf[0]) && (wSize == FAIL_ACK_LEN))
        {
            sysLoger(LINFO, "RSP_TcpServer::CmdUpdateRegs:Read Registers failure!answer pack:%s",
                                        cpBuf);
            return FALSE;
        }
        else
        {
            if(wSize < 3)
            {
                sysLoger(LINFO, "RSP_TcpServer::CmdUpdateRegs:Read Registers failure!The recieved pack"\
                                           " was invalid.");
                return FALSE;
            }

            T_CHAR *ptr = &cpBuf[2];
            //�õ�����
            T_WORD length = 0;
            Hex2int(&ptr, &length);
            if(wSize < length)
            {
                sysLoger(LINFO, "RSP_TcpServer::CmdUpdateRegs:Invalid answer pack!");
                return FALSE;
            }
            //��������
            ptr++;
            //�õ��Ĵ����ĸ���
            T_WORD regNum = length / 4;
            //���ݴ�С�˽�����ת��Ϊ���ػ���ļĴ���ֵ
            for(T_WORD i = 0; i < regNum; i++)
            {
                sysLoger(LDEBUG, "RSP_TcpServer::CmdUpdateRegs: Update Register [%d] value from :%x"\
                                                "  to :%x.", i, m_currentRegs[i], Char2Int(ptr, TRUE));
                m_currentRegs[i] = Char2Int(ptr, TRUE); 
                ptr += 4;
            }            
            return TRUE;
        }
    }



    return TRUE;

}

/*
* @brief
*    ͨ��ICE�ظ����±����ڴ滺��,��Ӧ��������CMD_UPDATE_READ_MEM\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] cpBuf  ICE�ظ�������
* @param[in] wSize  ICE�ظ������ݵĴ�С
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdUpdateMems(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pSize = static_cast<T_WORD*>(tpArg->takeFirst());

    if(bFlag)
    {
        //�����ڴ�
        assert((NULL != cpBuf));

        if(cpBuf[0] == 'E')
        {
            //m_errLogger->write(1, QString("ICE�ظ�"), QString("���ڴ�ʧ��"));

            delete pAddr;
            delete pSize;
            return FALSE;
        }
        //FIXME:û��������У��

        //�����ڴ��ĸ���
        T_WORD blockNum = *pSize / RSP_Memory::MEMORY_BLOCK_SIZE;
        T_WORD addr = *pAddr;

        T_CHAR *ptr = &cpBuf[0];
        T_CHAR *pdest = strstr(ptr,CONTROL_FLAG);
        
        //----------------------------------------------------------------
        //2006.06.19  ���ֻظ��������ֻ��OK��������ڴ����ݲ�û�еõ��ظ�
        //������ӶԻظ������Ƿ������ļ�飬�����ָ���쳣
        if(pdest == NULL)
        {
            //m_errLogger->write(1, QString("ICE�ظ�"), QString("�ظ��Ķ��ڴ���������ȷ��ֻ��OK"));
            delete pAddr;
            delete pSize;
            return FALSE;
        }
        //-----------------------------------------------------------------
        pdest += strlen(CONTROL_FLAG);

        ptr = pdest;

        while(blockNum)
        {
            //����Ѿ��и��ڴ�飬���ø���
            if(m_addr2mem.contains(addr))
            {
                blockNum--;
                continue;
            }

            RSP_Memory *memory = m_memMan.Alloc();
            if(memory == NULL)
            {
                //�����ǲ��������������ģ���������˸������˵�����������⣬�����˳����ͷ������ڴ�
                RefreshMemory();
                delete pAddr;
                delete pSize;
                return FALSE;
            }

            memory->SetBaseAddr(addr);
            memcpy(memory->GetValue(), ptr, RSP_Memory::MEMORY_BLOCK_SIZE);

            ptr += RSP_Memory::MEMORY_BLOCK_SIZE;
            m_addr2mem.insert(addr, memory);
            blockNum--;
            addr += RSP_Memory::MEMORY_BLOCK_SIZE;
        }
    }

    //�ͷ���Դ
    delete pAddr;
    delete pSize;

    return TRUE;
}

/*
* @brief
*    �ظ�GDB���ڴ�����,��Ӧ��������CMD_REPLY_READ_MEM\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdReplyMems(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize,T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pSize = static_cast<T_WORD*>(tpArg->takeFirst());

    if(bFlag)
    {
        T_CHAR sendBuf[PACKET_DATA_SIZE] = "\0", *pBuf = NULL;
        int ret = 0;
        if(GetMop())
        {
            ret = GetLocalMemory(*pAddr, *pSize, sendBuf, 0);
            //��Ϊ�������֮ǰһ����ִ��read memory����˴˴����ret <= 0���϶��Ǵ����д���
            assert((ret > 0));
        }
        else
        {
            /*���ڴ����*/
            if((cpBuf[0] == 'E') && ( wSize == FAIL_ACK_LEN))
            {
                delete pAddr;
                delete pSize;
                PutData("E03", sizeof("E03"));
                return FALSE;
            }
            /*�ظ���ʽOK/Exx len, value*/
            T_CHAR *ptr = &cpBuf[0];
            //ok
            ptr += strlen("OK");
            //len,
            T_CHAR *pdest = strstr(ptr,CONTROL_FLAG);
            if(pdest == NULL)
            {
                delete pAddr;
                delete pSize;
                PutData("E03", sizeof("E03"));
                return FALSE;
            }            
            T_WORD len = 0;
            hex2int(&ptr, &len);
            ptr = pdest + CONTROL_FLAG_LEN;
            pBuf = sendBuf;
            for(int i = 0; i < min(*pSize, len); i++)
            {
                pBuf += Int2Hex(pBuf, static_cast<int>(*(ptr+i)), 2);
            }
            ret = pBuf - sendBuf;
        }
        PutData(sendBuf, ret);
    }

    m_waitForIce = FALSE;

    //�ͷ���Դ
    delete pAddr;
    delete pSize;

    return TRUE;
}

/*
* @brief
*    �ظ�GDB������,��Ӧ��������CMD_SIMPLE_REPLY_GDB\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdSimpleReply(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        PutData("OK", strlen("OK"));

    }
    else
    {
        PutData("E03", strlen("E03"));

    }

    m_waitForIce = FALSE;

    return TRUE;
}

/*
* @brief
*    �ظ�GDB S05,��Ӧ��������CMD_REPLY_S05\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdReplyS05(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    m_CurCmdSource = SOCKET_DATA;
    if(bFlag)
    {
        if(m_NeedReplyS05 == TRUE)
        {
            PutData("S05", strlen("S05"));
        }

    }

    m_waitForIce = FALSE;

    return TRUE;

}


/*
* @brief
*    ����Ŀ���״̬����,��Ӧ��������CMD_SET_TRT_STATUS\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdUpdateStatus(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        SetTargetStatus(RSP_ArmConst::CORE_DEBUG_BKWTPT);
    }
    else
    {
        SetTargetStatus(RSP_ArmConst::CORE_RUNNING);
    }

    //����CPSR
    if(m_cpsr != -1)
    {
        m_currentRegs[RSP_ArmConst::JTAG_CPSR] = m_cpsr;
    }
    m_waitForIce = FALSE;

    return TRUE;
}

/*
* @brief 
*    ����ICE�ظ�����Ŀ���״̬�����ƶ����̷�չCMD_UPDATE_TRT_STATUS\n
* @param[in] tpArg  ����Ŀ�����������Ҫ�Ĳ��� 
* @return  
*/
T_BOOL RSP_TcpServer::CmdQueryTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(bFlag)
    {
        QueryTarget();
    }

    return TRUE;
}


/*
* @brief
*    ����Ӳ���ϵ�,��Ӧ��������CMD_INSERT_HBP\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdInsertHardwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pLength = static_cast<T_WORD*>(tpArg->takeFirst());


    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdInsertHardwareBp:Insert hardware breakpoint, addr:%x, length:%d",
                                        *pAddr, *pLength);
        //m_lambdaIce.InsertBreakPoint(*pAddr, *pLength);
        m_lambdaIce.InsertBreakPointORWatchPoint(HARD_BP, *pAddr, *pLength);
    }

    //����Ӳ���ϵ��Ѿ�����
    m_hardBp = TRUE;

    //�ͷ���Դ
    delete pAddr;
    delete pLength;

    return TRUE;
}

/*
* @brief
*    ɾ��Ӳ���ϵ�,��Ӧ��������CMD_REMOVE_HBP\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdRemoveHardwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pLength = static_cast<T_WORD*>(tpArg->takeFirst());


    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdRemoveHardwareBp:Remove hardware breakpoint, addr:%x, length:%d",
                                        *pAddr, *pLength);
        m_lambdaIce.DeleteBreakPointORWatchPoint(HARD_BP, *pAddr, *pLength);
    }

    m_hardBp = FALSE;

    //�ͷ���Դ
    delete pAddr;
    delete pLength;

    return TRUE;

}

/*
* @brief
*    ����Ӳ���۲��,��Ӧ��������CMD_INSERT_WBP\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdInsertWatchPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pWidth = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pAccess = static_cast<T_WORD*>(tpArg->takeFirst());


    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdInsertWatchPoint:Insert watch breakpoint, addr:%x, length:%d, Access:%x",
                                        *pAddr, *pWidth, *pAccess);
        m_lambdaIce.InsertBreakPointORWatchPoint(*pAccess, *pAddr, *pWidth);
    }

    //����Ӳ���ϵ��Ѿ�����
    m_hardBp = TRUE;

    //�ͷ���Դ
    delete pAddr;
    delete pWidth;
    delete pAccess;

    return TRUE;

}

/*
* @brief
*    ɾ��Ӳ���۲��,��Ӧ��������CMD_REMOVE_WBP\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdRemoveWatchPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pWidth = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pAccess = static_cast<T_WORD*>(tpArg->takeFirst());


    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdRemoveWatchPoint:Remove watch breakpoint, addr:%x, length:%d, Access:%x",
                                        *pAddr, *pWidth, *pAccess);
        m_lambdaIce.DeleteBreakPointORWatchPoint(*pAccess, *pAddr, *pWidth);
    }

    m_hardBp = FALSE;

    //�ͷ���Դ
    delete pAddr;
    delete pWidth;
    delete pAccess;

    return TRUE;

}

/*
* @brief
*    ��������ϵ�,��Ӧ��������CMD_INSERT_SBP\n
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdInsertSoftwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_CHAR buf[4];
    memset(buf,0,4);

    if(bFlag)
    {
        int ret = 0;
        if(GetMop())
        {
           ret = GetLocalMemory(*pAddr, 4, buf, 1);
            assert((ret > 0));
        }
        else
        {
            sysLoger(LDEBUG, "RSP_TcpServer::CmdInsertSoftwareBp:Insert breakpoint, push READ_MEM, ANALY_BP_READ_MEM command");
            PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_BP_READ_MEM);
            //װ�ض��ڴ����
            QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
            T_WORD *pReadAddr = new T_WORD(*pAddr);
            T_WORD *pReadSize = new T_WORD(4);//һ�������Ĵ�С
            readMemPara->push_back(pReadAddr);
            readMemPara->push_back(pReadSize);
            //װ�ط������ڴ���������ϵ�Ĳ���
            QList<T_VOID*> *analyPara = new QList<T_VOID*>();
            T_WORD *pBpAddr = new T_WORD(*pAddr);
            analyPara->push_back(pBpAddr);
            //װ�ز����б�
            m_atomParas.push_back(readMemPara);
            m_atomParas.push_back(analyPara);            
        }
    }

    //�ͷ���Դ
    delete pAddr;
    return TRUE;
}


/*
* @brief
*    ��ѯICE�Ĵ�С��
* @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdQueryIceEndian(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdQueryIceEndian:Query ICE endian!");
        m_lambdaIce.QueryTargetEndian(m_iceId);
    }

    return TRUE;
}

/*
* @brief
*    ����ICE�Ự
* @param[in] tpArg  �����Ự������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdCreateSession(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdCreateSession:Create session !%s", m_IceBuf.toAscii().data());
        m_lambdaIce.CreateSeesion(m_IceBuf.toAscii().data(), m_IceBuf.length()+1, m_iceId);
    }

    return TRUE;
}
/*
* @brief
*   ɾ��ICE�Ự
* @param[in] tpArg ɾ���Ự������Ҫ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdDeleteSession(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdDeleteSession:Delete session !");
        m_lambdaIce.DeleteSeesion(m_iceId);
    }

    return TRUE;
}
/*
* @brief
*    ������ѯICE��С�˵Ľ��
* @param[in] cpBuf  ��ѯ�����Ϣ
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdAnalyQueryICEEndian(char *cpBuf, T_BOOL bFlag)
{
    if(NULL == cpBuf)
    {
        return FALSE;
    }
    if(bFlag)
    {
        if(cpBuf[0] == 'l')
        {
            m_iceEndian = FALSE;
        }
        else
        {
            m_iceEndian = TRUE;
        }
    }
    m_waitForIce = FALSE;
    return TRUE;
}

/*
* @brief
*    ���������Ự�Ľ��
* @param[in] cpBuf  �����Ự�����Ϣ
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdAnalyCreateSession(char *cpBuf, T_BOOL bFlag)
{
    if(NULL == cpBuf)
    {
        return FALSE;
    }
    if(bFlag)
    {
        if((cpBuf[0] == 'O') && (cpBuf[1] == 'K'))
        {
            //char *pBuf = &cpBuf[2];
            //Hex2int(&(pBuf), &m_sessId);
            m_sessId = 11;          //Ŀǰ�̶�Ϊ11
        }
        else
        {
            m_sessId = -1;
        }
    }
    m_waitForIce = FALSE;
    return TRUE;
}

/*
* @brief
*    ����ɾ���Ự�Ľ��
* @param[in] cpBuf  ɾ���Ự�����Ϣ
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdAnalyDeleteSession(char *cpBuf, T_BOOL bFlag)
{
    if(NULL == cpBuf)
    {
        return FALSE;
    }
    if(bFlag)
    {
        if((cpBuf[0] == 'O') && (cpBuf[1] == 'K'))
        {
            //char *pBuf = &cpBuf[2];
            //Hex2int(&(pBuf), &m_sessId);
            m_sessId = -1;
        }
        else
        {
            m_sessId = -1;
        }
    }
    m_waitForIce = FALSE;
    return TRUE;
}

/*
* @brief
*    ����Ŀ���
* @param[in] tpArg  ����Ŀ����Ĳ���(��������)
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdResetTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(bFlag)
    {
        if(tpArg->isEmpty())
        {
            return FALSE;
        }    
        T_UWORD *pType = static_cast<T_UWORD*>(tpArg->takeFirst());
        if(pType == NULL)
        {
            return FALSE;
        }
        sysLoger(LINFO, "RSP_TcpServer::CmdResetTarget: Reset target, type:%d", *pType);
        m_lambdaIce.ReSetTarget(*pType);
        delete pType;
    }

    return TRUE;
}

/*
* @brief
*    ��������Ŀ������
* @param[in] cpBuf  ����Ŀ����Ľ������
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdAnalyResetTarget(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != cpBuf));
    assert((NULL != tpArg));
    
    if(tpArg->size() < 2)
    {
        return FALSE;
    }
    T_HWORD *hwSrcAid = static_cast<T_HWORD*>(tpArg->takeFirst());
    T_HWORD *hwSrcSaid = static_cast<T_HWORD*>(tpArg->takeFirst());
    if((hwSrcAid == NULL) || (hwSrcSaid == NULL))
    {
        if(hwSrcAid)
           delete hwSrcAid;
        if(hwSrcSaid)
           delete hwSrcAid;
        
        return FALSE;
    }
    if(bFlag)
    {
        if(strlen(cpBuf) < SUCC_ACK_LEN)
        {
            delete hwSrcAid;
            delete hwSrcSaid;
            return FALSE;
        }
        SendIDEResult(cpBuf, strlen(cpBuf), *hwSrcAid, *hwSrcSaid);
        
    }
    else
    {
        SendIDEResult("E03", strlen("E03"), *hwSrcAid, *hwSrcSaid);
    }
    m_waitForIce = FALSE;
    
    delete hwSrcAid;
    delete hwSrcSaid;
    return TRUE;
}

/*
* @brief
*    ����Ŀ���
* @param[in] tpArg  ����Ŀ����Ĳ���(��������)
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdQueryTargetCpuIDCode(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdQueryTargetCpuIDCode: Query CPU ID CODE");
        m_lambdaIce.QueryCPUIDCode();
    }
    return TRUE;
}

/*
* @brief
*    ����Ŀ���
* @param[in] tpArg  ����Ŀ����Ĳ���(��������)
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdAnalyQueryTargetCpuIDCode(QList<T_VOID*> *tpArg, 
                                                                                    T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != cpBuf));
    assert((NULL != tpArg));
    if(tpArg->size() < 2)
    {
        return FALSE;
    }
    T_HWORD *pSrcAid = static_cast<T_HWORD*>(tpArg->takeFirst());
    T_HWORD *pSrcSaid = static_cast<T_HWORD*>(tpArg->takeFirst());
    if((pSrcAid == NULL) || (pSrcSaid == NULL))
    {
        if(pSrcAid)
           delete pSrcAid;
        if(pSrcSaid)
           delete pSrcSaid;        
        return FALSE;
    }
    if(bFlag)
    {
        if(strlen(cpBuf) < min(SUCC_ACK_LEN, FAIL_ACK_LEN))
        {
            SendIDEResult(ERR_SYSTEM_ERROR, FAIL_ACK_LEN, *pSrcAid, *pSrcSaid);            
            delete pSrcAid;
            delete pSrcSaid;
            return FALSE;
        }
        SendIDEResult(cpBuf, wSize, *pSrcAid, *pSrcSaid);
    }
    else
    {
        SendIDEResult(ERR_SYSTEM_ERROR, FAIL_ACK_LEN, *pSrcAid, *pSrcSaid);
    }
    delete pSrcAid;
    delete pSrcSaid;
    return TRUE;
}

/*
* @brief
*    ��������ϵ�ʱ���ڴ�Ľ����������ϵ�
* @param[in] tpArg  ����ϵ���ڴ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdAnalyBreakPointReadMem(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != cpBuf));
    assert((NULL != tpArg));
    if(tpArg->size() < 1)
    {
        return FALSE;
    }
    T_WORD *pAddr =  static_cast<T_WORD*>(tpArg->takeFirst());
    if(NULL == pAddr)
    {
        return FALSE;
    }
    sysLoger(LDEBUG, "CmdAnalyBreakPointReadMem:Recived read breakpoint mem:");
    if(bFlag)
    {        
        if((cpBuf[0] == 'E') && (wSize == FAIL_ACK_LEN))
        {
            sysLoger(LDEBUG,"CmdAnalyBreakPointReadMem: ICE read breakpoint memory failure!");
            delete pAddr;
            return FALSE;
        }
        /*�ظ���ʽOK/Exx len, value*/
        T_CHAR *ptr = &cpBuf[0];
        //ok/exx
        ptr += strlen("OK");
        
        //len,
        T_CHAR *pdest = strstr(ptr,CONTROL_FLAG);
        if(pdest == NULL)
        {
            sysLoger(LDEBUG,"CmdAnalyBreakPointReadMem: ICE read breakpoint memory , lack of len part!");
            delete pAddr;
            return FALSE;
        }
        T_WORD len = 0;
        hex2int(&ptr, &len);
        ptr = pdest + CONTROL_FLAG_LEN;
        //value
        T_CHAR buf[10] = "\0";
        memcpy(buf, ptr, min(4, len));
        //�����ڴ���Ч
        T_WORD value = Char2Int(buf, m_targetEndian);
        m_bpMap.insert(*pAddr, value);
        sysLoger(LINFO, "RSP_TcpServer::CmdInsertSoftwareBp:Insert soft breakpoint, addr:%x, value:%x",
                                        *pAddr, value);
    }
    delete pAddr;
    return TRUE;
}

/*
* @brief
*    ����PC�Ĵ�����ֵ���������SIMIHOSTע��ĵ�ֵַ�����ϱ�SWI
* @param[in] tpArg  ����ϵ���ڴ�Ĳ���
* @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
* @return  �ɹ�����ʧ��
*/
T_BOOL RSP_TcpServer::CmdAnalySimihostBreakPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    m_NeedReplyS05 = TRUE;
    
    if(bFlag == TRUE)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdAnalySimihostBreakPoint: PC register value:%x, simihost registered value:%x!",
                        m_currentRegs[RSP_ArmConst::JTAG_R15], m_SimiPCAddr);
        if(m_currentRegs[RSP_ArmConst::JTAG_R15] == m_SimiPCAddr)
        {
            SendIDEResult("SWI", strlen("SWI"), m_SimiAid, m_SimiSaid);
            m_NeedReplyS05 = FALSE;
        }
    }

    return TRUE;
}

/**
 * @Funcname:  CmdOpenDccFunction
 * @brief        :  ����DCC����
 * @para1      : [IN] tpArg DCC���ܵĵ�ַ
 * @para2      : [IN] bFlag  �Ƿ�����ִ�в���
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��4��18�� 15:45:30
 *   
**/
T_BOOL RSP_TcpServer::CmdOpenDccFunction(QList < T_VOID * > * tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    if(tpArg->size() < 1)
    {
        return FALSE;
    }
    T_WORD *pDccAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    if(NULL == pDccAddr)
    {
        return FALSE;
    }
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdOpenDccFunction:Open Dcc function, dcc starting address:%x", *pDccAddr);
        m_lambdaIce.OpenDccFunction(*pDccAddr);
    }
    delete pDccAddr;
    return TRUE;
}

/**
 * @Funcname:  CmdAnalyOpenDccResult
 * @brief        :  ������DCC���ܵĽ��
 * @para1      : [IN]tpArg, �����б�
 * @para2      : [IN]cpBuf, �յ�ICE�ظ��Ľ��
 * @para3      : [IN]wSize, �ظ�����ĳ���
 * @para4      : [IN]bFlag, �Ƿ�����ִ�в���
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��4��18�� 16:04:18
 *   
**/
T_BOOL RSP_TcpServer::CmdAnalyOpenDccResult(QList < T_VOID * > * tpArg, T_CHAR * cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != cpBuf));
    assert((NULL != tpArg));

    if(bFlag)
    {
        if(cpBuf[0] == 'E' && wSize == FAIL_ACK_LEN)
        {
            sysLoger(LINFO, "RSP_TcpServer::CmdAnalyOpenDccResult: Open Dcc function failure!");
            return FALSE;
        }
        else if(strcmp(cpBuf, "OK") == 0)
        {
            sysLoger(LINFO, "RSP_TcpServer::CmdAnalyOpenDccResult: Open Dcc function success!");
            return TRUE;
        }
        sysLoger(LINFO, "RSP_TcpServer::CmdAnalyOpenDccResult: unknow data recived from ICE: %s!", cpBuf);
        return FALSE;
    }
    
    return TRUE;
}
/*
* @brief
*    ���ݴ������ĵ�ǰ״̬���üĴ���\n
* @param[in] regNo   Ҫд�ļĴ�����
* @param[in] value   �Ĵ�����ֵ
* @return  SUCC��FAIL
*/
T_BOOL RSP_TcpServer::WriteSingleRegister(T_WORD regNo, T_WORD value)
{

    sysLoger(LINFO, "RSP_TcpServer::WriteSingleRegister: Set the register [no:%d] as %x", regNo, value);

    //�жϴ�����ģʽ
    if(((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f)  == RSP_ArmConst::ARM_USER_MODE)
        || ((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SYSTEM_MODE))
    {
        //�û���ϵͳģʽ��
        if(waCurUsrRegs[regNo] == -1)
        {
            return TRUE;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ�����ȷ
        }
        m_currentRegs[waCurUsrRegs[regNo]] = value;
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_FIQ_MODE)
    {
        //FIQģʽ��
        if(waCurFiqRegs[regNo] == -1)
        {
            return TRUE;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ�����ȷ
        }
        m_currentRegs[waCurFiqRegs[regNo]] = value;

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_IRQ_MODE)
    {
        //IRQģʽ��
        if(waCurIrqRegs[regNo] == -1)
        {
            return TRUE;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ�����ȷ
        }
        m_currentRegs[waCurIrqRegs[regNo]] = value;

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SUPER_MODE)
    {
        //��Ȩģʽ��
        if(waCurSvcRegs[regNo] == -1)
        {
            return TRUE;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ�����ȷ
        }
        m_currentRegs[waCurSvcRegs[regNo]] = value;

    }
    else
        if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_ABORT_MODE)
    {
        //ABTģʽ��
        if(waCurAbtRegs[regNo] == -1)
        {
            return TRUE;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ�����ȷ
        }
        m_currentRegs[waCurAbtRegs[regNo]] = value;

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_UNDEFINED_MODE)
    {
        //δ����ģʽ��
        if(waCurUndRegs[regNo] == -1)
        {
            return TRUE;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ�����ȷ
        }
        m_currentRegs[waCurUndRegs[regNo]] = value;

    }
    else
    {
        //�������������ģʽ
        return FALSE;
    }

    return TRUE;
}

/*
* @brief
*    ���ݴ�����ģʽ��ȡ�����Ĵ���\n
* @param[in] wRegNo    �Ĵ�����
* @return  �ɹ��򷵻ؼĴ�����ֵ��ʧ���򷵻�FAIL
*/
T_WORD RSP_TcpServer::ReadSingleRegister(T_WORD wRegNo)
{
        //�жϴ�����ģʽ
    sysLoger(LINFO, "RSP_TcpServer::ReadSingleRegister: Read the register [no:%d] .", wRegNo);
    if(((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_USER_MODE)
        || ((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SYSTEM_MODE))
    {
        //�û���ϵͳģʽ��
        if(waCurUsrRegs[wRegNo] == -1)
        {
            return 0;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ���0
        }
        return m_currentRegs[waCurUsrRegs[wRegNo]];
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_FIQ_MODE)
    {
        //FIQģʽ��
        if(waCurFiqRegs[wRegNo] == -1)
        {
            return 0;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ���0
        }
        return m_currentRegs[waCurFiqRegs[wRegNo]];

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_IRQ_MODE)
    {
        //IRQģʽ��
        if(waCurIrqRegs[wRegNo] == -1)
        {
            return 0;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ���0
        }
        return m_currentRegs[waCurIrqRegs[wRegNo]];

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SUPER_MODE)
    {
        //��Ȩģʽ��
        if(waCurSvcRegs[wRegNo] == -1)
        {
            return 0;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ���0
        }
        return m_currentRegs[waCurSvcRegs[wRegNo]];

    }
    else
        if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_ABORT_MODE)
    {
        //ABTģʽ��
        if(waCurAbtRegs[wRegNo] == -1)
        {
            return 0;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ���0
        }
        return m_currentRegs[waCurAbtRegs[wRegNo]];

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_UNDEFINED_MODE)
    {
        //δ����ģʽ��
        if(waCurUndRegs[wRegNo] == -1)
        {
            return 0;  //���Ϊ-1��ʾ��ǰICE��֧�ָø�ֵ�����ɷ���0
        }
        return m_currentRegs[waCurUndRegs[wRegNo]];

    }
    else
    {
        //�������������ģʽ
        return 0;
    }
}

/*
* @brief
*    ���ݴ�����ģʽ��ȡ���мĴ���\n
* @param[in] wRegs    �Ĵ�������
* @return  �ɹ��򷵻�SUCC��ʧ���򷵻�FAIL
*/
T_BOOL RSP_TcpServer::ReadAllRegisters(T_WORD *wRegs)
{
            //�жϴ�����ģʽ
    sysLoger(LINFO, "RSP_TcpServer::ReadAllRegisters:Enter the function!");
    if(((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_USER_MODE)
        || ((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SYSTEM_MODE))
    {
        //�û���ϵͳģʽ��
        GetRegs(wRegs, waCurUsrRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_FIQ_MODE)
    {
        //FIQģʽ��
        GetRegs(wRegs, waCurFiqRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_IRQ_MODE)
    {
        //IRQģʽ��
        GetRegs(wRegs, waCurIrqRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SUPER_MODE)
    {
        //��Ȩģʽ��
        GetRegs(wRegs, waCurSvcRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_ABORT_MODE)
    {
        //ABTģʽ��
        GetRegs(wRegs, waCurAbtRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_UNDEFINED_MODE)
    {
        //δ����ģʽ��
        GetRegs(wRegs, waCurUndRegs);
    }
    else
    {
        //�������������ģʽ
        return FALSE;
    }

    return TRUE;

}

/*
* @brief
*    ��������������޶���ȡ����Ӧ�ļĴ���\n
* @param     waRegs    �������ṩ�ļĴ������黺��
* @param[in] wTable    �������ṩ�ļĴ���ӳ���
* @return
*/
T_VOID RSP_TcpServer::GetRegs(T_WORD *waRegs, T_CONST T_WORD *waTable)
{
    for(int i = 0; i < RSP_ArmConst::RSP_REG_NUM; i++)
    {
        if( -1 == waTable[i])
        {
            waRegs[i] = 0;
            continue;
        }
        waRegs[i] = m_currentRegs[waTable[i]];
    }
}

/*
* @brief
*    ��һ�����͸��ݴ�С��תΪCHAR����\n
* @param[in] wReg    �������ṩ�ļĴ�����ֵ
* @param[in] cpBuf   �������ṩ�Ļ���
* @param[in] bIsBigEndian  �������ṩ�Ĵ�С����Ϣ
* @return
*/
T_VOID RSP_TcpServer::Int2Char(T_WORD wReg, T_CHAR *cpBuf, T_BOOL bIsBigEndian)
{
    if(bIsBigEndian)
    {
        *cpBuf = ((wReg >> 24) & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 16) & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 8)  & 0xff);
        cpBuf++;
        *cpBuf = (wReg & 0xff);
        }
    else
    {
        *cpBuf = (wReg & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 8)  & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 16) & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 24) & 0xff);
    }
}

/*
* @brief
*    ��һ��CHAR������ݴ�С��תΪ����\n
* @param[in] cpBuf   �������ṩ�Ļ���
* @param[in] bIsBigEndian  �������ṩ�Ĵ�С����Ϣ
* @return ������
*/
T_WORD RSP_TcpServer::Char2Int(T_CHAR *cpBuf, T_BOOL bIsBigEndian)
{
    if(bIsBigEndian)
    {
        return    (*(cpBuf + 3) & 0xff)       |
                (*(cpBuf + 2) & 0xff) <<  8 |
                (*(cpBuf + 1) & 0xff) << 16 |
                (*cpBuf)       << 24;

    }
    else
    {
        return  (*(cpBuf + 3))        << 24 |
                  (*(cpBuf + 2) & 0xff) << 16 |
                  (*(cpBuf + 1) & 0xff) <<  8 |
                  (*cpBuf & 0xff);
    }
}


/*
* @brief
*    �����ڷ���������GDB�����ݣ��յ����ݺ�\n
*    �����ݿ������������ṩ�Ļ������󷵻�\n
* @param     cpBuf    �������ṩ�����ݻ���
* @param[in] wSize    �������ṩ�Ļ������Ĵ�С
* @return  ʵ�ʽ��յ����ݵĴ�С������Ͽ�����-1
*/
int RSP_TcpServer::GetData(T_CHAR *cpBuf, T_WORD wSize)
{

    //��������GDB����
    T_CHAR head;
    T_WORD offset = 0;

    while(TRUE)
    {
        //���SOCKET�Ϸ���
        if (NULL == m_sClient || NULL == cpBuf)
        {
            sysLoger(LDEBUG, "RSP_TcpServer::GetData:m_sClient or cpBuf is bad pointer!FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            return FAIL;
        }


        offset = 0;
        int ret = m_sClient->Recv(&head, 1);

        //�������Ͽ����˳�ѭ��
        if(ret < 0)
        {
            sysLoger(LDEBUG, "RSP_TcpServer::GetData:m_sClient->Recv return failure!FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            return FAIL;
        }

        //������յ�ͷ�ַ�����$�����������
        if('$'!= head)
        {
            continue;
        }

        //��������ֱ��#
        ret = m_sClient->Recv(&head, 1);
        while((offset < wSize) && (FAIL != ret))
        {
            if('#' == head)
            {
                break;
            }
            cpBuf[offset++] = head;
            ret = m_sClient->Recv(&head, 1);
        }

        if(ret < 0)
        {
            sysLoger(LDEBUG, "RSP_TcpServer::GetData:m_sClient->Recv return failure!FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            return FAIL;
        }

        if(offset > wSize)
        {
            continue;
        }

        //Ӧ����Ϊ��TCP���գ���û�н���У��
        m_sClient->Send("+", strlen("+"));
        //sysLoger(LDEBUG, "RSP_TcpServer::GetData:Recieved data: %s", cpBuf);
        return offset;
    }
}

/*
* @brief
*    ���ظ����ݷ��͸�GDB\n
* @param     cpBuf    ������Ҫ���͵����ݻ���
* @param[in] size    ������Ҫ�������ݵĴ�С
* @return  ʵ�ʷ��͵����ݵĴ�С
*/
int RSP_TcpServer::PutData(T_CHAR *cpBuf, T_WORD wSize)
{

    T_WORD i = 0;
    T_UBYTE *sendBuf;
    T_UBYTE checkSum = 0;
    T_WORD    count = wSize;
    int result = FAIL;
    //������Ϣ��Դ��������ִ�н����Դ��Դ���ͻظ�
    if(m_CurCmdSource == TSAPI_DATA)
    {
        SendIDEResult(cpBuf, wSize, m_CurSrcAid, m_CurSrcSaid);
    }
    else if(m_CurCmdSource == SOCKET_DATA)
    {
        if(m_sClient == NULL || cpBuf == NULL)
        {
            return FAIL;
        }

        //��־
#ifdef IS_PACK_LOG
        if(TRUE == IsLog())
        {
            m_logger.write(SEND_PACK, cpBuf, wSize);
        }        
#endif

        //���䷢�ͻ�����
        sendBuf = new T_UBYTE[wSize+10];

        if(sendBuf == NULL)
        {
            return FAIL;
        }

        memset(sendBuf, 0, wSize + 10);
        //�����ͷ
        sendBuf[0] = '$';

        //��У���
        while(count--)
        {
            checkSum += static_cast<int>(cpBuf[i]);
            sendBuf[i+1] = static_cast<T_UBYTE>(cpBuf[i]);
            i++;
        }
        //�����β
        sendBuf[wSize+1] = '#';
        sendBuf[wSize+2] = vcaHexchars[(checkSum >> 4) & 0xf];
        sendBuf[wSize+3] = vcaHexchars[checkSum & 0xf];
        //��������
        //--------------------------------------------------------------------------
        //2006.06.19 �ڷ�������ǰ��SOCKET�ĺϷ��Խ��м��飬����SOCKETɾ��˲�䣬�����߳����ڵ��÷��ͷ�����������
        m_clientMutex.lock();
        if(m_sClient != NULL)
        {
            result = m_sClient->Send(reinterpret_cast<T_CHAR *>(sendBuf),wSize+4);
        }
        m_clientMutex.unlock();



        delete []sendBuf;
        return result;
    }
    else
    {
        sysLoger(LINFO, "RSP_TcpServer::PutData:Invalid msg source!s");
    }
    return result;
}

/*
* @brief
*    ��������16�����ַ���ת��Ϊ����,��Ҫע����Ǹò������ƶ���������ָ��ĵ�ַ\n
* @param[in]     cptr    16�����ַ���
* @param[out] wIntValue ת����INT����
* @return  ָ���ƶ����ֽ���
*/
T_UWORD RSP_TcpServer::Hex2int(T_CHAR **cptr, T_WORD *wIntValue)
{
  int numChars = 0;
  int hexValue;

  *wIntValue = 0;

  while (**cptr)
    {
      hexValue = Hex2num(**cptr);
      if (hexValue < 0)
      break;

      *wIntValue = (*wIntValue << 4) | hexValue;
      numChars ++;

      (*cptr)++;
    }

  return (numChars);
}


/*
* @brief
*    ��������16����ASCII�ַ�ת���ɶ�Ӧ������\n
* @param[in]     cptr   ������16����ASCII�ַ�
* @return  ���ض�Ӧ������
*/
int RSP_TcpServer::Hex2num(T_UBYTE vubCh)
{
  if (vubCh >= 'a' && vubCh <= 'f')
    return vubCh-'a'+10;
  if (vubCh >= '0' && vubCh <= '9')
    return vubCh-'0';
  if (vubCh >= 'A' && vubCh <= 'F')
    return vubCh-'A'+10;
  return -1;
}


/**
 * @Funcname:  parseConfig
 * @brief        :  ���������ļ�
 * @return      : true /false
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��3��6�� 18:04:23
 *   
**/
bool RSP_TcpServer::parseConfig()
{
    QFile f(m_CfgFile);
    if(!f.open(QIODevice::ReadOnly))
    {
        sysLoger(LWARNING, "RSP_TcpServer::parseConfig: Open the config file [%s] failure!", 
                        m_CfgFile.toAscii().data());
        return false;
    }
    QByteArray block = f.readAll();
    
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8")) ;    
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextDecoder *decoder = codec->makeDecoder();
    QString buf = decoder->toUnicode(block);
    
    //QString buf = QString::fromLocal8Bit(block.data(),block.size());
    QXmlInputSource in;
    in.setData(buf);
    QXmlSimpleReader reader;
    // don't click on these!
    reader.setFeature("http://xml.org/sax/features/namespaces", false);
    reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
    reader.setFeature("http://trolltech.com/xml/features/report-whitespace"
        "-only-CharData", false);
    XmlHandler *hand = new XmlHandler();    //��������
    reader.setContentHandler(hand);
    reader.setErrorHandler(hand);

    reader.parse(in);    //��ȡ�ļ�����
    reader.setContentHandler(0);
    reader.setErrorHandler(0);
    //delete hand;
    f.close();

    //����������
    sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: Save the parse result!");
    m_targetEndian = hand->getTargetEndian();
    m_cpsr = hand->getCPSR();
    m_bpMem = hand->getBpMem();
    sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: Endian:%d, CPSR:0x%x, breakPoint:0x%x.",
                                    m_targetEndian, m_cpsr, m_bpMem);
    
    QMap<T_WORD, T_WORD> mcpp = hand->getCppRegisterInfo();
    QMap<T_WORD, T_WORD>::iterator itrCpp = mcpp.begin();
    m_cpp.clear();
    while(itrCpp != mcpp.end())
    {
        sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: cp15_regno:d, value:0x%x", 
                                        itrCpp.key(), itrCpp.value());
        m_cpp.insert(itrCpp.key(), itrCpp.value());
        itrCpp++;
    }

    QList<pair<T_WORD, T_WORD>*> listMem = hand->getMemeryConfigInfo();
    m_memCfg.clear();
    for(int i = 0; i < listMem.size(); i++)
    {
        sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: Memory_addr:0x%x, value:0x%x", 
                                        listMem[i]->first, listMem[i]->second);
        m_memCfg.push_back(listMem[i]);
    }
    
    QList<T_WORD> listDelay = hand->getMemConfigInfoWithDelay();
    m_delayCfg.clear();
    for(int i = 0; i < listDelay.size(); i++)
    {
        sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: MemoryDelay value:%d", 
                                        listDelay[i]);
        m_delayCfg.push_back(listDelay[i]);
    }

    QList<T_WORD> listWidth = hand->getMemWidthCfg();
    m_widthCfg.clear();
    for(int i = 0; i < listWidth.size(); i++)
    {
        sysLoger(LDEBUG, "RSP_TcpServer::parseConfig:MemoryWidth value:%d", listWidth[i]);
        m_widthCfg.push_back(listWidth[i]);
    }
    
    m_IceBuf = hand->getICEConfigBuffer();

    m_targetType = hand->getTargetType();
    m_CpuType = hand->getTargetCpuType();
    
    m_DccEnable = hand->getDccEnable();
    m_DccAddr = hand->getDccAddr();
    sysLoger(LDEBUG, "RSP_TcpServer::parseConfig:DccEnable:%d, DccAddress:%x", m_DccEnable, m_DccAddr);
    delete hand;
    
    return true;
}

T_WORD RSP_TcpServer::handleWriteSingleRegister(T_CHAR *cpBuf, T_WORD wSize)
{
    //д�����Ĵ���������Ҫװ����������
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    T_WORD regNo = 0;
    T_WORD value = 0;
    T_CHAR* ptr = &cpBuf[1];
    if( Hex2int(&ptr, &regNo) && (*(ptr++) == '='))
    {
        if (Hex2int(&ptr, &value))
        {
            T_CHAR tmpBuf[4];
            Int2Char(value, tmpBuf, m_targetEndian);
            value = Char2Int(tmpBuf, TRUE);
            
            //060929 �����޸�cpsr�Ĵ��������� ̷��ΰ
            if( regNo == 0x19 )
            {
                int mode = value & 0x1f;
                if( (mode != RSP_ArmConst::ARM_USER_MODE) 
                    && (mode != RSP_ArmConst::ARM_SYSTEM_MODE)
                    && (mode != RSP_ArmConst::ARM_FIQ_MODE)
                    && (mode != RSP_ArmConst::ARM_IRQ_MODE)
                    && (mode != RSP_ArmConst::ARM_SUPER_MODE)
                    && (mode != RSP_ArmConst::ARM_ABORT_MODE)
                    && (mode != RSP_ArmConst::ARM_UNDEFINED_MODE))
                {
                    PutData("E03", strlen("E03"));
                    return FAIL;
                }
            }

            if(WriteSingleRegister(regNo, value))
            {
                PutData("OK", strlen("OK"));
                return SUCC;
            }
        }
    }
    PutData("E03", strlen("E03"));
    return FAIL;

}
T_WORD RSP_TcpServer::handleReadRegisters(T_CHAR *cpBuf, T_WORD wSize)
{
    //�ӱ��ػ����ȡ�Ĵ���,����Ҫװ����������
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    T_CHAR sendBuf[PACKET_DATA_SIZE] = "\0";
    T_CHAR *pBuf = sendBuf;
    //�����ж��Ƕ������Ĵ������Ƕ�����
    if(wSize > 1 && cpBuf[1] == 'g')
    {
        //�������Ĵ���,�����ʽgRegNo
        T_WORD regNo = 0;
        T_WORD value = 0;
        T_CHAR* ptr = &cpBuf[2];
        if( Hex2int(&ptr,&regNo))
        {
            value = ReadSingleRegister(regNo);
        }

        Int2Char(value, pBuf, TRUE);
        value = Char2Int(pBuf, m_targetEndian);
        pBuf += Int2Hex(pBuf, value, 8);
        sysLoger(LDEBUG, "RSP_TcpServer::handleReadRegisters:RegNo:%d, int value:%x.", regNo, value);
    }
    else
    {
        //�����мĴ�������
        T_WORD regs[RSP_ArmConst::RSP_REG_NUM];
        T_WORD value = 0;
        T_WORD ret = 0;
        ReadAllRegisters(regs);

        //T_BOOL endian = !m_iceEndian;
        //��װ����,����Ĵ���24�ֽ�,һ��Ĵ���Ϊ8�ֽ�,fps8�ֽ�,cpsr8�ֽ�
        for(T_WORD i = 0; i < RSP_ArmConst::RSP_REG_NUM; i++)
        {
            //��С��ת��
            if(m_targetEndian == FALSE)
            {
                Int2Char(regs[i], pBuf, TRUE);
                value = Char2Int(pBuf, m_targetEndian);
            }
            else
            {
                value = regs[i];
            }
            if(i > 15 && i < 24)//��Χ�ڶ��Ǹ���Ĵ���
            {
                ret = Int2Hex(pBuf, value, 24);
            }
            else
            {
                ret = Int2Hex(pBuf, value, 8);
            }

            pBuf += ret;
            sysLoger(LDEBUG, "RSP_TcpServer::handleReadRegisters:RegNo:%d, int value:%x.", i, value);
        }
    }

    PutData(sendBuf, pBuf - sendBuf);
    
    return FAIL;
}

T_WORD RSP_TcpServer::handleWriteMemory(T_CHAR *cpBuf, T_WORD wSize)
{
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    
    T_WORD *addr = new T_WORD;
    T_WORD *pSize = new T_WORD;
    T_CHAR *ptr = &cpBuf[1];
    
    if(Hex2int(&ptr, addr) &&
            *(ptr++)==','         &&
            Hex2int(&ptr, pSize)  &&
            *(ptr++) == ':')
    {
        //�����ַ�ʹ�С��Ϊ���ʾ����Ƿ�֧�ֶ��������أ�Ӧ��ֱ�ӻظ�OK
        if(*pSize == 0)
        {
            delete addr;
            delete pSize;
            PutData("OK", strlen("OK"));
            return FAIL;
        }

        //��д�ڴ����ձ��ػ�����ڴ档�������±�������ݲ�һ��
        RefreshMemory();
        //����GDB 0x7d����������
        if(*pSize < 0)
        {
            delete addr;
            delete pSize;
            PutData("E03", strlen("E03"));
            return FAIL;
        }
        //д�������ڴ�,װ��3������ 1.д�ڴ� 2.����������� 3.�ظ�GDB�������
        PushBackCmd(RSP_ArmConst::CMD_WRITE_MEM);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
        PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);
        
        T_CHAR *content = new T_CHAR[*pSize];
        T_CHAR *src = ptr;
        T_CHAR *des = content;
        T_WORD cnt = *pSize;
        while(cnt--)
        {
            if(*src == 0x7d)
            {
                src++;
                *des++ = *src++ ^ 0x20;
            }
            else
            {
                *des++ = *src++;
            }
        }

        //1 װ��д�ڴ����
        QList<T_VOID*> *wma = new QList<T_VOID*>();
        wma->push_back(static_cast<T_VOID*>(addr));
        wma->push_back(static_cast<T_VOID*>(pSize));
        wma->push_back(static_cast<T_VOID*>(content));

        //2 װ�ط����������,Ŀǰû�в���
        QList<T_VOID*> *rwm = new QList<T_VOID*>();

        //3 װ�ػظ�GDB����,Ŀǰû�в���
        QList<T_VOID*> *replyToGdb = new QList<T_VOID*>();


        m_atomParas.push_back(wma);
        m_atomParas.push_back(rwm);
        m_atomParas.push_back(replyToGdb);
    }
    else
    {
        //��������ʧ�ܣ�ֱ�ӻظ�
        PutData("E03", strlen("E03"));
        return FAIL;
    }

    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleReadMemory(T_CHAR *cpBuf, T_WORD wSize)
{
    //���ڴ�,���ȼ�鱾�ػ�����ڴ��Ƿ���Ч,�����Ч,�򽫱��ػ���ֱ�ӿ�����GDB
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    T_CHAR sendBuf[PACKET_DATA_SIZE] = "\0";
    T_WORD addr = 0; //�ڴ��ַ
    T_WORD size = 0; //�ڴ泤��
    T_CHAR *ptr = &cpBuf[1];
    T_CHAR *pSendBuf = &sendBuf[0];

    if(Hex2int(&ptr, &addr) && *(ptr++) == ',' && Hex2int(&ptr, &size))
    {
        if(size < 0 || size > 2048)
        {
            PutData("E03", strlen("E03"));
            return FAIL;
        }
        if(GetMop() == FALSE)
        {
            //װ���������� 1.���ڴ� 2.�ظ������
            PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
            PushBackCmd(RSP_ArmConst::CMD_REPLY_READ_MEM);

            //װ�ض��ڴ����
            QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
            T_WORD *pReadAddr = new T_WORD(addr);
            T_WORD *pReadSize = new T_WORD(size);
            readMemPara->push_back(pReadAddr);
            readMemPara->push_back(pReadSize);
            //װ�ػظ�GDB�ڴ����
            QList<T_VOID*> *replyMemPara = new QList<T_VOID*>();
            T_WORD *pReplyAddr = new T_WORD(addr);
            T_WORD *pReplySize = new T_WORD(size);
            replyMemPara->push_back(pReplyAddr);
            replyMemPara->push_back(pReplySize);

            //װ�ز�������
            m_atomParas.push_back(readMemPara);
            m_atomParas.push_back(replyMemPara);
        }
        else
        {
            T_WORD ret = GetLocalMemory(addr, size, pSendBuf, 0);
            if(ret > 0)
            {//��������ݣ��ͷ��ͣ����򣬷���
                PutData(pSendBuf, ret);
                return FAIL;
            }
            else
            {
                //װ�ػظ�GDB�ڴ���������
                PushBackCmd(RSP_ArmConst::CMD_REPLY_READ_MEM);

                //װ�ػظ�GDB�ڴ����
                QList<T_VOID*> *replyMemPara = new QList<T_VOID*>();
                T_WORD *pReplyAddr = new T_WORD(addr);
                T_WORD *pReplySize = new T_WORD(size);
                replyMemPara->push_back(pReplyAddr);
                replyMemPara->push_back(pReplySize);

                m_atomParas.push_back(replyMemPara);
            }
        }
    }
    else
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    return IS_NEED_EXC_CMD;
}
T_WORD RSP_TcpServer::handleContinueTarget(T_CHAR *cpBuf, T_WORD wSize)
{
    //����Ƿ�������е�ַ
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    
    if(wSize > 1)
    {
        T_WORD addr = 0;
        T_CHAR *ptr = &cpBuf[1];
        if(Hex2int(&ptr, &addr))
        {
            WriteSingleRegister(RSP_ArmConst::JTAG_R15, addr);
        }
        else
        {
            PutData("E03", strlen("E03"));
            return FAIL;
        }
    }

    //װ���������� 1.д�Ĵ��� 2. ����ִ�н�� 3.������������ 4.����ִ�н��
    PushBackCmd(RSP_ArmConst::CMD_WRITE_REG);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
    PushBackCmd(RSP_ArmConst::CMD_RUN_TRT);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
    //װ�ز�������,���в���Ŀǰ��û�в���
    QList<T_VOID*> *cWreg = new QList<T_VOID*>();
    QList<T_VOID*> *cAnaly_1 = new QList<T_VOID*>();
    QList<T_VOID*> *cRun = new QList<T_VOID*>();
    QList<T_VOID*> *cAnaly_2 = new QList<T_VOID*>();

    m_atomParas.push_back(cWreg);
    m_atomParas.push_back(cAnaly_1);
    m_atomParas.push_back(cRun);
    m_atomParas.push_back(cAnaly_2);

    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleSetBreakPoint(T_CHAR *cpBuf, T_WORD wSize)
{
    if(cpBuf == NULL)
    {
        sysLoger(LINFO, "RSP_TcpServer::handleSetBreakPoint:Argument cpBuf is bad pointer!");
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    //����ϵ�������ȼ��ϵ�����
    T_WORD type = 0;  //����
    T_WORD addr = 0;  //��ַ
    T_WORD length = 0; //���
    T_CHAR *ptr = &cpBuf[1];
    sysLoger(LDEBUG, "RSP_TcpServer::handleSetBreakPoint:enter the function!");
    if(Hex2int(&ptr, &type) && *(ptr++) == ','
        && Hex2int(&ptr, &addr) && *(ptr++) == ','
        && Hex2int(&ptr, &length))
    {
        switch(type)
        {
        case SOFT_BP:
            {
                //����ڴ�
                RefreshMemory();

                //����ϵ�
                //����ȣ��ж��Ƿ���Ҫ����
                if(RSP_ArmConst::RSP_SB_ARM_STATE == length)
                {
                    //��ARM̬,��ַ����,�����������λ��Ϊ��
                    T_WORD armAddr = addr & 0xFFFFFFFC;

                    //�������ϵ��Ƿ����
                    if(m_bpMap.contains(armAddr))
                    {
                        PutData("OK", strlen("OK"));
                        return FAIL;
                    }

                    //������,�齨��������  1.���ڴ�(��GetMemװ��) 2.���汾�ضϵ� 3. д�ڴ� 4. ����ִ�н�� 5 ���ڴ������ 6.�Ƚ��Ƿ�����д��ɹ����ɹ���ظ�GDB���
                    T_CHAR memBuf[4];
                    bool bNeedReadMem = true;
                    if(GetMop())
                    {
                        T_WORD ret = GetLocalMemory(addr, 4, memBuf, 1);
                        if(ret > 0)
                            bNeedReadMem = false;
                    }
                    if(bNeedReadMem)
                    {//�����ڴ���Ч,���ڴ��Ѿ���GetLocalMemoryװ�أ���Ҫװ�ر��汾�ضϵ�����
                        //װ����������
                        /*PushBackCmd(RSP_ArmConst::CMD_INSERT_SBP);

                        //װ�ز������У���Ҫ������ �ϵ��ַ
                        QList<T_VOID*> *insertSbp = new QList<T_VOID*>();
                        T_WORD *pArmAddr = new T_WORD(armAddr);
                        insertSbp->push_back(pArmAddr);

                        m_atomParas.push_back(insertSbp);*/
                        sysLoger(LDEBUG, "RSP_TcpServer::MessageHandler:Start to push inserting breakpoint cmds!");

                        PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
                        PushBackCmd(RSP_ArmConst::CMD_ANALY_BP_READ_MEM);
                        //װ�ض��ڴ����
                        QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
                        T_WORD *pReadAddr = new T_WORD(armAddr);
                        T_WORD *pReadSize = new T_WORD(4);//һ�������Ĵ�С
                        readMemPara->push_back(pReadAddr);
                        readMemPara->push_back(pReadSize);
                        //װ�ط������ڴ���������ϵ�Ĳ���
                        QList<T_VOID*> *analyPara = new QList<T_VOID*>();
                        T_WORD *pBpAddr = new T_WORD(armAddr);
                        analyPara->push_back(pBpAddr);
                        //װ�ز����б�
                        m_atomParas.push_back(readMemPara);
                        m_atomParas.push_back(analyPara);            

                    }
                    else
                    {
                        //�����ڴ���Ч
                        T_WORD value = Char2Int(memBuf, m_targetEndian);


                        m_bpMap.insert(armAddr, value);
                    }

                    //װ������ 3 4 5
                    PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
                    PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                    PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
                    PushBackCmd(RSP_ArmConst::CMD_ANALY_INSERT_SBP);
                    PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                    //װ����������Ҫ�Ĳ��� //FIXME: ����û�п���д�ڴ�ʧ�ܺ�Ĵ���������Ժ󲹳�
                    //3д�ڴ棬��Ҫ����: ��ַ ֵ
                    QList<T_VOID*> *writeMem = new QList<T_VOID*>();
                    T_WORD *pWriteMemAddr = new T_WORD(armAddr);
                    T_WORD *pWriteMemValue = new T_WORD(m_bpMem);
                    T_WORD *pWriteMemWidth = new T_WORD(32);
                    writeMem->push_back(pWriteMemAddr);
                    writeMem->push_back(pWriteMemValue);
                    writeMem->push_back(pWriteMemWidth);
                    m_atomParas.push_back(writeMem);

                    //4.����ִ�н��������Ҫ����
                    QList<T_VOID*> *analyWriteMem = new QList<T_VOID*>();
                    m_atomParas.push_back(analyWriteMem);

                    //5���ڴ�
                    QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
                    T_WORD *pReadAddr = new T_WORD(armAddr);
                    T_WORD *pReadSize = new T_WORD(4);
                    readMemPara->push_back(pReadAddr);
                    readMemPara->push_back(pReadSize);
                    m_atomParas.push_back(readMemPara);

                    //�������ϵ��Ƿ����ɹ�
                    QList<T_VOID*> *cmpMemPara = new QList<T_VOID*>();
                    T_WORD *pCheckAddr = new T_WORD(armAddr);
                    cmpMemPara->push_back(pCheckAddr);
                    m_atomParas.push_back(cmpMemPara);

                    //5. �ظ�GDB���������Ҫ����
                    QList<T_VOID*> *replyGdb = new QList<T_VOID*>();
                    m_atomParas.push_back(replyGdb);

                }
                else
                {
                    //THUMB̬��Ŀǰ��֧��
                }
                break;
            }
        case HARD_BP:
            {
                //Ӳ���ϵ�,
                //���ȼ���Ƿ�Ӳ���ϵ��Ѿ�����
                if(m_hardBp)
                {
                    sysLoger(LINFO, "RSP_TcpServer::handleSetBreakPoint: already set the hardware "\
                                                "breakpoint, the system just permit only one hardware breakpoint!");
                    PutData("E03", strlen("E03"));
                    return FAIL;
                }

                //�ϵ�û�����ã�װ���������� 1. ����Ӳ���ϵ� 2.������� 3. �ظ��������
                PushBackCmd(RSP_ArmConst::CMD_INSERT_HBP);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //װ�ز���
                //1. ����Ӳ���ϵ�, ��������ַ�����
                QList<T_VOID*> *pInsertHbp = new QList<T_VOID*>();
                T_WORD *pBpAddr = new T_WORD(addr);
                T_WORD *pBpLength = new T_WORD(length);
                pInsertHbp->push_back(pBpAddr);
                pInsertHbp->push_back(pBpLength);
                m_atomParas.push_back(pInsertHbp);

                //2. �������������Ҫ����
                QList<T_VOID*> *pCheckInsertHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pCheckInsertHbp);

                //3. �ظ��������,����Ҫ����
                QList<T_VOID*> *pReplyInsertHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pReplyInsertHbp);

                break;
            }
        case WRITEWATCH_BP:
            //д�۲��
        case READWATCH_BP:
            //���۲��
        case ACCESS_BP:
            //���ʹ۲��
            {
                if(m_hardBp)
                {
                    //PutData("E03", strlen("E03"));
                    PutData("OK", strlen("OK"));
                    return FAIL;
                }

                //--------------------------------------------------------
                //2006.06.21 GDB����۲���ͬʱ����������������ڶ���Ϊ��Ч����
                //��˵��յ�GDB��һ������������Ե���ֱ�ӻظ�GDB OK
                //--------------------------------------------------------
                //2006.09.29 GDB����۲��ʱ���͵�������������,�п���Ϊ��һ��Ϊ��Ч
                //����,�Ķ���Ϊ0��ַ������,����������ĵ�ַΪ0��ַ,ֱ�ӻظ�GDB OK
                //--------------------------------------------------------------------
                if(0x0 == addr)
                {
                    PutData("OK", strlen("OK"));
                    return FAIL;
                }

                //��װ�������� 1.����۲�� 2. ����� 3.�ظ�GDB
                PushBackCmd(RSP_ArmConst::CMD_INSERT_WBP);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //��װ��������
                T_WORD *pAddr = new T_WORD(addr);
                T_WORD *pLength = new T_WORD(length);
                T_WORD *pAccess = new T_WORD(type);

                //1.����۲��
                QList<T_VOID*> *pInsertWp = new QList<T_VOID*>();
                pInsertWp->push_back(pAddr);
                pInsertWp->push_back(pLength);
                pInsertWp->push_back(pAccess);
                m_atomParas.push_back(pInsertWp);

                //2. �������������Ҫ����
                QList<T_VOID*> *pCheckInsertHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pCheckInsertHbp);

                //3. �ظ��������,����Ҫ����
                QList<T_VOID*> *pReplyInsertHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pReplyInsertHbp);

                break;
            }
        default:
            sysLoger(LINFO, "RSP_TcpServer::handleSetBreakPoint: Breakpoint type:%d invalid!", type);
            PutData("E03", strlen("E03"));
            return FAIL;
        }
    }
    else
    {
        sysLoger(LINFO, "RSP_TcpServer::handleSetBreakPoint: Parse the packet failure!");
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    sysLoger(LDEBUG, "RSP_TcpServer::handleSetBreakPoint:exit the function!");
    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleRemoveBreakPoint(T_CHAR *cpBuf, T_WORD wSize)
{
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    //ɾ���ϵ�������ȼ��ϵ�����
    T_WORD type = 0;  //����
    T_WORD addr = 0;  //��ַ
    T_WORD length = 0; //���
    T_CHAR *ptr = &cpBuf[1];

    if(Hex2int(&ptr, &type) && *(ptr++) == ','
        && Hex2int(&ptr, &addr) && *(ptr++) == ','
        && Hex2int(&ptr, &length))
    {
        switch(type)
        {
        case SOFT_BP:
            {//����ϵ�,���ȵ�ַ����
            if(RSP_ArmConst::RSP_SB_ARM_STATE == length)
            {
                //����ڴ�
                RefreshMemory();
                PutData("OK", strlen("OK"));
                return FAIL;
            }
            else
            {
                //THUMB̬��Ŀǰ��֧�֣���ʱΪ��
                PutData("E03", strlen("E03"));
                return FAIL;

            }

                break;
            }
        case HARD_BP:
            {

                //���ȼ���Ƿ�Ӳ���ϵ��Ѿ�����
                if(!m_hardBp)
                {
                    sysLoger(LINFO, "RSP_TcpServer::handleRemoveBreakPoint: there is no hardware "\
                                                "breakpoint exited!");
                    PutData("E03", strlen("E03"));
                    return FAIL;
                }

                //��װ�������� 1.ɾ��Ӳ���ϵ� 2.������� 3.�ظ�GDB
                PushBackCmd(RSP_ArmConst::CMD_REMOVE_HBP);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //��װ��������
                //1. ɾ��Ӳ���ϵ�
                T_WORD *pArmAddr = new T_WORD(addr);
                T_WORD *pWidth = new T_WORD(length);
                QList<T_VOID*> *pRemoveHbp = new QList<T_VOID*>();
                pRemoveHbp->push_back(pArmAddr);
                pRemoveHbp->push_back(pWidth);
                m_atomParas.push_back(pRemoveHbp);

                //2.�������,û�в���
                QList<T_VOID*> *pCheckRemoveHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pCheckRemoveHbp);

                //3.�ظ�GDB
                QList<T_VOID*> *pReply = new QList<T_VOID*>();
                m_atomParas.push_back(pReply);

                break;
            }
        case WRITEWATCH_BP:
        case READWATCH_BP:
        case ACCESS_BP:
            {
                if(!m_hardBp)
                {
                    //PutData("E03", strlen("E03"));
                    PutData("OK", strlen("OK"));
                    return FAIL;
                }

                //--------------------------------------------------------
                //2006.06.21 GDBɾ���۲���ͬʱ��������ɾ������ڶ���Ϊ��Ч����
                //��˵��յ�GDB��һ��ɾ��������Ե���ֱ�ӻظ�GDB OK
                //--------------------------------------------------------
                //2006.09.29 GDBɾ���۲��ʱ���͵�����ɾ������,�п���Ϊ��һ��Ϊ��Ч
                //����,�Ķ���Ϊ0��ַ������,������ɾ���ĵ�ַΪ0��ַ,ֱ�ӻظ�GDB OK

                if(0x0 == addr)
                {
                    PutData("OK", strlen("OK"));
                    return FAIL;
                }

                //---------------------------------------------------------

                //��װ�������� 1.ɾ���۲�� 2. ����� 3.�ظ�GDB
                PushBackCmd(RSP_ArmConst::CMD_REMOVE_WBP);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //��װ��������
                T_WORD *pAddr = new T_WORD(addr);
                T_WORD *pLength = new T_WORD(length);
                T_WORD *pAccess = new T_WORD(type);

                //1.ɾ���۲��
                QList<T_VOID*> *pRemoveWp = new QList<T_VOID*>();
                pRemoveWp->push_back(pAddr);
                pRemoveWp->push_back(pLength);
                pRemoveWp->push_back(pAccess);
                m_atomParas.push_back(pRemoveWp);

                //2. �������������Ҫ����
                QList<T_VOID*> *pCheckpRemoveWp = new QList<T_VOID*>();
                m_atomParas.push_back(pCheckpRemoveWp);

                //3. �ظ��������,����Ҫ����
                QList<T_VOID*> *pReplypRemoveWp = new QList<T_VOID*>();
                m_atomParas.push_back(pReplypRemoveWp);

                break;
            }
        default:
            sysLoger(LINFO, "RSP_TcpServer::handleRemoveBreakPoint: Breakpoint type:%d invalid!", type);
            break;
        }
    }
    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleResetTarget(T_CHAR * cpBuf, T_WORD wSize)
{
    if(NULL == cpBuf)
    {
        sysLoger(LWARNING, "RSP_TcpServer::handleResetTarget:Argument cpBuf is bad pointer!");
        return FAIL;
    }
    if(wSize < 2)
    {
        sysLoger(LINFO, "handleResetTarget:Unknow Reset Target command, lost the reset type!");
        PutData(ERR_UNKONW_DATA, FAIL_ACK_LEN);
        return FAIL;
    }
    else
    {
        T_WORD *type = new T_WORD();     //��λ����
        char *pBuf = &cpBuf[1];
        hex2int(&pBuf, type);
        sysLoger(LINFO, "handleResetTarget:Recieved Reset Target command, reset type:%d!", *type);
        //װ�ظ�λĿ�������
        PushBackCmd(RSP_ArmConst::CMD_RESET_TARGET);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESET_TARGET);
        //װ���������
        QList<T_VOID*> *resetType = new QList<T_VOID*>();
        QList<T_VOID*> *ReplyDest = new QList<T_VOID*>();
        *type = *type % 4;
        resetType->push_back(type);
        T_HWORD *pSrcAid = new T_HWORD(m_CurSrcAid);
        T_HWORD *pSrcSAid = new T_HWORD(m_CurSrcSaid);
        ReplyDest->push_back(pSrcAid);
        ReplyDest->push_back(pSrcSAid);                
        m_atomParas.push_back(resetType);
        m_atomParas.push_back(ReplyDest);
    }
    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleSimiHostCommand(T_CHAR * cpBuf, T_WORD wSize, T_HWORD hwSrcAid, T_HWORD hwSrcSaid)
{
    if(NULL == cpBuf)
    {
        sysLoger(LWARNING, "RSP_TcpServer::handleSimiHostCommand: Argument cpBuf is bad pointer!");
        return FAIL;
    }
    if(wSize < 2)
    {
        sysLoger(LWARNING, "RSP_TcpServer::handleSimiHostCommand: command size [%d] is invalid", wSize);
        return FAIL;
    }
    sysLoger(LINFO, "RSP_TcpServer::handleSimiHostCommand: simihost cmd:%c", cpBuf[1]);
    switch(cpBuf[1])
    {
        case 'R':
            //ע��
            {
                if(wSize < 4)
                {
                    sysLoger(LWARNING, "RSP_TcpServer::handleSimiHostCommand: Invalid simihost register command!");
                    SendIDEResult(ERR_GDB_NORMAL, FAIL_ACK_LEN, hwSrcAid, hwSrcSaid);
                    return FAIL;
                }
                m_SimiSaid = hwSrcSaid;
                m_SimiAid = hwSrcAid;
                m_SimiRegisterFlag = TRUE;
                // format : YR, xxxxxxx(addr)
                T_CHAR *pBuf = &cpBuf[3];
                hex2int(&pBuf, &m_SimiPCAddr);
                sysLoger(LINFO, "RSP_TcpServer::handleSimiHostCommand: Recieved simihost's register CMD, pcAddr:%x!",
                                            m_SimiPCAddr);
                SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
                
            }
            break;
            
        case 'r':
            //ȡ��ע��
            {
                m_SimiRegisterFlag = FALSE;
                m_SimiAid = INVALID_AID;
                m_SimiSaid = INVALID_AID;
                SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
            }
            break;
            
        case 'A':
            //����, ����������
            {
                ExcuteCommand(NULL, 0, FALSE);
                SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
            }
            break;
            
        case 'B':
            //·�ɹ��ϣ�֪ͨGDB���Թ���
            {
                m_MtxMsgSrc.lock();
                m_CurCmdSource = SOCKET_DATA;
                m_MtxMsgSrc.unlock();
                PutData("S05", strlen("S05"));
                SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
            }
            break;
            
        default:
            sysLoger(LINFO, "RSP_TcpServer::handleSimiHostCommand: Unknow simihost command!");
            return FAIL;
    }
    return SUCC;
}


T_WORD RSP_TcpServer::getTargetType()
{
    return m_targetType;
}

void RSP_TcpServer::setTargetType(T_WORD type)
{
    m_targetType = type;
    return;
}

T_WORD RSP_TcpServer::getCpuType()
{
    return m_CpuType;
}

void RSP_TcpServer::setCputType(T_WORD type)
{
    m_CpuType = type;
    return;
}
#if 0
/*
* @brief
*    ����ICE�ظ�����Ŀ���״̬�����ƶ����̷�չCMD_QUERY_TRT_STATUS\n
* @param[in] tpArg  ����Ŀ�����������Ҫ�Ĳ���
* @return
*/

T_VOID RSP_TcpServer::CmdQueryTarget(T_CHAR *cpBuf, T_WORD wSize, QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        assert((NULL != cpBuf));

        if(cpBuf[0] == 'P' && cpBuf[1] == '0')
        {
            SetTargetStatus(cpBuf[3]);
        }

        //��������ϵ�״̬
        if(GetTargetStatus() == RSP_ArmConst::CORE_DEBUG_BKWTPT
            || GetTargetStatus() ==  RSP_ArmConst::CORE_RUNNING)
        {
            //װ�س�ʼ��Ŀ�����������
            InitTarget();

        }
    }

}
#endif

/*
* @brief
*    ��һ�����͸��ݿ��תΪ16�����ַ���\n
* @param[in] cpBuf   �������ṩ�Ļ���
* @param[in] wNum    �������ṩ������
* @param[in] wWidth  �������ṩ���ַ������
* @return ���
*/
T_MODULE T_WORD Int2Hex(T_CHAR *cpBuf, T_WORD wNum, T_WORD wWidth)
{
  T_WORD i;

  //buf[width] = '\0';

  for (i = wWidth - 1; i >= 0; i--)
    {
      cpBuf[i] = "0123456789abcdef"[(wNum & 0xf)];
      wNum >>= 4;
    }

  return wWidth;
}

