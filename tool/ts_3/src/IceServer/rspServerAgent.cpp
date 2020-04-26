/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: rspServerAgent.cpp,v $
  * Revision 1.7  2008/03/19 10:25:11  guojc
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
  * Revision 1.6  2007/06/21 11:37:29  guojc
  * 1. �޸���IS��simihost��ɾ��Ӳ���ϵ�Ĵ���
  * 2. ͳһ����IS������־����
  *
  * Revision 1.5  2007/04/20 07:10:50  guojc
  * �����˶�ICE DCC���ܵ�֧��
  * �����˶�Ŀ��帴λ���ܵ�֧��
  * �����˶��ڴ��ȿ����õ�֧��
  * �����˶Ի���ģ�������Լ��ݵ�֧��
  *
  * Revision 1.4  2007/04/16 02:20:53  guojc
  * ����ICE Server��Э��semihosting������ֵ�һЩ����
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
  * Revision 1.3  2006/07/03 07:18:20  tanjw
  * 1.����iceserver��gdb�������Ӻ�ִ��Ŀ����ϵ��ʼ���������յ�gdb����س����bug
  * 2.����ͬʱ�������iceserver��־�ļ������ͻ��bug
  * 3.����ts�����߳���־�ļ���¼��.�ɷ��ͺ��¼��Ϊ����ǰ��¼
  *
  * Revision 1.2  2006/06/06 12:08:11  pengh
  * no message
  *
  * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
  * no message
  *
  * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
  * TS��������
  *
  * Revision 1.4  2006/04/06 01:14:08  guojc
  * �����˴򿪹رյ����bug,ȷ����̬���ܹ���ȷ�˳�
  *
  * Revision 1.3  2006/03/29 08:38:14  guojc
  * �����˹�������ϵ�ָ�������
  *
  * Revision 1.1  2006/03/04 09:25:11  guojc
  * �״��ύICE Server��̬��Ĵ���
  *
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * �����ļ�;���� DLL Ӧ�ó������ڵ�
  */

/**
* @file     rspServerAgent.h
* @brief     
*     <li>���ܣ� ʵ��TS��׼��̬��Ļ����ӿ�</li>
* @author     ������
* @date     2006-02-20
* <p>���ţ�ϵͳ�� 
*/

/************************************���ò���******************************************/
#include <windows.h>
#include <QMap>
#include "rspTcpServer.h"
#include "sysTypes.h"
#include "common.h"
/**********************************ǰ����������******************************************/
using namespace std;
/************************************���岿��******************************************/    
T_MODULE QMap<T_WORD, RSP_TcpServer*> vtRSP_Id2Server;  //ID����������ӳ���
T_MODULE QMutex vtRSP_MapMutex;                            //��ӳ�������������
T_MODULE T_WORD count = 0;
/************************************ʵ�ֲ���******************************************/    



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{

    return TRUE;
}

static unsigned int int2hex(char *ptr, int size,int intValue)
{
    memset(ptr,'\0',size);    //���ô�С,��󻺳����Ĵ�С
    sprintf(ptr,"%x",intValue);
    return strlen(ptr);
}


/**
* @brief 
*    ����: ��̬����TSע��Ļص�����, SA DLL��TX�̻߳���ô˷������յ����ݽ��д���
* @param[in] id sa��aid��
* @param[in] des_said Ŀ��said��
* @param[in] src_aid  ca��aid��
* @param[in] src_said ca��said��
* @param pBuf ���͵Ļ�����
* @param[in] size ��������С
* @return 0��ʾ�ɹ�
*/
FUNC_INT putpkt(T_HWORD vwServerId,T_HWORD vhwDesSaid,T_HWORD vhwSrcAid,T_HWORD vwSrcSaid,T_CHAR* cpBuf,T_WORD vwSize)
{
    //�������Ϸ���
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    //�ӱ����ҵ���Ӧ�ķ��������󣬽����˼�������
    RSP_TcpServer *server = NULL;
    vtRSP_MapMutex.lock();
    server = vtRSP_Id2Server.value(vwServerId);
    vtRSP_MapMutex.unlock();
    
    if(NULL == server)
    {
        return FAIL;
    }

    int ret = server->Putpkt(vhwDesSaid, vhwSrcAid, vwSrcSaid, cpBuf, vwSize);

    return ret;
}

/**
* @brief 
*     ����: ��̬����TSע��Ļص�����, SA DLL��RX�̻߳���ô˷������ն�̬������ݽ���ת��
* @param[in] id sa��Դsaid��
* @param[out] des_aid Ŀ��aid��
* @param[out] des_said Ŀ��said��
* @param[out] src_said ca��said��
* @param pBuf ���͵Ļ�����
* @param[in] size ��������С
* @return ʵ�ʵõ������ݵĴ�С
*/
FUNC_INT getpkt(T_HWORD vwServerId,T_HWORD *vwpDesAid,T_HWORD *vwpDesSaid,T_HWORD *vwpSrcSaid,T_CHAR* cpBuf,T_WORD vwSize)
{
    //�������Ϸ���
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    //�ӱ����ҵ���Ӧ�ķ��������󣬽����˼�������
    RSP_TcpServer *server = NULL;
    vtRSP_MapMutex.lock();
    server = vtRSP_Id2Server.value(vwServerId);
    vtRSP_MapMutex.unlock();
    
    if(NULL == server)
    {
        return FAIL;
    }

    int ret = server->Getpkt(vwpDesAid, vwpDesSaid, vwpSrcSaid, cpBuf, vwSize);

    return ret;
}

/**
* @brief 
*    ����: �رն���,���ڶ�̬���ڲ��ͷ�ռ����Դ
* @param[in] id sa��aid��
* @return 0��ʾ�ɹ�
*/
FUNC_INT close(T_HWORD hwId)
{
    //���ȼ���ID��Ӧ�ķ����������Ƿ����
    vtRSP_MapMutex.lock();

    T_BOOL ret = vtRSP_Id2Server.contains(hwId);

    //��������ڣ�ֱ�ӷ��سɹ�
    if(ret == FALSE)
    {
        vtRSP_MapMutex.unlock();
        return TRUE;
    }

    RSP_TcpServer *tcpServer = vtRSP_Id2Server.take(hwId);
    

    tcpServer->Close();
    //ȷ����̬��ر�,�����߳��˳�
    {
        int killCount = 0;
        QMutex mutex;                //�����������������

        QWaitCondition waitCondition;  //���������������жϹرշ����Ƿ������������

        while (tcpServer->isRunning() && killCount < 3) {
            killCount++;
            QMutexLocker lock(&mutex);
            waitCondition.wait(&mutex, 1);
        }
    }

    delete tcpServer;

    count--;
    if(count == 0)
    {
        /*SystemLogger* logger = SystemLogger::getInstance();
        if(NULL != logger)
        {    
            logger->close();
            delete logger;
            logger = NULL;
        }*/
    }
    vtRSP_MapMutex.unlock();


    return TRUE;
}

/**
* @brief 
*    ����: �򿪶���,���ݽ���pBuf����RSP�����������ڶ�̬���ڲ�����ʼ������
* @param[in] hwId TSΪ�䴴����ServerAgent��ID��
* @param[in] cpBuf ���ݵĲ���������
* @return 0��ʾ�ɹ�,
*/
FUNC_INT  open(T_HWORD hwId,T_CHAR* cpBuf, TCHAR* cpOutBuf)
{
    //���ȼ���Ƿ��Ѿ���IDע��
    vtRSP_MapMutex.lock();
    if(count == 0)
    {
        /*SystemLogger* logger = SystemLogger::getInstance();
        if(NULL != logger)
        {
            logger->open();
        }
        else
        {
            return FAIL;
        }*/
    }
    count++;

    T_BOOL ret = vtRSP_Id2Server.contains(hwId);
    vtRSP_MapMutex.unlock();
    
    if(ret == TRUE)
    {
        return FALSE;
    }
    char tmp[100] = "\0";
    sprintf(tmp, "%s%d", "sysIceServer",  hwId);
    //����ϵͳ��־
    SystemLog::getInstance()->openLogFile(string(tmp));
    SystemLog::getInstance()->setLogLevel(IS_CURSYS_LOG_LEVEL);
    //����ID��������������
    RSP_TcpServer *tcpServer = new RSP_TcpServer();
    int result = tcpServer->Open(hwId, cpBuf);
    if(result == FAIL)
    {
        //�ͷŴ�������Դ
        tcpServer->forceExit();
        tcpServer->Close();
        delete tcpServer;
        return FAIL;
    }
    
    //�����ɹ������뵽�б���ȥ
    vtRSP_MapMutex.lock();
    vtRSP_Id2Server.insert(hwId, tcpServer);
    vtRSP_MapMutex.unlock();

    int2hex(cpOutBuf, 4, result);
    
    return result;
}

