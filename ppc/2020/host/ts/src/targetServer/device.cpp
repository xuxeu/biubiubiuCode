/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  device.cpp
* @brief
*       功能：
*       <li>设备抽象层</li>
*/



/************************头文件********************************/

#include "Device.h"
#include "tcpDevice.h"
#include "deviceUdp.h"
#include "deviceUart.h"
#include "common.h"

/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/
///////////////////////DeviceBase
DeviceBase::~DeviceBase()
{
}

///////////////////////////////////////////////DeviceManage
DeviceManage* DeviceManage::pDeviceManage = NULL;
TMutex DeviceManage::mutex;

/**
* 功能：提供创建协议的方法
* @param  协议类型
* @return 一个具体协议
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

