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
* @file     deviceBase.h
* @brief     �豸�����
* @author     ���
* @date     2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/

#ifndef DEVICE_INCLUDE
#define DEVICE_INCLUDE

#include "Config.h"

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
                pDeviceManage = new DeviceManage();
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


#endif









