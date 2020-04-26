/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  DeviceUSB.h
 * @Version        :  1.0.0
 * @Brief           :  USB设备层的接口抽象
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年3月21日 11:29:33
 * @History        : 
 *   
 */
#ifndef _DEVICE_USB_H_
#define _DEVICE_USB_H_

#include "stdlib.h"
#include "windows.h"
#include "setupapi.h"
#include "initguid.h"
#include "common.h"
#include "device.h"

#pragma comment(lib, "Setupapi")


//DEFINE_GUID(GUID_CLASS_I82930_BULK, 
//0x873fdf, 0x61a8, 0x11d1, 0xaa, 0x5e, 0x0, 0xc0, 0x4f, 0xb1, 0x72, 0x8b);

class DeviceUSB:public DeviceBase
{
    private:
        bool                 m_isExit;                                      //退出标志
        GUID                m_Guid;                                         //ICE guid
        OVERLAPPED   m_RxOverLap;                             //读线程的异步事件
        OVERLAPPED   m_TxOverLap;                               //写线程的异步事件
        HDEVINFO       deviceInfoSet;  
        QString           inPipe;
        QString           outPipe;
        char                 completeDeviceName[256];
        HANDLE           m_hFileIn;
        HANDLE           m_hFileOut;                                       //handle to the usb device
        WINVERSION   m_ver;
    public:

        DeviceUSB(GUID guid = LambdaICE_GUID);
        ~DeviceUSB();

        //open the usb device;
        virtual bool open(ServerConfig* config);

        //interface for writing data
        virtual void    write(char *pData, int size);

        //interface for Reading data
        virtual int     read(char *pBuf, int size);

        //close the usb device
        virtual bool close();

        virtual bool connect()
        {
            return true;
        }
        
    protected:

        HANDLE OpenOneDeviceForWin2k(HDEVINFO   HardwareDeviceInfo,
                                                 PSP_DEVICE_INTERFACE_DATA   DeviceInfoData,
                                                 char *devName);
        HANDLE OpenOneDeviceForWinXP(HDEVINFO   HardwareDeviceInfo,
                                                 PSP_DEVICE_INTERFACE_DATA   DeviceInfoData,
                                                 char *devName);
        HANDLE OpenUsbDevice( LPGUID  pGuid, char *outNameBuf);
        BOOL GetUsbDeviceFileName( LPGUID  pGuid, char *outNameBuf);
        HANDLE open_file( char *filename);
            
        
}; 



#endif /*_DEVICE_USB_H_*/
