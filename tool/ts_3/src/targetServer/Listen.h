/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* 
*
* @file     listen.h
* @brief     �����߳�
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/


#ifndef    LISTEN1_INCLUDE
#define LISTEN1_INCLUDE

#include "tsInclude.h"
#include "tcpDevice.h"

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
//    QMap<int ,ClientAgent*> mapID2CA;            //ID�ŵ�ClientAgent��ӳ��
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
#endif
