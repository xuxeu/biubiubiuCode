/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  device.cpp
* @brief
*       ���ܣ�
*       <li>�豸�����</li>
*/



/************************ͷ�ļ�********************************/

#include "Device.h"
#include "tcpDevice.h"
#include "deviceUdp.h"
#include "deviceUart.h"
#include "common.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/
///////////////////////DeviceBase
DeviceBase::~DeviceBase()
{
}

///////////////////////////////////////////////DeviceManage
DeviceManage* DeviceManage::pDeviceManage = NULL;
TMutex DeviceManage::mutex;

/**
* ���ܣ��ṩ����Э��ķ���
* @param  Э������
* @return һ������Э��
*/
DeviceBase* DeviceManage::createDevice(int type)
{
    switch (type)
    {
    case DeviceConfigBase::TCP_DEVICE:
        return new DeviceTcp();
        break;

    case DeviceConfigBase::UDP_DEVICE:
        return new DeviceUdp();
        break;

    case DeviceConfigBase::UART_DEVICE:
        return new DeviceUart();
        break;

    default:
        sysLoger(LWARNING, "DeviceManage::createDevice: unknow device type %d. FILE:%s, LINE:%d",
                 type, __MYFILE__, __LINE__);
        break;
    }

    return NULL;
}

