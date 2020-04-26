
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
* @file     config.cpp
* @brief     TS的配置对象
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#include "Config.h"
#include "OwnerArchive.h"
#include "tcpDevice.h"
#include "common.h"

////////////////////////////////DeviceConfigBase

DeviceConfigBase::DeviceConfigBase(int deviceType) 
{
   this->deviceType = deviceType;
}

void DeviceConfigBase::serialize(OwnerArchive& ar) 
{
   if(ar.IsStoring()) 
      (*ar.pDataStream) << deviceType;
   else 
      (*ar.pDataStream) >> deviceType;        
}

DeviceConfigBase* DeviceConfigBase::newDeviceConfig(int type)
{
    switch (type)
    {
        case DeviceConfigBase::TCP_DEVICE:
            return new TcpConfig(type);    //tcp
            break;
            
        case DeviceConfigBase::UDP_DEVICE:    
            return new UdpConfig(type);    //udp
            break;
            
        case DeviceConfigBase::UART_DEVICE:
            return new UartConfig(type);    //串口
            break;
        case DeviceConfigBase::USB_DEVICE:
            return new USBConfig();
            break;
            
        default:
            sysLoger(LWARNING, "DeviceConfigBase::newDeviceConfig: unknow device type %d", type);
            break;
    }
    return NULL;
}


////////////////////////////////TcpConfig
TcpConfig::TcpConfig(int deviceType): DeviceConfigBase(deviceType) 
{
}

TcpConfig::TcpConfig(int deviceType,int port,QString IP) : 
                       DeviceConfigBase(deviceType) 
{
   this->port = port;
   this->IP = IP;
}

void TcpConfig::serialize(OwnerArchive& ar)
{

   DeviceConfigBase::serialize(ar);
    if(ar.IsStoring()) 
    {
      (*ar.pDataStream) << IP;
      (*ar.pDataStream) << port;
    } 
    else 
    {
      (*ar.pDataStream) >> IP;    
      (*ar.pDataStream) >> port;        
   }
}
////////////////////////////////UDPpConfig
UdpConfig::UdpConfig(int deviceType) : DeviceConfigBase(deviceType)
{
}

UdpConfig::UdpConfig(int deviceType,int port,QString IP,
                     int timeout,int retry): DeviceConfigBase(deviceType) 
{
   this->port = port;
   this->IP = IP;
   this->timeout = timeout;
   this->retry = retry;
}

void UdpConfig::serialize(OwnerArchive& ar) 
{

   DeviceConfigBase::serialize(ar);
    if(ar.IsStoring()) 
    {
      (*ar.pDataStream) << IP;
      (*ar.pDataStream) << port;
      (*ar.pDataStream) << timeout;
      (*ar.pDataStream) << retry;
    }
    else 
    {
      (*ar.pDataStream) >> IP;    
      (*ar.pDataStream) >> port;        
      (*ar.pDataStream) >> timeout;
      (*ar.pDataStream) >> retry;
   }    
}
////////////////////////////////UartConfig
UartConfig::UartConfig(int deviceType,QString com,int baud,
                  int timeout,int retry) : DeviceConfigBase(deviceType) 
{
   this->com = com;
   this->baud = baud;
   this->timeout = timeout;
   this->retry = retry;
}

void UartConfig::serialize(OwnerArchive& ar)
{
   DeviceConfigBase::serialize(ar);
   if(ar.IsStoring())
    {
      (*ar.pDataStream) << com;
      (*ar.pDataStream) << baud;
      (*ar.pDataStream) << timeout;
      (*ar.pDataStream) << retry;
    }   
    else 
    {
      (*ar.pDataStream) >> com;
      (*ar.pDataStream) >> baud;
      (*ar.pDataStream) >> timeout;
      (*ar.pDataStream) >> retry;
  }   
}
///////////////////ServerConfig
ServerConfig::ServerConfig(DeviceConfigBase* pDevice,int protocolType,
                                             bool isLog)
{
   this->pDevice = pDevice;
   this->isLog = isLog;
   this->protocolType = protocolType;
}

ServerConfig::~ServerConfig() 
{
   if (pDevice != NULL)
      delete pDevice;                              
}                              

void ServerConfig::serialize(OwnerArchive& ar)
{

   if(ar.IsStoring()) 
    {
      (*ar.pDataStream) << isLog;
      (*ar.pDataStream) << protocolType;
   }
   else
   {
      (*ar.pDataStream) >> isLog;        
      (*ar.pDataStream) >> protocolType;
   }

   if (pDevice != NULL)
      pDevice->serialize(ar);

}

int ServerConfig::getProtocolType()
{
    return protocolType;
}

void ServerConfig::setProtocolType(int protocolType)
{
     this->protocolType = protocolType;
}
/**
* 功能：设置设备类型配置信息
*/
void ServerConfig::setDeviceConfig(DeviceConfigBase* pDevice) 
{
    if (this->pDevice != NULL)
        delete this->pDevice;
    this->pDevice = pDevice;
}

void ServerConfig::setLogFlag(bool isLog)
{
     this->isLog = isLog;
}

