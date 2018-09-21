/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  DeviceUart.h
* @brief
*       ���ܣ�
*       <li>�����豸��</li>
*/



#ifndef UART_INCLUDE
#define UART_INCLUDE

/************************ͷ�ļ�********************************/

#include "Device.h"
#include "windows.h"
#include <QString>

/************************�궨��********************************/
/************************���Ͷ���******************************/
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

/************************�ӿ�����******************************/
#endif    //DEVICE_UART_INCLUDE

