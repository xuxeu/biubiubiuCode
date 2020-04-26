/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: rspTcpServer.h,v $
  * Revision 1.8  2008/03/19 10:25:11  guojc
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
  * Revision 1.7  2007/06/21 11:37:29  guojc
  * 1. �޸���IS��simihost��ɾ��Ӳ���ϵ�Ĵ���
  * 2. ͳһ����IS������־����
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
  * Revision 1.7  2006/07/17 09:30:01  tanjw
  * 1.����arm,x86 rsp��־�򿪺͹رչ���
  * 2.����arm,x86 rsp�ڴ��ȡ�Ż��㷨�򿪺͹رչ���
  *
  * Revision 1.6  2006/07/14 09:19:30  tanjw
  * 1.�޸�release������
  * 2.����ice��������
  *
  * Revision 1.5  2006/06/26 09:14:32  tanjw
  * 1.�޸�arm��x86�ڴ��ȡ����bug
  * 2.iceserverȥ��wsocket
  *
  * Revision 1.4  2006/06/22 06:37:08  tanjw
  * 1.���ʹ۲�㴦��ʽ
  *
  * Revision 1.1.1.1  2006/04/19 06:29:13  guojc
  * no message
  *
  * Revision 1.2  2006/04/14 02:02:28  guojc
  * �����˶�CP15Э�������Ĵ����޸ĵ�֧��
  *
  * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
  * TS��������
  *
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * �����ļ�;�����ļ��Ļ����ӿ�
  */

/**
* @file     rspTcpServer.h
* @brief     
*     <li>���ܣ� ����RSP_TcpServer�������������</li>
* @author     ������
* @date     2006-02-20
* <p>���ţ�ϵͳ�� 
*/

#ifndef _RSP_TCP_SERVER_H
#define _RSP_TCP_SERVER_H

/************************************���ò���******************************************/
#include <vector>
#include <map>
#include <QList>
#include <QtCore>
//#include <QThread>
#include <QMutexLocker>
#include <QWaitCondition.h>
#include <QTextOStream>

#include "sysTypes.h"
#include "wsocket.h"
#include "memory.h"
#include "lambdaIce.h"
#include "packet.h"
#include "armConst.h"
#include "IceServerComm.h"
#include "common.h"
#include "common.h"
/************************************��������******************************************/
using namespace std;


/************************************���岿��******************************************/
/**
* @brief
*    RSP_TcpServer��ICE Server�ṩ��֧�ֽ�����׼RSPЭ���TCP��������������׼RSPЭ�������\n
*   ר����Lambda ICE�ṩ��ԭ�Ӳ��������ICE���н������ﵽ����Ŀ���ϵͳ��Ŀ�ġ�\n
* @author     ������
* @version 1.0
* @date     2006-02-20
* <p>���ţ�ϵͳ�� 
*/


/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:�õ���д���߳����滻QT�߳���*/
class RSP_TcpServer : public baseThread
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
public:
    /**
    * @brief 
    *    ���캯��
    */   
    RSP_TcpServer();              

    /**
    * @brief 
    *    ��������
    */   
    ~RSP_TcpServer();

    T_BOOL GetIceEndian();
    
    T_BOOL GetTargetEndian();
    /**
    * @brief 
    *    �򿪲���������Э��������õ���Ҫ�Ĳ�����Ϣ
    * @param[in] id ��Ӧ���������󶨵�AID��
    * @param[in] cpBuf ��������ָ��,���������Դ��н���������Ĳ���
    * @return �����ɹ����ؼ����Ķ˿ںţ�ʧ�ܷ���FAIL
    */   
    int Open(T_WORD wId, T_CHAR *cpBuf);

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
    int Putpkt(T_HWORD hwDesSaid,T_HWORD hwSrcAid,T_HWORD hwSrcSaid,T_CHAR* cpBuf,T_WORD wSize);

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
    int Getpkt(T_HWORD *hwpDesAid,T_HWORD *hwpDesSaid,T_HWORD *hwpSrcSaid,T_CHAR* cpBuf,T_WORD wSize);

    /**
    * @brief 
    *    �رղ������رշ�����
    */
    T_VOID Close();
    /**
    * @brief 
    *    ǿ���˳�����Ŀǰ��������open����ʧ��ʱ��
    *    ��Ϊopenʧ�ܺ��߳�һֱ�޷��˳�������TS����
    */
    T_VOID forceExit()
    {
        m_quitMutex.lock();
        m_rxExitReady = TRUE;
        m_localExitReady = TRUE;
        m_txExitReady = TRUE;
        m_quitMutex.unlock();
    }

    /**
    * @brief 
    *    �߳�����ִ����ڷ���
    */
    T_VOID run();
    
    /**
    * @brief 
    *    ����������뵽������ά���Ļ��������
    * @param[in] tpPack    �����
    */
    T_VOID Enqueue(RSP_Packet* vtpPack);
    
        /*
    * @brief 
    *    ��һ�����͸��ݴ�С��תΪCHAR����\n
    * @param[in] vwReg    �������ṩ�ļĴ�����ֵ
    * @param[in] vcpBuf   �������ṩ�Ļ���
    * @param[in] vbIsBigEndian  �������ṩ�Ĵ�С����Ϣ
    * @return  
    */
    T_VOID Int2Char(T_WORD vwReg, T_CHAR *vcpBuf, T_BOOL vbIsBigEndian);

    /*
    * @brief 
    *    ��һ��CHAR������ݴ�С��תΪ����\n
    * @param[in] vcpBuf   �������ṩ�Ļ���
    * @param[in] vbIsBigEndian  �������ṩ�Ĵ�С����Ϣ
    * @return ������  
    */
    T_WORD Char2Int(T_CHAR *vcpBuf, T_BOOL vbIsBigEndian);
    
    /*
    * @brief 
    *    �����ļ��Ľ���
    * @return �ɹ�����TRUE��ʧ�ܷ���FALSE
    */
    bool parseConfig();
    T_WORD handleWriteSingleRegister(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleReadRegisters(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleWriteMemory(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleReadMemory(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleContinueTarget(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleSetBreakPoint(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleRemoveBreakPoint(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleResetTarget(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleSimiHostCommand(T_CHAR *cpBuf, T_WORD wSize, T_HWORD hwSrcAid,T_HWORD hwSrcSaid);

/*Modified by qingxiaohai on 2007��4��6�� 14:20:13 for TS3.2  [Begin]*/
/*  Modified brief:WCS simulator*/
    T_WORD getTargetType();
    void        setTargetType(T_WORD type);
    T_WORD getCpuType();
    void         setCputType(T_WORD type);
/*Modified for TS3.2 [End]*/
private:
    QMutex m_sktMutex;
    T_WORD m_port;   //�����������˿ں�
    WSocket m_serverSocket; //������SOCKET����
    WSocket *m_sClient; //�ͻ��˶���ָ��
    QMutex m_clientMutex;                //��SOCKET��������

    T_VBOOL m_runFlag;   //�������߳����б�ʶ
    T_VBOOL m_waitForIce;  //�ȴ�ICE
    QMutex m_excuteLock;    //ִ����������״̬��������

    RSP_LambdaIce m_lambdaIce; //ICE��Э�����ӿ�
    
    T_WORD m_iceId;  //ICE��ID��
    T_WORD m_sessId; //��ӦICE�ϵĻỰID��

    T_WORD m_bpMem;  //����ϵ��ַ����
    QMap<T_WORD, T_WORD> m_bpMap; //����ϵ��б�
    T_WORD m_retryTimes;    //��������ϵ����Դ���

    T_VBOOL m_hardBp;  //Ӳ���ϵ��Ƿ��Ѿ�������
    
    T_WORD m_cpsr;   //CPSR�Ĵ�������   
    T_WORD m_currentRegs[RSP_ArmConst::ARM_REG_NUM]; //��ǰ�Ĵ���ֵ
    QMap<T_WORD, T_WORD> m_cpp;   //CP15�Ĵ�����

    QList<pair<T_WORD, T_WORD>*> m_memCfg; //�ڴ��ʼ������ pair->first() addr pair->second() value;
    QList<T_WORD> m_delayCfg; //�ӳ�����
    QList<T_WORD> m_widthCfg; //�������
    
    QMap<T_WORD, RSP_Memory*> m_addr2mem; //�ڴ�ӳ���

    T_MODULE T_CONST T_WORD ENDIAN_SMALL = 0;
    T_MODULE T_CONST T_WORD ENDIAN_BIG = 1;
    T_BOOL m_targetEndian;     //Ŀ����Ĵ�С��
    T_BOOL m_iceEndian;        //ICE�Ĵ�С��
    T_BOOL m_isSupportPrivateProtocol;         //�Ƿ�֧��˽����չЭ��
    
    T_VWORD m_targetStatus; //Ŀ���״̬
    QMutex m_statusLock;    //Ŀ���״̬��������

    QList<T_WORD> m_atomCmds;  //ԭ�Ӳ�����������
    QList< QList<T_VOID*>* > m_atomParas; //ԭ�Ӳ�������Ĳ���

    QList<RSP_Packet*> m_packetQueue;    //���ݰ��������
    QWaitCondition m_queueNotFull;    //��������,�����ж϶��в�Ϊ��
    QMutex m_packMutex;                //�����ݰ���������������,

    QMutex m_cmdMutex;                //�����������������

    QWaitCondition m_quitReady;  //���������������жϹرշ����Ƿ������������
    QMutex m_quitMutex;    
    T_VBOOL m_rxExitReady;  //RX�̷߳��ر�־����
    T_VBOOL m_localExitReady;  //�����߳��˳���־����
    T_VBOOL m_txExitReady;  //TX�̷߳��ر�־����

    RSP_MemoryManager m_memMan;  //�ڴ����Ԫ
    QMutex m_mopMutex;
    
    T_BOOL m_mopFlag;  //�ڴ��Ż��㷨������־
    int SetMop(T_BOOL mop);
    T_BOOL GetMop();

    T_BOOL m_validForWp;  //���Դ�۲����

    T_BOOL m_validDeleWp; //����ɾ���۲����

    QString m_CfgFile;              //�����ļ�
    char m_ICEName[NAME_SIZE];            //ICEĿ����������
    QString m_IceBuf;                 //����ICE�����û���
    
    QMutex m_MtxMsgSrc;                 //��ǰ��Ϣ��Դ����
    T_WORD m_CurCmdSource;             //��ǰִ�е�����
    T_HWORD m_CurSrcAid;          //��ǰ��Ϣ������Դ�Ĵ���
    T_HWORD m_CurSrcSaid;         //��ǰ��Ϣ������Դ��������

    T_HWORD m_SimiAid;                            //simihost �Ŀͻ�����ID
    T_HWORD m_SimiSaid;                         //simihost �Ķ�������
    T_BOOL    m_SimiRegisterFlag;           //simihost ע���־
    T_WORD   m_SimiPCAddr;                  //simihost �ж�������ַ
    T_BOOL    m_NeedReplyS05;              //simihost,�յ�s05�Ƿ���Ҫ��GDB����

    T_BOOL      m_DccEnable;                    //DCCͨ���Ƿ��
    T_WORD      m_DccAddr;                      //DCC handler�ĵ�ַ
/*Modified by qingxiaohai on 2007��4��6�� 14:20:13 for TS3.2  [Begin]*/
/*  Modified brief:WCS simulator*/
    T_WORD   m_targetType;          /*ָʾ�������Ŀ���������
                                                        ICE����WCS Simulator, (0: ICE,   1: wcs simulator)*/
    T_WORD   m_CpuType;        /*ģ����ģ���CPU����*/
/*Modified for TS3.2 [End]*/
#ifdef IS_PACK_LOG
    QMutex m_logMutex;
    T_BOOL m_isLog;  //�Ƿ���Ҫ���ݰ���־ѡ�Ĭ��ΪFALSE
    PackLog m_logger;

    T_VOID SetLog(T_BOOL isLog)
    {
        m_isLog = isLog;
    }
    T_BOOL IsLog()
    {
        return m_isLog;
    }
#endif

    //SystemLogger *m_errLogger;  //������־��¼
    /*
    * @brief 
    *    �����ڷ���������GDB�����ݣ��յ����ݺ�\n
    *    �����ݿ������������ṩ�Ļ������󷵻�\n
    * @param     pBuf    �������ṩ�����ݻ���
    * @param[in] size    �������ṩ�Ļ������Ĵ�С
    * @return  ʵ�ʽ��յ����ݵĴ�С������Ͽ�����-1
    */
    int GetData(T_CHAR *pBuf, T_WORD size);

    /*
    * @brief 
    *    ���ظ����ݷ��͸�GDB\n
    * @param     pBuf    ������Ҫ���͵����ݻ���
    * @param[in] size    ������Ҫ�������ݵĴ�С
    * @return  ʵ�ʷ��͵����ݵĴ�С
    */
    int PutData(T_CHAR *pBuf, T_WORD size);

    /*
    * @brief 
    *    ������GDB�����ݽ���Э�������װ����������\n
    * @param     pBuf    �������ṩ�����ݻ���
    * @param[in] size    �������ṩ�Ļ������Ĵ�С
    * @return  
    */
    int MessageHandler(T_CHAR *pBuf, T_WORD size);

    /*
    * @brief 
    *    ���ݴ������ĵ�ǰ״̬���üĴ���\n
    * @param[in] regNo   Ҫд�ļĴ����� 
    * @param[in] value   �Ĵ�����ֵ
    * @return  SUCC��FAIL
    */
    T_BOOL WriteSingleRegister(T_WORD regNo, T_WORD value);

    /*
    * @brief 
    *    ���ݴ�����ģʽ��ȡ�����Ĵ���\n
    * @param[in] vwRegNo    �Ĵ�����
    * @return  �ɹ��򷵻ؼĴ�����ֵ��ʧ���򷵻�0
    */
    T_WORD ReadSingleRegister(T_WORD vwRegNo);

    /*
    * @brief 
    *    ���ݴ�����ģʽ��ȡ���мĴ���\n
    * @param[in] vwRegs    �Ĵ�������
    * @return  �ɹ��򷵻�SUCC��ʧ���򷵻�FAIL
    */
    T_BOOL ReadAllRegisters(T_WORD *vwRegs);

    /*
    * @brief 
    *    ��������������޶���ȡ����Ӧ�ļĴ���\n
    * @param     vwaRegs    �������ṩ�ļĴ������黺��
    * @param[in] vwTable    �������ṩ�ļĴ���ӳ���
    * @return  
    */
    T_VOID GetRegs(T_WORD *vwaRegs, T_CONST T_WORD *vwaTable);



    /*
    * @brief 
    *    ��������������ȡ����һ������\n
    * @param[in] vcpBuf   ����ֵ������Ϊ��
    * @return  ������ţ����û�з���ʧ��
    */
    int GetNextCmd();

    /*
    * @brief 
    *    �鿴�������еĴ�С\n
    * @return  �������еĴ�С
    */
    T_WORD CmdSize();

    /*
    * @brief 
    *    װ��һ�����������������\n
    * @para [in] wCmd ������� 
    * @return  ������ţ����û�з���ʧ��
    */
    T_VOID PushBackCmd(T_WORD wCmd);
    
    /*
    * @brief 
    *    �Ӳ�����������ȡ����һ�����\n
    * @return  ����ֵ�����û�з���ʧ��
    */
    QList<T_VOID*> * GetNextPara();

    /**
    * @brief 
    *    ����Ŀ���״̬����������
    * @param[in] vwStatus Ŀ���״̬
    */
    T_VOID SetTargetStatus(T_WORD vwStatus);

    /**
    * @brief 
    *    �õ�Ŀ���״̬����������
    * @param[in] vwStatus Ŀ���״̬
    */
    T_WORD GetTargetStatus();

    /*
    * @brief 
    *    ��������16�����ַ���ת��Ϊ����,��Ҫע����Ǹò������ƶ���������ָ��ĵ�ַ\n
    * @param[in]     cptr    16�����ַ���
    * @param[out] wIntValue ת����INT����
    * @return  ָ���ƶ����ֽ���
    */
    T_UWORD Hex2int(T_CHAR **vcptr, T_WORD *vwIntValue);

    /*
    * @brief 
    *    ��������16����ASCII�ַ�ת���ɶ�Ӧ������\n
    * @param[in]     cptr   ������16����ASCII�ַ�
    * @return  ���ض�Ӧ������
    */
    int Hex2num(T_UBYTE vubCh);

    /*
    * @brief 
    *    ִ���������У��ƶ�����������\n
    * @param[in] cpBuf   ����ֵ������Ϊ��
    * @return  SUCC��FAIL
    */
    int ExcuteCommand(T_CHAR *cpBuf, T_WORD wSize, T_BOOL bOpsFlag);


    /*
    * @brief 
    *    �ӱ����ڴ滺���л�ȡ��Ҫ���ڴ���Ϣ�����û�У��÷�����ѹ���������д�Ŀ����ж�ȡ��Ӧ���ڴ��\n
    * @param[in] wAddr   �ڴ��ַ
    * @param[in] wSize   �ڴ��С
    * @param[in] cpBuf   �����߸����Ļ�����
    * @param[in] type    ���������õ��ڴ�ĸ�ʽ 0ΪGDB��ʽ 1Ϊ�����Ƹ�ʽ
    * @return  ���������������ڴ����Ĵ�С��0Ϊ�ޱ��ػ���
    */
    int GetLocalMemory(T_WORD wAddr, T_WORD wSize, T_CHAR *cpBuf, T_WORD type);

    /*
    * @brief 
    *    д�ڴ�,��Ӧ��������CMD_WRITE_MEM\n
    * @param[in] tpArg  д�ڴ�����Ҫ�Ĳ��� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdWriteIceMemory(QList<T_VOID*> *vtpArg, T_BOOL vbFlag);

    /*
    * @brief 
    *    ���ڴ�\n
    * @param[in] tpArg  ���ڴ�����Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdReadIceMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ���Ĵ���,��Ӧ��������CMD_READ_REG\n
    * @param[in] tpArg  ���Ĵ�����������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdReadIceRegisters(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    д�Ĵ���,��Ӧ��������CMD_WRITE_REG\n
    * @param[in] tpArg  д�Ĵ�����������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdWriteIceRegisters(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ����Ŀ���,��Ӧ��������CMD_RUN_TRT\n
    * @param[in] tpArg  ����Ŀ�����������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdRunTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ֹͣĿ���,��Ӧ��������CMD_STOP_TRT\n
    * @param[in] tpArg  ֹͣĿ�����������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdStopTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ���ICE�ظ��Ƿ�ɹ�,��Ӧ��������CMD_ANALY_RESULT\n
    * @param[in] cpBuf  ICE�ظ������� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdCheckReply(T_CHAR *cpBuf, T_BOOL bFlag);

    /*
    * @brief 
    *    ͨ��ICE�ظ����±��ؼĴ�������,��Ӧ��������CMD_UPDATE_READ_REG\n
    * @param[in] cpBuf  ICE�ظ������� 
    * @param[in] wSize  ICE�ظ������ݵĴ�С 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdUpdateRegs(T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);

    /*
    * @brief 
    *    ͨ��ICE�ظ����±����ڴ滺��,��Ӧ��������CMD_UPDATE_READ_MEM\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] cpBuf  ICE�ظ������� 
    * @param[in] wSize  ICE�ظ������ݵĴ�С 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdUpdateMems(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);

    /*
    * @brief 
    *    �ظ�GDB���ڴ�����,��Ӧ��������CMD_REPLY_READ_MEM\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdReplyMems(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);

    /*
    * @brief 
    *    ����Ŀ���״̬����,��Ӧ��������CMD_SET_TRT_STATUS\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdUpdateStatus(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ����ICE�ظ�����Ŀ���״̬�����ƶ����̷�չCMD_UPDATE_TRT_STATUS\n
    * @param[in] tpArg  ����Ŀ�����������Ҫ�Ĳ��� 
    * @return  
    */
   T_BOOL CmdQueryTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    �ظ�GDB������,��Ӧ��������CMD_SIMPLE_REPLY_GDB\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdSimpleReply(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ����Ӳ���ϵ�,��Ӧ��������CMD_INSERT_HBP\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdInsertHardwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ��������ϵ�,��Ӧ��������CMD_INSERT_SBP\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdInsertSoftwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    д���Ͳ����ڴ�,��Ӧ��������CMD_WRITE_INT_MEM\n
    * @param[in] tpArg  д�ڴ�����Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdWriteIntMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ɾ��Ӳ���ϵ�,��Ӧ��������CMD_REMOVE_HBP\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdRemoveHardwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    �ظ�GDB S05,��Ӧ��������CMD_REPLY_S05\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdReplyS05(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ����Ӳ���۲��,��Ӧ��������CMD_INSERT_WBP\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdInsertWatchPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag);


    /*
    * @brief 
    *    ɾ��Ӳ���۲��,��Ӧ��������CMD_REMOVE_WBP\n
    * @param[in] tpArg  �����ڴ滺���������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdRemoveWatchPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag);


    T_BOOL CmdCheckSbp(QList<T_VOID*> *tpArg,  T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);


    /*
    * @brief 
    *    ��ʱ����,��Ӧ��������CMD_DELAY\n
    * @param[in] tpArg  ��ʱ����Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdDelay(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    дCP15�Ĵ���,��Ӧ��������CMD_INIT_CP15\n
    * @param[in] tpArg  д�Ĵ�����������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdWriteCp15Registers(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ��CP15�Ĵ���,��Ӧ��������CMD_INIT_CP15\n
    * @param[in] tpArg  д�Ĵ�����������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdReadCp15Registers(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    
    /*
    * @brief 
    *    ��ѯICE�Ĵ�С��
    * @param[in] tpArg  д�Ĵ�����������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdQueryIceEndian(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    ����ICE�Ự
    * @param[in] tpArg  д�Ĵ�����������Ҫ�Ĳ��� 
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdCreateSession(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    /*
    * @brief 
    *    ɾ��ICE�Ự
    * @param[in] tpArg  
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdDeleteSession(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    /*
    * @brief 
    *    ����ɾ��ICE�Ự���
    * @param[in] tpArg  
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ���� 
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdAnalyDeleteSession(T_CHAR *cpBuf, T_BOOL bFlag);
    /*
    * @brief
    *    ������ѯICE��С�˵Ľ��
    * @param[in] cpBuf  ��ѯ�����Ľ��
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdAnalyQueryICEEndian(T_CHAR *cpBuf, T_BOOL bFlag);
    
    /*
    * @brief
    *    ���������Ự�Ľ��
    * @param[in] cpBuf  �����Ự�����Ľ��
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdAnalyCreateSession(T_CHAR *cpBuf, T_BOOL bFlag);  

    /*
    * @brief
    *    ����Ŀ���
    * @param[in] tpArg  ����Ŀ����Ĳ���(��������)
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdResetTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag);  
    
    /*
    * @brief
    *    ����Ŀ���
    * @param[in] tpArg  ����Ŀ����Ĳ���(��������)
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdAnalyResetTarget(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);  
    
    /*
    * @brief
    *    ����Ŀ���
    * @param[in] tpArg  ����Ŀ����Ĳ���(��������)
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdQueryTargetCpuIDCode(QList<T_VOID*> *tpArg, T_BOOL bFlag);  

    /*
    * @brief
    *    ����Ŀ���
    * @param[in] tpArg  ����Ŀ����Ĳ���(��������)
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdAnalyQueryTargetCpuIDCode(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);  
    /*
    * @brief
    *    ��������ϵ�ʱ���ڴ�Ľ����������ϵ�
    * @param[in] tpArg  ����ϵ���ڴ�Ĳ���
    * @param[in] bFlag  ȫ�ֲ�����ǣ����Ϊʧ�ܣ��򷽷��ڲ�ֻ���ͷ���Դ����
    * @return  �ɹ�����ʧ��
    */
    T_BOOL CmdAnalyBreakPointReadMem(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);  

    /*
     * @brief   
     * ����PC�Ĵ���ֵ�Ƿ����SIMIHOSTע��ʱ�Ǽǵ�ֵ��������������ϱ�SWI
     */
    T_BOOL CmdAnalySimihostBreakPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    
    /*
     * @brief   
     * ����DCC ����
     */
    T_BOOL CmdOpenDccFunction(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    /*
     * @brief   
     * ��������DCC  ���ܵĽ��
     */
    T_BOOL CmdAnalyOpenDccResult(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);

    /*
    * @brief 
    *    ��ѯĿ���״̬��ͨ��ICE�Ļظ��ƶ�״̬����ִ�У�װ����������\n
    * @return  
    */
    T_VOID QueryTarget();

    /*
    * @brief 
    *    ��ʼ��Ŀ�����������Ŀ����ϵ��Ժ���У�װ����������\n
    * @return  
    */
    T_VOID InitTarget();

    /*
    * @brief 
    *    ��ձ����ڴ滺��,
    */
    T_VOID RefreshMemory();

    /*
    * @brief 
    *    ��GDB�Ͽ���IS�����Ӻͼ��Ŀ������Ƿ��жϵ����ã�����У���װ��������նϵ�����\n
    */
    T_BOOL ClearTarget();

    /*
    * @brief 
    *    �÷�����������RX�߳�\n
    */
    T_VOID WaitUp();

    /*
    * @brief 
    *    �÷������ڻ���RX�߳�\n
    */
    T_VOID WakeUp();

    /*
    * @brief 
    *    �ӻ����������ȡ��һ�������\n
    * @return  �������ݰ�
    */
    RSP_Packet* Dequeue();

    /*
    * @brief 
    *    �رշ������̷߳���
    */
    T_VOID CloseThread();

    T_VOID DeleteSocket();

    T_VOID HandleIDECmd(T_HWORD hwSrcAid,T_HWORD hwSrcSaid, T_CHAR* cpBuf,T_WORD wSize);
    T_VOID SendIDEResult(T_CHAR *cpBuf, T_WORD wSize, T_HWORD hwSrcAid,T_HWORD hwSrcSaid);
};

#endif