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
* @file     deviceBase.cpp
* @brief     设备抽象层
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/


#include "Device.h"
#include "tcpDevice.h"
#include "deviceUdp.h"
#include "deviceUart.h"
#include "deviceUSB.h"
#include "common.h"

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

        case DeviceConfigBase::USB_DEVICE:
            return new  DeviceUSB();
            break;
            
        default:
            sysLoger(LWARNING, "DeviceManage::createDevice: unknow device type %d. FILE:%s, LINE:%d",
                                                type, __MYFILE__, __LINE__);
            break;      
    }
    return NULL;
}

