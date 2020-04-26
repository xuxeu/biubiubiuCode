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
 * @file     DeviceUart.h
 * @brief     �����豸��
 * @author     ���
 * @date     2006-1-11
 * @version 3.0
 * @warning �Ͻ������ļ������ַ�
 *
 * ���ţ�ϵͳ�� 
 *
 */

#ifndef UART_INCLUDE
#define UART_INCLUDE

#include "Device.h"
#include "windows.h"
#include <QString>
class DeviceUart : public DeviceBase
{
    bool isExit;
    OVERLAPPED RxOverlap;        //�����ݵ��첽�ṹ��,��Ҫ��Ա��һ���¼�����
    OVERLAPPED TxOverlap;        //���;ݵ��첽�ṹ��,��Ҫ��Ա��һ���¼�����
    HANDLE hUart ;                //���ھ��
    TMutex mutex;
    QString        com;            //���ں�
    int            baud;            //������
    int            timeout;        //��ʱ
    int            retry;            //���Դ���
    /**
    * ���ܣ����ô�������
    * @return true��ʾ�ɹ�,false��ʾʧ��
    */
    bool SetCommState(); //���ô���״̬
public:
    /**
    * ���ܣ��õ���ʱ
    */
    int getTimeout() 
    {
        return timeout;
    }
    /**
    * ���ܣ����ó�ʱ
    */
    void setTimeout(int timeout)
    {
        this->timeout = timeout;
    }

    /**
    * ���ܣ��õ����Դ���
    */
    int getRetry()
    {
        return retry;
    }
    /**
    * ���ܣ��������Դ���
    */
    void setRetry(int retry) 
    {
        this->retry = retry;
    }
    DeviceUart(void);
    virtual ~DeviceUart(void);
    /**
     * ���ܣ��򿪾����豸
     * @return true��ʾ�ɹ�,false��ʾʧ��
     */
    virtual bool open(ServerConfig* config);
    /**
     * ���ܣ��رվ����豸
     * @return true��ʾ�ɹ�,false��ʾʧ��
     */
    virtual bool close();
    /**
     * ���ܣ����豸�ж�ȡ��Ϣ
     * @param  pBuff �õ���Ϣ������
     * @param  size  �������Ĵ�С
     * @param  timeout �ȴ�ʱ��,-1��ʾ����ȴ�,
     * @return ���豸�еõ������ݴ�С
     */
    virtual int  read(char* pBuff, int size);
    /**
     * ���ܣ����豸��������
     * @param  pBuff ������Ϣ��
     * @param  size ��Ϣ����С
     */
    virtual void write(char* pBuff, int size) ;
    /**
     * ���ܣ�����豸�������
     * @return  true��ʾ���ӳɹ�
     */
    virtual bool connect() ;
};
#endif    //DEVICE_UART_INCLUDE

