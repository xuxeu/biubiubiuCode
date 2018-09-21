/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  device.h
* @brief
*       ���ܣ�
*       <li>�豸�����</li>
*/



#ifndef DEVICE_INCLUDE
#define DEVICE_INCLUDE

/************************ͷ�ļ�********************************/
#include "Config.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/
class DeviceBase : public QObject
{
    //  Q_OBJECT
private:
public:
    virtual ~DeviceBase() ;
    /**
     * ���ܣ��򿪾����豸
     * @return true��ʾ�ɹ�,false��ʾʧ��
     */
    virtual bool open(ServerConfig* config) = 0;
    /**
     * ���ܣ��رվ����豸
     * @return true��ʾ�ɹ�,false��ʾʧ��
     */
    virtual bool close() = 0;
    /**
     * ���ܣ����豸�ж�ȡ��Ϣ
     * @param  pBuff �õ���Ϣ������
     * @param  size  �������Ĵ�С
     * @param  timeout �ȴ�ʱ��,-1��ʾ����ȴ�,
     * @return ���豸�еõ������ݴ�С
     */
    virtual int  read(char* pBuff, int size) = 0;
    /**
     * ���ܣ����豸��������
     * @param  pBuff ������Ϣ��
     * @param  size ��Ϣ����С
     */
    virtual void write(char* pBuff, int size) = 0;
    /**
     * ���ܣ�����豸�������
     * @return  true��ʾ���ӳɹ�
     */
    virtual bool connect() = 0;
};


//////////////////tcp�豸ͨ��/////////////////////////////////


/**
 * ģ�飺  �豸�����
 * ���ܣ�  �豸����
 */
class DeviceManage
{
private:
    static DeviceManage* pDeviceManage;
    static TMutex mutex;                //�Գ�ʼ�����л���
public:
    static DeviceManage* getInstance()
    {
        if (pDeviceManage == NULL)
        {
            TMutexLocker locker(&mutex);

            if (pDeviceManage == NULL)
            {
                pDeviceManage = new DeviceManage();
            }
        }

        return pDeviceManage;
    }
    /**
     * ���ܣ��ṩ����Э��ķ���
     * @param  Э������
     * @return һ������Э��
     */
    static DeviceBase* createDevice(int type);
};
/************************�ӿ�����******************************/

#endif









