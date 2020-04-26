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
* @brief     设备抽象层
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
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
     * 功能：打开具体设备
     * @return true表示成功,false表示失败
     */
    virtual bool open(ServerConfig* config) = 0;
    /**
     * 功能：关闭具体设备
     * @return true表示成功,false表示失败
     */
    virtual bool close() = 0;
    /**
     * 功能：从设备中读取信息
     * @param  pBuff 得到信息缓冲区
     * @param  size  缓冲区的大小
     * @param  timeout 等待时间,-1表示无穷等待,
     * @return 从设备中得到的数据大小
     */
    virtual int  read(char* pBuff, int size) = 0;
    /**
     * 功能：向设备发送数据
     * @param  pBuff 发送信息包
     * @param  size 信息包大小
     */
    virtual void write(char* pBuff, int size) = 0;
    /**
     * 功能：检查设备连接情况
     * @return  true表示连接成功
     */
    virtual bool connect() = 0;
};


//////////////////tcp设备通信/////////////////////////////////


/**
 * 模块：  设备抽象层
 * 功能：  设备管理
 */
class DeviceManage
{
private:
    static DeviceManage* pDeviceManage;
    static TMutex mutex;                //对初始化进行互斥
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
     * 功能：提供创建协议的方法
     * @param  协议类型
     * @return 一个具体协议
     */
    static DeviceBase* createDevice(int type);
};


#endif









