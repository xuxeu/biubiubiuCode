/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  device.h
* @brief
*       功能：
*       <li>设备抽象层</li>
*/



#ifndef DEVICE_INCLUDE
#define DEVICE_INCLUDE

/************************头文件********************************/
#include "Config.h"

/************************宏定义********************************/
/************************类型定义******************************/
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
            {
                pDeviceManage = new DeviceManage();
            }
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
/************************接口声明******************************/

#endif









