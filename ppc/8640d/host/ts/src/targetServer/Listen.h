/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  listen.h
* @brief
*       ���ܣ�
*       <li>�����߳�</li>
*/




#ifndef    LISTEN1_INCLUDE
#define LISTEN1_INCLUDE
/************************ͷ�ļ�********************************/
#include "tsInclude.h"
#include "tcpDevice.h"


/************************�궨��********************************/
/************************���Ͷ���******************************/
class DeviceBase;
class ClientAgent;

/*Modified by tangxp for BUG No.0002430 on 2007.12.29 [begin]*/
/*  Modifed brief:�õ���д���߳����滻QT�߳���*/

class Listen :    public baseThread
/*Modified by tangxp for BUG No.0002430 on 2007.12.29 [end]*/
{
private:
    DeviceBase* pDeviceBase;
    int port;                                    //�˿ں�

    /* ���ܣ��������봴��ClientAgent�߳�*/
    bool isRun;    //�߳����б�־
public:
    Listen(int port);

    /*����: �رռ����߳�*/
    void close();
    /**
    * ���ܣ��߳����к���
    */
    void run();

};

/************************�ӿ�����******************************/
#endif
